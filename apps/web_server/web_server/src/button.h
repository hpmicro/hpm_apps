/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef WEB_SERVER_BUTTON_BUTTON_H
#define WEB_SERVER_BUTTON_BUTTON_H

#include <stdint.h>
#include <stdbool.h>
#include "hpm_segment_led.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* External variables for button state and segment LED */
extern const uint8_t s_disp_code_8_seg[];
extern uint8_t gpio_number[6];
extern volatile uint8_t di_state;

/* Function declarations */
void init_eui_config(void);
void seg_led_disp_config(void);
void update_gpio_number(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* WEB_SERVER_BUTTON_BUTTON_H */