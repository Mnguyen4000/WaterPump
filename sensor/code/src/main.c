// Main
/*
int main(void) {

    // Code

    return 0;
}
*/

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include "sensors.h"
/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define POWERLED DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(POWERLED, gpios);


int main(void)
{
    // Initialising
	int ret;
	bool led_state = true;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}
	i2c_initt();
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT | GPIO_ACTIVE_LOW);
	

	//adc_test();
	soil_init();
	uv_init();

	LTR329_init();

	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_set_dt(&led, 0);
	while (1) {
		printk("---READING---\n");
		BME680_init();
		BME680_read();
		//LTR329_read();
		soil_read_percent();
		uv_read_percent();


		ret = gpio_pin_toggle_dt(&led);
		led_state = !led_state;
		//printf("LED state: %s\n", led_state ? "ON" : "OFF");
		k_msleep(SLEEP_TIME_MS);
	}
    return 0;
}
