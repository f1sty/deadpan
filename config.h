#include <sys/statvfs.h>
#define DATE_FORMAT "%F %H:%M:%S"
#define MOUNT_POINT "/"
#define INTERVAL 1
#define DELIMITER " | "
// to get right thermal zone number for CPU temp do
// `cat /sys/class/thermal/thermal_zone<THERMAL_ZONE>/type`
// You are looking for `x86_pkg_temp` type.
#define THERMAL_ZONE 2
#define VOLUME_CMD "volume.sh"

void date_time_str(char *dt_str);
void free_space_str(char *fs_str);
void free_memory_str(char *fm_str);
void cpu_temperature_str(char *ct_str);
void volume_str(char *vm_str);
void delimiter(char *str);
void run(char *cmd[]);
