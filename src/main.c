#include "../include/debug_functions.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TOKEN_BUFF_SIZE 64
#define TOKEN_DELIMITER " \t\r\n\a"

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

  char *token = strtok(line, TOKEN_DELIMITER);
  int position = 0;
  while (token != NULL) {
    args[position] = token;

    if (position * sizeof(char *) >= buffer_size) {

      buffer_size += TOKEN_BUFF_SIZE;
      args = realloc(args, buffer_size * sizeof(char *));
      if (!args) {
        fprintf(stderr, "mantish: allocation error!");
        exit(EXIT_FAILURE);
      }
    }

    position++;
    token = strtok(NULL, TOKEN_DELIMITER);
  }

  return args;
}

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

void start_shell() {
  printf("starting mantish....\n");
  printf("starting mantish....\n");
  system("clear");
  bool keep_alive = true;

  char *line;
  char **args;

  while (keep_alive) {
    // printf("> ");
    printf("\e[32m~/projects/mantish >-{°°}-< \e[0m");

    fflush(stdout);
    line = read_command();
    args = parse_command(line);

    keep_alive = execute_command(args);

    free(line);
    free(args);
  }
}

int main() {

  start_shell();

  return EXIT_SUCCESS;
}
