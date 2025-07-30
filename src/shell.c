#include "../include/shell.h"
#include "../include/common.h"
#include "../include/debug_functions.h"
#include "../include/parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

char *read_command() {
  char *line;
  size_t buff_size = 0;
  if (getline(&line, &buff_size, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);
    }
    perror("readline");
    exit(EXIT_FAILURE);
  }
  printf("%s\n", line);
  return line;
}

char **parse_command(char *line) {

  size_t buffer_size = TOKEN_BUFF_SIZE;
  char **args = malloc(buffer_size * sizeof(char *));
  if (!args) {
    fprintf(stderr, "mantish: allocation error!");
    exit(EXIT_FAILURE);
  }

  if (!line) {
    fprintf(stderr, "mantish: empty command!");
    exit(EXIT_FAILURE);
  }

  char *token;
  char *rest = line;
  int position = 0;
  while ((token = strtok_r(rest, TOKEN_DELIMITER, &rest)) && rest) {
    printf("token: %s\n", token);
    printf("rest: %s\n", rest);
    args[position] = token;
    position++;

    handle_quotes(&rest, args, &position);

    if (position * sizeof(char *) >= buffer_size) {

      buffer_size += TOKEN_BUFF_SIZE;
      args = realloc(args, buffer_size * sizeof(char *));
      if (!args) {
        fprintf(stderr, "mantish: allocation error!");
        exit(EXIT_FAILURE);
      }
    }
  }

  print_string_array(args, 10);

  return args;
}

bool execute_command(char **args) {

  pid_t pid, wpid;
  int process_status;

  pid = fork();

  if (pid < 0) {
    fprintf(stderr, "mantish: error forking process");
  }

  if (pid == 0) {
    // child process block
    if (execvp(args[0], args) == -1) {
      fprintf(stderr, "mantish: either the program \"%s\" doesn't exist or an internal error occured\n", args[0]);
    }
    exit(EXIT_FAILURE);

  } else {
    // parent process block
    do {
      wpid = waitpid(pid, &process_status, WUNTRACED);
    } while (!WIFEXITED(process_status) && !WIFSIGNALED(process_status));
  }

  return true;
}
