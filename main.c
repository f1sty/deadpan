#include "config.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>

static const float divider = 1024 * 1024 * 1024;
static const short buf_len = 100;

int main(int argc, char *argv[]) {
  char *datetime = (char *)malloc(sizeof(char) * 50);
  char *free_space = (char *)malloc(sizeof(char) * 15);
  char *free_mem = (char *)malloc(sizeof(char) * 15);
  char *cpu_temprature = (char *)malloc(sizeof(char) * 15);
  // size should be size of all components + num of components
  char *status = (char *)malloc(sizeof(char) * (95 + 3));

  while (1) {
    datetime_str(datetime);
    free_space_str(free_space);
    free_mem_str(free_mem);
    cpu_temperature_str(cpu_temprature);

    strcpy(status, cpu_temprature);
    strcat(status, free_space);
    strcat(status, free_mem);
    strcat(status, datetime);

    char *status_cmd[] = {"/usr/bin/xsetroot", "-name", status, NULL};
    pid_t pid = fork();

    // ignore dead childs so we won't create lots of zombies!
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
      perror("signal");
      exit(EXIT_FAILURE);
    }

    switch (pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      run(status_cmd);

      free(status);
      free(cpu_temprature);
      free(free_mem);
      free(free_space);
      free(datetime);

      exit(EXIT_SUCCESS);
    default:
      sleep(INTERVAL);
    }
  }

  return 0;
}

void datetime_str(char *dt_str) {
  time_t current_time = time(NULL);
  struct tm *tm = localtime(&current_time);

  strftime(dt_str, 50, DATE_FORMAT, tm);
}

void free_space_str(char *fs_str) {
  struct statvfs fs_stats;
  if (statvfs(MOUNT_POINT, &fs_stats) == -1) {
    perror("statvfs");
    exit(EXIT_FAILURE);
  }
  // getting free space in Gb
  float free_space = (float)(fs_stats.f_bavail * fs_stats.f_bsize) / divider;
  snprintf(fs_str, 15, "%.1f Gb%s", free_space, DELIMITER);
}

void free_mem_str(char *fm_str) {
  // float free_mem =
  //     (float)(sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGE_SIZE)) / divider;
  FILE *mem_info = fopen("/proc/meminfo", "r");
  char buf[buf_len];
  unsigned long free_mem_kb;

  // reading until third line, which contains MemAvailable info
  for (int i = 0; i < 3; i++) {
    fgets(buf, buf_len, mem_info);
  }

  fclose(mem_info);

  sscanf(buf, "MemAvailable:%lu", &free_mem_kb);
  snprintf(fm_str, 15, "%.1f Gb%s", (float)free_mem_kb / (1024 * 1024),
           DELIMITER);
}

void cpu_temperature_str(char *ct_str) {
  float temp;
  char zone_path[buf_len];

  sprintf(zone_path, "/sys/class/thermal/thermal_zone%d/temp", THERMAL_ZONE);

  FILE *temp_info = fopen(zone_path, "r");
  fscanf(temp_info, "%f", &temp);
  fclose(temp_info);

  snprintf(ct_str, 15, "%.1fC°%s", temp / 1000, DELIMITER);
}

void run(char *cmd[]) {
  char *env[] = {"DISPLAY=:0", NULL};

  execve(cmd[0], cmd, env);
  perror("execve");
}
