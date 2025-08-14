
#include "../include/utils.h"

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
