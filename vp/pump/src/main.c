
#include "gpio.h"
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   100
#define PUMP 0
#define VALVE 1

#define MODE PUMP

int main(void)
{
	gpio_init();
	powerLED_off();
	while (1) {
		read_button();
		//powerLED_off();
		//debugLED_off();
		//relay_off();
		k_msleep(SLEEP_TIME_MS);
		//powerLED_on();
		
		read_button();
		//debugLED_on();
		//relay_on();
		k_msleep(SLEEP_TIME_MS);
	}
    return 0;
}
