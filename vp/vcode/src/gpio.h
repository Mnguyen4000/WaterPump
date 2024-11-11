#ifndef INC_GPIO_H_
#define INC_GPIO_H_

int gpio_init(void);

void relay_on(void);
void powerLED_on(void);
void debugLED_on(void);

void relay_off(void);
void powerLED_off(void);
void debugLED_off(void);

#endif