#include "../include/operators.h"

extern char *operaters[];
bool check_operators(const char *token, command_t *command, char **tokens, int *token_position) {

  int ind = 0;
  while (operaters[ind]) {
    if (strcmp(token, operaters[ind]) == 0) {

      printf("operator token found: %s\n", token);
      command->operater = operaters[ind];
      if (strcmp(command->operater, operaters[PIPE]) == 0 || strcmp(command->operater, operaters[AND]) == 0 || strcmp(command->operater, operaters[OR]) == 0) {

        command->next_command = malloc(sizeof(command_t));
        command->next_command->args = &tokens[*token_position + 1];

      } else {
        command->operand = &tokens[*token_position + 1];
      }

      *token_position = *token_position + 1;
      return true;
    }
    ind++;
  }

  return false;
}
void read_from_file(char *file_name) {
  int fd = open(file_name, O_RDONLY);
  if (fd == -1) {

    fprintf(stderr, "mantish: the file you're program trying to read from \"%s\" doesn't exist\n", file_name);
    exit(EXIT_FAILURE);
  }

  dup2(fd, STDIN_FILENO);
  close(fd);
}

int built_in_to_file(command_t *command) {

  int fd;
  if (strcmp(command->operater, operaters[WRITE]) == 0) {
    fd = open(*command->operand, O_WRONLY | O_CREAT | O_TRUNC, 0644);

  } else {
    fd = open(*command->operand, O_WRONLY | O_CREAT | O_APPEND);
  }
  return fd;
}

void write_to_file(char *file_name, char *operator_type) {
  int fd;
  if (strcmp(operator_type, operaters[WRITE]) == 0) {
    fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);

  } else {
    fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND);
  }

  if (strcmp(operator_type, "2>") == 0) {

    dup2(fd, STDERR_FILENO);
  } else {
    dup2(fd, STDOUT_FILENO);
  }
  close(fd);
}
void write_to_pipe(enum pipe_channels current_pipe_fds[2]) {
  dup2(current_pipe_fds[WRITE_END], STDOUT_FILENO); // Redirect stdout to pipe's write end
  close(current_pipe_fds[WRITE_END]);               // Close unused read end
  close(current_pipe_fds[READ_END]);                // no need for original read end
}
