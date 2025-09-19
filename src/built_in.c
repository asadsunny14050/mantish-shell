#include "../include/built_in.h"
#include "../include/utils.h"

extern Session shell_session;

bool create_job(command_t *command, int process_id) {
  size_t job_count = shell_session.jobs_count;
  shell_session.jobs_count += 1;

  if (job_count == JOBS_LIMIT)
    return false;

  shell_session.jobs[job_count].name = (char *)malloc(CMD_SIZE);
  memset(shell_session.jobs[job_count].name, 0, CMD_SIZE);

  strncpy(shell_session.jobs[job_count].name, command->args[0], CMD_SIZE);
  strncpy(shell_session.jobs[job_count].status, "running", 20);
  shell_session.jobs[job_count].process_id = process_id;

  printf("[%lu] %s %s\n", job_count, shell_session.jobs[job_count].name, shell_session.jobs[job_count].status);

  return true;
}

int wait_for_job(int job_count) {
  printf("waiting......");
  int job_index = job_count - 1;
  printf("\e[32m%s\e[0m", shell_session.jobs[job_index].name);
  int process_status;
  waitpid(shell_session.jobs[job_index].process_id, &process_status, 0); // Blocking wait for child
  if (WIFEXITED(process_status)) {
    int exit_code = WEXITSTATUS(process_status); // Get actual exit code
    if (exit_code == 0) {
      return 1;
    } else {
      return -1;
    }
  }
  return -1;
}

void remove_job(int job_index) {
  free(shell_session.jobs[job_index].name);
  shell_session.jobs[job_index].name = NULL;
  memset(shell_session.jobs[job_index].status, 0, 20);
  shell_session.jobs[job_index].process_id = 0;
  shell_session.jobs_count--;
}

void print_jobs(int output_fd) {
  bool print_to_console = output_fd == STDOUT_FILENO;
  if (print_to_console)
    dprintf(output_fd, "\e[32m");

  dprintf(output_fd, "------------------------------------------------\n");
  dprintf(output_fd, "[Jobs List]\n");
  dprintf(output_fd, "------------------------------------------------\n");
  for (size_t i = 0; i < shell_session.jobs_count; i++) {
    int status;
    waitpid(shell_session.jobs[i].process_id, &status, WNOHANG);

    if (WIFEXITED(status))
      strncpy(shell_session.jobs[i].status, "finished", 20);

    dprintf(output_fd, "%-6zu│   ", i + 1);
    dprintf(output_fd, "%-6d│ %-6s│ %6s\n", shell_session.jobs[i].process_id, shell_session.jobs[i].name, shell_session.jobs[i].status);

    if (WIFEXITED(status))
      remove_job(i);
  }

  if (shell_session.jobs_count == 0) {

    dprintf(output_fd, "[ EMPTY ]\n");
  }
  dprintf(output_fd, "------------------------------------------------\n");

  if (print_to_console)
    dprintf(output_fd, "\e[0m");

  if (!print_to_console) {
    close(output_fd);
  }
}

void print_history(int output_fd) {
  int head = shell_session.history_list.head;
  int tail = shell_session.history_list.tail;
  int index = head;
  int count = 1;
  bool print_to_console = output_fd == STDOUT_FILENO;

  if (print_to_console)
    dprintf(output_fd, "\e[32m");

  dprintf(output_fd, "------------------------------------------------\n");
  dprintf(output_fd, "[Commands History]\n");
  dprintf(output_fd, "------------------------------------------------\n");
  do {
    dprintf(output_fd, "%-6i│   ", count);
    dprintf(output_fd, "%s", shell_session.history_list.items[index]);
    index = (index + 1) % CAPACITY;
    count++;

  } while (index != (tail + 1) % CAPACITY);
  dprintf(output_fd, "------------------------------------------------\n");

  if (print_to_console)
    dprintf(output_fd, "\e[0m");

  if (!print_to_console) {
    close(output_fd);
  }
}

int print_working_directory(int output_fd) {
  char working_directory[DIR_SIZE];
  getcwd(working_directory, DIR_SIZE);
  if (output_fd != STDOUT_FILENO) {
    dprintf(output_fd, "Working Directory: %s\n", working_directory);
    close(output_fd);
    return 0;
  }

  printf("Working Directory: %s\n", working_directory);
  return 0;
}

int change_directory(char *path_name) {
  printf("running change directory\n");
  printf("path_name: %s\n", path_name);

  if (strcmp(path_name, "-") == 0) {
    path_name = shell_session.previous_directory;

  } else if (strcmp(path_name, "..") == 0) {
    printf("going back up a directory\n");
    char buffer[DIR_SIZE];
    strncpy(buffer, shell_session.current_directory, DIR_SIZE);
    char *last_slash = strrchr(buffer, '/');
    *last_slash = 0;
    path_name = buffer;
  }

  int result = chdir(path_name);

  if (result == -1) {
    fprintf(stderr, "mantish: directory \"%s\" doesn't exist\n", path_name);
    path_name = NULL;
    return -1;
  }

  strncpy(shell_session.previous_directory, shell_session.current_directory, DIR_SIZE);
  getcwd(shell_session.current_directory, DIR_SIZE);
  invalidate_current_directory();
  printf("shell_session.current_directory: %s\n", shell_session.current_directory);
  return 0;
}

extern char *operaters[];

int execute_built_ins(command_t *command, int *prev_pipe_read_end, enum pipe_channels current_pipe_fds[2]) {
  bool has_next_command = command->next_command;

  bool redirect_to_file = command->operater && (strcmp(command->operater, operaters[WRITE]) == 0 || strcmp(command->operater, operaters[APPEND]) == 0 || strcmp(command->operater, operaters[WRITE_ERR]) == 0);

  bool redirect_to_pipe = has_next_command && (strcmp(command->operater, operaters[PIPE]) == 0);

  bool not_stdin = command->operater && (strcmp(command->operater, operaters[READ]) == 0);

  if (strcmp("cd", command->args[0]) == 0) {
    clean_up_fds(prev_pipe_read_end, current_pipe_fds);
    if (!command->args[1]) {

      fprintf(stderr, "mantish: cd needs a directory path to go to\n");
      return -1;
    }

    if (command->args[2]) {

      fprintf(stderr, "mantish: cd received more than one argument\n");
      return -1;
    }

    if (redirect_to_pipe || not_stdin) {

      fprintf(stderr, "mantish: cd doesn't work with | or < operator\n");
      return -1;
    }
    if (redirect_to_file) {
      printf("will write to file\n");
      int saved_stdout = dup(STDOUT_FILENO); // Save terminal
      write_to_file(*command->operand, command->operater);

      int cd_result = change_directory(command->args[1]);

      dup2(saved_stdout, STDOUT_FILENO); // Restore terminal
      close(saved_stdout);               // Clean up saved fd
      return cd_result == 0 ? 1 : cd_result;
    }
    printf("found cd\n");

    if (change_directory(command->args[1]) == -1)
      return -1;

    return 1;
  } else if (strcmp("pwd", command->args[0]) == 0) {

    if (command->args[1]) {
      fprintf(stderr, "mantish: pwd doesn't take any argument\n");
      return -1;
    }

    if (not_stdin) {

      fprintf(stderr, "mantish: pwd doesn't work with | or < operator\n");
      return -1;
    }

    if (redirect_to_pipe) {
      printf("will write to pipe\n");
      *prev_pipe_read_end = current_pipe_fds[READ_END];
      printf("prev_pipe_read_end after mutation: %d\n", *prev_pipe_read_end);
      int pwd_result = print_working_directory(current_pipe_fds[WRITE_END]);
      return pwd_result == 0 ? 1 : pwd_result;
    }

    if (redirect_to_file) {
      printf("will write to file\n");
      int fd;
      if (strcmp(command->operater, operaters[WRITE]) == 0) {
        fd = open(*command->operand, O_WRONLY | O_CREAT | O_TRUNC, 0644);

      } else {
        fd = open(*command->operand, O_WRONLY | O_CREAT | O_APPEND);
      }

      int pwd_result = print_working_directory(fd);

      return pwd_result == 0 ? 1 : pwd_result;
    }
    printf("i am running pwd\n");

    print_working_directory(STDOUT_FILENO);
    return 1;
  } else if (strcmp("history", command->args[0]) == 0) {
    if (command->args[1]) {
      fprintf(stderr, "mantish: history doesn't take any argument\n");
      return -1;
    }

    if (not_stdin) {

      fprintf(stderr, "mantish: history doesn't work with | or < operator\n");
      return -1;
    }

    if (redirect_to_file) {
      printf("will write to file\n");
      int fd = built_in_to_file(command);
      print_history(fd);
      return 1;
    }

    if (redirect_to_pipe) {
      printf("will write to pipe\n");
      *prev_pipe_read_end = current_pipe_fds[READ_END];
      print_history(current_pipe_fds[WRITE_END]);

      return 1;
    }

    print_history(STDOUT_FILENO);
    return 1;

  } else if (strcmp("jobs", command->args[0]) == 0) {
    if (command->args[1]) {
      fprintf(stderr, "mantish: jobs doesn't take any argument\n");
      return -1;
    }

    if (not_stdin) {

      fprintf(stderr, "mantish: jobs doesn't work with | or < operator\n");
      return -1;
    }

    if (redirect_to_file) {
      printf("will write to file\n");
      int fd = built_in_to_file(command);
      print_jobs(fd);
      return 1;
    }

    if (redirect_to_pipe) {
      printf("will write to pipe\n");
      *prev_pipe_read_end = current_pipe_fds[READ_END];
      print_jobs(current_pipe_fds[WRITE_END]);

      return 1;
    }

    print_jobs(STDOUT_FILENO);
    return 1;

  } else if (strcmp("fg", command->args[0]) == 0) {

    if (not_stdin) {
      fprintf(stderr, "mantish: 'fg' doesn't work with | or < operator\n");
      return -1;
    }

    if (redirect_to_file || redirect_to_pipe) {
      fprintf(stderr, "mantish: 'fg' doesn't redirect to to file or pipe'\n");
      return -1;
    }

    size_t job_count = shell_session.jobs_count;
    if (command->args[1]) {
      if (!is_integer(command->args[1])) {
        fprintf(stderr, "mantish: fg takes an integer\n");
        return -1;
      }

      job_count = atoi(command->args[1]);
      if (job_count <= 0 || job_count > shell_session.jobs_count) {
        fprintf(stderr, "mantish: job doesn't exit\n");
        return -1;
      }
    }

    return wait_for_job(job_count);

  } else if (strcmp("exit", command->args[0]) == 0) {
    printf("\e[32mending mantish....\e[0m\n");
    sleep(1);
    exit(EXIT_SUCCESS);
  }

  return 0;
}
