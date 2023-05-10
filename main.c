#include "config.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>

static const long space_divider = 1024 * 1024 * 1024;

int main(int argc, char *argv[]) {
  char *datetime = (char *)malloc(sizeof(char) * 50);
  char *free_space = (char *)malloc(sizeof(char) * 20);
  // size should be size of all components + num of components
  char *status = (char *)malloc(sizeof(char) * (70 + 2));

  while (1) {
    datetime_str(datetime);
    free_space_str(free_space);

    strcpy(status, free_space);
    strcat(status, datetime);

    char *update_cmd[] = {"/usr/bin/xsetroot", "-name", status, NULL};
    pid_t pid = fork();

    // Ignore dead childs so we won't create lots of zombies!
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
      perror("signal");
      exit(EXIT_FAILURE);
    }

    switch (pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      run(update_cmd);
      free(status);
      free(free_space);
      free(datetime);
      exit(EXIT_SUCCESS);
    default:
      sleep(INTERVAL);
    }
  }

  return 0;
}

void datetime_str(char *datetime) {
  time_t current_time = time(NULL);
  struct tm *tm = localtime(&current_time);

  strftime(datetime, 100, DATE_FORMAT, tm);
}

void free_space_str(char *fs_str) {
  struct statvfs fs_stats;
  if (statvfs(MOUNT_POINT, &fs_stats) == -1) {
    perror("statvfs");
    exit(EXIT_FAILURE);
  }
  // getting free space in bytes
  long free_space = fs_stats.f_bavail * fs_stats.f_bsize;
  snprintf(fs_str, 20, "%ld Gb%s", free_space / space_divider, DELIMITER);
}

void run(char *cmd[]) {
  char *env[] = {"DISPLAY=:0", NULL};

  execve(cmd[0], cmd, env);
  perror("execve");
}
