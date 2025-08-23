#include "../include/built_in.h"
#include <stdlib.h>
#include <unistd.h>

char *built_in_commands[] = {"cd", "pwd", "history", "exit"};

extern Session shell_session;

void print_history(int output_fd) {
  int head = shell_session.history_list.head;
  int tail = shell_session.history_list.tail;
  int index = head;
  int count = 1;
  // printf("head: %d\n", head);
  // printf("tail: %d\n", tail);
  dprintf(output_fd, "\e[32m------------------------------------------------\n");
  dprintf(output_fd, "[Commands History]\n");
  dprintf(output_fd, "------------------------------------------------\n");
  do {
    dprintf(output_fd, "%-6i│   ", count);
    dprintf(output_fd, "%s", shell_session.history_list.items[index]);
    index = (index + 1) % CAPACITY;
    count++;

  } while (index != (tail + 1) % CAPACITY);
  dprintf(output_fd, "------------------------------------------------\n\e[0m");

  if (output_fd != STDOUT_FILENO) {
    close(output_fd);
  }
}

int print_working_directory(int output_fd) {
  char working_directory[DIR_SIZE];
  getcwd(working_directory, DIR_SIZE);
  if (output_fd != STDOUT_FILENO) {
    dprintf(output_fd, "Working Directory: %s\n", working_directory);
    close(output_fd);
    return 0;
  }

  printf("Working Directory: %s\n", working_directory);
  return 0;
}

int change_directory(char *path_name) {
  printf("running change directory\n");
  printf("path_name: %s\n", path_name);

  if (strcmp(path_name, "-") == 0) {
    path_name = shell_session.previous_directory;

  } else if (strcmp(path_name, "..") == 0) {
    printf("going back up a directory\n");
    char buffer[DIR_SIZE];
    strncpy(buffer, shell_session.current_directory, DIR_SIZE);
    char *last_slash = strrchr(buffer, '/');
    *last_slash = 0;
    path_name = buffer;
  }

  int result = chdir(path_name);

  if (result == -1) {
    fprintf(stderr, "mantish: directory \"%s\" doesn't exist\n", path_name);
    path_name = NULL;
    return -1;
  }

  strncpy(shell_session.previous_directory, shell_session.current_directory, DIR_SIZE);
  getcwd(shell_session.current_directory, DIR_SIZE);
  printf("shell_session.current_directory: %s\n", shell_session.current_directory);
  return 0;
}

extern char *operaters[];

int execute_built_ins(command_t *command, int *prev_pipe_read_end, enum pipe_channels current_pipe_fds[2]) {
  bool has_next_command = command->next_command;

  bool redirect_to_file = command->operater && (strcmp(command->operater, operaters[WRITE]) == 0 || strcmp(command->operater, operaters[APPEND]) == 0 || strcmp(command->operater, operaters[WRITE_ERR]) == 0);

  bool redirect_to_pipe = has_next_command && (strcmp(command->operater, operaters[PIPE]) == 0);

  bool not_stdin = command->operater && (strcmp(command->operater, operaters[READ]) == 0);

  if (strcmp("cd", command->args[0]) == 0) {
    clean_up_fds(prev_pipe_read_end, current_pipe_fds);
    if (!command->args[1]) {

      fprintf(stderr, "mantish: cd needs a directory path to go to\n");
      return -1;
    }

    if (command->args[2]) {

      fprintf(stderr, "mantish: cd received more than one argument\n");
      return -1;
    }

    if (redirect_to_pipe || not_stdin) {

      fprintf(stderr, "mantish: cd doesn't work with | or < operator\n");
      return -1;
    }
    if (redirect_to_file) {
      printf("will write to file\n");
      int saved_stdout = dup(STDOUT_FILENO); // Save terminal
      write_to_file(*command->operand, command->operater);

      int cd_result = change_directory(command->args[1]);

      dup2(saved_stdout, STDOUT_FILENO); // Restore terminal
      close(saved_stdout);               // Clean up saved fd
      return cd_result == 0 ? 1 : cd_result;
    }
    printf("found cd\n");

    if (change_directory(command->args[1]) == -1)
      return -1;

    return 1;
  } else if (strcmp("pwd", command->args[0]) == 0) {

    if (command->args[1]) {
      fprintf(stderr, "mantish: pwd doesn't take any argument\n");
      return -1;
    }

    if (not_stdin) {

      fprintf(stderr, "mantish: pwd doesn't work with | or < operator\n");
      return -1;
    }

    if (redirect_to_pipe) {
      printf("will write to pipe\n");
      *prev_pipe_read_end = current_pipe_fds[READ_END];
      printf("prev_pipe_read_end after mutation: %d\n", *prev_pipe_read_end);
      int pwd_result = print_working_directory(current_pipe_fds[WRITE_END]);
      return pwd_result == 0 ? 1 : pwd_result;
    }

    if (redirect_to_file) {
      printf("will write to file\n");
      int fd;
      if (strcmp(command->operater, operaters[WRITE]) == 0) {
        fd = open(*command->operand, O_WRONLY | O_CREAT | O_TRUNC, 0644);

      } else {
        fd = open(*command->operand, O_WRONLY | O_CREAT | O_APPEND);
      }

      int pwd_result = print_working_directory(fd);

      return pwd_result == 0 ? 1 : pwd_result;
    }
    printf("i am running pwd\n");

    print_working_directory(STDOUT_FILENO);
    return 1;
  } else if (strcmp("history", command->args[0]) == 0) {
    if (command->args[1]) {
      fprintf(stderr, "mantish: history doesn't take any argument\n");
      return -1;
    }

    if (not_stdin) {

      fprintf(stderr, "mantish: history doesn't work with | or < operator\n");
      return -1;
    }

    if (redirect_to_file) {
      printf("will write to file\n");
      int fd = built_in_to_file(command);
      print_history(fd);
      return 1;
    }

    if (redirect_to_pipe) {
      printf("will write to pipe\n");
      *prev_pipe_read_end = current_pipe_fds[READ_END];
      print_history(current_pipe_fds[WRITE_END]);

      return 1;
    }

    print_history(STDOUT_FILENO);
    return 1;

  } else if (strcmp("exit", command->args[0]) == 0) {
    printf("\e[32mending mantish....\e[0m\n");
    sleep(1);
    exit(EXIT_SUCCESS);
  }

  return 0;
}
