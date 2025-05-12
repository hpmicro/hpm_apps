/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "full_port_config_drv.h"

static full_port_irq_cb_func s_full_port_irq_keya_cb;
static full_port_irq_cb_func s_full_port_irq_keyb_cb;
static full_port_irq_cb_func s_full_port_irq_keyc_cb;
static full_port_irq_cb_func s_full_port_irq_keyd_cb;


void full_port_key_init(void)
{
    board_init_key();
}

int8_t full_port_key_get_status(FULL_PORT_KEY_INDEX_ID_e eKeyIndexId)
{
    int8_t u8KeyValue = FULL_PORT_EER_ERROR;

    switch (eKeyIndexId)
    {
        case FULL_PORT_KEY_INDEX_KEYA_ID:
             u8KeyValue = gpio_read_pin(BOARD_KEYA_GPIO_CTRL, BOARD_KEYA_GPIO_INDEX, BOARD_KEYA_GPIO_PIN);
        break;
        case FULL_PORT_KEY_INDEX_KEYB_ID:
             u8KeyValue = gpio_read_pin(BOARD_KEYB_GPIO_CTRL, BOARD_KEYB_GPIO_INDEX, BOARD_KEYB_GPIO_PIN);
        break;
        case FULL_PORT_KEY_INDEX_KEYC_ID:
             u8KeyValue = gpio_read_pin(BOARD_KEYC_GPIO_CTRL, BOARD_KEYC_GPIO_INDEX, BOARD_KEYC_GPIO_PIN);
        break;
        case FULL_PORT_KEY_INDEX_KEYD_ID:
             u8KeyValue = gpio_read_pin(BOARD_KEYD_GPIO_CTRL, BOARD_KEYD_GPIO_INDEX, BOARD_KEYD_GPIO_PIN);
        break;
        default:
        break;
    }

    return u8KeyValue;
}


static void full_port_irq_key_isr0(void)
{
    if(gpio_check_pin_interrupt_flag(BOARD_KEYA_GPIO_CTRL, BOARD_KEYA_GPIO_INDEX, BOARD_KEYA_GPIO_PIN)) {
        gpio_clear_pin_interrupt_flag(BOARD_KEYA_GPIO_CTRL, BOARD_KEYA_GPIO_INDEX, BOARD_KEYA_GPIO_PIN);
        s_full_port_irq_keya_cb();
    }

    if(gpio_check_pin_interrupt_flag(BOARD_KEYB_GPIO_CTRL, BOARD_KEYB_GPIO_INDEX, BOARD_KEYB_GPIO_PIN)) {
        gpio_clear_pin_interrupt_flag(BOARD_KEYB_GPIO_CTRL, BOARD_KEYB_GPIO_INDEX, BOARD_KEYB_GPIO_PIN);
        s_full_port_irq_keyb_cb();
    }

    if(gpio_check_pin_interrupt_flag(BOARD_KEYC_GPIO_CTRL, BOARD_KEYC_GPIO_INDEX, BOARD_KEYC_GPIO_PIN)) {
        gpio_clear_pin_interrupt_flag(BOARD_KEYC_GPIO_CTRL, BOARD_KEYC_GPIO_INDEX, BOARD_KEYC_GPIO_PIN);
        s_full_port_irq_keyc_cb();
    }

    if(gpio_check_pin_interrupt_flag(BOARD_KEYD_GPIO_CTRL, BOARD_KEYD_GPIO_INDEX, BOARD_KEYD_GPIO_PIN)) {
        gpio_clear_pin_interrupt_flag(BOARD_KEYD_GPIO_CTRL, BOARD_KEYD_GPIO_INDEX, BOARD_KEYD_GPIO_PIN);
        s_full_port_irq_keyd_cb();
    }
}

SDK_DECLARE_EXT_ISR_M(FULL_PORT_IRQ_KEY_IRQ0, full_port_irq_key_isr0);

void full_port_irq_keya_init(full_port_irq_cb_func cb)
{
    s_full_port_irq_keya_cb = cb;
    gpio_config_pin_interrupt(BOARD_KEYA_GPIO_CTRL, BOARD_KEYA_GPIO_INDEX, BOARD_KEYA_GPIO_PIN, gpio_interrupt_trigger_edge_falling);
    gpio_enable_pin_interrupt(BOARD_KEYA_GPIO_CTRL, BOARD_KEYA_GPIO_INDEX, BOARD_KEYA_GPIO_PIN);
    intc_m_enable_irq_with_priority(FULL_PORT_IRQ_KEYA_IRQ, FULL_PORT_IRQ_KEYA_PRIORITY);
}

void full_port_irq_keyb_init(full_port_irq_cb_func cb)
{
    s_full_port_irq_keyb_cb = cb;
    gpio_config_pin_interrupt(BOARD_KEYB_GPIO_CTRL, BOARD_KEYB_GPIO_INDEX, BOARD_KEYB_GPIO_PIN, gpio_interrupt_trigger_edge_falling);
    gpio_enable_pin_interrupt(BOARD_KEYB_GPIO_CTRL, BOARD_KEYB_GPIO_INDEX, BOARD_KEYB_GPIO_PIN);
    intc_m_enable_irq_with_priority(FULL_PORT_IRQ_KEYB_IRQ, FULL_PORT_IRQ_KEYB_PRIORITY);
}

void full_port_irq_keyc_init(full_port_irq_cb_func cb)
{
    s_full_port_irq_keyc_cb = cb;
    gpio_config_pin_interrupt(BOARD_KEYC_GPIO_CTRL, BOARD_KEYC_GPIO_INDEX, BOARD_KEYC_GPIO_PIN, gpio_interrupt_trigger_edge_falling);
    gpio_enable_pin_interrupt(BOARD_KEYC_GPIO_CTRL, BOARD_KEYC_GPIO_INDEX, BOARD_KEYC_GPIO_PIN);
    intc_m_enable_irq_with_priority(FULL_PORT_IRQ_KEYC_IRQ, FULL_PORT_IRQ_KEYC_PRIORITY);
}

void full_port_irq_keyd_init(full_port_irq_cb_func cb)
{
    s_full_port_irq_keyd_cb = cb;
    gpio_config_pin_interrupt(BOARD_KEYD_GPIO_CTRL, BOARD_KEYD_GPIO_INDEX, BOARD_KEYD_GPIO_PIN, gpio_interrupt_trigger_edge_falling);
    gpio_enable_pin_interrupt(BOARD_KEYD_GPIO_CTRL, BOARD_KEYD_GPIO_INDEX, BOARD_KEYD_GPIO_PIN);
    intc_m_enable_irq_with_priority(FULL_PORT_IRQ_KEYD_IRQ, FULL_PORT_IRQ_KEYD_PRIORITY);
}

