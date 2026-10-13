#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>


#define DT_DRV_COMPAT my_driver
LOG_MODULE_REGISTER(my_driver, CONFIG_SENSOR_LOG_LEVEL);

struct our_driver_config {
    struct gpio_dt_spec led;
};

struct our_driver_data {
    int cnt;
};

static int my_driver_sample_fetch(const struct device *dev, enum sensor_channel chan) {
    // LOG_INF("Sample fetch");
    const struct our_driver_config *cfg = dev->config;
    gpio_pin_set_dt(&cfg->led, 1);

    return 0;
}
static int my_driver_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val) {
    // LOG_INF("Channel get %d", chan);
    const struct our_driver_config *cfg = dev->config;
    gpio_pin_set_dt(&cfg->led, 0);

    return 0;
}

static DEVICE_API(sensor, my_driver_api) = {
    .sample_fetch = my_driver_sample_fetch,
    .channel_get = my_driver_channel_get,
};

static int my_driver_init(const struct device *dev)
{
    const struct our_driver_config *cfg = dev->config;
    if (!gpio_is_ready_dt(&cfg->led)) 
    {
        return -ENODEV;
    }

    if (gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_ACTIVE) < 0) 
    {
        return 0;
    }


    LOG_INF("Device initialised");

    return 0;
}

#define MY_DRIVER_DEFINE(inst)\
    static struct our_driver_data data_##inst;\
    static const struct our_driver_config cfg_##inst = {\
    .led = GPIO_DT_SPEC_GET(DT_INST_PHANDLE(inst, led), gpios),\
    };\
    DEVICE_DT_INST_DEFINE(inst,\
    my_driver_init, NULL,\
    &data_##inst, &cfg_##inst,\
    POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,\
    &my_driver_api)

DT_INST_FOREACH_STATUS_OKAY(MY_DRIVER_DEFINE)