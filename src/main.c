#include "../include/common.h"
#include "../include/debug_functions.h"
#include "../include/shell.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void start_shell() {
  printf("starting mantish....\n");
  // sleep(1);
  system("clear");
  bool keep_alive = true;

  char *line;
  char **args;
  command_t command = {0};

  while (keep_alive) {
    // printf("> ");
    printf("\e[32m%s >-{°°}-< \e[0m", getenv("PWD"));
    fflush(stdout);

    line = read_command();
    args = parse_command(line, &command);
    print_linked_list(&command);

    keep_alive = execute_command(args, &command);

    free(line);
    free(args);
  }
}

int main() {

  start_shell();

  return EXIT_SUCCESS;
}
