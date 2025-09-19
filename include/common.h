#pragma once
#include <stdbool.h>
#include <stddef.h>
#define TOKEN_BUFF_SIZE 64
#define TOKEN_DELIMITER " \t\r\n\a"
#define DIR_SIZE 1024
#define CMD_SIZE 1024
#define JOBS_LIMIT 64
#define CAPACITY 200
#define MAX_BRANCH_NAME_LEN 256

typedef struct {
  int head;
  int tail;
  char *items[CAPACITY];
} Queue;

enum job_status { running, finished };

typedef struct {
  char *name;
  char status[20];
  int process_id;
} Jobs;

typedef struct {
  char current_directory[DIR_SIZE];
  bool inside_home_directory;
  char *directory_offset;
  char previous_directory[DIR_SIZE];
  char git_branch[MAX_BRANCH_NAME_LEN];
  Queue history_list;
  Jobs jobs[JOBS_LIMIT];
  size_t jobs_count;
} Session;

typedef struct command {
  char **args;
  char *operater;
  char **operand;
  struct command *next_command;
} command_t;

enum operator_type { PIPE, APPEND, WRITE, WRITE_ERR, READ, AND, OR, JOB };

enum pipe_channels {
  READ_END,
  WRITE_END,
};
