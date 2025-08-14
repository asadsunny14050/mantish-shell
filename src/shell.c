#include "../include/shell.h"
#include "../include/built_in.h"
#include "../include/common.h"
#include "../include/debug.h"
#include "../include/operators.h"
#include "../include/parse.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *operaters[] = {"|", ">>", ">", "2>", "<", "&&", "||"};

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
    printf("token: %p, %s\n", &token, token);
    printf("rest: %s\n", rest);

    if (check_operators(token, command, args, &position))
      continue;

    args[position] = token;
    position++;

    handle_quotes(&rest, args, &position);
  }

  print_string_array(args, 10);

  return args;
}

bool execute_command(command_t *command) {

  int prev_pipe_read_end = STDIN_FILENO;
  pid_t child_pid;
  pid_t child_list[1024];
  pid_t num_of_child = 0;
  enum pipe_channels current_pipe_fds[2] = {-1, -1}; // Stores the new pipe created for current_cmd -> next_command

  bool redirection;

  bool run_permit = true; // means depends on the exectution result of the previous command if there any at all
  command_t *current_cmd = command;
  while (current_cmd) {
    printf("current_cmd: %s\n", current_cmd->args[0]);
    redirection = false;

    if (!run_permit) {
      printf("hold on buddy, you can't run\n");
      clean_up_fds(&prev_pipe_read_end, current_pipe_fds);
      break;
    }

    if (current_cmd->operater) {
      if ((strcmp(command->operater, operaters[PIPE]) == 0 || strcmp(command->operater, operaters[AND]) == 0 || strcmp(command->operater, operaters[OR]) == 0)) {
        if (!current_cmd->next_command || !current_cmd->next_command->args[0]) {
          fprintf(stderr, "incorrect syntax: didn't input program after the %s operator\n", current_cmd->operater);
          clean_up_fds(&prev_pipe_read_end, current_pipe_fds);
          break;
        }

        if (strcmp(current_cmd->operater, operaters[PIPE]) == 0 && pipe((int *)current_pipe_fds) == -1) {
          perror("mantish: pipe creation failed\n");
          clean_up_fds(&prev_pipe_read_end, current_pipe_fds);
          break;
        }
      } else if (!*current_cmd->operand) {
        fprintf(stderr, "incorrect syntax: didn't input file after the %s operator\n", current_cmd->operater);
        clean_up_fds(&prev_pipe_read_end, current_pipe_fds);
        break;
      }

      redirection = true;
      printf("redirection: %d\n", redirection);
      printf("operator: %s\n", current_cmd->operater);
    }

    child_pid = -1;
    int built_in_result = execute_built_ins(current_cmd, &prev_pipe_read_end, current_pipe_fds);
    // 0 means no build_in to execute fork a child
    // 1 means a built_in command was executed no need to fork a child process, move on the the next command
    // -1 means build_in command was found but returned an error thus canceling the command chain loop and signaling for repromt
    if (built_in_result == 0) {
      child_pid = fork();
    } else if (built_in_result == -1) {
      clean_up_fds(&prev_pipe_read_end, current_pipe_fds);
      if (!set_run_permit(current_cmd, built_in_result, &run_permit))
        break;
    } else {
      set_run_permit(current_cmd, built_in_result, &run_permit);
    }

    if (child_pid == 0) {
      printf("i'm the child\n");

      if (redirection) {
        printf("let's redirect stdout of %s\n", current_cmd->args[0]);
        if (strcmp(current_cmd->operater, operaters[WRITE]) == 0 || strcmp(current_cmd->operater, operaters[APPEND]) == 0 || strcmp(current_cmd->operater, operaters[WRITE_ERR]) == 0) {
          printf("will write to file\n");
          write_to_file(*current_cmd->operand, current_cmd->operater);
        } else if (strcmp(current_cmd->operater, operaters[PIPE]) == 0) {
          write_to_pipe(current_pipe_fds);
        }
      }

      if (prev_pipe_read_end != STDIN_FILENO) {
        printf("%s has to read from a pipe\n", current_cmd->args[0]);
        dup2(prev_pipe_read_end, STDIN_FILENO);
        close(prev_pipe_read_end);
      } else if (current_cmd->operater && strcmp(current_cmd->operater, operaters[READ]) == 0) {
        printf("%s has to read from a file\n", current_cmd->args[0]);
        read_from_file(*current_cmd->operand);
      }

      if (execvp(current_cmd->args[0], current_cmd->args) == -1) {
        fprintf(stderr, "mantish: either the program \"%s\" doesn't exist or an internal error occured\n", current_cmd->args[0]);
        clean_up_fds(&prev_pipe_read_end, current_pipe_fds);
        exit(EXIT_FAILURE);
      }
    } else if (child_pid == -1) {
      printf("mantish: no forking since built_in\n");
      clean_up_fds(&prev_pipe_read_end, current_pipe_fds);

    } else {
      child_list[num_of_child++] = child_pid;
      if (current_pipe_fds[WRITE_END] != -1) {
        prev_pipe_read_end = current_pipe_fds[READ_END];
        close(current_pipe_fds[WRITE_END]);
      } else {
        prev_pipe_read_end = STDIN_FILENO;
      }

      if (current_cmd->operater && ((strcmp(current_cmd->operater, operaters[AND]) == 0 || strcmp(current_cmd->operater, operaters[OR]) == 0))) {
        int status;
        waitpid(child_pid, &status, 0);
        int exit_code;
        if (WIFEXITED(status)) {
          exit_code = WEXITSTATUS(status); // Get actual exit code
          run_permit = strcmp(current_cmd->operater, operaters[AND]) ? exit_code != 0 : exit_code == 0;
          num_of_child--;
        }
      }
    }

    command_t *previous_cmd = current_cmd;
    current_cmd = current_cmd->next_command;
    memset(previous_cmd, 0, sizeof(command_t));
  }

  for (int i = 0; i < num_of_child; i++) {
    int process_status;
    waitpid(child_list[i], &process_status, 0); // Blocking wait for each child
  }

  return true;
}
