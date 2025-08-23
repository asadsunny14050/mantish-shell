#include "../include/common.h"
#include "../include/debug.h"
#include "../include/queue.h"
#include "../include/shell.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Session shell_session;

void init_session() {

  printf("\e[32mstarting mantish....\e[0m\n");
  strncpy(shell_session.current_directory, getenv("PWD"), DIR_SIZE);
  init_queue();
  // sleep(1);
  system("clear");
}

void start_shell() {

  init_session();

  const char *art =
      "\x1b[32m"
      "⠀⣤⡀⠀⠀⠀⠀⠀⠀⣤⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
      "⠀⠘⢷⡄⠀⠀⠀⠀⢰⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
      "⠀⠀⠈⠻⠆⢀⣀⣀⣈⣁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
      "⠀⠀⠀⢀⣤⣈⠙⢿⣿⣿⣷⠘⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
      "⠀⠀⠀⢹⣿⣿⡇⠘⣿⣿⣿⡆⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
      "⠀⢀⣠⣄⡉⠛⠁⠼⣿⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠀⡄⠀⠀⠀⠀⠀\n"
      "⠀⣿⣿⡟⢁⠀⠀⠀⠀⠙⠛⠿⡆⠸⣦⠀⠀⠀⠀⠀⢀⠞⠋⣀⡤⡀⠀⠀⠀⠀\n"
      "⠀⡿⠋⣀⠘⢷⣄⠀⠀⢹⡇⠀⠀⠀⠈⠆⠀⠀⠀⠀⣠⣴⣿⣿⢁⡇⠀⠀⠀⠀\n"
      "⠀⠀⠀⢹⣷⡌⠻⣦⡀⠀⠙⠀⠀⠀⠀⠀⠀⠀⣠⣾⣿⣿⡿⠁⢸⡇⢸⡆⠀⠀\n"
      "⠀⠀⠀⠀⢿⣿⣆⠙⢿⣆⠀⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⠟⠁⠀⣾⡇⢸⣿⡄⠀\n"
      "⠀⠀⠀⠀⠈⢿⣿⣧⠈⢿⣷⡀⠀⠀⢀⣴⣿⣿⣿⡿⠃⠀⠀⢰⣿⡇⠘⠻⣷⠀\n"
      "⠀⠀⠀⠀⠀⠀⠻⣿⣷⡀⠻⣿⠂⣠⣿⣿⣿⣿⠟⠀⠀⠀⠀⣼⣿⠃⠀⢀⣿⠀\n"
      "⠀⠀⠀⠀⠀⠀⠀⠙⢿⣷⡄⠀⣴⣿⣿⣿⠟⠁⠀⠀⠀⠀⢀⣼⠋⠀⠀⠸⡇⠀\n"
      "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠃⣸⣿⣿⠟⢁⡴⠂⠀⠀⠀⠴⠛⠁⠀⠀⠀⠀⠀⠀\n"
      "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠋⠀⠀⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
      "\x1b[0m";

  fputs(art, stdout);
  bool keep_alive = true;

  char *line;
  char **args;
  command_t command = {0};

  while (keep_alive) {
    printf("\e[32m⾕%s >-{°°}-< \e[0m", shell_session.current_directory);
    fflush(stdout);

    line = read_command();
    args = parse_command(line, &command);
    print_linked_list(&command);

    keep_alive = execute_command(&command);

    command_t *curr_cmd = command.next_command;
    while (curr_cmd) {
      command_t *prev_cmd = curr_cmd;
      curr_cmd = curr_cmd->next_command;
      free(prev_cmd);
    }

    free(line);
    free(args);
  }
}

int main() {

  start_shell();

  return EXIT_SUCCESS;
}
