#pragma once
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>

void invalidate_current_directory();
void clean_up_fds(int *prev_pipe_read_end,
                  enum pipe_channels current_pipe_fds[2]);
bool set_run_permit(command_t *command, bool built_in_result, bool *run_permit);
void free_resources(command_t *command);
