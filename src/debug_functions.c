#include "../include/debug_functions.h"

void print_string_array(char **array_in_question, size_t size) {
  for (int i = 0; i < size; i++) {
    printf("i%d, %s\n", i, array_in_question[i]);
  }
}
