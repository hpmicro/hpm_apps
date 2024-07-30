#ifndef DEMO_LED_H
#define DEMO_LED_H

typedef enum
{
    DEMO_LED1 = 0,
    DEMO_LED2,
    DEMO_LED3,
    DEMO_LED_MAX
}DEMO_LED_INDEX;

void demo_leds_set_on(uint8_t index, bool on);

void demo_leds_toggle(uint8_t index);

void demo_leds_init(void);

#endif