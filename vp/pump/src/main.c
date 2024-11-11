

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

#include "gpio.h"

int main(void)
{
	gpio_init();
	while (1) {
		// Toggle LED
		powerLED_on();
		debugLED_on();
		relay_on();
		k_msleep(SLEEP_TIME_MS);
	}
    return 0;
}
