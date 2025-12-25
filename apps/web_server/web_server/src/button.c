/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "button.h"
#include "board.h"
#include "hpm_clock_drv.h"

const uint8_t s_disp_code_8_seg[] = BOARD_EUI_SEG_ENCODE_DATA;
uint32_t s_eui_clock_freq;
volatile bool s_key_changed;
volatile bool s_esc_key_pressed;
volatile bool s_up_key_pressed;
volatile bool s_enter_key_pressed;
volatile bool s_left_key_pressed;
volatile bool s_down_key_pressed;
volatile bool s_right_key_pressed;
/* Digital input states corresponding to 6 keys: gpio_number[0-5] mapped to IN1-IN6 */
uint8_t gpio_number[6] = { 0 };
/* Persistent DI state (bits 0..5 correspond to IN1..IN6). Each key press toggles the corresponding bit. */
volatile uint8_t di_state = 0;
uint8_t s_disp_mode;
uint8_t s_disp_data[3];
uint8_t s_data_sel;

enum gpio_pos {
    esc_key,
    up_key,
    enter_key,
    left_key,
    down_key,
    right_key,
};

void init_eui_config(void)
{
    clock_add_to_group(BOARD_EUI_CLOCK_NAME, 0);
    s_eui_clock_freq = clock_get_frequency(BOARD_EUI_CLOCK_NAME);

    eui_ctrl_config_t ctrl_config;

    eui_get_default_ctrl_config(BOARD_EUI, &ctrl_config);
    ctrl_config.work_mode = eui_work_mode_8x8;
    ctrl_config.clko_freq_khz = 100;
    ctrl_config.key_filter_ms = 50;
    ctrl_config.disp_data_invert = 0xFF;
    ctrl_config.scan_invert = 0x00;
    ctrl_config.dedicate_out_cfg = BOARD_EUI_DEDICATE_OUT_LINES;
    ctrl_config.dedicate_in_cfg = 0x0000;
    eui_config_ctrl(BOARD_EUI, s_eui_clock_freq, &ctrl_config);

    printf("clko_tm_us:%d, slot_tm_us:%d, hold_tm_us:%d, disp_tm_us:%d, filter_tm_us:%d\n\n", eui_get_time_us(BOARD_EUI, s_eui_clock_freq, eui_clko_time),
           eui_get_time_us(BOARD_EUI, s_eui_clock_freq, eui_slot_time), eui_get_time_us(BOARD_EUI, s_eui_clock_freq, eui_hold_time),
           eui_get_time_us(BOARD_EUI, s_eui_clock_freq, eui_disp_time), eui_get_time_us(BOARD_EUI, s_eui_clock_freq, eui_filter_time));

    eui_set_irq_enable(BOARD_EUI, eui_irq_area_mask);
    intc_m_enable_irq_with_priority(BOARD_EUI_IRQ, 1);

    eui_set_enable(BOARD_EUI, true);
}

void seg_led_disp_config(void)
{
    segment_led_config_eui_instance(BOARD_EUI, s_eui_clock_freq);
    segment_led_config_blink_period(500, 500);
    segment_led_config_disp_blink(0, BOARD_EUI_SEG_DP_BIT_MASK);
    segment_led_config_disp_blink(1, BOARD_EUI_SEG_DP_BIT_MASK);
    segment_led_config_disp_blink(2, BOARD_EUI_SEG_DP_BIT_MASK);
    segment_led_config_disp_blink(3, BOARD_EUI_SEG_DP_BIT_MASK);
    segment_led_config_disp_blink(4, BOARD_EUI_SEG_DP_BIT_MASK);
    s_disp_mode = 1;
    s_data_sel = 0;
}

void update_gpio_number(void)
{
    if (eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_ESC_KEY_ROW, BOARD_EUI_ESC_KEY_COL)) {
        gpio_number[esc_key] = 1;
    } else if (eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_UP_KEY_ROW, BOARD_EUI_UP_KEY_COL)) {
        gpio_number[up_key] = 1;
    } else if (eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_ENTER_KEY_ROW, BOARD_EUI_ENTER_KEY_COL)) {
        gpio_number[enter_key] = 1;
    } else if (eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_LEFT_KEY_ROW, BOARD_EUI_LEFT_KEY_COL)) {
        gpio_number[left_key] = 1;
    } else if (eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_DOWN_KEY_ROW, BOARD_EUI_DOWN_KEY_COL)) {
        gpio_number[down_key] = 1;
    } else if (eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_RIGHT_KEY_ROW, BOARD_EUI_RIGHT_KEY_COL)) {
        gpio_number[right_key] = 1;
    }
}
