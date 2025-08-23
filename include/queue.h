#pragma once
#include "../include/common.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_queue();
void enqueue(char *item_to_add);
void print_queue();
void print_queue_reverse();
