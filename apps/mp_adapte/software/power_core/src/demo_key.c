#include "board.h"
#include "hpm_gpio_drv.h"
#include "demo_key.h"

static demo_key_cb key_cb = NULL;

static void isr_gpio(void)
{
    if(gpio_check_pin_interrupt_flag(BOARD_KEY1_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN))
    {
        gpio_clear_pin_interrupt_flag(BOARD_KEY1_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN);
        if(key_cb)
            key_cb(DEMO_KEY1, gpio_read_pin(BOARD_KEY1_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN));
    }

    if(gpio_check_pin_interrupt_flag(BOARD_KEY2_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN))
    {
        gpio_clear_pin_interrupt_flag(BOARD_KEY2_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN);
        if(key_cb)
            key_cb(DEMO_KEY2, gpio_read_pin(BOARD_KEY2_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN));
    }

    if(gpio_check_pin_interrupt_flag(BOARD_KEY3_GPIO_CTRL, BOARD_KEY3_GPIO_INDEX, BOARD_KEY3_GPIO_PIN))
    {
        gpio_clear_pin_interrupt_flag(BOARD_KEY3_GPIO_CTRL, BOARD_KEY3_GPIO_INDEX, BOARD_KEY3_GPIO_PIN);
        if(key_cb)
            key_cb(DEMO_KEY3, gpio_read_pin(BOARD_KEY3_GPIO_CTRL, BOARD_KEY3_GPIO_INDEX, BOARD_KEY3_GPIO_PIN));
    }

    if(gpio_check_pin_interrupt_flag(BOARD_KEY4_GPIO_CTRL, BOARD_KEY4_GPIO_INDEX, BOARD_KEY4_GPIO_PIN))
    {
        gpio_clear_pin_interrupt_flag(BOARD_KEY4_GPIO_CTRL, BOARD_KEY4_GPIO_INDEX, BOARD_KEY4_GPIO_PIN);
        if(key_cb)
            key_cb(DEMO_KEY4, gpio_read_pin(BOARD_KEY4_GPIO_CTRL, BOARD_KEY4_GPIO_INDEX, BOARD_KEY4_GPIO_PIN));
    }

}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_GPIO_IRQ, isr_gpio)


uint8_t demo_keys_read_level(uint8_t index)
{
    uint8_t level;
    switch(index)
    {
        case DEMO_KEY1:
        {
            level = gpio_read_pin(BOARD_KEY1_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN);
        }
        break;
        case DEMO_KEY2:
        {
            level = gpio_read_pin(BOARD_KEY2_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN);
        }
        break;
        case DEMO_KEY3:
        {
            level = gpio_read_pin(BOARD_KEY3_GPIO_CTRL, BOARD_KEY3_GPIO_INDEX, BOARD_KEY3_GPIO_PIN);
        }
        break;
        case DEMO_KEY4:
        {
            level = gpio_read_pin(BOARD_KEY4_GPIO_CTRL, BOARD_KEY4_GPIO_INDEX, BOARD_KEY4_GPIO_PIN);
        }
        break;
        default:
        break;
    }
    return level;
}

void demo_keys_init(demo_key_cb cb)
{
    board_keys_gpio_pins();
    gpio_set_pin_input(BOARD_KEY1_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN);
    gpio_set_pin_input(BOARD_KEY2_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN);
    gpio_set_pin_input(BOARD_KEY3_GPIO_CTRL, BOARD_KEY3_GPIO_INDEX, BOARD_KEY3_GPIO_PIN);
    gpio_set_pin_input(BOARD_KEY4_GPIO_CTRL, BOARD_KEY4_GPIO_INDEX, BOARD_KEY4_GPIO_PIN);

    gpio_interrupt_trigger_t trigger;

    if(cb != NULL)
    {
        key_cb = cb;
        trigger = gpio_interrupt_trigger_edge_falling;
        gpio_config_pin_interrupt(BOARD_KEY1_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN, trigger);
        gpio_enable_pin_interrupt(BOARD_KEY1_GPIO_CTRL, BOARD_KEY1_GPIO_INDEX, BOARD_KEY1_GPIO_PIN);
        gpio_config_pin_interrupt(BOARD_KEY2_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN, trigger);
        gpio_enable_pin_interrupt(BOARD_KEY2_GPIO_CTRL, BOARD_KEY2_GPIO_INDEX, BOARD_KEY2_GPIO_PIN);
        gpio_config_pin_interrupt(BOARD_KEY3_GPIO_CTRL, BOARD_KEY3_GPIO_INDEX, BOARD_KEY3_GPIO_PIN, trigger);
        gpio_enable_pin_interrupt(BOARD_KEY3_GPIO_CTRL, BOARD_KEY3_GPIO_INDEX, BOARD_KEY3_GPIO_PIN);
        gpio_config_pin_interrupt(BOARD_KEY4_GPIO_CTRL, BOARD_KEY4_GPIO_INDEX, BOARD_KEY4_GPIO_PIN, trigger);
        gpio_enable_pin_interrupt(BOARD_KEY4_GPIO_CTRL, BOARD_KEY4_GPIO_INDEX, BOARD_KEY4_GPIO_PIN);
        intc_m_enable_irq_with_priority(BOARD_APP_GPIO_IRQ, 1);
    }
}