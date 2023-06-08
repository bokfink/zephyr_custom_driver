#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(stepper_driver, LOG_LEVEL_DBG);

static const gpio_pin_t IN[] = {25, 26, 27, 14};
static const struct device *gpio_dev =
  DEVICE_DT_GET(DT_NODELABEL(gpio0));

enum rotation_direction {
  CLOCKWISE,
  COUNTER_CLOCKWISE,
};

int take_steps(const uint32_t target_num_steps,
	       const enum rotation_direction rot_dir,
	       const int32_t sleep_time_ms)
{
  if (!device_is_ready(gpio_dev)) {
    LOG_ERR("device %s is not ready", gpio_dev->name);
    return -ENODEV;
  }

  for (size_t i = 0; i < sizeof(IN); i++) {
    if (gpio_pin_configure(gpio_dev, IN[i],
			   GPIO_OUTPUT_LOW) != 0) {
      LOG_ERR("could not set GPIO pin %zu to 0", i);
      return -EIO;
    }
  }

  uint32_t curr_steps_count = 0U;
  if (rot_dir == CLOCKWISE) {
    while (curr_steps_count < target_num_steps) {
      // STEP 1
      k_msleep(sleep_time_ms);
      gpio_pin_set_raw(gpio_dev, IN[3], 0);
      gpio_pin_set_raw(gpio_dev, IN[1], 1);
      curr_steps_count++;

      // STEP 2
      k_msleep(sleep_time_ms);
      gpio_pin_set_raw(gpio_dev, IN[0], 0);
      gpio_pin_set_raw(gpio_dev, IN[2], 1);
      curr_steps_count++;

      // STEP 3
      k_msleep(sleep_time_ms);
      gpio_pin_set_raw(gpio_dev, IN[1], 0);
      gpio_pin_set_raw(gpio_dev, IN[3], 1);
      curr_steps_count++;

      // STEP 4
      k_msleep(sleep_time_ms);
      gpio_pin_set_raw(gpio_dev, IN[2], 0);
      gpio_pin_set_raw(gpio_dev, IN[0], 1);
      curr_steps_count++;
    }
  } else if (rot_dir == COUNTER_CLOCKWISE) {
    while (curr_steps_count < target_num_steps) {
      // STEP 1
      k_msleep(sleep_time_ms);
      gpio_pin_set_raw(gpio_dev, IN[0], 0);
      gpio_pin_set_raw(gpio_dev, IN[2], 1);
      curr_steps_count++;

      // STEP 2
      k_msleep(sleep_time_ms);
      gpio_pin_set_raw(gpio_dev, IN[3], 0);
      gpio_pin_set_raw(gpio_dev, IN[1], 1);
      curr_steps_count++;

      // STEP 3
      k_msleep(sleep_time_ms);
      gpio_pin_set_raw(gpio_dev, IN[2], 0);
      gpio_pin_set_raw(gpio_dev, IN[0], 1);
      curr_steps_count++;

      // STEP 4
      k_msleep(sleep_time_ms);
      gpio_pin_set_raw(gpio_dev, IN[1], 0);
      gpio_pin_set_raw(gpio_dev, IN[3], 1);
      curr_steps_count++;
    }
  } else {
    LOG_ERR("direction should be CLOCKWISE or \
	    COUNTER_CLOCKWISE");
    return -EINVAL;
  }

  return 0;
}
	       
void main(void)
{
  if (take_steps(2048, CLOCKWISE, 4) != 0) {
    LOG_ERR("could not take steps on motor");
    return;
  }

  if (take_steps(2048, COUNTER_CLOCKWISE, 4) != 0) {
    LOG_ERR("could not take steps on motor");
    return;
  }
}
