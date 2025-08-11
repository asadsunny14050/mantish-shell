#pragma once
#include "../include/common.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
bool check_operators(const char *token, command_t *command, char **tokens,
                     int *token_position);

void read_from_file(char *file_name);
void write_to_file(char *file_name, char *operator_type);
void write_to_pipe(enum pipe_channels current_pipe_fds[2]);
