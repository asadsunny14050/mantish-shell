
#pragma once

#include "../include/operators.h"
#include "./common.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int execute_built_ins(command_t *command, int *prev_pipe_read_end,
                      enum pipe_channels current_pipe_fds[2]);
