#include "../include/queue.h"

extern Session shell_session;

void init_queue() {

  shell_session.history_list.head = 0;
  shell_session.history_list.tail = -1;

  char *big_buffer = (char *)malloc(CAPACITY * CMD_SIZE);
  memset(big_buffer, 0, CAPACITY * CMD_SIZE);

  for (int i = 0; i < CAPACITY; i++) {
    shell_session.history_list.items[i] = big_buffer + (i * CMD_SIZE);
  }
}
void enqueue(char *item_to_add) {
  printf("checkpoint 1");
  if (shell_session.history_list.tail + 1 == CAPACITY) {
    shell_session.history_list.tail = -1;
  }
  char *add_index = shell_session.history_list.items[shell_session.history_list.tail + 1];
  memset(add_index, 0, CMD_SIZE);
  strncpy(add_index, item_to_add, CMD_SIZE);
  printf("checkpoint 1");

  shell_session.history_list.tail++;

  if (shell_session.history_list.head == shell_session.history_list.tail && shell_session.history_list.items[shell_session.history_list.head + 1][0]) {
    printf("incrementing head\n");
    shell_session.history_list.head++;
  }
  if (shell_session.history_list.head == CAPACITY) {
    shell_session.history_list.head = 0;
  }
}

void print_queue() {
  for (int i = 0; i < 5; i++) {
    printf("ind:%d, %s\n", i, shell_session.history_list.items[i]);
  }
}
void print_queue_reverse() {
  printf("printing in reverse\n");
  int i = shell_session.history_list.tail;
  while (i != shell_session.history_list.head) {
    if (i == -1) {
      i = 4;
    }
    printf("ind:%d, %s\n", i, shell_session.history_list.items[i]);
    i--;
  }
  if (i == shell_session.history_list.head) {

    printf("ind:%d, %s\n", i, shell_session.history_list.items[i]);
  }
}
