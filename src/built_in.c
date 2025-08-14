#include "../include/built_in.h"
#include "../include/utils.h"

char *built_in_commands[] = {"cd", "pwd", "history", "exit"};

extern Session shell_session;

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
  bool redirection = command->operater && (strcmp(command->operater, operaters[WRITE]) == 0 || strcmp(command->operater, operaters[APPEND]) == 0 || strcmp(command->operater, operaters[WRITE_ERR]) == 0);

  bool write_to_pipe = command->next_command && (strcmp(command->next_command->args[0], operaters[PIPE]));

  bool not_stdin = command->next_command && strcmp(command->next_command->args[0], operaters[READ]) == 0;

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

    if (write_to_pipe || not_stdin) {

      fprintf(stderr, "mantish: cd doesn't work with | or < operator\n");
      return -1;
    }
    if (redirection) {
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
  }
  // else if (strcmp("pwd", command->args[0]) == 0) {
  //   if (command->args[1]) {
  //     fprintf(stderr, "mantish: pwd doesn't take any argument\n");
  //     return -1;
  //   }
  // }

  return 0;
}
