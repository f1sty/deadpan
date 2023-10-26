#define DATE_FORMAT "%F %H:%M:%S"
#define NA_STRING "N/A"
#define MOUNT_POINT "/"
#define INTERVAL 1
#define DELIMITER " | "
/* To get right thermal zone number for CPU temperature run
   "cat /sys/class/thermal/thermal_zone<THERMAL_ZONE>/type".
   You are looking for `x86_pkg_temp` type. */
#define THERMAL_ZONE 1
/* /sys/class/power_supply/BAT<BATTERY> */
#define BATTERY 0 
#define VOLUME_CMD "volume.sh"
#define MUSIC_CMD "cmus-tagline.sh"
