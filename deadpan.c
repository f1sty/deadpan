#include "config.h"
/* #include <signal.h> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
/* #include <sys/wait.h> */
#include <time.h>
#include <unistd.h>

#define DIVIDER (1024 * 1024 * 1024)
#define BUF_SIZE 50

int main(void) {
  /* struct sigaction sa; */
  /* memset(&sa, 0, sizeof(struct sigaction)); */
  /* sa.sa_handler = SIG_DFL; */
  /* sa.sa_flags = SA_NOCLDWAIT; */

  /* sigaction(SIGCHLD, &sa, NULL); */

  char *date_time = calloc(33, sizeof(char));
  char *free_space = calloc(10, sizeof(char));
  char *free_memory = calloc(10, sizeof(char));
  char *cpu_temperature = calloc(10, sizeof(char));
  char *volume = calloc(10, sizeof(char));
  char *status = calloc(100, sizeof(char));

  while (1) {
    cpu_temperature_str(cpu_temperature);
    free_space_str(free_space);
    free_memory_str(free_memory);
    volume_str(volume);
    date_time_str(date_time);

    strcpy(status, cpu_temperature);
    delimiter(status);
    strcat(status, free_space);
    delimiter(status);
    strcat(status, free_memory);
    delimiter(status);
    strcat(status, volume);
    delimiter(status);
    strcat(status, date_time);

    char status_cmd[1024];
    snprintf(status_cmd, sizeof(status_cmd), "xsetroot -name \"%s\"", status);

    if(system(status_cmd) != 0) {
      perror("system(cmd)");
      break;
    }

    sleep(INTERVAL);
  }

  free(cpu_temperature);
  free(free_space);
  free(free_memory);
  free(volume);
  free(date_time);
  free(status);

  return 0;
}

void date_time_str(char *dt_str) {
  time_t current_time = time(NULL);
  struct tm *tm = localtime(&current_time);

  strftime(dt_str, 33, DATE_FORMAT, tm);
}

void free_space_str(char *fs_str) {
  struct statvfs fs_stats;
  if (statvfs(MOUNT_POINT, &fs_stats) == -1) {
    perror("statvfs");
    exit(EXIT_FAILURE);
  }
  // getting free space in Gb
  float free_space = (float)(fs_stats.f_bavail * fs_stats.f_bsize) / DIVIDER;
  snprintf(fs_str, 10, "%.1f Gb", free_space);
}

void free_memory_str(char *fm_str) {
  // float free_mem =
  //     (float)(sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGE_SIZE)) / divider;
  FILE *mem_info = fopen("/proc/meminfo", "r");
  char buf[BUF_SIZE] = {0};
  unsigned long free_mem_kb;

  // reading until third line, which contains MemAvailable info
  for (int i = 0; i < 3; i++) {
    fgets(buf, BUF_SIZE, mem_info);
  }

  fclose(mem_info);

  sscanf(buf, "MemAvailable:%lu", &free_mem_kb);
  snprintf(fm_str, 10, "%.1f Gb", (float)free_mem_kb / (1024 * 1024));
}

void cpu_temperature_str(char *ct_str) {
  float temp;
  char zone_path[BUF_SIZE] = {0};

  snprintf(zone_path, BUF_SIZE, "/sys/class/thermal/thermal_zone%d/temp",
           THERMAL_ZONE);

  FILE *fp;
  if ((fp = fopen(zone_path, "r")) == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  fscanf(fp, "%f", &temp);
  if (fclose(fp)) {
    perror("fclose");
    exit(EXIT_FAILURE);
  }

  snprintf(ct_str, 10, "%.1f°C", temp / 1000);
}

void volume_str(char *vm_str) {
  char *cmd = VOLUME_CMD;
  FILE *fp;

  if ((fp = popen(cmd, "r")) == NULL) {
    perror("popen");
    exit(EXIT_FAILURE);
  }

  fread(vm_str, 10, sizeof(char), fp);
  pclose(fp);
}

void delimiter(char *str) { strcat(str, DELIMITER); }
