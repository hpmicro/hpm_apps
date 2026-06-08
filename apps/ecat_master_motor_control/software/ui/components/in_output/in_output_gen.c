/**
 * @file in_output_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "in_output_gen.h"
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

static lv_anim_timeline_t * timeline_input_show_create(lv_obj_t * obj);
static lv_anim_timeline_t * timeline_output_show_create(lv_obj_t * obj);
static void free_timeline_event_cb(lv_event_t * e);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * in_output_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_data_card;

    static bool style_inited = false;

    if (!style_inited) {
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

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "in_output_#");
    lv_obj_set_width(lv_obj_0, 798);
    lv_obj_set_height(lv_obj_0, 450);
    lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x0f2558), 0);
    lv_obj_set_style_bg_opa(lv_obj_0, 20, 0);
    lv_obj_set_style_border_width(lv_obj_0, 3, 0);
    lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x234b7a), 0);
    lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

    lv_obj_t * row_0 = row_create(lv_obj_0);
    lv_obj_set_y(row_0, 20);
    lv_obj_set_align(row_0, LV_ALIGN_TOP_MID);
    lv_obj_set_style_pad_column(row_0, 20, 0);
    lv_obj_t * button_output = lv_button_create(row_0);
    lv_obj_set_name(button_output, "button_output");
    lv_obj_set_width(button_output, 360);
    lv_obj_set_height(button_output, 60);
    lv_obj_set_style_pad_all(button_output, 0, 0);
    lv_obj_set_style_radius(button_output, 20, 0);
    lv_obj_set_style_bg_color(button_output, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_bg_opa(button_output, 30, 0);
    lv_obj_set_style_border_color(button_output, lv_color_hex(0xed6666), 0);
    lv_obj_set_style_border_opa(button_output, 120, 0);
    lv_obj_set_style_border_width(button_output, 5, 0);
    lv_obj_add_event_cb(button_output, output_select_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * button_input = lv_button_create(row_0);
    lv_obj_set_name(button_input, "button_input");
    lv_obj_set_width(button_input, 360);
    lv_obj_set_height(button_input, 60);
    lv_obj_set_style_pad_all(button_input, 0, 0);
    lv_obj_set_style_radius(button_input, 20, 0);
    lv_obj_set_style_bg_color(button_input, lv_color_hex(0xb1a9a9), 0);
    lv_obj_set_style_bg_opa(button_input, 30, 0);
    lv_obj_set_style_border_color(button_input, lv_color_hex(0x00a0ef), 0);
    lv_obj_set_style_border_opa(button_input, 120, 0);
    lv_obj_set_style_border_width(button_input, 5, 0);
    lv_obj_add_event_cb(button_input, input_select_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * label_output = lv_label_create(lv_obj_0);
    lv_obj_set_name(label_output, "label_output");
    lv_obj_set_x(label_output, 120);
    lv_obj_set_y(label_output, 32);
    lv_label_set_text(label_output, "OUTPUT");
    lv_obj_set_style_text_color(label_output, lv_color_hex(0xb44545), 0);
    lv_obj_set_style_text_font(label_output, SemiBold_Font_44, 0);
    
    lv_obj_t * label_input = lv_label_create(lv_obj_0);
    lv_obj_set_name(label_input, "label_input");
    lv_obj_set_x(label_input, 520);
    lv_obj_set_y(label_input, 32);
    lv_label_set_text(label_input, "INPUT");
    lv_obj_set_style_text_color(label_input, lv_color_hex(0x399ab5), 0);
    lv_obj_set_style_text_font(label_input, SemiBold_Font_44, 0);
    
    lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_x(lv_obj_1, 20);
    lv_obj_set_y(lv_obj_1, 100);
    lv_obj_set_width(lv_obj_1, 748);
    lv_obj_set_height(lv_obj_1, 310);
    lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_add_style(lv_obj_1, &style_data_card, 0);
    lv_obj_t * in_outobj = lv_obj_create(lv_obj_1);
    lv_obj_set_name(in_outobj, "in_outobj");
    lv_obj_set_x(in_outobj, 20);
    lv_obj_set_style_bg_opa(in_outobj, 0, 0);
    lv_obj_set_style_pad_all(in_outobj, 0, 0);
    lv_obj_set_style_border_width(in_outobj, 0, 0);
    lv_obj_set_width(in_outobj, 1500);
    lv_obj_set_height(in_outobj, 310);
    lv_obj_t * column_0 = column_create(in_outobj);
    lv_obj_set_x(column_0, 35);
    lv_obj_set_y(column_0, 130);
    lv_obj_set_style_pad_row(column_0, 125, 0);
    lv_obj_t * row_1 = row_create(column_0);
    lv_obj_set_style_pad_column(row_1, 38, 0);
    lv_obj_t * label_output_01 = lv_label_create(row_1);
    lv_obj_set_name(label_output_01, "label_output_01");
    lv_label_set_text(label_output_01, "DO_01");
    lv_obj_set_style_text_font(label_output_01, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_01, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * label_output_02 = lv_label_create(row_1);
    lv_obj_set_name(label_output_02, "label_output_02");
    lv_label_set_text(label_output_02, "DO_02");
    lv_obj_set_style_text_font(label_output_02, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_02, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * label_output_03 = lv_label_create(row_1);
    lv_obj_set_name(label_output_03, "label_output_03");
    lv_label_set_text(label_output_03, "DO_03");
    lv_obj_set_style_text_font(label_output_03, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_03, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * label_output_04 = lv_label_create(row_1);
    lv_obj_set_name(label_output_04, "label_output_04");
    lv_label_set_text(label_output_04, "DO_04");
    lv_obj_set_style_text_font(label_output_04, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_04, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * label_output_05 = lv_label_create(row_1);
    lv_obj_set_name(label_output_05, "label_output_05");
    lv_label_set_text(label_output_05, "DO_05");
    lv_obj_set_style_text_font(label_output_05, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_05, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * label_output_06 = lv_label_create(row_1);
    lv_obj_set_name(label_output_06, "label_output_06");
    lv_label_set_text(label_output_06, "DO_06");
    lv_obj_set_style_text_font(label_output_06, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_06, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * row_2 = row_create(column_0);
    lv_obj_set_style_pad_column(row_2, 40, 0);
    lv_obj_t * label_output_07 = lv_label_create(row_2);
    lv_obj_set_name(label_output_07, "label_output_07");
    lv_label_set_text(label_output_07, "DO_07");
    lv_obj_set_style_text_font(label_output_07, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_07, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * label_output_08 = lv_label_create(row_2);
    lv_obj_set_name(label_output_08, "label_output_08");
    lv_label_set_text(label_output_08, "DO_08");
    lv_obj_set_style_text_font(label_output_08, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_08, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * label_output_09 = lv_label_create(row_2);
    lv_obj_set_name(label_output_09, "label_output_09");
    lv_label_set_text(label_output_09, "DO_09");
    lv_obj_set_style_text_font(label_output_09, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_09, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * label_output_10 = lv_label_create(row_2);
    lv_obj_set_name(label_output_10, "label_output_10");
    lv_label_set_text(label_output_10, "DO_10");
    lv_obj_set_style_text_font(label_output_10, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_10, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * label_output_11 = lv_label_create(row_2);
    lv_obj_set_name(label_output_11, "label_output_11");
    lv_label_set_text(label_output_11, "DO_11");
    lv_obj_set_style_text_font(label_output_11, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_11, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * label_output_12 = lv_label_create(row_2);
    lv_obj_set_name(label_output_12, "label_output_12");
    lv_label_set_text(label_output_12, "DO_12");
    lv_obj_set_style_text_font(label_output_12, SemiBold_Font_24, 0);
    lv_obj_set_style_text_color(label_output_12, lv_color_hex(0x5290e2), 0);
    
    lv_obj_t * column_1 = column_create(in_outobj);
    lv_obj_set_style_pad_row(column_1, 50, 0);
    lv_obj_set_y(column_1, 10);
    lv_obj_set_style_pad_all(column_1, 20, 0);
    lv_obj_t * row_3 = row_create(column_1);
    lv_obj_set_style_pad_column(row_3, 30, 0);
    lv_obj_set_style_pad_all(row_3, 10, 0);
    lv_obj_t * lv_button_0 = lv_button_create(row_3);
    lv_obj_set_width(lv_button_0, 85);
    lv_obj_set_height(lv_button_0, 80);
    lv_obj_set_style_pad_all(lv_button_0, 0, 0);
    lv_obj_set_style_bg_color(lv_button_0, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_0, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_0, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_0, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_0, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_0, 5, 0);
    lv_obj_add_event_cb(lv_button_0, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_1 = lv_button_create(row_3);
    lv_obj_set_width(lv_button_1, 85);
    lv_obj_set_height(lv_button_1, 80);
    lv_obj_set_style_pad_all(lv_button_1, 0, 0);
    lv_obj_set_style_bg_color(lv_button_1, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_1, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_1, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_1, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_1, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_1, 5, 0);
    lv_obj_add_event_cb(lv_button_1, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_2 = lv_button_create(row_3);
    lv_obj_set_width(lv_button_2, 85);
    lv_obj_set_height(lv_button_2, 80);
    lv_obj_set_style_pad_all(lv_button_2, 0, 0);
    lv_obj_set_style_bg_color(lv_button_2, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_2, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_2, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_2, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_2, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_2, 5, 0);
    lv_obj_add_event_cb(lv_button_2, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_3 = lv_button_create(row_3);
    lv_obj_set_width(lv_button_3, 85);
    lv_obj_set_height(lv_button_3, 80);
    lv_obj_set_style_pad_all(lv_button_3, 0, 0);
    lv_obj_set_style_bg_color(lv_button_3, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_3, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_3, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_3, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_3, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_3, 5, 0);
    lv_obj_add_event_cb(lv_button_3, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_4 = lv_button_create(row_3);
    lv_obj_set_width(lv_button_4, 85);
    lv_obj_set_height(lv_button_4, 80);
    lv_obj_set_style_pad_all(lv_button_4, 0, 0);
    lv_obj_set_style_bg_color(lv_button_4, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_4, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_4, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_4, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_4, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_4, 5, 0);
    lv_obj_add_event_cb(lv_button_4, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_5 = lv_button_create(row_3);
    lv_obj_set_width(lv_button_5, 85);
    lv_obj_set_height(lv_button_5, 80);
    lv_obj_set_style_pad_all(lv_button_5, 0, 0);
    lv_obj_set_style_bg_color(lv_button_5, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_5, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_5, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_5, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_5, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_5, 5, 0);
    lv_obj_add_event_cb(lv_button_5, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * row_4 = row_create(column_1);
    lv_obj_set_style_pad_column(row_4, 30, 0);
    lv_obj_set_style_pad_all(row_4, 10, 0);
    lv_obj_t * lv_button_6 = lv_button_create(row_4);
    lv_obj_set_width(lv_button_6, 85);
    lv_obj_set_height(lv_button_6, 80);
    lv_obj_set_style_pad_all(lv_button_6, 0, 0);
    lv_obj_set_style_bg_color(lv_button_6, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_6, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_6, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_6, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_6, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_6, 5, 0);
    lv_obj_add_event_cb(lv_button_6, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_7 = lv_button_create(row_4);
    lv_obj_set_width(lv_button_7, 85);
    lv_obj_set_height(lv_button_7, 80);
    lv_obj_set_style_pad_all(lv_button_7, 0, 0);
    lv_obj_set_style_bg_color(lv_button_7, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_7, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_7, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_7, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_7, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_7, 5, 0);
    lv_obj_add_event_cb(lv_button_7, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_8 = lv_button_create(row_4);
    lv_obj_set_width(lv_button_8, 85);
    lv_obj_set_height(lv_button_8, 80);
    lv_obj_set_style_pad_all(lv_button_8, 0, 0);
    lv_obj_set_style_bg_color(lv_button_8, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_8, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_8, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_8, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_8, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_8, 5, 0);
    lv_obj_add_event_cb(lv_button_8, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_9 = lv_button_create(row_4);
    lv_obj_set_width(lv_button_9, 85);
    lv_obj_set_height(lv_button_9, 80);
    lv_obj_set_style_pad_all(lv_button_9, 0, 0);
    lv_obj_set_style_bg_color(lv_button_9, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_9, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_9, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_9, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_9, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_9, 5, 0);
    lv_obj_add_event_cb(lv_button_9, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_10 = lv_button_create(row_4);
    lv_obj_set_width(lv_button_10, 85);
    lv_obj_set_height(lv_button_10, 80);
    lv_obj_set_style_pad_all(lv_button_10, 0, 0);
    lv_obj_set_style_bg_color(lv_button_10, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_10, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_10, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_10, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_10, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_10, 5, 0);
    lv_obj_add_event_cb(lv_button_10, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * lv_button_11 = lv_button_create(row_4);
    lv_obj_set_width(lv_button_11, 85);
    lv_obj_set_height(lv_button_11, 80);
    lv_obj_set_style_pad_all(lv_button_11, 0, 0);
    lv_obj_set_style_bg_color(lv_button_11, lv_color_hex(0x163b58), 0);
    lv_obj_set_style_border_color(lv_button_11, lv_color_hex(0x2b6384), 0);
    lv_obj_set_style_border_width(lv_button_11, 3, 0);
    lv_obj_set_style_shadow_color(lv_button_11, lv_color_hex(0x02a4ff), 0);
    lv_obj_set_style_shadow_spread(lv_button_11, 1, 0);
    lv_obj_set_style_shadow_width(lv_button_11, 5, 0);
    lv_obj_add_event_cb(lv_button_11, output_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * column_2 = column_create(in_outobj);
    lv_obj_set_x(column_2, 800);
    lv_obj_set_y(column_2, 130);
    lv_obj_set_style_pad_row(column_2, 125, 0);
    lv_obj_t * row_5 = row_create(column_2);
    lv_obj_set_style_pad_column(row_5, 48, 0);
    lv_obj_t * lv_label_0 = lv_label_create(row_5);
    lv_label_set_text(lv_label_0, "DI_01");
    lv_obj_set_style_text_font(lv_label_0, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_0, &style_label_normal, 0);
    
    lv_obj_t * lv_label_1 = lv_label_create(row_5);
    lv_label_set_text(lv_label_1, "DI_02");
    lv_obj_set_style_text_font(lv_label_1, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_1, &style_label_normal, 0);
    
    lv_obj_t * lv_label_2 = lv_label_create(row_5);
    lv_label_set_text(lv_label_2, "DI_03");
    lv_obj_set_style_text_font(lv_label_2, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_2, &style_label_normal, 0);
    
    lv_obj_t * lv_label_3 = lv_label_create(row_5);
    lv_label_set_text(lv_label_3, "DI_04");
    lv_obj_set_style_text_font(lv_label_3, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_3, &style_label_normal, 0);
    
    lv_obj_t * lv_label_4 = lv_label_create(row_5);
    lv_label_set_text(lv_label_4, "DI_05");
    lv_obj_set_style_text_font(lv_label_4, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_4, &style_label_normal, 0);
    
    lv_obj_t * lv_label_5 = lv_label_create(row_5);
    lv_label_set_text(lv_label_5, "DI_06");
    lv_obj_set_style_text_font(lv_label_5, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_5, &style_label_normal, 0);
    
    lv_obj_t * row_6 = row_create(column_2);
    lv_obj_set_style_pad_column(row_6, 52, 0);
    lv_obj_t * lv_label_6 = lv_label_create(row_6);
    lv_label_set_text(lv_label_6, "DI_07");
    lv_obj_set_style_text_font(lv_label_6, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_6, &style_label_normal, 0);
    
    lv_obj_t * lv_label_7 = lv_label_create(row_6);
    lv_label_set_text(lv_label_7, "DI_08");
    lv_obj_set_style_text_font(lv_label_7, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_7, &style_label_normal, 0);
    
    lv_obj_t * lv_label_8 = lv_label_create(row_6);
    lv_label_set_text(lv_label_8, "DI_09");
    lv_obj_set_style_text_font(lv_label_8, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_8, &style_label_normal, 0);
    
    lv_obj_t * lv_label_9 = lv_label_create(row_6);
    lv_label_set_text(lv_label_9, "DI_10");
    lv_obj_set_style_text_font(lv_label_9, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_9, &style_label_normal, 0);
    
    lv_obj_t * lv_label_10 = lv_label_create(row_6);
    lv_label_set_text(lv_label_10, "DI_11");
    lv_obj_set_style_text_font(lv_label_10, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_10, &style_label_normal, 0);
    
    lv_obj_t * lv_label_11 = lv_label_create(row_6);
    lv_label_set_text(lv_label_11, "DI_12");
    lv_obj_set_style_text_font(lv_label_11, SemiBold_Font_24, 0);
    lv_obj_add_style(lv_label_11, &style_label_normal, 0);
    
    lv_obj_t * column_3 = column_create(in_outobj);
    lv_obj_set_x(column_3, 765);
    lv_obj_set_y(column_3, 10);
    lv_obj_set_style_pad_row(column_3, 50, 0);
    lv_obj_set_style_pad_all(column_3, 20, 0);
    lv_obj_t * row_7 = row_create(column_3);
    lv_obj_set_style_pad_column(row_7, 30, 0);
    lv_obj_set_style_pad_all(row_7, 10, 0);
    lv_obj_t * obj_1 = lv_obj_create(row_7);
    lv_obj_set_name(obj_1, "obj_1");
    lv_obj_add_style(obj_1, &style_input_normal, 0);
    lv_obj_bind_style(obj_1, &style_input_change, 0, &subject_input_1, 1);
    
    lv_obj_t * obj_2 = lv_obj_create(row_7);
    lv_obj_set_name(obj_2, "obj_2");
    lv_obj_add_style(obj_2, &style_input_normal, 0);
    lv_obj_bind_style(obj_2, &style_input_change, 0, &subject_input_2, 1);
    
    lv_obj_t * obj_3 = lv_obj_create(row_7);
    lv_obj_set_name(obj_3, "obj_3");
    lv_obj_add_style(obj_3, &style_input_normal, 0);
    lv_obj_bind_style(obj_3, &style_input_change, 0, &subject_input_3, 1);
    
    lv_obj_t * obj_4 = lv_obj_create(row_7);
    lv_obj_set_name(obj_4, "obj_4");
    lv_obj_add_style(obj_4, &style_input_normal, 0);
    lv_obj_bind_style(obj_4, &style_input_change, 0, &subject_input_4, 1);
    
    lv_obj_t * obj_5 = lv_obj_create(row_7);
    lv_obj_set_name(obj_5, "obj_5");
    lv_obj_add_style(obj_5, &style_input_normal, 0);
    lv_obj_bind_style(obj_5, &style_input_change, 0, &subject_input_5, 1);
    
    lv_obj_t * obj_6 = lv_obj_create(row_7);
    lv_obj_set_name(obj_6, "obj_6");
    lv_obj_add_style(obj_6, &style_input_normal, 0);
    lv_obj_bind_style(obj_6, &style_input_change, 0, &subject_input_6, 1);
    
    lv_obj_t * row_8 = row_create(column_3);
    lv_obj_set_style_pad_column(row_8, 30, 0);
    lv_obj_set_style_pad_all(row_8, 10, 0);
    lv_obj_t * obj_7 = lv_obj_create(row_8);
    lv_obj_set_name(obj_7, "obj_7");
    lv_obj_add_style(obj_7, &style_input_normal, 0);
    lv_obj_bind_style(obj_7, &style_input_change, 0, &subject_input_7, 1);
    
    lv_obj_t * obj_8 = lv_obj_create(row_8);
    lv_obj_set_name(obj_8, "obj_8");
    lv_obj_add_style(obj_8, &style_input_normal, 0);
    lv_obj_bind_style(obj_8, &style_input_change, 0, &subject_input_8, 1);
    
    lv_obj_t * obj_9 = lv_obj_create(row_8);
    lv_obj_set_name(obj_9, "obj_9");
    lv_obj_add_style(obj_9, &style_input_normal, 0);
    lv_obj_bind_style(obj_9, &style_input_change, 0, &subject_input_9, 1);
    
    lv_obj_t * obj_10 = lv_obj_create(row_8);
    lv_obj_set_name(obj_10, "obj_10");
    lv_obj_add_style(obj_10, &style_input_normal, 0);
    lv_obj_bind_style(obj_10, &style_input_change, 0, &subject_input_10, 1);
    
    lv_obj_t * obj_11 = lv_obj_create(row_8);
    lv_obj_set_name(obj_11, "obj_11");
    lv_obj_add_style(obj_11, &style_input_normal, 0);
    lv_obj_bind_style(obj_11, &style_input_change, 0, &subject_input_11, 1);
    
    lv_obj_t * obj_12 = lv_obj_create(row_8);
    lv_obj_set_name(obj_12, "obj_12");
    lv_obj_add_style(obj_12, &style_input_normal, 0);
    lv_obj_bind_style(obj_12, &style_input_change, 0, &subject_input_12, 1);
    
    
    /* create animation timeline(s) */
    lv_anim_timeline_t ** at_array = lv_malloc(sizeof(lv_anim_timeline_t *) * _IN_OUTPUT_TIMELINE_CNT);
    at_array[IN_OUTPUT_TIMELINE_INPUT_SHOW] = timeline_input_show_create(lv_obj_0);
    at_array[IN_OUTPUT_TIMELINE_OUTPUT_SHOW] = timeline_output_show_create(lv_obj_0);
    lv_obj_set_user_data(lv_obj_0, at_array);
    lv_obj_add_event_cb(lv_obj_0, free_timeline_event_cb, LV_EVENT_DELETE, at_array);

    lv_obj_add_play_timeline_event(button_output, LV_EVENT_CLICKED, in_output_get_timeline(lv_obj_0, IN_OUTPUT_TIMELINE_OUTPUT_SHOW), 0, false);
    lv_obj_add_play_timeline_event(button_input, LV_EVENT_CLICKED, in_output_get_timeline(lv_obj_0, IN_OUTPUT_TIMELINE_INPUT_SHOW), 0, false);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

lv_anim_timeline_t * in_output_get_timeline(lv_obj_t * obj, in_output_timeline_t timeline_id)
{
    if (timeline_id >= _IN_OUTPUT_TIMELINE_CNT) {
        LV_LOG_WARN("in_output has no timeline with %d ID", timeline_id);
        return NULL;
    }

    lv_anim_timeline_t ** at_array = lv_obj_get_user_data(obj);
    return at_array[timeline_id];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Helper function to execute animations */
static void int_anim_exec_cb(lv_anim_t * a, int32_t v)
{
    uint32_t data = (lv_uintptr_t)lv_anim_get_user_data(a);
    lv_style_prop_t prop = data >> 24;
    lv_style_selector_t selector = data & 0x00ffffff;

    lv_style_value_t style_value;
    style_value.num = v;
    lv_obj_set_local_style_prop(a->var, prop, style_value, selector);
}

static lv_anim_timeline_t * timeline_input_show_create(lv_obj_t * obj)
{
    lv_anim_timeline_t * at = lv_anim_timeline_create();
    lv_anim_timeline_t * at_to_merge = NULL;

    lv_anim_t a;
    uint32_t selector_and_prop;

    selector_and_prop = ((LV_STYLE_TRANSLATE_X & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, lv_obj_find_by_name(obj, "in_outobj"));
    lv_anim_set_values(&a, 20, -760);
    lv_anim_set_duration(&a, 250);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_timeline_add(at, 0, &a);

    return at;
}

static lv_anim_timeline_t * timeline_output_show_create(lv_obj_t * obj)
{
    lv_anim_timeline_t * at = lv_anim_timeline_create();
    lv_anim_timeline_t * at_to_merge = NULL;

    lv_anim_t a;
    uint32_t selector_and_prop;

    selector_and_prop = ((LV_STYLE_TRANSLATE_X & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, lv_obj_find_by_name(obj, "in_outobj"));
    lv_anim_set_values(&a, -750, 5);
    lv_anim_set_duration(&a, 250);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_timeline_add(at, 0, &a);

    return at;
}

static void free_timeline_event_cb(lv_event_t * e)
{
    lv_anim_timeline_t ** at_array = lv_event_get_user_data(e);
    uint32_t i;
    for(i = 0; i < _IN_OUTPUT_TIMELINE_CNT; i++) {
        lv_anim_timeline_delete(at_array[i]);
    }
    lv_free(at_array);
}

