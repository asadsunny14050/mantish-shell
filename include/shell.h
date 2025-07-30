#pragma once

#include <stdbool.h>

char *read_command();
char **parse_command(char *line);
bool execute_command(char **args);
