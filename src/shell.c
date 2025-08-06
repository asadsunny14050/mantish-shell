#include "../include/shell.h"
#include "../include/debug_functions.h"
#include "../include/parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

enum operator_type {
  PIPE,
  APPEND,
  WRITE
};

enum pipe_channels {
  READ_END,
  WRITE_END,
};

char *operaters[] = {"|", "<<", "<"};

char *read_command() {
  char *line;
  size_t buff_size = 0;
  if (getline(&line, &buff_size, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);
    }
    perror("readline");
    exit(EXIT_FAILURE);
  }
  printf("%s\n", line);
  return line;
}

char **parse_command(char *line, command_t *command) {

  size_t buffer_size = TOKEN_BUFF_SIZE;
  char **args = malloc(buffer_size * sizeof(char *));
  if (!args) {
    fprintf(stderr, "mantish: allocation error!");
    exit(EXIT_FAILURE);
  }
  memset(args, 0, buffer_size * sizeof(char *));
  command->args = args;

  if (!line) {
    fprintf(stderr, "mantish: empty command!");
    exit(EXIT_FAILURE);
  }

  char *token;
  char *rest = line;
  int position = 0;
  while (rest && (token = strtok_r(rest, TOKEN_DELIMITER, &rest))) {
    if (position * sizeof(char *) >= buffer_size) {

      buffer_size += TOKEN_BUFF_SIZE;
      args = realloc(args, buffer_size * sizeof(char *));
      if (!args) {
        fprintf(stderr, "mantish: allocation error!");
        exit(EXIT_FAILURE);
      }
    }
    printf("token: %s\n", token);
    printf("rest: %s\n", rest);

    if (strcmp(token, operaters[PIPE]) == 0) {

      printf("pipe token found: %s\n", token);
      command->operater = operaters[PIPE];
      command->next_command = malloc(sizeof(command_t));
      command->next_command->args = &args[position + 1];
      position++;
      // args[position] = operaters[PIPE];
      // position++;
      continue;
    }

    args[position] = token;
    position++;

    handle_quotes(&rest, args, &position);
  }

  print_string_array(args, 10);

  return args;
}

bool execute_command(char **args, command_t *command) {

  int process_status;

  int prev_pipe_read_end = STDIN_FILENO;
  pid_t child_pid;
  pid_t child_list[1024];
  pid_t num_of_child = 0;
  enum pipe_channels current_pipe_fds[2] = {-1, -1}; // Stores the new pipe created for current_cmd -> next_command
  bool redirection = false;

  command_t *current_cmd = command;
  while (current_cmd) {
    printf("current_cmd: %s\n", current_cmd->args[0]);
    redirection = false;

    if (current_cmd->operater && (strcmp(current_cmd->operater, operaters[PIPE]) == 0)) {
      if (!current_cmd->next_command) {
        fprintf(stderr, "incorrect syntax: didn't input program name after the | operator");
        break;
      }

      if (pipe((int *)current_pipe_fds) == -1) {
        perror("mantish: pipe creation failed");
        break;
      }
      redirection = true;
      printf("redirection: %d\n", redirection);
      printf("operator: %s\n", current_cmd->operater);
    }

    child_pid = fork();
    if (child_pid == 0) {
      if (redirection) {
        printf("let's redirect stdout of %s\n", current_cmd->args[0]);
        dup2(current_pipe_fds[WRITE_END], STDOUT_FILENO); // Redirect stdout to pipe's write end
        close(current_pipe_fds[WRITE_END]);               // Close unused read end
        close(current_pipe_fds[READ_END]);                // no need for original read end
      }
      if (prev_pipe_read_end != STDIN_FILENO) {
        printf("%s has to read from a pipe\n", current_cmd->args[0]);
        dup2(prev_pipe_read_end, STDIN_FILENO);
        close(prev_pipe_read_end);
      }
      if (execvp(current_cmd->args[0], current_cmd->args) == -1) {
        fprintf(stderr, "mantish: either the program \"%s\" doesn't exist or an internal error occured\n", args[0]);
        break;
      }
    } else if (child_pid == -1) {
      fprintf(stderr, "mantish: forking failed\n");
      break;

    } else {
      child_list[num_of_child++] = child_pid;
      if (current_pipe_fds[WRITE_END] != -1) {
        prev_pipe_read_end = current_pipe_fds[READ_END];
        close(current_pipe_fds[WRITE_END]);
      } else {
        prev_pipe_read_end = STDIN_FILENO;
      }
    }

    current_cmd = current_cmd->next_command;
  }

  // do {
  //   wpid = waitpid(pid, &process_status, WUNTRACED);
  // } while (!WIFEXITED(process_status) && !WIFSIGNALED(process_status));
  for (int i = 0; i < num_of_child; i++) {
    int status;
    // Use WNOHANG if cleanup is on error and you don't want to block forever
    // if a child is misbehaving. For normal pipeline completion, blocking is fine.
    waitpid(child_list[i], &status, 0); // Blocking wait for each child
    // You could log status here if needed
  }

  return true;
}
