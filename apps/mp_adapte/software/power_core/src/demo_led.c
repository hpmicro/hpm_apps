#include "board.h"
#include "hpm_gpio_drv.h"
#include "demo_led.h"

void demo_leds_set_on(uint8_t index, bool on)
{
    switch(index)
    {
        case DEMO_LED1:
        {
            gpio_write_pin(BOARD_LED1_GPIO_CTRL, BOARD_LED1_GPIO_INDEX, BOARD_LED1_GPIO_PIN, on ? BOARD_LED_ON_LEVEL : BOARD_LED_OFF_LEVEL);
        }
        break;
        case DEMO_LED2:
        {
            gpio_write_pin(BOARD_LED2_GPIO_CTRL, BOARD_LED2_GPIO_INDEX, BOARD_LED2_GPIO_PIN, on ? BOARD_LED_ON_LEVEL : BOARD_LED_OFF_LEVEL);
        }
        break;
        case DEMO_LED3:
        {
            gpio_write_pin(BOARD_LED3_GPIO_CTRL, BOARD_LED3_GPIO_INDEX, BOARD_LED3_GPIO_PIN, on ? BOARD_LED_ON_LEVEL : BOARD_LED_OFF_LEVEL);
        }
        break;
        default:
        break;
    }
}

void demo_leds_toggle(uint8_t index)
{
    switch(index)
    {
        case DEMO_LED1:
        {
            gpio_toggle_pin(BOARD_LED1_GPIO_CTRL, BOARD_LED1_GPIO_INDEX, BOARD_LED1_GPIO_PIN);
        }
        break;
        case DEMO_LED2:
        {
            gpio_toggle_pin(BOARD_LED2_GPIO_CTRL, BOARD_LED2_GPIO_INDEX, BOARD_LED2_GPIO_PIN);
        }
        break;
        case DEMO_LED3:
        {
            gpio_toggle_pin(BOARD_LED3_GPIO_CTRL, BOARD_LED3_GPIO_INDEX, BOARD_LED3_GPIO_PIN);
        }
        break;
        default:
        break;
    }
}

void demo_leds_init(void)
{
    board_leds_gpio_pins();
    gpio_set_pin_output(BOARD_LED1_GPIO_CTRL, BOARD_LED1_GPIO_INDEX, BOARD_LED1_GPIO_PIN);
    gpio_set_pin_output(BOARD_LED2_GPIO_CTRL, BOARD_LED2_GPIO_INDEX, BOARD_LED2_GPIO_PIN);
    gpio_set_pin_output(BOARD_LED3_GPIO_CTRL, BOARD_LED3_GPIO_INDEX, BOARD_LED3_GPIO_PIN);
    gpio_write_pin(BOARD_LED1_GPIO_CTRL, BOARD_LED1_GPIO_INDEX, BOARD_LED1_GPIO_PIN, BOARD_LED_OFF_LEVEL);
    gpio_write_pin(BOARD_LED2_GPIO_CTRL, BOARD_LED2_GPIO_INDEX, BOARD_LED2_GPIO_PIN, BOARD_LED_OFF_LEVEL);
    gpio_write_pin(BOARD_LED3_GPIO_CTRL, BOARD_LED3_GPIO_INDEX, BOARD_LED3_GPIO_PIN, BOARD_LED_OFF_LEVEL);
}