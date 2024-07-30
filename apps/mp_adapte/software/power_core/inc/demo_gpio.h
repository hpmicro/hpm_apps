#ifndef DEMO_GPIO_H
#define DEMO_GPIO_H

typedef enum
{
    DEMO_CONN0 = 0,
    DEMO_CONN1,
    DEMO_CONN2,
    DEMO_CONN3,
    DEMO_CONN_MAX
}DEMO_CONN_INDEX;

typedef enum
{
    DEMO_GPIO0 = 0,
    DEMO_GPIO1,
    DEMO_GPIO2,
    DEMO_GPIO_MAX
}DEMO_GPIO_INDEX;

void demo_gpios_set_level(uint8_t conn_index, uint8_t pin_index, uint8_t level);

void demo_gpios_toggle(uint8_t conn_index, uint8_t pin_index);

void demo_gpios_init(uint8_t conn_index);

#endif