#pragma once

#include "../include/common.h"

void clean_up_fds(int *prev_pipe_read_end,
                  enum pipe_channels current_pipe_fds[2]);
