#include <sys/statvfs.h>
#define DATE_FORMAT "%F %H:%M:%S"
#define MOUNT_POINT "/"
#define INTERVAL 1
#define DELIMITER " | "

void datetime_str(char *dt_str);
void free_space_str(char *fs_str);
void free_mem_str(char *fm_str);
void run(char *cmd[]);
