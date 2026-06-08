/**
 * @file monitor_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "monitor_gen.h"
#include "ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *monitor_create(lv_obj_t *parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_inner_card;
    static lv_style_t style_data_card;
    static lv_style_t style_desire_card;
    static lv_style_t style_current_card;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&style_inner_card);
        lv_style_set_width(&style_inner_card, 790);
        lv_style_set_height(&style_inner_card, 455);
        lv_style_set_bg_opa(&style_inner_card, 0);
        lv_style_set_align(&style_inner_card, LV_ALIGN_CENTER);
        lv_style_set_border_width(&style_inner_card, 0);

        lv_style_init(&style_data_card);
        lv_style_set_bg_color(&style_data_card, lv_color_hex(0x152445));
        lv_style_set_bg_opa(&style_data_card, 30);
        lv_style_set_border_width(&style_data_card, 2);
        lv_style_set_border_color(&style_data_card, lv_color_hex(0x234b7a));
        lv_style_set_shadow_color(&style_data_card, lv_color_hex(0x313539));
        lv_style_set_shadow_width(&style_data_card, 20);
        lv_style_set_shadow_opa(&style_data_card, 150);
        lv_style_set_shadow_spread(&style_data_card, 10);
        lv_style_set_radius(&style_data_card, 16);
        lv_style_set_pad_all(&style_data_card, 0);

        lv_style_init(&style_desire_card);
        lv_style_set_bg_color(&style_desire_card, lv_color_hex(0x22c55e));
        lv_style_set_bg_opa(&style_desire_card, 10);
        lv_style_set_border_width(&style_desire_card, 2);
        lv_style_set_border_color(&style_desire_card, lv_color_hex(0x22c55e));
        lv_style_set_border_opa(&style_desire_card, 30);
        lv_style_set_shadow_color(&style_desire_card, lv_color_hex(0x22c55e));
        lv_style_set_shadow_width(&style_desire_card, 10);
        lv_style_set_shadow_opa(&style_desire_card, 100);
        lv_style_set_shadow_spread(&style_desire_card, 2);
        lv_style_set_radius(&style_desire_card, 16);
        lv_style_set_pad_all(&style_desire_card, 0);

        lv_style_init(&style_current_card);
        lv_style_set_bg_color(&style_current_card, lv_color_hex(0xef4444));
        lv_style_set_bg_opa(&style_current_card, 10);
        lv_style_set_border_width(&style_current_card, 2);
        lv_style_set_border_color(&style_current_card, lv_color_hex(0xef4444));
        lv_style_set_border_opa(&style_current_card, 30);
        lv_style_set_shadow_color(&style_current_card, lv_color_hex(0xef4444));
        lv_style_set_shadow_width(&style_current_card, 10);
        lv_style_set_shadow_opa(&style_current_card, 100);
        lv_style_set_shadow_spread(&style_current_card, 2);
        lv_style_set_radius(&style_current_card, 16);
        lv_style_set_pad_all(&style_current_card, 0);

        style_inited = true;
    }

    lv_obj_t *lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "monitor_#");
    lv_obj_set_x(lv_obj_0, 1);
    lv_obj_set_width(lv_obj_0, 798);
    lv_obj_set_height(lv_obj_0, 470);
    lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x0f2558), 0);
    lv_obj_set_style_bg_opa(lv_obj_0, 20, 0);
    lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_border_width(lv_obj_0, 3, 0);
    lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x234b7a), 0);

    lv_obj_t *lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_1, 185);
    lv_obj_set_height(lv_obj_1, 90);
    lv_obj_set_x(lv_obj_1, 15);
    lv_obj_set_y(lv_obj_1, 20);
    lv_obj_add_style(lv_obj_1, &style_desire_card, 0);
    lv_obj_t *desire_position_speed = lv_label_create(lv_obj_1);
    lv_obj_set_name(desire_position_speed, "desire_position_speed");
    lv_label_set_text(desire_position_speed, "Desire Position");
    lv_obj_set_x(desire_position_speed, 10);
    lv_obj_set_y(desire_position_speed, 12);
    lv_obj_set_style_text_font(desire_position_speed, SemiBold_Font_16, 0);
    lv_obj_set_style_text_color(desire_position_speed, lv_color_hex(0x86efac), 0);
    lv_obj_set_style_text_letter_space(desire_position_speed, 1, 0);

    lv_obj_t *label_desire_position = lv_label_create(lv_obj_1);
    lv_obj_set_name(label_desire_position, "label_desire_position");
    lv_obj_set_x(label_desire_position, 15);
    lv_obj_set_y(label_desire_position, 48);
    lv_label_set_text(label_desire_position, "0");
    lv_obj_set_style_text_font(label_desire_position, SemiBold_Font_36, 0);
    lv_obj_set_style_text_color(label_desire_position, lv_color_hex(0x4ade80), 0);

    lv_obj_t *lv_obj_2 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_2, 185);
    lv_obj_set_height(lv_obj_2, 90);
    lv_obj_set_x(lv_obj_2, 210);
    lv_obj_set_y(lv_obj_2, 20);
    lv_obj_add_style(lv_obj_2, &style_current_card, 0);
    lv_obj_t *lv_label_0 = lv_label_create(lv_obj_2);
    lv_obj_set_x(lv_label_0, 10);
    lv_obj_set_y(lv_label_0, 12);
    lv_label_set_text(lv_label_0, "Current Position");
    lv_obj_set_style_text_font(lv_label_0, SemiBold_Font_16, 0);
    lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xfca5a5), 0);
    lv_obj_set_style_text_letter_space(lv_label_0, 1, 0);

    lv_obj_t *label_current_position = lv_label_create(lv_obj_2);
    lv_obj_set_name(label_current_position, "label_current_position");
    lv_obj_set_x(label_current_position, 15);
    lv_obj_set_y(label_current_position, 48);
    lv_label_set_text(label_current_position, "0");
    lv_obj_set_style_text_font(label_current_position, SemiBold_Font_36, 0);
    lv_obj_set_style_text_color(label_current_position, lv_color_hex(0xf87171), 0);

    lv_obj_t *lv_obj_3 = lv_obj_create(lv_obj_0);
    lv_obj_set_x(lv_obj_3, 15);
    lv_obj_set_y(lv_obj_3, 130);
    lv_obj_set_width(lv_obj_3, 380);
    lv_obj_set_height(lv_obj_3, 310);
    lv_obj_add_style(lv_obj_3, &style_data_card, 0);
    lv_obj_t *column_0 = column_create(lv_obj_3);
    lv_obj_set_x(column_0, 15);
    lv_obj_set_y(column_0, 20);
    lv_obj_set_style_pad_row(column_0, 25, 0);
    lv_obj_t *row_0 = row_create(column_0);
    lv_obj_set_style_pad_column(row_0, 12, 0);
    lv_obj_set_style_flex_cross_place(row_0, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_t *lv_image_0 = lv_image_create(row_0);
    lv_image_set_src(lv_image_0, image_device);
    lv_obj_set_width(lv_image_0, 28);
    lv_obj_set_height(lv_image_0, 28);
    lv_image_set_inner_align(lv_image_0, LV_IMAGE_ALIGN_STRETCH);

    lv_obj_t *lv_label_1 = lv_label_create(row_0);
    lv_label_set_text(lv_label_1, "Device:");
    lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0xfb7185), 0);
    lv_obj_set_style_text_font(lv_label_1, SemiBold_Font_28, 0);

    lv_obj_t *label_device_name = lv_label_create(row_0);
    lv_obj_set_name(label_device_name, "label_device_name");
    lv_label_set_text(label_device_name, "No slave");
    lv_obj_set_style_text_font(label_device_name, NotoSerifCJKsc_SemiBold_28, 0);
    lv_obj_set_style_text_color(label_device_name, lv_color_hex(0xfda4af), 0);

    lv_obj_t *row_1 = row_create(column_0);
    lv_obj_set_style_pad_column(row_1, 12, 0);
    lv_obj_set_style_flex_cross_place(row_1, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_t *lv_image_1 = lv_image_create(row_1);
    lv_image_set_src(lv_image_1, image_state);
    lv_obj_set_width(lv_image_1, 28);
    lv_obj_set_height(lv_image_1, 28);
    lv_image_set_inner_align(lv_image_1, LV_IMAGE_ALIGN_STRETCH);

    lv_obj_t *lv_label_2 = lv_label_create(row_1);
    lv_label_set_text(lv_label_2, "State:");
    lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0x38bdf8), 0);
    lv_obj_set_style_text_font(lv_label_2, SemiBold_Font_28, 0);

    lv_obj_t *label_state = lv_label_create(row_1);
    lv_obj_set_name(label_state, "label_state");
    lv_label_set_text(label_state, "Close");
    lv_obj_set_style_text_font(label_state, SemiBold_Font_28, 0);
    lv_obj_set_style_text_color(label_state, lv_color_hex(0x7dd3fc), 0);

    lv_obj_t *row_2 = row_create(column_0);
    lv_obj_set_style_pad_column(row_2, 12, 0);
    lv_obj_set_style_flex_cross_place(row_2, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_t *lv_image_2 = lv_image_create(row_2);
    lv_image_set_src(lv_image_2, image_address);
    lv_obj_set_width(lv_image_2, 28);
    lv_obj_set_height(lv_image_2, 28);
    lv_image_set_inner_align(lv_image_2, LV_IMAGE_ALIGN_STRETCH);

    lv_obj_t *lv_label_3 = lv_label_create(row_2);
    lv_label_set_text(lv_label_3, "Address:");
    lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0x4ade80), 0);
    lv_obj_set_style_text_font(lv_label_3, SemiBold_Font_28, 0);

    lv_obj_t *label_address = lv_label_create(row_2);
    lv_obj_set_name(label_address, "label_address");
    lv_label_set_text(label_address, "0");
    lv_obj_set_style_text_font(label_address, SemiBold_Font_28, 0);
    lv_obj_set_style_text_color(label_address, lv_color_hex(0x86efac), 0);

    lv_obj_t *row_3 = row_create(column_0);
    lv_obj_set_style_pad_column(row_3, 12, 0);
    lv_obj_set_style_flex_cross_place(row_3, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_t *lv_image_3 = lv_image_create(row_3);
    lv_image_set_src(lv_image_3, image_input);
    lv_obj_set_width(lv_image_3, 28);
    lv_obj_set_height(lv_image_3, 28);
    lv_image_set_inner_align(lv_image_3, LV_IMAGE_ALIGN_STRETCH);

    lv_obj_t *lv_label_4 = lv_label_create(row_3);
    lv_label_set_text(lv_label_4, "Input_Bits:");
    lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0xfbbf24), 0);
    lv_obj_set_style_text_font(lv_label_4, SemiBold_Font_28, 0);

    lv_obj_t *label_input_bits = lv_label_create(row_3);
    lv_obj_set_name(label_input_bits, "label_input_bits");
    lv_label_set_text(label_input_bits, "0");
    lv_obj_set_style_text_font(label_input_bits, SemiBold_Font_28, 0);
    lv_obj_set_style_text_color(label_input_bits, lv_color_hex(0xfcd34d), 0);

    lv_obj_t *row_4 = row_create(column_0);
    lv_obj_set_style_pad_column(row_4, 12, 0);
    lv_obj_set_style_flex_cross_place(row_4, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_t *lv_image_4 = lv_image_create(row_4);
    lv_image_set_src(lv_image_4, image_output);
    lv_obj_set_width(lv_image_4, 28);
    lv_obj_set_height(lv_image_4, 28);
    lv_image_set_inner_align(lv_image_4, LV_IMAGE_ALIGN_STRETCH);

    lv_obj_t *lv_label_5 = lv_label_create(row_4);
    lv_label_set_text(lv_label_5, "Output_Bits:");
    lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0xa78bfa), 0);
    lv_obj_set_style_text_font(lv_label_5, SemiBold_Font_28, 0);

    lv_obj_t *label_output_bits = lv_label_create(row_4);
    lv_obj_set_name(label_output_bits, "label_output_bits");
    lv_label_set_text(label_output_bits, "0");
    lv_obj_set_style_text_font(label_output_bits, SemiBold_Font_28, 0);
    lv_obj_set_style_text_color(label_output_bits, lv_color_hex(0xc4b5fd), 0);

    lv_obj_t *row_5 = row_create(lv_obj_0);
    lv_obj_set_x(row_5, 420);
    lv_obj_set_y(row_5, 12);
    lv_obj_set_style_pad_column(row_5, 20, 0);
    lv_obj_set_style_flex_cross_place(row_5, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_t *lv_label_6 = lv_label_create(row_5);
    lv_label_set_text(lv_label_6, "current_slave:");
    lv_obj_set_style_text_font(lv_label_6, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0x0fe2c8), 0);

    lv_obj_t *slave_dropdown = lv_dropdown_create(row_5);
    lv_obj_set_name(slave_dropdown, "slave_dropdown");
    lv_obj_set_width(slave_dropdown, 160);
    lv_obj_set_height(slave_dropdown, 40);
    lv_obj_set_style_radius(slave_dropdown, 5, 0);
    lv_obj_set_style_bg_color(slave_dropdown, lv_color_hex(0x1b2a3c), 0);
    lv_dropdown_set_options(slave_dropdown, "slave_1");
    lv_obj_set_style_text_font(slave_dropdown, SemiBold_Font_16, 0);
    lv_obj_set_style_text_color(slave_dropdown, lv_color_hex(0x19f0e1), 0);
    lv_obj_set_style_text_align(slave_dropdown, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_letter_space(slave_dropdown, 4, 0);
    lv_obj_set_style_border_width(slave_dropdown, 1, 0);
    lv_obj_set_style_border_color(slave_dropdown, lv_color_hex(0x20618a), 0);
    lv_obj_set_flag(slave_dropdown, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_pad_all(slave_dropdown, 5, 0);
    lv_obj_t *lv_dropdown_list_0 = lv_dropdown_get_list(slave_dropdown);
    lv_obj_set_style_border_width(lv_dropdown_list_0, 2, 0);
    lv_obj_set_style_border_color(lv_dropdown_list_0, lv_color_hex(0x2f5190), 0);
    lv_obj_set_style_bg_color(lv_dropdown_list_0, lv_color_hex(0x1b2a3c), 0);
    lv_obj_set_style_shadow_width(lv_dropdown_list_0, 20, 0);
    lv_obj_set_style_shadow_opa(lv_dropdown_list_0, 150, 0);
    lv_obj_set_style_shadow_spread(lv_dropdown_list_0, 3, 0);
    lv_obj_set_style_text_font(lv_dropdown_list_0, SemiBold_Font_20, 0);
    lv_obj_set_style_text_align(lv_dropdown_list_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_letter_space(lv_dropdown_list_0, 3, 0);
    lv_obj_set_style_text_color(lv_dropdown_list_0, lv_color_hex(0x1be6e2), 0);
    lv_obj_add_event_cb(lv_dropdown_list_0, slave_dropdown_change_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *lv_obj_4 = lv_obj_create(lv_obj_0);
    lv_obj_set_x(lv_obj_4, 405);
    lv_obj_set_y(lv_obj_4, 60);
    lv_obj_set_width(lv_obj_4, 370);
    lv_obj_set_height(lv_obj_4, 380);
    lv_obj_add_style(lv_obj_4, &style_data_card, 0);
    lv_obj_t *row_6 = row_create(lv_obj_4);
    lv_obj_set_x(row_6, 15);
    lv_obj_set_y(row_6, 22);
    lv_obj_set_style_pad_column(row_6, 8, 0);
    lv_obj_set_style_flex_cross_place(row_6, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_t *lv_image_5 = lv_image_create(row_6);
    lv_image_set_src(lv_image_5, image_monitor);
    lv_obj_set_width(lv_image_5, 26);
    lv_obj_set_height(lv_image_5, 26);
    lv_image_set_inner_align(lv_image_5, LV_IMAGE_ALIGN_STRETCH);

    lv_obj_t *lv_label_7 = lv_label_create(row_6);
    lv_label_set_text(lv_label_7, "Monitor");
    lv_obj_set_style_text_color(lv_label_7, lv_color_hex(0x73bff5), 0);
    lv_obj_set_style_text_font(lv_label_7, SemiBold_Font_28, 0);

    lv_obj_t *monitor_position_mode_obj = lv_obj_create(lv_obj_4);
    lv_obj_set_name(monitor_position_mode_obj, "monitor_position_mode_obj");
    lv_obj_set_x(monitor_position_mode_obj, 180);
    lv_obj_set_y(monitor_position_mode_obj, 15);
    lv_obj_set_width(monitor_position_mode_obj, LV_SIZE_CONTENT);
    lv_obj_set_height(monitor_position_mode_obj, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(monitor_position_mode_obj, 0, 0);
    lv_obj_set_style_bg_opa(monitor_position_mode_obj, 0, 0);
    lv_obj_set_flag(monitor_position_mode_obj, LV_OBJ_FLAG_HIDDEN, false);
    lv_obj_set_style_border_width(monitor_position_mode_obj, 0, 0);
    lv_obj_t *column_1 = column_create(monitor_position_mode_obj);
    lv_obj_set_style_pad_row(column_1, 6, 0);
    lv_obj_t *row_7 = row_create(column_1);
    lv_obj_set_style_flex_cross_place(row_7, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(row_7, 8, 0);
    lv_obj_t *lv_obj_5 = lv_obj_create(row_7);
    lv_obj_set_width(lv_obj_5, 70);
    lv_obj_set_height(lv_obj_5, 4);
    lv_obj_set_style_pad_all(lv_obj_5, 0, 0);
    lv_obj_set_style_bg_color(lv_obj_5, lv_color_hex(0x4ade80), 0);
    lv_obj_set_style_border_width(lv_obj_5, 0, 0);
    lv_obj_set_style_radius(lv_obj_5, 2, 0);

    lv_obj_t *lv_label_8 = lv_label_create(row_7);
    lv_label_set_text(lv_label_8, "current");
    lv_obj_set_style_text_color(lv_label_8, lv_color_hex(0x4ade80), 0);
    lv_obj_set_style_text_font(lv_label_8, SemiBold_Font_24, 0);

    lv_obj_t *row_8 = row_create(column_1);
    lv_obj_set_style_flex_cross_place(row_8, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(row_8, 8, 0);
    lv_obj_t *lv_obj_6 = lv_obj_create(row_8);
    lv_obj_set_width(lv_obj_6, 70);
    lv_obj_set_height(lv_obj_6, 4);
    lv_obj_set_style_pad_all(lv_obj_6, 0, 0);
    lv_obj_set_style_bg_color(lv_obj_6, lv_color_hex(0xf87171), 0);
    lv_obj_set_style_border_width(lv_obj_6, 0, 0);
    lv_obj_set_style_radius(lv_obj_6, 2, 0);

    lv_obj_t *lv_label_9 = lv_label_create(row_8);
    lv_label_set_text(lv_label_9, "desire");
    lv_obj_set_style_text_color(lv_label_9, lv_color_hex(0xf87171), 0);
    lv_obj_set_style_text_font(lv_label_9, SemiBold_Font_24, 0);

    lv_obj_t *monitor_speed_mode_obj = lv_obj_create(lv_obj_4);
    lv_obj_set_name(monitor_speed_mode_obj, "monitor_speed_mode_obj");
    lv_obj_set_x(monitor_speed_mode_obj, 180);
    lv_obj_set_y(monitor_speed_mode_obj, 25);
    lv_obj_set_width(monitor_speed_mode_obj, LV_SIZE_CONTENT);
    lv_obj_set_height(monitor_speed_mode_obj, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(monitor_speed_mode_obj, 0, 0);
    lv_obj_set_style_bg_opa(monitor_speed_mode_obj, 0, 0);
    lv_obj_set_flag(monitor_speed_mode_obj, LV_OBJ_FLAG_HIDDEN, true);
    lv_obj_set_style_border_width(monitor_speed_mode_obj, 0, 0);
    lv_obj_t *column_2 = column_create(monitor_speed_mode_obj);
    lv_obj_set_style_pad_row(column_2, 6, 0);
    lv_obj_t *row_9 = row_create(column_2);
    lv_obj_set_style_flex_cross_place(row_9, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(row_9, 8, 0);
    lv_obj_t *lv_obj_7 = lv_obj_create(row_9);
    lv_obj_set_width(lv_obj_7, 70);
    lv_obj_set_height(lv_obj_7, 4);
    lv_obj_set_style_pad_all(lv_obj_7, 0, 0);
    lv_obj_set_style_bg_color(lv_obj_7, lv_color_hex(0x4ade80), 0);
    lv_obj_set_style_border_width(lv_obj_7, 0, 0);
    lv_obj_set_style_radius(lv_obj_7, 2, 0);

    lv_obj_t *lv_label_10 = lv_label_create(row_9);
    lv_label_set_text(lv_label_10, "speed");
    lv_obj_set_style_text_color(lv_label_10, lv_color_hex(0x4ade80), 0);
    lv_obj_set_style_text_font(lv_label_10, SemiBold_Font_24, 0);

    lv_obj_t *monitor_chart = lv_chart_create(lv_obj_4);
    lv_obj_set_name(monitor_chart, "monitor_chart");
    lv_obj_set_width(monitor_chart, 350);
    lv_obj_set_height(monitor_chart, 280);
    lv_chart_set_point_count(monitor_chart, 32);
    lv_obj_set_style_align(monitor_chart, LV_ALIGN_BOTTOM_MID, 0);
    lv_chart_set_update_mode(monitor_chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_obj_set_style_line_opa(monitor_chart, 0, 0);
    lv_obj_set_style_bg_color(monitor_chart, lv_color_hex(0x0f172a), 0);
    lv_obj_set_style_bg_opa(monitor_chart, 30, 0);
    lv_obj_set_style_radius(monitor_chart, 12, 0);
    lv_obj_set_style_border_width(monitor_chart, 1, 0);
    lv_obj_set_style_border_color(monitor_chart, lv_color_hex(0x3a6baf), 0);
    lv_obj_set_style_shadow_color(monitor_chart, lv_color_hex(0x313539), 0);
    lv_obj_set_style_shadow_width(monitor_chart, 20, 0);
    lv_obj_set_style_shadow_spread(monitor_chart, 3, 0);
    lv_obj_set_style_pad_all(monitor_chart, 10, 0);
    lv_obj_set_y(monitor_chart, -10);

    lv_obj_t *lv_obj_8 = lv_obj_create(lv_obj_0);
    lv_obj_set_width(lv_obj_8, 15);
    lv_obj_set_height(lv_obj_8, 20);
    lv_obj_set_style_border_width(lv_obj_8, 0, 0);
    lv_obj_set_flag(lv_obj_8, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_flag(lv_obj_8, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_radius(lv_obj_8, 0, 0);
    lv_obj_set_style_bg_color(lv_obj_8, lv_color_hex(0x1b2a3c), 0);
    lv_obj_set_x(lv_obj_8, 745);
    lv_obj_set_y(lv_obj_8, 20);

    lv_obj_t *lv_label_11 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_11, "v");
    lv_obj_set_style_text_font(lv_label_11, SemiBold_Font_16, 0);
    lv_obj_set_style_text_color(lv_label_11, lv_color_hex(0x19f0e1), 0);
    lv_obj_set_x(lv_label_11, 745);
    lv_obj_set_y(lv_label_11, 25);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
