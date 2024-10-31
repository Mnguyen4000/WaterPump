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

// Set LED
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(POWERLED, gpios);


int main(void)
{
    // Initialising

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}
	

	gpio_pin_configure_dt(&led, GPIO_OUTPUT | GPIO_ACTIVE_LOW);

	// Initialise modules
	i2c_init();
	soil_init();
	uv_init();
	BME680_init();
	LTR329_init();

	gpio_pin_set_dt(&led, 0);
	while (1) {
		printk("---READING---\n");
		BME680_measure();
		BME680_read();
		LTR329_read();
		soil_read_percent();
		uv_read_percent();

		// Toggle LED
		gpio_pin_toggle_dt(&led);
		k_msleep(SLEEP_TIME_MS);
	}
    return 0;
}
