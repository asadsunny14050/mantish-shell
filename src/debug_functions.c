#include "../include/debug_functions.h"

void print_string_array(char **array_in_question, size_t size) {
  for (int i = 0; i < size; i++) {
    printf("i%d, %s\n", i, array_in_question[i]);
  }
}

void print_linked_list(command_t *head) {
  command_t *iterator = head;
  while (iterator) {
    printf("(args: %p, %s, operator: %s, next_command: %p) -> ", iterator->args, iterator->args[0], iterator->operater, iterator->next_command);
    iterator = iterator->next_command;
  }
  printf("\n");
}
