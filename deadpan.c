#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>

#define DIVIDER (1024 * 1024 * 1024)
#define BUFFER_SIZE 128

void add_widget(char *widgets, void(widget)(char *), bool add_delimiter) {
  widget(widgets);
  if (add_delimiter)
    delimiter(widgets);
}

void date_time(char *widgets) {
  char retval[BUFFER_SIZE] = {0};
  time_t current_time = time(NULL);
  struct tm *tm = localtime(&current_time);

  strftime(retval, BUFFER_SIZE, DATE_FORMAT, tm);
  strcat(widgets, retval);
}

void free_space(char *widgets) {
  char retval[BUFFER_SIZE] = {0};
  struct statvfs fs_stats;
  if (statvfs(MOUNT_POINT, &fs_stats) == -1) {
    perror("statvfs");
    exit(EXIT_FAILURE);
  }
  // getting free space in Gb
  float free_space = (float)(fs_stats.f_bavail * fs_stats.f_bsize) / DIVIDER;
  snprintf(retval, 10, "%.1f Gb", free_space);
  strcat(widgets, retval);
}

void free_memory(char *widgets) {
  char retval[BUFFER_SIZE] = {0};
  // float free_mem =
  //     (float)(sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGE_SIZE)) / divider;
  FILE *mem_info = fopen("/proc/meminfo", "r");
  char buf[BUFFER_SIZE] = {0};
  unsigned long free_mem_kb;

  // reading until third line, which contains MemAvailable info
  for (int i = 0; i < 3; i++) {
    fgets(buf, BUFFER_SIZE, mem_info);
  }

  fclose(mem_info);

  sscanf(buf, "MemAvailable:%lu", &free_mem_kb);
  snprintf(retval, BUFFER_SIZE, "%.1f Gb", (float)free_mem_kb / (1024 * 1024));
  strcat(widgets, retval);
}

void cpu_temperature(char *widgets) {
  float temperature;
  char zone_path[BUFFER_SIZE] = {0};
  char retval[BUFFER_SIZE] = {0};

  snprintf(zone_path, BUFFER_SIZE, "/sys/class/thermal/thermal_zone%d/temp",
           THERMAL_ZONE);

  FILE *fp;
  if ((fp = fopen(zone_path, "r")) == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  fscanf(fp, "%f", &temperature);
  if (fclose(fp)) {
    perror("fclose");
    exit(EXIT_FAILURE);
  }

  snprintf(retval, BUFFER_SIZE, "%.1f°C", temperature / 1000);
  strcat(widgets, retval);
}

char *run_external(char *cmd) {
  char retval[BUFFER_SIZE] = {0};
  FILE *fp;

  if ((fp = popen(cmd, "r")) == NULL) {
    perror("popen");
    exit(EXIT_FAILURE);
  }

  fread(retval, BUFFER_SIZE, sizeof(char), fp);

  if (pclose(fp) == -1) {
    perror("pclose");
    exit(EXIT_FAILURE);
  }
  return strdup(retval);
}

void volume(char *widgets) {
  strcat(widgets, run_external(VOLUME_CMD));
}

void music(char *widgets) {
  strcat(widgets, run_external(MUSIC_CMD));
}

void delimiter(char *str) { strcat(str, DELIMITER); }

int main(void) {
  char status_cmd[1024] = {0};
  char widgets[768] = {0};

  while (true) {
    add_widget(widgets, music, true);
    add_widget(widgets, cpu_temperature, true);
    add_widget(widgets, free_space, true);
    add_widget(widgets, free_memory, true);
    add_widget(widgets, volume, true);
    add_widget(widgets, date_time, false);

    snprintf(status_cmd, 1024, "xsetroot -name \"%s\"", widgets);

    if (system(status_cmd) != 0) {
      perror("system(cmd)");
      break;
    }

    memset(widgets, 0, 768);
    sleep(INTERVAL);
  }
  return 0;
}
