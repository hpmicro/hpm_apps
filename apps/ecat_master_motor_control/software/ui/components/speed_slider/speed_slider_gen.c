/**
 * @file speed_slider_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "speed_slider_gen.h"
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

lv_obj_t * speed_slider_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_slider_knob;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&style_slider_knob);
        lv_style_set_bg_image_src(&style_slider_knob, image_gear);
        lv_style_set_bg_opa(&style_slider_knob, 0);
        lv_style_set_pad_all(&style_slider_knob, 20);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "speed_slider_#");
    lv_obj_set_width(lv_obj_0, 800);
    lv_obj_set_height(lv_obj_0, 145);
    lv_obj_set_style_bg_opa(lv_obj_0, 0, 0);
    lv_obj_set_style_border_width(lv_obj_0, 0, 0);
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_radius(lv_obj_0, 24, 0);
    lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
    lv_obj_set_y(lv_obj_0, 600);

    lv_obj_t * label_control = lv_label_create(lv_obj_0);
    lv_obj_set_name(label_control, "label_control");
    lv_obj_set_x(label_control, 35);
    lv_obj_set_y(label_control, 10);
    lv_label_set_text(label_control, "CONTROL");
    lv_obj_set_style_text_color(label_control, lv_color_hex(0x24cad0), 0);
    lv_obj_set_style_text_font(label_control, SemiBold_Font_32, 0);
    lv_obj_set_style_text_letter_space(label_control, 2, 0);
    
    lv_obj_t * row_0 = row_create(lv_obj_0);
    lv_obj_set_y(row_0, 0);
    lv_obj_set_x(row_0, 645);
    lv_obj_set_style_flex_cross_place(row_0, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_t * lv_image_0 = lv_image_create(row_0);
    lv_image_set_src(lv_image_0, image_speed);
    lv_obj_set_width(lv_image_0, 50);
    lv_obj_set_height(lv_image_0, 55);
    lv_image_set_inner_align(lv_image_0, LV_IMAGE_ALIGN_STRETCH);
    
    lv_obj_t * label_speed_value = lv_label_create(row_0);
    lv_obj_set_name(label_speed_value, "label_speed_value");
    lv_label_bind_text(label_speed_value, &speed_slider_value_subject, NULL);
    lv_obj_set_style_text_color(label_speed_value, lv_color_hex(0x24cad0), 0);
    lv_obj_set_style_text_font(label_speed_value, SemiBold_Font_44, 0);
    lv_obj_set_style_bg_opa(label_speed_value, 0, 0);
    
    lv_obj_t * slider_control = lv_slider_create(lv_obj_0);
    lv_obj_set_name(slider_control, "slider_control");
    lv_obj_set_x(slider_control, 60);
    lv_obj_set_y(slider_control, 65);
    lv_obj_set_width(slider_control, 680);
    lv_obj_set_height(slider_control, 32);
    lv_slider_set_min_value(slider_control, 0);
    lv_slider_set_max_value(slider_control, 100);
    lv_slider_bind_value(slider_control, &speed_slider_value_subject);
    lv_obj_set_style_bg_color(slider_control, lv_color_hex(0xe0e0e0), 0);
    lv_obj_set_style_bg_opa(slider_control, 100, 0);
    lv_obj_set_style_radius(slider_control, 10, 0);
    lv_obj_set_style_pad_all(slider_control, 0, 0);
    lv_obj_set_style_border_width(slider_control, 1, 0);
    lv_obj_set_style_border_color(slider_control, lv_color_hex(0x334155), 0);
    lv_obj_set_style_border_opa(slider_control, 150, 0);
    lv_obj_set_style_outline_width(slider_control, 0, 0);
    lv_obj_add_style(slider_control, &style_slider_knob, LV_PART_KNOB);
    lv_obj_add_event_cb(slider_control, speed_slider_change_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(slider_control, speed_slider_change_cb, LV_EVENT_RELEASED, NULL);
    
    lv_obj_t * row_1 = row_create(lv_obj_0);
    lv_obj_set_x(row_1, 55);
    lv_obj_set_y(row_1, 110);
    lv_obj_set_width(row_1, 700);
    lv_obj_set_style_flex_main_place(row_1, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_style_bg_opa(row_1, 0, 0);
    lv_obj_t * lv_label_0 = lv_label_create(row_1);
    lv_label_set_text(lv_label_0, "0");
    lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0x24cad0), 0);
    lv_obj_set_style_text_font(lv_label_0, SemiBold_Font_24, 0);
    
    lv_obj_t * lv_label_1 = lv_label_create(row_1);
    lv_label_set_text(lv_label_1, "10");
    lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0x24cad0), 0);
    lv_obj_set_style_text_font(lv_label_1, SemiBold_Font_24, 0);
    
    lv_obj_t * lv_label_2 = lv_label_create(row_1);
    lv_label_set_text(lv_label_2, "20");
    lv_obj_set_style_text_color(lv_label_2, lv_color_hex(0x24cad0), 0);
    lv_obj_set_style_text_font(lv_label_2, SemiBold_Font_24, 0);
    
    lv_obj_t * lv_label_3 = lv_label_create(row_1);
    lv_label_set_text(lv_label_3, "30");
    lv_obj_set_style_text_color(lv_label_3, lv_color_hex(0x24cad0), 0);
    lv_obj_set_style_text_font(lv_label_3, SemiBold_Font_24, 0);
    
    lv_obj_t * lv_label_4 = lv_label_create(row_1);
    lv_label_set_text(lv_label_4, "40");
    lv_obj_set_style_text_color(lv_label_4, lv_color_hex(0x24cad0), 0);
    lv_obj_set_style_text_font(lv_label_4, SemiBold_Font_24, 0);
    
    lv_obj_t * lv_label_5 = lv_label_create(row_1);
    lv_label_set_text(lv_label_5, "50");
    lv_obj_set_style_text_color(lv_label_5, lv_color_hex(0xd3ab46), 0);
    lv_obj_set_style_text_font(lv_label_5, SemiBold_Font_24, 0);
    
    lv_obj_t * lv_label_6 = lv_label_create(row_1);
    lv_label_set_text(lv_label_6, "60");
    lv_obj_set_style_text_color(lv_label_6, lv_color_hex(0xd3ab46), 0);
    lv_obj_set_style_text_font(lv_label_6, SemiBold_Font_24, 0);
    
    lv_obj_t * lv_label_7 = lv_label_create(row_1);
    lv_label_set_text(lv_label_7, "70");
    lv_obj_set_style_text_color(lv_label_7, lv_color_hex(0xd3ab46), 0);
    lv_obj_set_style_text_font(lv_label_7, SemiBold_Font_24, 0);
    
    lv_obj_t * lv_label_8 = lv_label_create(row_1);
    lv_label_set_text(lv_label_8, "80");
    lv_obj_set_style_text_color(lv_label_8, lv_color_hex(0xd3ab46), 0);
    lv_obj_set_style_text_font(lv_label_8, SemiBold_Font_24, 0);
    
    lv_obj_t * lv_label_9 = lv_label_create(row_1);
    lv_label_set_text(lv_label_9, "90");
    lv_obj_set_style_text_color(lv_label_9, lv_color_hex(0xe13e3e), 0);
    lv_obj_set_style_text_font(lv_label_9, SemiBold_Font_24, 0);
    
    lv_obj_t * lv_label_10 = lv_label_create(row_1);
    lv_label_set_text(lv_label_10, "100");
    lv_obj_set_style_text_color(lv_label_10, lv_color_hex(0xe13e3e), 0);
    lv_obj_set_style_text_font(lv_label_10, SemiBold_Font_24, 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

