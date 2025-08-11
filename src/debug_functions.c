#include "../include/debug_functions.h"

void print_string_array(char **array_in_question, size_t size) {
  for (size_t i = 0; i < size; i++) {
    printf("i%zu, %p, %s\n", i, &array_in_question[i], array_in_question[i]);
  }
}

void print_linked_list(command_t *head) {
  command_t *iterator = head;
  while (iterator) {

    printf("(args: %p, %s, operator: %s, operand: %p, next_command: %p) -> ", iterator->args, iterator->args[0], iterator->operater, iterator->operand, iterator->next_command);
    iterator = iterator->next_command;
  }
  printf("\n");
}
