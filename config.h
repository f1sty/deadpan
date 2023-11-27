#define DATE_FORMAT "%a %Y-%m-%d %H:%M:%S"
#define NA_STRING "N/A"
#define MOUNT_POINT "/"
#define INTERVAL 1
#define DELIMITER " | "
/* To get right thermal zone number for CPU temperature run
   "cat /sys/class/thermal/thermal_zone<THERMAL_ZONE>/type".
   You are looking for `x86_pkg_temp` type. */
#define THERMAL_ZONE 2
/* /sys/class/power_supply/BAT<BATTERY> */
#define BATTERY 0
#define VOLUME_CMD "volume.sh"
#define MUSIC_CMD "cmus-tagline.sh"

struct widget {
  void (*name)(char *);
  bool with_delimiter;
};

const struct widget widgets_list[] = {
    {.name = music, .with_delimiter = true},
    {.name = cpu_temperature, .with_delimiter = true},
    // {.name = battery_charge, .with_delimiter = true},
    {.name = free_disk, .with_delimiter = true},
    {.name = free_memory, .with_delimiter = true},
    {.name = volume, .with_delimiter = true},
    {.name = date_time, .with_delimiter = false},
    {.name = NULL, .with_delimiter = NULL},
};
