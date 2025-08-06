#pragma once
#define TOKEN_BUFF_SIZE 64
#define TOKEN_DELIMITER " \t\r\n\a"

typedef struct command {
  char **args;
  char *operater;
  struct command *next_command;
} command_t;
