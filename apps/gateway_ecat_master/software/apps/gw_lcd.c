/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "gw_lcd.h"


// 全局界面组件
static lv_obj_t *ui_status_bar;
static lv_obj_t *ui_bottom_bar;
static lv_obj_t *ui_main_content;
static lv_obj_t *ui_set_content;
static lv_obj_t *ui_data_content;
static lv_obj_t *ui_time_label;
static lv_obj_t *ui_master_label;
static lv_obj_t *ui_batt_label;
static lv_obj_t *btn_main;
static lv_obj_t *btn_set;
static lv_obj_t *btn_data;
static lv_obj_t *table_set;
static int32_t table_set_row = 0;
static int32_t table_set_col = 1;
static lv_obj_t *table_data;
static int32_t table_data_row = 1;
static int32_t table_data_col = 0;
static uint32_t clock0_time_div;
static lv_obj_t *lbl_slaves_num;
lv_obj_t *lbl_state;
lv_obj_t *lbl_txnum;
lv_obj_t *lbl_rxnum;
lv_obj_t *lbl_mode;
lv_obj_t *lbl_start;

static uint32_t last_baudrate = 0;
static char* last_mode = "UN";
static uint8_t mode_index = 0;
static uint32_t last_cycle_time = 0;
static char* last_frame_mode = "UN";
static uint8_t frame_mode_index = 0;

static uint32_t baudrate_table[9] = {1000000, 800000, 500000, 250000, 125000, 100000, 50000, 20000, 10000};
char* mode_table[2] = {"Trigger", "Cycle"};
char* frame_mode_table[2] = {"Standard", "Extended"};

uint8_t last_can_rx_len = 0;
uint32_t last_can_rx_id = 0;
uint8_t last_can_rx_rtr = 0;
uint8_t last_can_rx_id_flag = 0;
uint8_t last_can_rx_data[8] = {0};

extern gw_can_tx_std can_tx_std;
extern gw_can_tx_ext can_tx_ext;
extern gw_can_rx_std can_rx_std;
extern gw_can_rx_ext can_rx_ext;

typedef enum {
    GW_LCD_PAGE_SETTINGS=0,
    GW_LCD_PAGE_MAIN,
    GW_LCD_PAGE_DATA
} gw_lcd_ui_page_t;

static gw_lcd_ui_page_t current_page = GW_LCD_PAGE_MAIN;

static void gw_lcd_update_time() {
    static uint8_t c = 0;
    c++;
    if(c >= 10) { //100x10ms
        c = 0;
        uint64_t t = hpm_csr_get_core_cycle()/clock0_time_div/1000000;
        static uint8_t h = 23, m = 59, s = 50;
        s = t % 60;
        m = t / 60 % 60;
        h = t / 3600 % 24;
        char buf[9];
        sprintf(buf, "%02d:%02d:%02d", h, m, s);
        lv_label_set_text(ui_time_label, buf);
    }

    if(lv_strcmp(lv_table_get_cell_value(table_set, 1, 1), lv_label_get_text(lbl_mode))) {
        lv_label_set_text(lbl_mode, lv_table_get_cell_value(table_set, 1, 1));
    }
    gw_mq_msg mq_msg;
    if(lv_strcmp(lv_label_get_text(lbl_state), "PREOP") == 0) {
        //设置can 波特率
        if(last_baudrate != atoi(lv_table_get_cell_value(table_set, 0, 1))) {
            last_baudrate = atoi(lv_table_get_cell_value(table_set, 0, 1));
            mq_msg.msg_id = GW_MQ_MSG_ECAT_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_ECAT_SET_CAN_BAUDRATE_ID;
            switch (last_baudrate)
            {
                case 1000000:
                    mq_msg.param = 0;
                    break;
                case 800000:
                    mq_msg.param = 1;
                    break;
                case 500000:
                    mq_msg.param = 2;
                    break;
                case 250000:
                    mq_msg.param = 3;
                    break;
                case 125000:
                    mq_msg.param = 4;
                    break;
                case 100000:
                    mq_msg.param = 5;
                    break;
                case 50000:
                    mq_msg.param = 6;
                    break;
                case 20000:
                    mq_msg.param = 7;
                    break;
                case 10000:
                    mq_msg.param = 8;
                    break;
                default:
                    mq_msg.param = 0;
                    break;
            }
            gw_mq_send(GW_MQ_ECAT_ID, &mq_msg, 10);
        }
        if(lv_strcmp(last_mode, mode_table[mode_index%2])) {
            last_mode = mode_table[mode_index%2];
            mq_msg.msg_id = GW_MQ_MSG_ECAT_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_ECAT_SET_MODE_ID;
            if(lv_strcmp(last_mode, mode_table[0]) == 0) {
                mq_msg.param = true;
            } else {
                mq_msg.param = false;
            }
            gw_mq_send(GW_MQ_ECAT_ID, &mq_msg, 10);
        }
        if(last_cycle_time != atoi(lv_table_get_cell_value(table_set, 2, 1))) {
            last_cycle_time = atoi(lv_table_get_cell_value(table_set, 2, 1));
            mq_msg.msg_id = GW_MQ_MSG_ECAT_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_ECAT_SET_CYCLE_TIME_ID;
            mq_msg.param = last_cycle_time;
            gw_mq_send(GW_MQ_ECAT_ID, &mq_msg, 10);
        }
        if(lv_strcmp(last_frame_mode, frame_mode_table[frame_mode_index%2])) {
            last_frame_mode = frame_mode_table[frame_mode_index%2];
            mq_msg.msg_id = GW_MQ_MSG_ECAT_ID;
            mq_msg.submsg_id = GW_MQ_SUBMSG_ECAT_SET_FRAME_MODE_ID;
            if(lv_strcmp(last_frame_mode, frame_mode_table[0]) == 0) {
                mq_msg.param = false;
            } else {
                mq_msg.param = true;
            }
            gw_mq_send(GW_MQ_ECAT_ID, &mq_msg, 10);
        }
    }


    if(lv_strcmp(frame_mode_table[0], frame_mode_table[frame_mode_index%2]) == 0) { //standard frame mode
        can_tx_std.tx_num = atoi(lv_table_get_cell_value(table_data, 1, 0));
        can_tx_std.dlc = atoi(lv_table_get_cell_value(table_data, 1, 1));
        can_tx_std.id = atoi(lv_table_get_cell_value(table_data, 1, 2));
        can_tx_std.rtr = atoi(lv_table_get_cell_value(table_data, 1, 3));
        for(int i = 0; i < 8; i++) {
            can_tx_std.data[i] = atoi(lv_table_get_cell_value(table_data, 3, i+1));
        }
        memset((uint8_t*)&can_tx_ext, 0, sizeof(can_tx_ext));

        if(last_can_rx_len != can_rx_std.dlc) {
            last_can_rx_len = can_rx_std.dlc;
            lv_table_set_cell_value_fmt(table_data, 1, 5, "%d", last_can_rx_len);
        }
        if(last_can_rx_id != can_rx_std.id) {
            last_can_rx_id = can_rx_std.id;
            lv_table_set_cell_value_fmt(table_data, 1, 6, "%d", last_can_rx_id);
        }
        if(last_can_rx_rtr != can_rx_std.rtr) {
            last_can_rx_rtr = can_rx_std.rtr;
            lv_table_set_cell_value_fmt(table_data, 1, 7, "%d", last_can_rx_rtr);
        }
        for(int i = 0; i < last_can_rx_len; i++) {
            if(last_can_rx_data[i] != can_rx_std.data[i]) {
                last_can_rx_data[i] = can_rx_std.data[i];
                lv_table_set_cell_value_fmt(table_data, 4, i+1, "%d", last_can_rx_data[i]);
            }
        }
    } else if(lv_strcmp(frame_mode_table[1], frame_mode_table[frame_mode_index%2]) == 0) { //extended frame mode
        can_tx_ext.tx_num = atoi(lv_table_get_cell_value(table_data, 1, 0));
        can_tx_ext.dlc = atoi(lv_table_get_cell_value(table_data, 1, 1));
        can_tx_ext.id = atoi(lv_table_get_cell_value(table_data, 1, 2));
        can_tx_ext.rtr = atoi(lv_table_get_cell_value(table_data, 1, 3));
        can_tx_ext.std_ext_bit = atoi(lv_table_get_cell_value(table_data, 1, 4));
        for(int i = 0; i < 8; i++) {
            can_tx_ext.data[i] = atoi(lv_table_get_cell_value(table_data, 3, i+1));
        }
        memset((uint8_t*)&can_tx_std, 0, sizeof(can_tx_std));

        if(last_can_rx_len != can_rx_ext.dlc) {
            last_can_rx_len = can_rx_ext.dlc;
            lv_table_set_cell_value_fmt(table_data, 1, 5, "%d", last_can_rx_len);
        }
        if(last_can_rx_id != can_rx_ext.id) {
            last_can_rx_id = can_rx_ext.id;
            lv_table_set_cell_value_fmt(table_data, 1, 6, "%d", last_can_rx_id);
        }
        if(last_can_rx_rtr != can_rx_ext.rtr) {
            last_can_rx_rtr = can_rx_ext.rtr;
            lv_table_set_cell_value_fmt(table_data, 1, 7, "%d", last_can_rx_rtr);
        }
        if(last_can_rx_id_flag != can_rx_ext.std_ext_bit) {
            last_can_rx_id_flag = can_rx_ext.std_ext_bit;
            lv_table_set_cell_value_fmt(table_data, 1, 8, "%d", last_can_rx_id_flag);
        }
        for(int i = 0; i < last_can_rx_len; i++) {
            if(last_can_rx_data[i] != can_rx_ext.data[i]) {
                last_can_rx_data[i] = can_rx_ext.data[i];
                lv_table_set_cell_value_fmt(table_data, 4, i+1, "%d", last_can_rx_data[i]);
            }
        }
    }
}

/* 页面切换函数 */
static void gw_lcd_switch_page(gw_lcd_ui_page_t page) {
    if(current_page == page) return;
    /* 隐藏所有页面 */
    lv_obj_add_flag(ui_main_content, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_set_content, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_data_content, LV_OBJ_FLAG_HIDDEN);
    
    /* 显示选中的页面 */
    switch(page) {
        case GW_LCD_PAGE_SETTINGS:
            lv_obj_clear_flag(ui_set_content, LV_OBJ_FLAG_HIDDEN);
            break;
        case GW_LCD_PAGE_MAIN:
            lv_obj_clear_flag(ui_main_content, LV_OBJ_FLAG_HIDDEN);
            break;
        case GW_LCD_PAGE_DATA:
            lv_obj_clear_flag(ui_data_content, LV_OBJ_FLAG_HIDDEN);
            break;
        default:
            break;
    }
    
    current_page = page;
}

uint32_t gw_lcd_table_set_baudrate(uint8_t flag) {
    static int8_t baudrate_index = 0;
    if(flag) {
        baudrate_index++;
        if(baudrate_index>8) baudrate_index = 0;
    } else {
        baudrate_index--;
        if(baudrate_index<0) baudrate_index = 8;
    }
    return baudrate_table[baudrate_index%9];
}


static char* gw_lcd_table_set_mode(uint8_t flag) {
    if(flag) {
        mode_index++;
    } else {
        mode_index--;
    }
    return mode_table[mode_index%2];
}

static char* gw_lcd_table_set_frame_mode(uint8_t flag) {  
    if(flag) {
        frame_mode_index++;
    } else {
        frame_mode_index--;
    }
    return frame_mode_table[frame_mode_index%2];
}

static void gw_lcd_main_set_slaves_num(uint8_t num)
{
    lv_label_set_text_fmt(lbl_slaves_num, "%d", num);
}

static void gw_lcd_main_set_start(void)
{
    gw_mq_msg mq_msg;

    lv_label_set_text(lbl_start, LV_SYMBOL_STOP);
    mq_msg.msg_id = GW_MQ_MSG_ECAT_ID;
    mq_msg.submsg_id = GW_MQ_SUBMSG_ECAT_START_ID;
    gw_mq_send(GW_MQ_ECAT_ID, &mq_msg, 0);
    lv_obj_set_style_text_color(lbl_start, lv_color_hex(0x00ff00), LV_PART_MAIN);

}

static void gw_lcd_main_set_stop(void)
{
    gw_mq_msg mq_msg;

    lv_label_set_text(lbl_start, LV_SYMBOL_PLAY);
    mq_msg.msg_id = GW_MQ_MSG_ECAT_ID;
    mq_msg.submsg_id = GW_MQ_SUBMSG_ECAT_STOP_ID;
    gw_mq_send(GW_MQ_ECAT_ID, &mq_msg, 0);
    lv_obj_set_style_text_color(lbl_start, lv_color_hex(0x000000), LV_PART_MAIN);
}

static void gw_lcd_main_set_state(uint8_t state)
{
    switch(state) {
        case 0x01:
            lv_label_set_text(lbl_state, "INIT");
            last_baudrate = 0;
            last_mode = "-";
            last_cycle_time = 0;
            last_frame_mode = "-";
            gw_lcd_switch_page(GW_LCD_PAGE_MAIN);
            break;
        case 0x02:
            lv_label_set_text(lbl_state, "PREOP");
            break;
        case 0x03:
            lv_label_set_text(lbl_state, "BOOT");
            break;
        case 0x04:
            lv_label_set_text(lbl_state, "SAFEOP");
            break;
        case 0x08:
            lv_label_set_text(lbl_state, "OP");
            break;
        default:
            lv_label_set_text(lbl_state, "UNKNOWN");
            break;
    }
}

static void gw_lcd_main_set_txnum(uint32_t num)
{
    lv_label_set_text_fmt(lbl_txnum, "%d", num);
}

static void gw_lcd_main_set_rxnum(uint32_t num)
{
    lv_label_set_text_fmt(lbl_rxnum, "%d", num);
}

static void gw_lcd_set_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        if(current_page == GW_LCD_PAGE_MAIN) {

        } else if(current_page == GW_LCD_PAGE_SETTINGS) {
            table_set_row--;
            if(table_set_row < 0) {
                table_set_row = 0;
            }
            lv_obj_send_event(table_set, LV_EVENT_CLICKED, NULL);
        } else if(current_page == GW_LCD_PAGE_DATA) {
            if(table_data_row == 1) {
                table_data_col--;
                if (table_data_col < 0)
                {
                    table_data_col = 0;
                }
            } else {
                table_data_col--;
                if (table_data_col < 1)
                {
                    table_data_row = 1;
                    table_data_col = 4;
                }
            }
            lv_obj_send_event(table_data, LV_EVENT_CLICKED, NULL);
        }
    } else if(code == LV_EVENT_LONG_PRESSED) {
        if(lv_strcmp(lv_label_get_text(lbl_state), "PREOP") == 0) {
            gw_lcd_switch_page(GW_LCD_PAGE_SETTINGS);
            lv_obj_set_style_bg_color(btn_set, lv_color_hex(0xcc0066), LV_PART_MAIN);
            lv_obj_set_style_bg_color(btn_main, lv_color_hex(0x4a5568), LV_PART_MAIN);
            lv_obj_set_style_bg_color(btn_data, lv_color_hex(0x4a5568), LV_PART_MAIN);
        }
    }
}

static void gw_lcd_main_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    gw_mq_msg mq_msg;
    
    if(code == LV_EVENT_CLICKED) {
        if(current_page == GW_LCD_PAGE_MAIN) {

        } else if(current_page == GW_LCD_PAGE_SETTINGS) {
            if(table_set_row == 0 && table_set_col == 1) {
                //set baudrate
                lv_table_set_cell_value_fmt(table_set, 0, 1, "%d", gw_lcd_table_set_baudrate(true));
            } else if(table_set_row == 1 && table_set_col == 1) {
                //set mode
                lv_table_set_cell_value(table_set, 1, 1, gw_lcd_table_set_mode(true));
            } else if(table_set_row == 2 && table_set_col == 1) {
                    //set cycle time
                    char *cell_value = lv_table_get_cell_value(table_set, table_set_row, table_set_col);
                    if(cell_value != NULL) {
                        int32_t data = atoi(cell_value);
                        data--;
                        if(data<0) data = 0;
                        lv_table_set_cell_value_fmt(table_set, table_set_row, table_set_col, "%d", data);
                    }
            } else if(table_set_row == 3 && table_set_col == 1) {
                //set frame mode
                lv_table_set_cell_value(table_set, 3, 1, gw_lcd_table_set_frame_mode(true));
            }
        } else if(current_page == GW_LCD_PAGE_DATA) {
            lv_table_get_selected_cell(table_data, &table_data_row, &table_data_col);
            char *cell_value = lv_table_get_cell_value(table_data, table_data_row, table_data_col);
            if(cell_value != NULL) {
                int32_t data = atoi(cell_value);
                data--;
                if(data<0) data = 0;
                lv_table_set_cell_value_fmt(table_data, table_data_row, table_data_col, "%d", data);
                if(lv_strcmp(frame_mode_table[0], frame_mode_table[frame_mode_index%2]) == 0) { //standard frame mode
                    if(table_data_row == 1) {
                        if(table_data_col == 0) {
                            can_tx_std.tx_num = data;
                        } else if(table_data_col == 1) {
                            can_tx_std.dlc = data;
                        } else if(table_data_col == 2) {
                            can_tx_std.id = data;
                        } else if(table_data_col == 3){
                            can_tx_std.rtr = data;
                        }
                    } else if(table_data_row == 3) {
                        can_tx_std.data[table_data_col-1] = data;
                    }
                } else if(lv_strcmp(frame_mode_table[1], frame_mode_table[frame_mode_index%2]) == 0) { //extended frame mode
                    if(table_data_row == 1) {
                        if(table_data_col == 0) {
                            can_tx_ext.tx_num = data;
                        } else if(table_data_col == 1) {
                            can_tx_ext.dlc = data;
                        } else if(table_data_col == 2) {
                            can_tx_ext.id = data;
                        } else if(table_data_col == 3){
                            can_tx_ext.rtr = data;
                        } else if(table_data_col == 4) {
                            can_tx_ext.std_ext_bit = data;
                        }
                    } else if(table_data_row == 3) {
                        can_tx_ext.data[table_data_col-1] = data;
                    }
                }
            }
        }
    } else if(code == LV_EVENT_LONG_PRESSED) {
        gw_lcd_switch_page(GW_LCD_PAGE_MAIN);
        lv_obj_set_style_bg_color(btn_set, lv_color_hex(0x4a5568), LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn_main, lv_color_hex(0xcc0066), LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn_data, lv_color_hex(0x4a5568), LV_PART_MAIN);
    }
}

static void gw_lcd_data_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        if(current_page == GW_LCD_PAGE_MAIN) {
            if(lv_strcmp(lv_label_get_text(lbl_state), "PREOP") == 0) {
                gw_lcd_main_set_start();
            } else if(lv_strcmp(lv_label_get_text(lbl_state), "OP") == 0) {
                gw_lcd_main_set_stop();
            }
        } else if(current_page == GW_LCD_PAGE_SETTINGS) {
            table_set_row++;
            if(table_set_row > lv_table_get_row_cnt(table_set) - 1) {
                table_set_row = lv_table_get_row_cnt(table_set) - 1;
            }
            lv_obj_send_event(table_set, LV_EVENT_CLICKED, NULL);
        } else if(current_page == GW_LCD_PAGE_DATA) {
            if(table_data_row == 1) {
                table_data_col++;
                if (table_data_col > 4)
                {
                    table_data_row = 3;
                    table_data_col = 1;
                }
            } else {
                table_data_col++;
                if (table_data_col > 8)
                {
                    table_data_col = 8;
                }
            }
            lv_obj_send_event(table_data, LV_EVENT_CLICKED, NULL);
        }
    } else if(code == LV_EVENT_LONG_PRESSED) {
        gw_lcd_switch_page(GW_LCD_PAGE_DATA);
        lv_obj_set_style_bg_color(btn_set, lv_color_hex(0x4a5568), LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn_main, lv_color_hex(0x4a5568), LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn_data, lv_color_hex(0xcc0066), LV_PART_MAIN);
    }
}

static void gw_lcd_table_set_event_handler(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        // 获取当前选中的单元格位置
        // lv_table_get_selected_cell(table_set, &table_set_row, &table_set_col);
        // LOG_I("选中单元格: 行=%d, 列=%d\r\n", table_set_row, table_set_col);
    } else if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        // 计算选中单元格的ID（LVGL内部使用行列组合的ID标识单元格）
        lv_table_set_selected_cell(table_set, table_set_row, table_set_col);
        lv_obj_set_state(table_set, LV_STATE_FOCUSED, true);
    }
}

static void gw_lcd_table_data_event_handler(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {

    } else if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        // 计算选中单元格的ID（LVGL内部使用行列组合的ID标识单元格）
        lv_table_set_selected_cell(table_data, table_data_row, table_data_col);
        lv_obj_set_state(table_data, LV_STATE_FOCUSED, true);
    }
}

/********************************create ui************************************/

static void gw_lcd_create_status_bar(void) {
    ui_status_bar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_status_bar, LCD_W, STATUS_BAR_H);
    lv_obj_set_pos(ui_status_bar, 0, 0);
    lv_obj_set_style_bg_color(ui_status_bar, lv_color_hex(0x2d3748), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_status_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(ui_status_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ui_status_bar, 1, LV_PART_MAIN); 
    
    // 时间显示
    ui_time_label = lv_label_create(ui_status_bar);
    lv_obj_set_style_text_color(ui_time_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_time_label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(ui_time_label, LV_ALIGN_LEFT_MID, 8, 0);
    lv_label_set_text(ui_time_label, "00:00:00");
    
    // 标题显示
    ui_master_label = lv_label_create(ui_status_bar);
    lv_obj_set_style_text_color(ui_master_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_master_label, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_align(ui_master_label, LV_ALIGN_LEFT_MID, 85, 0);
    lv_label_set_text(ui_master_label, "Ethercat Master");

    // 电池显示
    ui_batt_label = lv_label_create(ui_status_bar);
    lv_obj_set_style_text_color(ui_batt_label, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_text_font(ui_batt_label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(ui_batt_label, LV_ALIGN_RIGHT_MID, -8, 0);
    lv_label_set_text(ui_batt_label, LV_SYMBOL_BATTERY_FULL);

    clock0_time_div = clock_get_frequency(clock_cpu0) / 1000000;
}


static void gw_lcd_create_bottom_bar(void) {
    ui_bottom_bar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_bottom_bar, LCD_W, BOTTOM_BAR_H);
    lv_obj_set_pos(ui_bottom_bar, 0, LCD_H - BOTTOM_BAR_H);
    lv_obj_set_style_bg_color(ui_bottom_bar, lv_color_hex(0x2d3748), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_bottom_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(ui_bottom_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ui_bottom_bar, 0, LV_PART_MAIN);       // 移除内边距

    // 设置按钮
    btn_set = lv_btn_create(ui_bottom_bar);
    lv_obj_set_size(btn_set, LCD_W / 3, BOTTOM_BAR_H);
    lv_obj_set_pos(btn_set, 0, 0);
    lv_obj_set_style_radius(btn_set, 3, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn_set, lv_color_hex(0x4a5568), LV_PART_MAIN);
    lv_obj_set_style_text_font(btn_set, &lv_font_montserrat_32, LV_PART_MAIN);

    lv_obj_t *lbl_set = lv_label_create(btn_set);
    lv_label_set_text(lbl_set, LV_SYMBOL_SETTINGS);
    lv_obj_center(lbl_set);
    
    lv_obj_add_event_cb(btn_set, gw_lcd_set_event_handler, LV_EVENT_ALL, NULL);    

    // 主页面按钮
    btn_main = lv_btn_create(ui_bottom_bar);
    lv_obj_set_size(btn_main, LCD_W / 3, BOTTOM_BAR_H);
    lv_obj_set_pos(btn_main, LCD_W / 3 + 1, 0);
    lv_obj_set_style_radius(btn_main, 3, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn_main, lv_color_hex(0xcc0066), LV_PART_MAIN);
    
    lv_obj_t *lbl_main = lv_label_create(btn_main);
    lv_obj_set_style_text_font(lbl_main, &lv_font_montserrat_32, LV_PART_MAIN);
    lv_label_set_text(lbl_main, LV_SYMBOL_HOME);
    lv_obj_center(lbl_main);

    lv_obj_add_event_cb(btn_main, gw_lcd_main_event_handler, LV_EVENT_ALL, NULL);

    // 数据按钮
    btn_data = lv_btn_create(ui_bottom_bar);
    lv_obj_set_size(btn_data, LCD_W / 3, BOTTOM_BAR_H);
    lv_obj_set_pos(btn_data, 2 * LCD_W / 3 + 1, 0);
    lv_obj_set_style_radius(btn_data, 3, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn_data, lv_color_hex(0x4a5568), LV_PART_MAIN);
    lv_obj_set_style_text_font(btn_data, &lv_font_montserrat_32, LV_PART_MAIN);
    
    lv_obj_t *lbl_data = lv_label_create(btn_data);
    lv_label_set_text(lbl_data, LV_SYMBOL_EDIT);
    lv_obj_center(lbl_data);

    lv_obj_add_event_cb(btn_data, gw_lcd_data_event_handler, LV_EVENT_ALL, NULL);
    
}


static void gw_lcd_create_settings_content(void) {
    ui_set_content = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_set_content, LCD_W, CONTENT_H);
    lv_obj_set_pos(ui_set_content, 0, STATUS_BAR_H);
    lv_obj_set_style_bg_color(ui_set_content, lv_color_hex(0x1a802c), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_set_content, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(ui_set_content, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ui_set_content, 0, LV_PART_MAIN);
    
    // 创建表格对象
    table_set = lv_table_create(ui_set_content);
    lv_obj_set_style_text_font(table_set, &lv_font_montserrat_16, LV_PART_ITEMS);
    lv_obj_set_style_text_color(table_set, lv_color_hex(0xc0c0c0), LV_PART_ITEMS);
    lv_obj_set_style_border_width(table_set, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(table_set, lv_color_hex(0xc0c0c0), LV_PART_ITEMS);
    lv_obj_set_style_border_side(table_set, LV_BORDER_SIDE_FULL, LV_PART_ITEMS);

    // 设置表格行列数
    lv_table_set_row_cnt(table_set, 4);    // 4行
    lv_table_set_col_cnt(table_set, 2);    // 2列

    // 设置行高
    lv_obj_set_style_pad_ver(table_set, 3, LV_PART_ITEMS);

    // 设置列宽
    lv_table_set_col_width(table_set, 0, 155);
    lv_table_set_col_width(table_set, 1, 155);
    
    // // 设置表格数据
    lv_table_set_cell_value(table_set, 0, 0, "Baud Rate:");
    lv_table_set_cell_value(table_set, 1, 0, "Mode:");
    lv_table_set_cell_value(table_set, 2, 0, "Cycle Time(ms):");
    lv_table_set_cell_value(table_set, 3, 0, "Frame Mode:");
    lv_table_set_cell_value_fmt(table_set, 0, 1, "%d", baudrate_table[0]);
    lv_table_set_cell_value(table_set, 1, 1, mode_table[0]);
    lv_table_set_cell_value(table_set, 2, 1, "1000");
    lv_table_set_cell_value(table_set, 3, 1, frame_mode_table[0]);

    lv_obj_add_event_cb(table_set, gw_lcd_table_set_event_handler, LV_EVENT_ALL, NULL);
    // 将表格居中显示
    lv_obj_center(table_set);

    static lv_style_t table_set_style_selected;
    lv_style_init(&table_set_style_selected);
    lv_style_set_bg_color(&table_set_style_selected, lv_palette_main(LV_PALETTE_CYAN)); // 选中背景色
    lv_style_set_text_color(&table_set_style_selected, lv_color_white()); 
    lv_obj_add_style(table_set, &table_set_style_selected, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_table_set_selected_cell(table_set, table_set_row, table_set_col);
    lv_obj_set_state(table_set, LV_STATE_FOCUSED, true);

    lv_obj_add_flag(ui_set_content, LV_OBJ_FLAG_HIDDEN);
}

// 创建主页面内容
static void gw_lcd_create_main_content(void) {
    ui_main_content = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_main_content, LCD_W, CONTENT_H);
    lv_obj_set_pos(ui_main_content, 0, STATUS_BAR_H);
    lv_obj_set_style_text_font(ui_main_content, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_main_content, lv_color_hex(0x1a802c), LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_main_content, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_main_content, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(ui_main_content, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(ui_main_content, 0, LV_PART_MAIN);

    static int32_t col_dsc[] = {50, 50, 50, 50, 50, 50, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {50, 50, LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    lv_obj_set_style_grid_column_dsc_array(ui_main_content, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(ui_main_content, row_dsc, 0);
    lv_obj_center(ui_main_content);
    lv_obj_set_layout(ui_main_content, LV_LAYOUT_GRID);

    lv_obj_t *lbl_slaves = lv_label_create(ui_main_content);
    lv_label_set_text(lbl_slaves, "Slaves\n Num:");
    lv_obj_center(lbl_slaves);
    lv_obj_set_grid_cell(lbl_slaves, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lbl_slaves_num = lv_label_create(ui_main_content);
    gw_lcd_main_set_slaves_num(0);
    lv_obj_center(lbl_slaves_num);
    lv_obj_set_grid_cell(lbl_slaves_num, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_t *lbl_state_ = lv_label_create(ui_main_content);
    lv_label_set_text(lbl_state_, "State:");
    lv_obj_center(lbl_state_);
    lv_obj_set_grid_cell(lbl_state_, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lbl_state = lv_label_create(ui_main_content);
    gw_lcd_main_set_state(0x01);
    lv_obj_center(lbl_state);
    lv_obj_set_grid_cell(lbl_state, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    
    lv_obj_t *lbl_tx = lv_label_create(ui_main_content);
    lv_label_set_text(lbl_tx, " Tx\nNum:");
    lv_obj_center(lbl_tx);
    lv_obj_set_grid_cell(lbl_tx, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lbl_txnum = lv_label_create(ui_main_content);
    gw_lcd_main_set_txnum(0);
    lv_obj_center(lbl_txnum);
    lv_obj_set_grid_cell(lbl_txnum, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_obj_t *lbl_rx = lv_label_create(ui_main_content);
    lv_label_set_text(lbl_rx, " Rx\nNum:");
    lv_obj_center(lbl_rx);
    lv_obj_set_grid_cell(lbl_rx, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lbl_rxnum = lv_label_create(ui_main_content);
    gw_lcd_main_set_rxnum(0);
    lv_obj_center(lbl_rxnum);
    lv_obj_set_grid_cell(lbl_rxnum, LV_GRID_ALIGN_CENTER, 3, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lbl_mode = lv_label_create(ui_main_content);
    lv_label_set_text(lbl_mode, lv_table_get_cell_value(table_set, 1, 1));
    lv_obj_center(lbl_mode);
    lv_obj_set_grid_cell(lbl_mode, LV_GRID_ALIGN_CENTER, 4, 2, LV_GRID_ALIGN_CENTER, 0, 1);

    lbl_start = lv_label_create(ui_main_content);
    lv_obj_set_style_text_font(lbl_start, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_label_set_text(lbl_start, LV_SYMBOL_PLAY);
    gw_lcd_main_set_stop();
    lv_obj_center(lbl_start);
    lv_obj_set_grid_cell(lbl_start, LV_GRID_ALIGN_CENTER, 4, 2, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_obj_clear_flag(ui_main_content, LV_OBJ_FLAG_HIDDEN);
}

// 创建数据页面内容
static void gw_lcd_create_data_content(void) {
    ui_data_content = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ui_data_content, LCD_W, CONTENT_H);
    lv_obj_set_pos(ui_data_content, 0, STATUS_BAR_H);
    lv_obj_set_style_bg_color(ui_data_content, lv_color_hex(0x1a802c), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_data_content, 0, LV_PART_MAIN);
    lv_obj_add_flag(ui_data_content, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_pad_all(ui_data_content, 0, LV_PART_MAIN);
    
    // 创建表格对象
    table_data = lv_table_create(ui_data_content);
    lv_obj_set_style_text_font(table_data, &lv_font_montserrat_16, LV_PART_ITEMS);
    lv_obj_set_style_text_color(table_data, lv_color_hex(0xc0c0c0), LV_PART_ITEMS);
    lv_obj_set_style_border_width(table_data, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(table_data, lv_color_hex(0xc0c0c0), LV_PART_ITEMS);
    lv_obj_set_style_border_side(table_data, LV_BORDER_SIDE_FULL, LV_PART_ITEMS);

    // 设置表格行列数
    lv_table_set_row_cnt(table_data, 5);
    lv_table_set_col_cnt(table_data, 9);

    lv_obj_set_style_pad_all(table_data, 0, LV_PART_ITEMS);
    lv_obj_set_style_text_align(table_data, LV_TEXT_ALIGN_CENTER, LV_PART_ITEMS);

    // 设置列宽
    lv_table_set_col_width(table_data, 0, 35);
    lv_table_set_col_width(table_data, 1, 35);
    lv_table_set_col_width(table_data, 2, 35);
    lv_table_set_col_width(table_data, 3, 35);
    lv_table_set_col_width(table_data, 4, 35);
    lv_table_set_col_width(table_data, 5, 35);
    lv_table_set_col_width(table_data, 6, 35);
    lv_table_set_col_width(table_data, 7, 35);
    lv_table_set_col_width(table_data, 8, 35);
    
    // // 设置表格数据
    lv_table_set_cell_value(table_data, 0, 0, "tnu");
    lv_table_set_cell_value(table_data, 0, 1, "tlen");
    lv_table_set_cell_value(table_data, 0, 2, "tid");
    lv_table_set_cell_value(table_data, 0, 3, "trtr");
    lv_table_set_cell_value(table_data, 0, 4, "tidf");
    lv_table_set_cell_value(table_data, 0, 5, "rlen");
    lv_table_set_cell_value(table_data, 0, 6, "rid");
    lv_table_set_cell_value(table_data, 0, 7, "rrtr");
    lv_table_set_cell_value(table_data, 0, 8, "ridf");
    lv_table_set_cell_value(table_data, 1, 0, "0");
    lv_table_set_cell_value(table_data, 1, 1, "0");
    lv_table_set_cell_value(table_data, 1, 2, "0");
    lv_table_set_cell_value(table_data, 1, 3, "0");
    lv_table_set_cell_value(table_data, 1, 4, "0");
    lv_table_set_cell_value(table_data, 1, 5, "0");
    lv_table_set_cell_value(table_data, 1, 6, "0");
    lv_table_set_cell_value(table_data, 1, 7, "0");
    lv_table_set_cell_value(table_data, 1, 8, "0");
    lv_table_set_cell_value(table_data, 2, 0, "");
    lv_table_set_cell_value(table_data, 2, 1, "D0");
    lv_table_set_cell_value(table_data, 2, 2, "D1");
    lv_table_set_cell_value(table_data, 2, 3, "D2");
    lv_table_set_cell_value(table_data, 2, 4, "D3");
    lv_table_set_cell_value(table_data, 2, 5, "D4");
    lv_table_set_cell_value(table_data, 2, 6, "D5");
    lv_table_set_cell_value(table_data, 2, 7, "D6");
    lv_table_set_cell_value(table_data, 2, 8, "D7");
    lv_table_set_cell_value(table_data, 3, 0, "TX");
    lv_table_set_cell_value(table_data, 3, 1, "0");
    lv_table_set_cell_value(table_data, 3, 1, "0");
    lv_table_set_cell_value(table_data, 3, 2, "0");
    lv_table_set_cell_value(table_data, 3, 3, "0");
    lv_table_set_cell_value(table_data, 3, 4, "0");
    lv_table_set_cell_value(table_data, 3, 5, "0");
    lv_table_set_cell_value(table_data, 3, 6, "0");
    lv_table_set_cell_value(table_data, 3, 7, "0");
    lv_table_set_cell_value(table_data, 3, 8, "0");
    lv_table_set_cell_value(table_data, 4, 0, "RX");
    lv_table_set_cell_value(table_data, 4, 1, "0");
    lv_table_set_cell_value(table_data, 4, 2, "0");
    lv_table_set_cell_value(table_data, 4, 3, "0");
    lv_table_set_cell_value(table_data, 4, 4, "0");
    lv_table_set_cell_value(table_data, 4, 5, "0");
    lv_table_set_cell_value(table_data, 4, 6, "0");
    lv_table_set_cell_value(table_data, 4, 7, "0");
    lv_table_set_cell_value(table_data, 4, 8, "0");

    lv_obj_add_event_cb(table_data, gw_lcd_table_data_event_handler, LV_EVENT_ALL, NULL);
    // 将表格居中显示
    lv_obj_center(table_data);

    static lv_style_t table_data_style_selected;
    lv_style_init(&table_data_style_selected);
    lv_style_set_bg_color(&table_data_style_selected, lv_palette_main(LV_PALETTE_CYAN)); // 选中背景色
    lv_style_set_text_color(&table_data_style_selected, lv_color_white()); 
    lv_obj_add_style(table_data, &table_data_style_selected, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_table_set_selected_cell(table_data, table_data_row, table_data_col);
    lv_obj_set_state(table_data, LV_STATE_FOCUSED, true);
}


// 初始化UI
void gw_lcd_ui_init(void) {

    gw_lcd_create_status_bar();
    gw_lcd_create_bottom_bar();
    gw_lcd_create_settings_content();
    gw_lcd_create_main_content();
    gw_lcd_create_data_content();

}

static void gw_lcd_handle(gw_mq_msg msg)
{
    gw_mq_msg mq_msg;
    
    switch (msg.submsg_id)
    {
        case GW_MQ_SUBMSG_LCD_UPDATE_ID:
            gw_lcd_update_time();
            lv_label_set_text(ui_batt_label, LV_SYMBOL_BATTERY_FULL);
            break;
        case GW_MQ_SUBMSG_LCD_KEYA_CLICKED_ID:
            lv_obj_send_event(btn_set, LV_EVENT_CLICKED, NULL);
            break;
        case GW_MQ_SUBMSG_LCD_KEYA_LONG_CLICKED_ID:
            lv_obj_send_event(btn_set, LV_EVENT_LONG_PRESSED, NULL);
            break;
        case GW_MQ_SUBMSG_LCD_KEYB_CLICKED_ID:
            lv_obj_send_event(btn_main, LV_EVENT_CLICKED, NULL);
            break;
        case GW_MQ_SUBMSG_LCD_KEYB_LONG_CLICKED_ID:
            lv_obj_send_event(btn_main, LV_EVENT_LONG_PRESSED, NULL);
            break;
        case GW_MQ_SUBMSG_LCD_KEYC_CLICKED_ID:
            lv_obj_send_event(btn_data, LV_EVENT_CLICKED, NULL);
            break;
        case GW_MQ_SUBMSG_LCD_KEYC_LONG_CLICKED_ID:
            lv_obj_send_event(btn_data, LV_EVENT_LONG_PRESSED, NULL);
            break;
        case GW_MQ_SUBMSG_LCD_KEYD_CLICKED_ID:
            if(current_page == GW_LCD_PAGE_MAIN) {
                if(lv_strcmp(lv_label_get_text(lbl_state), "OP") == 0 && lv_strcmp(lv_label_get_text(lbl_mode), "Trigger") == 0) {
                    mq_msg.msg_id = GW_MQ_MSG_ECAT_ID;
                    mq_msg.submsg_id = GW_MQ_SUBMSG_ECAT_SET_TRIGGER_ID;
                    mq_msg.param = true;
                    gw_mq_send(GW_MQ_ECAT_ID, &mq_msg, 0);
                }
            } else if(current_page == GW_LCD_PAGE_SETTINGS) {
                if(table_set_row == 0 && table_set_col == 1) {
                    //set baudrate
                    lv_table_set_cell_value_fmt(table_set, 0, 1, "%d", gw_lcd_table_set_baudrate(false));
                } else if(table_set_row == 1 && table_set_col == 1) {
                    //set mode
                    lv_table_set_cell_value(table_set, 1, 1, gw_lcd_table_set_mode(false));
                } else if(table_set_row == 2 && table_set_col == 1) {
                    //set cycle time
                    char *cell_value = lv_table_get_cell_value(table_set, table_set_row, table_set_col);
                    if(cell_value != NULL) {
                        int32_t data = atoi(cell_value);
                        data++;
                        if(data>0x7fffffff) data = 0x7fffffff;
                        lv_table_set_cell_value_fmt(table_set, table_set_row, table_set_col, "%d", data);
                    }
                } else if(table_set_row == 3 && table_set_col == 1) {
                    //set frame mode
                    lv_table_set_cell_value(table_set, 3, 1, gw_lcd_table_set_frame_mode(false));
                }
            } else if(current_page == GW_LCD_PAGE_DATA) {
                lv_table_get_selected_cell(table_data, &table_data_row, &table_data_col);
                char *cell_value = lv_table_get_cell_value(table_data, table_data_row, table_data_col);
                if(cell_value != NULL) {
                    int32_t data = atoi(cell_value);
                    data++;
                    if(table_data_row == 1) {
                        if(table_data_col == 0) {
                            if(data>0xffff) data = 0xffff;
                            lv_table_set_cell_value_fmt(table_data, table_data_row, table_data_col, "%d", data);
                        } else if(table_data_col == 1) {
                            if(data>8) data = 8;
                            lv_table_set_cell_value_fmt(table_data, table_data_row, table_data_col, "%d", data);
                        } else if(table_data_col == 2) {
                            cell_value = lv_table_get_cell_value(table_data, table_data_row, 4);
                            if(cell_value != NULL) {
                                int8_t id_flag = atoi(cell_value);
                                if(id_flag) {
                                    if(data>0x1fffffff) data = 0x1fffffff;
                                } else {
                                    if(data>0x7ff) data = 0x7ff;
                                }
                                lv_table_set_cell_value_fmt(table_data, table_data_row, table_data_col, "%d", data);
                            }
                        } else if(table_data_col == 3){
                            if(data>1) data = 1;
                            lv_table_set_cell_value_fmt(table_data, table_data_row, table_data_col, "%d", data);
                        } else if(table_data_col == 4) {
                            if(lv_strcmp(frame_mode_table[0], frame_mode_table[frame_mode_index%2]) == 0) { //standard frame mode
                                lv_table_set_cell_value_fmt(table_data, table_data_row, table_data_col, "%d", 0);
                            } else {
                                if(data>1) data = 1;
                                lv_table_set_cell_value_fmt(table_data, table_data_row, table_data_col, "%d", data);
                            }
                        }
                    } else if(table_data_row == 3) {
                            if(data>0xff) data = 0xff;
                            lv_table_set_cell_value_fmt(table_data, table_data_row, table_data_col, "%d", data);
                    }
                    if(lv_strcmp(frame_mode_table[0], frame_mode_table[frame_mode_index%2]) == 0) { //standard frame mode
                        if(table_data_row == 1) {
                            if(table_data_col == 0) {
                                can_tx_std.tx_num = data;
                            } else if(table_data_col == 1) {
                                can_tx_std.dlc = data;
                            } else if(table_data_col == 2) {
                                can_tx_std.id = data;
                            } else if(table_data_col == 3){
                                can_tx_std.rtr = data;
                            }
                        } else if(table_data_row == 3) {
                            can_tx_std.data[table_data_col-1] = data;
                        }
                    } else if(lv_strcmp(frame_mode_table[1], frame_mode_table[frame_mode_index%2]) == 0) { //extended frame mode
                        if(table_data_row == 1) {
                            if(table_data_col == 0) {
                                can_tx_ext.tx_num = data;
                            } else if(table_data_col == 1) {
                                can_tx_ext.dlc = data;
                            } else if(table_data_col == 2) {
                                can_tx_ext.id = data;
                            } else if(table_data_col == 3){
                                can_tx_ext.rtr = data;
                            } else if(table_data_col == 4) {
                                can_tx_ext.std_ext_bit = data;
                            }
                        } else if(table_data_row == 3) {
                            can_tx_ext.data[table_data_col-1] = data;
                        }
                    }
                }
            }
            break;
        case GW_MQ_SUBMSG_LCD_KEYD_LONG_CLICKED_ID:
            break;
        case GW_MQ_SUBMSG_LCD_SET_SLAVES_NUM_ID:
            gw_lcd_main_set_slaves_num(msg.param);
            break;
        case GW_MQ_SUBMSG_LCD_SET_STATE_ID:
            gw_lcd_main_set_state(msg.param);
            break;
        case GW_MQ_SUBMSG_LCD_SET_TXNUM_ID:
            gw_lcd_main_set_txnum(msg.param);
            break;
        case GW_MQ_SUBMSG_LCD_SET_RXNUM_ID:
            gw_lcd_main_set_rxnum(msg.param);
            break;
        case GW_MQ_SUBMSG_LCD_SET_START_ID:
            gw_lcd_main_set_start();
            break;
        case GW_MQ_SUBMSG_LCD_SET_STOP_ID:
            gw_lcd_main_set_stop();
            break;
        default:
            break;
    }
}

void gw_lcd_thread(void *argument)
{
    (void)argument;
    osStatus_t ret;
    gw_mq_msg mq_msg;

    LOG_D("gw_lcd_thread run\r\n");

    lv_init();
    drv_lcd_init();
    gw_lcd_ui_init();  

    for(;;)
    {
        lv_timer_handler();
        
        ret = gw_mq_recv(GW_MQ_LCD_ID, &mq_msg, 10);

        if(ret >= 0) {
           switch (mq_msg.msg_id)
           {
               case GW_MQ_MSG_LCD_ID:
                   gw_lcd_handle(mq_msg);
                   break;
               default:
                   break;
           }
           osDelay(10);
        }
    }
}
