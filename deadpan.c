#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <time.h>

void music(char *);
void cpu_temperature(char *);
void battery_charge(char *);
void free_disk(char *);
void free_memory(char *);
void volume(char *);
void date_time(char *);

#include "config.h"

#define BUFFER_SIZE 128
#define WIDGETS_BUFFER_SIZE 896
#define STATUS_LINE_BUFFER_SIZE 1024
#define FREE_DISK_DIVIDER (1024 << 20)
#define FREE_RAM_DIVIDER (1024 << 10)

void delimiter(char *str) { strcat(str, DELIMITER); }

void add_widget(char *widgets, void(widget)(char *), bool add_delimiter) {
  widget(widgets);
  if (add_delimiter)
    delimiter(widgets);
}

void battery_charge(char *widgets) {
  char path[BUFFER_SIZE] = {0};
  snprintf(path, BUFFER_SIZE, "/sys/class/power_supply/BAT%d/capacity",
           BATTERY);

  char retval[BUFFER_SIZE] = {0};
  FILE *fd = fopen(path, "r");
  if (!fd) {
    perror("fopen");
    snprintf(retval, BUFFER_SIZE, NA_STRING);
  } else {
    fread(retval, sizeof(*retval), BUFFER_SIZE, fd);
    if (ferror(fd)) {
      fprintf(stderr, "fread error\n");
      exit(EXIT_FAILURE);
    };

    fclose(fd);
    strcat(retval, "%");
  };
  strcat(widgets, retval);
}

void date_time(char *widgets) {
  time_t current_time = time(NULL);
  struct tm *tm = localtime(&current_time);

  char retval[BUFFER_SIZE] = {0};
  strftime(retval, BUFFER_SIZE, DATE_FORMAT, tm);
  strcat(widgets, retval);
}

void free_disk(char *widgets) {
  char retval[BUFFER_SIZE] = {0};
  struct statvfs fs_stats;
  if (statvfs(MOUNT_POINT, &fs_stats) == -1) {
    perror("statvfs");
    snprintf(retval, BUFFER_SIZE, NA_STRING);
  } else {
    float free_space =
        (float)(fs_stats.f_bavail * fs_stats.f_bsize) / FREE_DISK_DIVIDER;

    snprintf(retval, BUFFER_SIZE, "%.1f Gb", free_space);
  };
  strcat(widgets, retval);
}

void free_memory(char *widgets) {
  char retval[BUFFER_SIZE] = {0};
  FILE *fd = fopen("/proc/meminfo", "r");
  if (!fd) {
    perror("fopen");
    snprintf(retval, BUFFER_SIZE, NA_STRING);
  } else {
    fread(retval, sizeof(*retval), BUFFER_SIZE, fd);
    if (ferror(fd)) {
      fprintf(stderr, "fread error\n");
      exit(EXIT_FAILURE);
    }

    fclose(fd);

    uint32_t free_mem_kb;
    char *needle = strtok(strstr(retval, "MemAvailable:"), "\n") + 13;
    sscanf(needle, "%u", &free_mem_kb);

    snprintf(retval, BUFFER_SIZE, "%.1f Gb",
             (float)free_mem_kb / FREE_RAM_DIVIDER);
  };
  strcat(widgets, retval);
}

void cpu_temperature(char *widgets) {
  char path[BUFFER_SIZE] = {0};
  switch (THERMAL_TYPE) {
    case 0:
      snprintf(path, BUFFER_SIZE, "/sys/class/thermal/thermal_zone%d/temp", THERMAL_ZONE);
      break;
    case 1:
      snprintf(path, BUFFER_SIZE, "/sys/class/hwmon/hwmon%d/temp1_input", THERMAL_ZONE);
      break;
    default:
      snprintf(path, BUFFER_SIZE, "/sys/class/thermal/thermal_zone%d/temp", THERMAL_ZONE);
  }

  char retval[BUFFER_SIZE] = {0};
  FILE *fd = fopen(path, "r");
  if (!fd) {
    perror("fopen");
    snprintf(retval, BUFFER_SIZE, NA_STRING);
  } else {
    uint32_t temperature;
    fscanf(fd, "%u", &temperature);
    fclose(fd);

    snprintf(retval, BUFFER_SIZE, "%u°C", (uint32_t)(temperature / 1000));
  };
  strcat(widgets, retval);
}

char *run_external(char *cmd) {
  FILE *fd = popen(cmd, "r");
  if (!fd) {
    perror("popen");
    exit(EXIT_FAILURE);
  }

  char retval[BUFFER_SIZE] = {0};
  fread(retval, sizeof(*retval), BUFFER_SIZE, fd);

  pclose(fd);

  return strdup(retval);
}

void volume(char *widgets) { strcat(widgets, run_external(VOLUME_CMD)); }

void music(char *widgets) { strcat(widgets, run_external(MUSIC_CMD)); }

int main(void) {
  char status_cmd[STATUS_LINE_BUFFER_SIZE] = {0};
  char widgets[WIDGETS_BUFFER_SIZE] = {0};
  struct timespec interval = {.tv_sec = INTERVAL, .tv_nsec = 0};

  while (true) {
    for (int i = 0; widgets_list[i].name != NULL; i++)
      add_widget(widgets, widgets_list[i].name, widgets_list[i].with_delimiter);
    // add_widget(widgets, music, true);
    // add_widget(widgets, cpu_temperature, true);
    // // add_widget(widgets, battery_charge, true);
    // add_widget(widgets, free_disk, true);
    // add_widget(widgets, free_memory, true);
    // add_widget(widgets, volume, true);
    // add_widget(widgets, date_time, false);

    snprintf(status_cmd, STATUS_LINE_BUFFER_SIZE, "xsetroot -name \"%s\"",
             widgets);

    if (system(status_cmd) != 0) {
      perror("system(cmd)");
      break;
    }

    memset(widgets, 0, WIDGETS_BUFFER_SIZE);
    nanosleep(&interval, NULL);
  }
  return 0;
}
