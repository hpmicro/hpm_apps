/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "drv_key.h"

static drv_irq_cb_func s_drv_irq_keya_cb;
static drv_irq_cb_func s_drv_irq_keyb_cb;
static drv_irq_cb_func s_drv_irq_keyc_cb;
static drv_irq_cb_func s_drv_irq_keyd_cb;


void drv_key_init(void)
{
    board_init_key();
}

int8_t drv_key_get_status(drv_key_id_e id)
{
    int8_t value = -1;

    switch (id)
    {
        case DRV_KEY_KEYA_ID:
             value = gpio_read_pin(BOARD_KEYA_GPIO_CTRL, BOARD_KEYA_GPIO_INDEX, BOARD_KEYA_GPIO_PIN);
        break;
        case DRV_KEY_KEYB_ID:
             value = gpio_read_pin(BOARD_KEYB_GPIO_CTRL, BOARD_KEYB_GPIO_INDEX, BOARD_KEYB_GPIO_PIN);
        break;
        case DRV_KEY_KEYC_ID:
             value = gpio_read_pin(BOARD_KEYC_GPIO_CTRL, BOARD_KEYC_GPIO_INDEX, BOARD_KEYC_GPIO_PIN);
        break;
        case DRV_KEY_KEYD_ID:
             value = gpio_read_pin(BOARD_KEYD_GPIO_CTRL, BOARD_KEYD_GPIO_INDEX, BOARD_KEYD_GPIO_PIN);
        break;
        default:
        break;
    }

    return value;
}


static void drv_irq_key_isr0(void)
{
    if(gpio_check_pin_interrupt_flag(BOARD_KEYA_GPIO_CTRL, BOARD_KEYA_GPIO_INDEX, BOARD_KEYA_GPIO_PIN)) {
        gpio_clear_pin_interrupt_flag(BOARD_KEYA_GPIO_CTRL, BOARD_KEYA_GPIO_INDEX, BOARD_KEYA_GPIO_PIN);
        s_drv_irq_keya_cb();
    }

    if(gpio_check_pin_interrupt_flag(BOARD_KEYB_GPIO_CTRL, BOARD_KEYB_GPIO_INDEX, BOARD_KEYB_GPIO_PIN)) {
        gpio_clear_pin_interrupt_flag(BOARD_KEYB_GPIO_CTRL, BOARD_KEYB_GPIO_INDEX, BOARD_KEYB_GPIO_PIN);
        s_drv_irq_keyb_cb();
    }

    if(gpio_check_pin_interrupt_flag(BOARD_KEYC_GPIO_CTRL, BOARD_KEYC_GPIO_INDEX, BOARD_KEYC_GPIO_PIN)) {
        gpio_clear_pin_interrupt_flag(BOARD_KEYC_GPIO_CTRL, BOARD_KEYC_GPIO_INDEX, BOARD_KEYC_GPIO_PIN);
        s_drv_irq_keyc_cb();
    }

    if(gpio_check_pin_interrupt_flag(BOARD_KEYD_GPIO_CTRL, BOARD_KEYD_GPIO_INDEX, BOARD_KEYD_GPIO_PIN)) {
        gpio_clear_pin_interrupt_flag(BOARD_KEYD_GPIO_CTRL, BOARD_KEYD_GPIO_INDEX, BOARD_KEYD_GPIO_PIN);
        s_drv_irq_keyd_cb();
    }
}

SDK_DECLARE_EXT_ISR_M(DRV_IRQ_KEY_IRQ0, drv_irq_key_isr0);

void drv_irq_keya_init(drv_irq_cb_func cb)
{
    s_drv_irq_keya_cb = cb;
    gpio_config_pin_interrupt(BOARD_KEYA_GPIO_CTRL, BOARD_KEYA_GPIO_INDEX, BOARD_KEYA_GPIO_PIN, gpio_interrupt_trigger_edge_falling);
    gpio_enable_pin_interrupt(BOARD_KEYA_GPIO_CTRL, BOARD_KEYA_GPIO_INDEX, BOARD_KEYA_GPIO_PIN);
    intc_m_enable_irq_with_priority(DRV_IRQ_KEYA_IRQ, DRV_IRQ_KEYA_PRIORITY);
}

void drv_irq_keyb_init(drv_irq_cb_func cb)
{
    s_drv_irq_keyb_cb = cb;
    gpio_config_pin_interrupt(BOARD_KEYB_GPIO_CTRL, BOARD_KEYB_GPIO_INDEX, BOARD_KEYB_GPIO_PIN, gpio_interrupt_trigger_edge_falling);
    gpio_enable_pin_interrupt(BOARD_KEYB_GPIO_CTRL, BOARD_KEYB_GPIO_INDEX, BOARD_KEYB_GPIO_PIN);
    intc_m_enable_irq_with_priority(DRV_IRQ_KEYB_IRQ, DRV_IRQ_KEYB_PRIORITY);
}

void drv_irq_keyc_init(drv_irq_cb_func cb)
{
    s_drv_irq_keyc_cb = cb;
    gpio_config_pin_interrupt(BOARD_KEYC_GPIO_CTRL, BOARD_KEYC_GPIO_INDEX, BOARD_KEYC_GPIO_PIN, gpio_interrupt_trigger_edge_falling);
    gpio_enable_pin_interrupt(BOARD_KEYC_GPIO_CTRL, BOARD_KEYC_GPIO_INDEX, BOARD_KEYC_GPIO_PIN);
    intc_m_enable_irq_with_priority(DRV_IRQ_KEYC_IRQ, DRV_IRQ_KEYC_PRIORITY);
}

void drv_irq_keyd_init(drv_irq_cb_func cb)
{
    s_drv_irq_keyd_cb = cb;
    gpio_config_pin_interrupt(BOARD_KEYD_GPIO_CTRL, BOARD_KEYD_GPIO_INDEX, BOARD_KEYD_GPIO_PIN, gpio_interrupt_trigger_edge_falling);
    gpio_enable_pin_interrupt(BOARD_KEYD_GPIO_CTRL, BOARD_KEYD_GPIO_INDEX, BOARD_KEYD_GPIO_PIN);
    intc_m_enable_irq_with_priority(DRV_IRQ_KEYD_IRQ, DRV_IRQ_KEYD_PRIORITY);
}

void drv_irq_key_enable(drv_key_id_e id)
{
    switch (id)
    {
        case DRV_KEY_KEYA_ID:
            intc_m_enable_irq_with_priority(DRV_IRQ_KEYA_IRQ, DRV_IRQ_KEYA_PRIORITY);
        break;
        case DRV_KEY_KEYB_ID:
            intc_m_enable_irq_with_priority(DRV_IRQ_KEYB_IRQ, DRV_IRQ_KEYB_PRIORITY);
        break;
        case DRV_KEY_KEYC_ID:
            intc_m_enable_irq_with_priority(DRV_IRQ_KEYC_IRQ, DRV_IRQ_KEYC_PRIORITY);
        break;
        case DRV_KEY_KEYD_ID:
            intc_m_enable_irq_with_priority(DRV_IRQ_KEYD_IRQ, DRV_IRQ_KEYD_PRIORITY);
        break;
        default:
        break;
    }
}

void drv_irq_key_disable(drv_key_id_e id)
{
    switch (id)
    {
        case DRV_KEY_KEYA_ID:
            intc_m_disable_irq(DRV_IRQ_KEYA_IRQ);
        break;
        case DRV_KEY_KEYB_ID:
            intc_m_disable_irq(DRV_IRQ_KEYB_IRQ);
        break;
        case DRV_KEY_KEYC_ID:
            intc_m_disable_irq(DRV_IRQ_KEYC_IRQ);
        break;
        case DRV_KEY_KEYD_ID:
            intc_m_disable_irq(DRV_IRQ_KEYD_IRQ);
        break;
        default:
        break;
    }
}
