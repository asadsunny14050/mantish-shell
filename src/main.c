#include "../include/common.h"
#include "../include/debug.h"
#include "../include/queue.h"
#include "../include/shell.h"
#include "../include/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Session shell_session;

void get_git_branch() {
  char *branch_name = shell_session.git_branch;

  FILE *fp;
  char command[MAX_BRANCH_NAME_LEN];

  // Construct the command to get the current Git branch
  snprintf(command, sizeof(command), "git -C %s rev-parse --abbrev-ref HEAD", "."); // Use "." for current directory

  // Execute the command and open a pipe to its output
  fp = popen(command, "r");
  if (fp == NULL) {
    perror("Failed to run command");
    return;
  }

  // Read the branch name from the pipe
  if (fgets(branch_name, MAX_BRANCH_NAME_LEN, fp) != NULL) {
    // Remove trailing newline character if present
    branch_name[strcspn(branch_name, "\n")] = 0;
  } else {
    memset(branch_name, 0, MAX_BRANCH_NAME_LEN);
  }

  // Close the pipe
  pclose(fp);
}

void show_command_prompt() {

  if (shell_session.inside_home_directory) {
    printf("\e[32m⾕\e[0m");
  }

  if (shell_session.directory_offset) {
    printf("\e[32m%s\e[0m", shell_session.directory_offset);
  }

  get_git_branch();

  if (shell_session.git_branch[0]) {
    printf("\e[33m [ %s]\e[0m", shell_session.git_branch);
  }

  printf("\e[32m >-{°°}-< \e[0m");
  fflush(stdout);
}

void init_session() {

  printf("\e[32mstarting mantish....\e[0m\n");
  strncpy(shell_session.current_directory, getenv("PWD"), DIR_SIZE);
  shell_session.directory_offset = shell_session.current_directory;
  invalidate_current_directory();
  init_queue(); // history list
  // sleep(1);
  // system("clear");
}

void start_shell() {

  init_session();

  const char *mantis_art =
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
  fputs(mantis_art, stdout);
  bool keep_alive = true;

  char *line;
  char **args;
  command_t command = {0};

  while (keep_alive) {

    show_command_prompt();
    size_t line_buff_size = 0;
    line = read_command(&line_buff_size);
    args = parse_command(line, &command);
    print_linked_list(&command);

    keep_alive = execute_command(&command);

    command_t *curr_cmd = command.next_command;
    while (curr_cmd) {
      command_t *prev_cmd = curr_cmd;
      curr_cmd = curr_cmd->next_command;
      free(prev_cmd);
    }

    memset(line, 0, line_buff_size);
    free(line);
    free(args);
  }
}

int main() {
  start_shell();
  return EXIT_SUCCESS;
}
