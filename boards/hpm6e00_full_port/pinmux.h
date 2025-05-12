/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file pinmux.h
 * @version 1.0
 * @author zhaoshuai (shuai.zhao@hpmicro.com)
 * @date 2024-07-30
 * @brief 
 */
#ifndef HPM_PINMUX_H
#define HPM_PINMUX_H

#include "hpm_uart_drv.h"

#ifdef __cplusplus
extern "C" {
#endif
void init_uart_pins(UART_Type *ptr);
void init_led_pins_as_gpio(void);
void init_key_pins(void);
void init_lcd_pins(void);
void init_esc_pins(void);
void init_esc_in_out_pin(void);
void init_tsw_pins(void);

#ifdef __cplusplus
}
#endif
#endif /* HPM_PINMUX_H */
