
#pragma once
#include "../include/operators.h"
#include "../include/utils.h"
#include "./common.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int execute_built_ins(command_t *command, int *prev_pipe_read_end,
                      enum pipe_channels current_pipe_fds[2]);

bool create_job(command_t *command, int process_id);
