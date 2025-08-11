#pragma once
#define TOKEN_BUFF_SIZE 64
#define TOKEN_DELIMITER " \t\r\n\a"

typedef struct command {
  char **args;
  char *operater;
  char **operand;
  struct command *next_command;
} command_t;

enum operator_type { PIPE, APPEND, WRITE, WRITE_ERR, READ, AND, OR };

enum pipe_channels {
  READ_END,
  WRITE_END,
};
