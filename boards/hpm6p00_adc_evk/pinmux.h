/*
 * Copyright (c) 2024 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_PINMUX_H
#define HPM_PINMUX_H

#ifdef __cplusplus
extern "C" {
#endif
void init_uart_pins(UART_Type *ptr);
void init_uart_pin_as_gpio(UART_Type *ptr);
void init_femc_pins(void);
void init_ppi_pins(void);
void init_sdm_pins(void);
void init_pwm_pin_as_sdm_clock(void);
void init_gpio_pins(void);

void init_gptmr_pins(GPTMR_Type *ptr);
void init_hall_trgm_pins(void);
void init_qei_trgm_pins(void);
void init_butn_pins(void);
void init_acmp_pins(void);
void init_pwm_pins(PWMV2_Type *ptr);
void init_usb_pins(USB_Type *ptr);

void init_adc16_pins(void);
void init_dac_pins(DAC_Type *ptr);
void init_adc_bldc_pins(void);
void init_adc_qeiv2_pins(void);

void init_pwm_fault_pins(void);
void init_uart_break_signal_pin(void);
void init_gptmr_channel_pin(GPTMR_Type *ptr, uint32_t channel, bool as_comp);


#ifdef __cplusplus
}
#endif
#endif /* HPM_PINMUX_H */
