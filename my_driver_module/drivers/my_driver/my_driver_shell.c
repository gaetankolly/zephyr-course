#include <zephyr/shell/shell.h>
#include <my_drivers/my_driver.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

// compatible with first instance of my_driver
#define SENSOR_INSTANCE_FOR_SHELL 0
#define SENSOR_CHANNEL_FOR_SHELL 0

static const struct device *const dev_s = DEVICE_DT_GET(DT_INST(SENSOR_INSTANCE_FOR_SHELL, my_driver));
static enum sensor_channel channel_s = (enum sensor_channel) SENSOR_CHANNEL_FOR_SHELL;


static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv) {
    int ret = sensor_sample_fetch(dev_s);

    if (ret < 0) {
        shell_error(sh, "fetch failed (%d)", ret);
        return ret;
    }
    return 0;
}
static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv) {

    struct sensor_value value;
    int ret = sensor_channel_get(dev_s, channel_s, &value);

    if (ret < 0) {
        shell_error(sh, "fetch failed (%d)", ret);
        return ret;
    }

    shell_print(sh, "Value = %d", value.val1);

    return 0;
}

static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv) {
    shell_print(sh, "name : %s", dev_s->name);
    shell_print(sh, "ready: %s", device_is_ready(dev_s) ? "yes" : "no");
    return 0;
}

static int cmd_sensor_set(const struct shell *sh, size_t argc, char **argv) {
    int err = 0;

    int value = (int) shell_strtol(argv[1], 0, &err);

    if (err) {
        shell_error(sh, "Invalid number: %s", argv[1]);
        return -EINVAL;
    }

    if (value < CONFIG_MY_DRIVER_CNT_MIN || value > CONFIG_MY_DRIVER_CNT_MAX) {
        shell_error(sh, "Value out of range (%d to %d)", CONFIG_MY_DRIVER_CNT_MIN, CONFIG_MY_DRIVER_CNT_MAX);
        return -EINVAL;
    }

    int ret = my_driver_set_cnt(dev_s, value);
    if (ret < 0) {
        shell_error(sh, "set_cnt failed (%d)", ret);
        return ret;
    }

    shell_print(sh, "cnt set to %d", value);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD(fetch, NULL, "Call sensor fetch", cmd_sensor_fetch),
    SHELL_CMD(read, NULL, "Call sensor read", cmd_sensor_read),
    SHELL_CMD(info, NULL, "Read sensor info", cmd_sensor_info),
    SHELL_CMD_ARG(set, NULL, "Set a new value for count", cmd_sensor_set, 2, 0),
    SHELL_SUBCMD_SET_END 
);


SHELL_CMD_REGISTER(sensor, &sub_sensor, "Sensor commands", NULL);

