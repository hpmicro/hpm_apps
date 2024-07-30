#include "board.h"
#include "hpm_gpio_drv.h"
#include "demo_gpio.h"

void demo_gpios_set_level(uint8_t conn_index, uint8_t pin_index, uint8_t level)
{
    switch(conn_index)
    {
        case DEMO_CONN0:
        {
            if(pin_index == DEMO_GPIO0)
            {
                gpio_write_pin(BOARD_CONN0_GPIO0_CTRL, BOARD_CONN0_GPIO0_INDEX, BOARD_CONN0_GPIO0_PIN, level);
            }
            else if(pin_index == DEMO_GPIO1)
            {
                gpio_write_pin(BOARD_CONN0_GPIO1_CTRL, BOARD_CONN0_GPIO1_INDEX, BOARD_CONN0_GPIO1_PIN, level);
            }
            else if(pin_index == DEMO_GPIO2)
            {
                gpio_write_pin(BOARD_CONN0_GPIO2_CTRL, BOARD_CONN0_GPIO2_INDEX, BOARD_CONN0_GPIO2_PIN, level);  
            }
        }
        break;
        case DEMO_CONN1:
        {
            if(pin_index == DEMO_GPIO0)
            {
                gpio_write_pin(BOARD_CONN1_GPIO0_CTRL, BOARD_CONN1_GPIO0_INDEX, BOARD_CONN1_GPIO0_PIN, level);
            }
            else if(pin_index == DEMO_GPIO1)
            {
                gpio_write_pin(BOARD_CONN1_GPIO1_CTRL, BOARD_CONN1_GPIO1_INDEX, BOARD_CONN1_GPIO1_PIN, level);
            }
            else if(pin_index == DEMO_GPIO2)
            {
                gpio_write_pin(BOARD_CONN1_GPIO2_CTRL, BOARD_CONN1_GPIO2_INDEX, BOARD_CONN1_GPIO2_PIN, level);  
            }
        }
        break;
        case DEMO_CONN2:
        {
            if(pin_index == DEMO_GPIO0)
            {
                gpio_write_pin(BOARD_CONN2_GPIO0_CTRL, BOARD_CONN2_GPIO0_INDEX, BOARD_CONN2_GPIO0_PIN, level);
            }
            else if(pin_index == DEMO_GPIO1)
            {
                gpio_write_pin(BOARD_CONN2_GPIO1_CTRL, BOARD_CONN2_GPIO1_INDEX, BOARD_CONN2_GPIO1_PIN, level);
            }
            else if(pin_index == DEMO_GPIO2)
            {
                gpio_write_pin(BOARD_CONN2_GPIO2_CTRL, BOARD_CONN2_GPIO2_INDEX, BOARD_CONN2_GPIO2_PIN, level);  
            }
        }
        break;
        case DEMO_CONN3:
        {
            if(pin_index == DEMO_GPIO0)
            {
                gpio_write_pin(BOARD_CONN3_GPIO0_CTRL, BOARD_CONN3_GPIO0_INDEX, BOARD_CONN3_GPIO0_PIN, level);
            }
            else if(pin_index == DEMO_GPIO1)
            {
                gpio_write_pin(BOARD_CONN3_GPIO1_CTRL, BOARD_CONN3_GPIO1_INDEX, BOARD_CONN3_GPIO1_PIN, level);
            }
            else if(pin_index == DEMO_GPIO2)
            {
                gpio_write_pin(BOARD_CONN3_GPIO2_CTRL, BOARD_CONN3_GPIO2_INDEX, BOARD_CONN3_GPIO2_PIN, level);  
            }
        }
        break;
        default:
        break;
    }
}

void demo_gpios_toggle(uint8_t conn_index, uint8_t pin_index)
{
    switch(conn_index)
    {
        case DEMO_CONN0:
        {
            if(pin_index == DEMO_GPIO0)
            {
                gpio_toggle_pin(BOARD_CONN0_GPIO0_CTRL, BOARD_CONN0_GPIO0_INDEX, BOARD_CONN0_GPIO0_PIN);
            }
            else if(pin_index == DEMO_GPIO1)
            {
                gpio_toggle_pin(BOARD_CONN0_GPIO1_CTRL, BOARD_CONN0_GPIO1_INDEX, BOARD_CONN0_GPIO1_PIN);
            }
            else if(pin_index == DEMO_GPIO2)
            {
                gpio_toggle_pin(BOARD_CONN0_GPIO2_CTRL, BOARD_CONN0_GPIO2_INDEX, BOARD_CONN0_GPIO2_PIN);  
            }
        }
        break;
        case DEMO_CONN1:
        {
            if(pin_index == DEMO_GPIO0)
            {
                gpio_toggle_pin(BOARD_CONN1_GPIO0_CTRL, BOARD_CONN1_GPIO0_INDEX, BOARD_CONN1_GPIO0_PIN);
            }
            else if(pin_index == DEMO_GPIO1)
            {
                gpio_toggle_pin(BOARD_CONN1_GPIO1_CTRL, BOARD_CONN1_GPIO1_INDEX, BOARD_CONN1_GPIO1_PIN);
            }
            else if(pin_index == DEMO_GPIO2)
            {
                gpio_toggle_pin(BOARD_CONN1_GPIO2_CTRL, BOARD_CONN1_GPIO2_INDEX, BOARD_CONN1_GPIO2_PIN);
            }
        }
        break;
        case DEMO_CONN2:
        {
            if(pin_index == DEMO_GPIO0)
            {
                gpio_toggle_pin(BOARD_CONN2_GPIO0_CTRL, BOARD_CONN2_GPIO0_INDEX, BOARD_CONN2_GPIO0_PIN);
            }
            else if(pin_index == DEMO_GPIO1)
            {
                gpio_toggle_pin(BOARD_CONN2_GPIO1_CTRL, BOARD_CONN2_GPIO1_INDEX, BOARD_CONN2_GPIO1_PIN);
            }
            else if(pin_index == DEMO_GPIO2)
            {
                gpio_toggle_pin(BOARD_CONN2_GPIO2_CTRL, BOARD_CONN2_GPIO2_INDEX, BOARD_CONN2_GPIO2_PIN);
            }
        }
        break;
        case DEMO_CONN3:
        {
            if(pin_index == DEMO_GPIO0)
            {
                gpio_toggle_pin(BOARD_CONN3_GPIO0_CTRL, BOARD_CONN3_GPIO0_INDEX, BOARD_CONN3_GPIO0_PIN);
            }
            else if(pin_index == DEMO_GPIO1)
            {
                gpio_toggle_pin(BOARD_CONN3_GPIO1_CTRL, BOARD_CONN3_GPIO1_INDEX, BOARD_CONN3_GPIO1_PIN);
            }
            else if(pin_index == DEMO_GPIO2)
            {
                gpio_toggle_pin(BOARD_CONN3_GPIO2_CTRL, BOARD_CONN3_GPIO2_INDEX, BOARD_CONN3_GPIO2_PIN);
            }
        }
        break;
        default:
        break;
    }
}

void demo_gpios_init(uint8_t conn_index)
{
    board_normal_gpio_pins(conn_index);
    switch(conn_index)
    {
        case DEMO_CONN0:
        {
            gpio_set_pin_output(BOARD_CONN0_GPIO0_CTRL, BOARD_CONN0_GPIO0_INDEX, BOARD_CONN0_GPIO0_PIN);
            gpio_set_pin_output(BOARD_CONN0_GPIO1_CTRL, BOARD_CONN0_GPIO1_INDEX, BOARD_CONN0_GPIO1_PIN);
            gpio_set_pin_output(BOARD_CONN0_GPIO2_CTRL, BOARD_CONN0_GPIO2_INDEX, BOARD_CONN0_GPIO2_PIN);
            gpio_write_pin(BOARD_CONN0_GPIO0_CTRL, BOARD_CONN0_GPIO0_INDEX, BOARD_CONN0_GPIO0_PIN, 0);
            gpio_write_pin(BOARD_CONN0_GPIO1_CTRL, BOARD_CONN0_GPIO1_INDEX, BOARD_CONN0_GPIO1_PIN, 0);
            gpio_write_pin(BOARD_CONN0_GPIO2_CTRL, BOARD_CONN0_GPIO2_INDEX, BOARD_CONN0_GPIO2_PIN, 0);
        }
        break;
        case DEMO_CONN1:
        {
            gpio_set_pin_output(BOARD_CONN1_GPIO0_CTRL, BOARD_CONN1_GPIO0_INDEX, BOARD_CONN1_GPIO0_PIN);
            gpio_set_pin_output(BOARD_CONN1_GPIO1_CTRL, BOARD_CONN1_GPIO1_INDEX, BOARD_CONN1_GPIO1_PIN);
            gpio_set_pin_output(BOARD_CONN1_GPIO2_CTRL, BOARD_CONN1_GPIO2_INDEX, BOARD_CONN1_GPIO2_PIN);
            gpio_write_pin(BOARD_CONN1_GPIO0_CTRL, BOARD_CONN1_GPIO0_INDEX, BOARD_CONN1_GPIO0_PIN, 0);
            gpio_write_pin(BOARD_CONN1_GPIO1_CTRL, BOARD_CONN1_GPIO1_INDEX, BOARD_CONN1_GPIO1_PIN, 0);
            gpio_write_pin(BOARD_CONN1_GPIO2_CTRL, BOARD_CONN1_GPIO2_INDEX, BOARD_CONN1_GPIO2_PIN, 0);
        }
        break;
        case DEMO_CONN2:
        {
            gpio_set_pin_output(BOARD_CONN2_GPIO0_CTRL, BOARD_CONN2_GPIO0_INDEX, BOARD_CONN2_GPIO0_PIN);
            gpio_set_pin_output(BOARD_CONN2_GPIO1_CTRL, BOARD_CONN2_GPIO1_INDEX, BOARD_CONN2_GPIO1_PIN);
            gpio_set_pin_output(BOARD_CONN2_GPIO2_CTRL, BOARD_CONN2_GPIO2_INDEX, BOARD_CONN2_GPIO2_PIN);
            gpio_write_pin(BOARD_CONN2_GPIO0_CTRL, BOARD_CONN2_GPIO0_INDEX, BOARD_CONN2_GPIO0_PIN, 0);
            gpio_write_pin(BOARD_CONN2_GPIO1_CTRL, BOARD_CONN2_GPIO1_INDEX, BOARD_CONN2_GPIO1_PIN, 0);
            gpio_write_pin(BOARD_CONN2_GPIO2_CTRL, BOARD_CONN2_GPIO2_INDEX, BOARD_CONN2_GPIO2_PIN, 0);
        }
        break;
        case DEMO_CONN3:
        {
            gpio_set_pin_output(BOARD_CONN3_GPIO0_CTRL, BOARD_CONN3_GPIO0_INDEX, BOARD_CONN3_GPIO0_PIN);
            gpio_set_pin_output(BOARD_CONN3_GPIO1_CTRL, BOARD_CONN3_GPIO1_INDEX, BOARD_CONN3_GPIO1_PIN);
            gpio_set_pin_output(BOARD_CONN3_GPIO2_CTRL, BOARD_CONN3_GPIO2_INDEX, BOARD_CONN3_GPIO2_PIN);
            gpio_write_pin(BOARD_CONN3_GPIO0_CTRL, BOARD_CONN3_GPIO0_INDEX, BOARD_CONN3_GPIO0_PIN, 0);
            gpio_write_pin(BOARD_CONN3_GPIO1_CTRL, BOARD_CONN3_GPIO1_INDEX, BOARD_CONN3_GPIO1_PIN, 0);
            gpio_write_pin(BOARD_CONN3_GPIO2_CTRL, BOARD_CONN3_GPIO2_INDEX, BOARD_CONN3_GPIO2_PIN, 0);
        }
        break;
        default:
        break;
    }
}