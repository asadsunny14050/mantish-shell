#pragma once

#include "./common.h"
#include <stdbool.h>

char *read_command(size_t *buff_size);
char **parse_command(char *line, command_t *command);
bool execute_command(command_t *command);
