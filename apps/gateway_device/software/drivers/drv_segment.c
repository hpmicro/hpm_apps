/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "drv_segment.h"

static const uint8_t s_disp_code_8_seg[] = BOARD_EUI_SEG_ENCODE_DATA;
static uint32_t s_eui_clock_freq;
static volatile bool s_key_changed;
static volatile bool s_esc_key_pressed;
static volatile bool s_up_key_pressed;
static volatile bool s_enter_key_pressed;
static volatile bool s_left_key_pressed;
static volatile bool s_down_key_pressed;
static volatile bool s_right_key_pressed;

drv_irq_cb_func cb_key_down = NULL;

static void init_eui_config(void)
{
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

    eui_set_irq_enable(BOARD_EUI, eui_irq_area_mask);
    intc_m_enable_irq_with_priority(BOARD_EUI_IRQ, 1);

    eui_set_enable(BOARD_EUI, true);
}

void drv_segment_ctrl(uint8_t ge, uint8_t shi, uint8_t bai, uint8_t qian, uint8_t wan)
{
    eui_scan_disp_data_t disp_data;
    eui_disp_config_t disp_config;

    disp_data.data_8x8[0] = s_disp_code_8_seg[wan];
    disp_data.data_8x8[1] = s_disp_code_8_seg[qian];
    disp_data.data_8x8[2] = s_disp_code_8_seg[bai];
    disp_data.data_8x8[3] = s_disp_code_8_seg[shi];
    disp_data.data_8x8[4] = s_disp_code_8_seg[ge];
    eui_set_scan_disp_data(BOARD_EUI, eui_disp_data_idx_a, &disp_data);
    eui_config_disp(BOARD_EUI, s_eui_clock_freq, eui_disp_ctrl_idx_ab, &disp_config);
}

void drv_segment_init(drv_irq_cb_func cb)
{

    init_eui_pins(BOARD_EUI);

    clock_add_to_group(BOARD_EUI_CLOCK_NAME, 0);
    s_eui_clock_freq = clock_get_frequency(BOARD_EUI_CLOCK_NAME);
    init_eui_config();
    drv_segment_ctrl(0, 0, 0, 0, 0);

    cb_key_down = cb;
}

SDK_DECLARE_EXT_ISR_M(BOARD_EUI_IRQ, eui_isr)
void eui_isr(void)
{
    if ((eui_get_irq_status(BOARD_EUI) & eui_irq_area_mask) == eui_irq_area_mask) {
        eui_clear_irq_flag(BOARD_EUI, eui_irq_area_mask);
        s_esc_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_ESC_KEY_ROW, BOARD_EUI_ESC_KEY_COL);
        s_up_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_UP_KEY_ROW, BOARD_EUI_UP_KEY_COL);
        s_enter_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_ENTER_KEY_ROW, BOARD_EUI_ENTER_KEY_COL);
        s_left_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_LEFT_KEY_ROW, BOARD_EUI_LEFT_KEY_COL);
        s_down_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_DOWN_KEY_ROW, BOARD_EUI_DOWN_KEY_COL);
        s_right_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_RIGHT_KEY_ROW, BOARD_EUI_RIGHT_KEY_COL);
        s_key_changed = true;
        if(s_down_key_pressed && cb_key_down) {
            cb_key_down();
        }
    }
}
