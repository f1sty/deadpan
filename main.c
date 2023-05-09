#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  while (1) {
    char *dt = datetime_str();
    char *update_cmd[] = {"/usr/bin/xsetroot", "-name", dt, NULL};
    pid_t pid = fork();

    switch (pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      run(update_cmd);
      free(dt);
      exit(EXIT_SUCCESS);
    default:
      free(dt);
      sleep(INTERVAL);
    }
  }

  return 0;
}

char *datetime_str() {
  char *buf = (char *)malloc(sizeof(char) * 100);
  time_t current_time = time(NULL);
  struct tm *tm = gmtime(&current_time);

  strftime(buf, 99, DATE_FORMAT, tm);

  return buf;
}

void run(char *cmd[]) {
  char *env[] = {"DISPLAY=:0", NULL};

  execve(cmd[0], cmd, env);
  perror("execve");
}
