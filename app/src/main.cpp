#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#ifdef CONFIG_MY_DRIVER
static const struct device *const sensor = DEVICE_DT_GET(DT_NODELABEL(my_sensor));
#else
/* The devicetree node identifier for the "led0" alias. */
#define LED_NODE DT_ALIAS(app_led)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
#endif


LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    LOG_INF("Now we are in main()");
    bool led_state = true;

    #ifdef CONFIG_MY_DRIVER
    if (!device_is_ready(sensor)) return 0;

    bool toggle = 0;
    while (1) {
        if (toggle) {
            sensor_sample_fetch(sensor);
        } 
        else {
            sensor_value value;
            sensor_channel channel = static_cast<sensor_channel>(0);
            sensor_channel_get(sensor, channel, &value);
        }

        toggle = !toggle;
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    #else
    if (!gpio_is_ready_dt(&led)) return 0;

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    while (1) {
        if (gpio_pin_toggle_dt(&led) < 0) return 0;

        led_state = !led_state;
        // LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    #endif
    return 0;
}
