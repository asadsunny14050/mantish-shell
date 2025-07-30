#include "../include/shell.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void start_shell() {
  printf("starting mantish....\n");
  sleep(1);
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
