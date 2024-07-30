/*
 *Copyright (c) 2022 HPMicro
 *
 *SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_PINMUX_H
#define HPM_PINMUX_H

#ifdef __cplusplus
extern "C" {
#endif
void init_uart_pins(UART_Type *ptr);
void init_uart_pin_as_gpio(UART_Type *ptr);
void init_adc_pins(void);
#ifdef __cplusplus
}
#endif
#endif /* HPM_PINMUX_H */
