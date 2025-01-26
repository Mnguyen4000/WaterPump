
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


/* The devicetree node identifier for the "led0" alias. */
#define POWERLED DT_ALIAS(led0)
#define DEBUGLED DT_ALIAS(led1)
#define RELAY DT_ALIAS(led2)
#define RELAY_BUTTON DT_ALIAS(sw1)

// Set LED
static const struct gpio_dt_spec power = GPIO_DT_SPEC_GET(POWERLED, gpios);
static const struct gpio_dt_spec debug = GPIO_DT_SPEC_GET(DEBUGLED, gpios);
static const struct gpio_dt_spec relay = GPIO_DT_SPEC_GET(RELAY, gpios);
static const struct gpio_dt_spec relayButton = GPIO_DT_SPEC_GET_OR(RELAY_BUTTON, gpios, {1});


int gpio_init(void) {
    // Initialising
	if (!gpio_is_ready_dt(&power) || !gpio_is_ready_dt(&debug) 
		|| !gpio_is_ready_dt(&relay) || !gpio_is_ready_dt(&relayButton)) {
		return 1;
	}

	gpio_pin_configure_dt(&power, GPIO_OUTPUT | GPIO_ACTIVE_LOW);
	gpio_pin_configure_dt(&debug, GPIO_OUTPUT | GPIO_ACTIVE_LOW);
	gpio_pin_configure_dt(&relay, GPIO_OUTPUT | GPIO_ACTIVE_LOW);
	gpio_pin_configure_dt(&relayButton, GPIO_INPUT);


	gpio_pin_set_dt(&power, 0);
	gpio_pin_set_dt(&debug, 0);
	gpio_pin_set_dt(&relay, 0);
    return 0;
}


void relay_on(void) {
	gpio_pin_set_dt(&relay, 0);
}

void powerLED_on(void) {
	gpio_pin_set_dt(&power, 0);
}

void debugLED_on(void) {
	gpio_pin_set_dt(&debug, 0);
}

void relay_off(void) {
	gpio_pin_set_dt(&relay, 1);
}

void powerLED_off(void) {
	gpio_pin_set_dt(&power, 1);
}

void debugLED_off(void) {
	gpio_pin_set_dt(&debug, 1);
}


void read_button(void) {
	if (gpio_pin_get_dt(&relayButton)) {
		relay_on();
		//debugLED_on();
	} else {
		relay_off();
		//debugLED_off();
	}
}