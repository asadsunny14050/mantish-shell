#pragma once

#include "./common.h"
#include <stdbool.h>

char *read_command();
char **parse_command(char *line, command_t *command);
bool execute_command(char **args, command_t *command);
