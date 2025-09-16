
#include "../include/utils.h"
extern Session shell_session;

void invalidate_current_directory() {

  char *current_directory = shell_session.current_directory;
  char *second_slash = strchr(current_directory + 1, '/');
  int num_of_characters = second_slash && ((second_slash - 1) - current_directory);
  if (num_of_characters && strncmp(current_directory, "/home", num_of_characters) == 0) {
    shell_session.inside_home_directory = true;
    char *third_slash = strchr(second_slash + 1, '/');
    if (third_slash) {
      shell_session.directory_offset = third_slash;
    } else {

      shell_session.directory_offset = 0;
    }
  } else {
    printf("now i'm outside home\n");
    shell_session.inside_home_directory = false;
    shell_session.directory_offset = shell_session.current_directory;
  }
}

void free_resources(command_t *command) {
  command_t *current_cmd = command->next_command;

  while (current_cmd) {
    command_t *next_cmd = current_cmd->next_command;
    free(current_cmd);
    current_cmd = next_cmd;
  }
}

void clean_up_fds(int *prev_pipe_read_end, enum pipe_channels current_pipe_fds[2]) {

  if (*prev_pipe_read_end != STDIN_FILENO) {
    close(*prev_pipe_read_end);
    *prev_pipe_read_end = STDIN_FILENO; // Reset for next call
  }
  if ((int)current_pipe_fds[READ_END] != -1) {
    close(current_pipe_fds[READ_END]);
    current_pipe_fds[READ_END] = -1;
  }
  if ((int)current_pipe_fds[WRITE_END] != -1) {
    close(current_pipe_fds[WRITE_END]);
    current_pipe_fds[WRITE_END] = -1;
  }
  // fflush(stdin); // Non-standard way to clear the buffer
}

extern char *operaters[];
bool set_run_permit(command_t *command, bool built_in_result, bool *run_permit) {
  if (!command->operater) {
    return false;
  }

  if (strcmp(command->operater, operaters[AND]) == 0) {
    *run_permit = built_in_result == 1 ? true : false;
    return true;
  }

  if (strcmp(command->operater, operaters[OR]) == 0) {
    *run_permit = built_in_result == 1 ? false : true;
    return true;
  }

  return false;
}
