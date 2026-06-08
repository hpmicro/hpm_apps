/**
 * @file header_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "header_gen.h"
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

lv_obj_t *header_create(lv_obj_t *parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_gauge_bg;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&style_gauge_bg);
        lv_style_set_width(&style_gauge_bg, 800);
        lv_style_set_height(&style_gauge_bg, 100);
        lv_style_set_radius(&style_gauge_bg, 0);
        lv_style_set_bg_color(&style_gauge_bg, lv_color_hex(0x0f172a));
        lv_style_set_bg_opa(&style_gauge_bg, 95);
        lv_style_set_bg_grad_color(&style_gauge_bg, lv_color_hex(0x1e1b4b));
        lv_style_set_bg_grad_dir(&style_gauge_bg, LV_GRAD_DIR_HOR);
        lv_style_set_border_width(&style_gauge_bg, 0);
        lv_style_set_border_side(&style_gauge_bg, LV_BORDER_SIDE_BOTTOM);
        lv_style_set_border_color(&style_gauge_bg, lv_color_hex(0x6366f1));
        lv_style_set_border_opa(&style_gauge_bg, 150);

        style_inited = true;
    }

    lv_obj_t *lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "header_#");
    lv_obj_set_width(lv_obj_0, 800);
    lv_obj_set_height(lv_obj_0, 100);
    lv_obj_set_style_pad_all(lv_obj_0, 0, 0);
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_radius(lv_obj_0, 0, 0);
    lv_obj_set_style_bg_opa(lv_obj_0, 0, 0);
    lv_obj_set_style_border_color(lv_obj_0, lv_color_hex(0x266f98), 0);
    lv_obj_set_style_border_width(lv_obj_0, 3, 0);
    lv_obj_set_style_border_side(lv_obj_0, LV_BORDER_SIDE_BOTTOM, 0);

    lv_obj_add_style(lv_obj_0, &style_gauge_bg, 0);
    lv_obj_t *lv_image_0 = lv_image_create(lv_obj_0);
    lv_image_set_src(lv_image_0, image_HPMicro_Logo);
    lv_obj_set_width(lv_image_0, 250);
    lv_obj_set_height(lv_image_0, 100);

    lv_obj_t *start_button = lv_button_create(lv_obj_0);
    lv_obj_set_name(start_button, "start_button");
    lv_obj_set_y(start_button, 15);
    lv_obj_set_x(start_button, 280);
    lv_obj_set_width(start_button, 100);
    lv_obj_set_height(start_button, 70);
    lv_obj_set_style_pad_all(start_button, 0, 0);
    lv_obj_set_style_bg_color(start_button, lv_color_hex(0x2a3c5e), 0);
    lv_obj_set_style_border_color(start_button, lv_color_hex(0x3b3939), 0);
    lv_obj_set_style_border_width(start_button, 4, 0);
    lv_obj_set_style_bg_image_src(start_button, image_start, 0);
    lv_obj_set_style_shadow_width(start_button, 0, 0);
    lv_obj_add_event_cb(start_button, start_button_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *direction_button = lv_button_create(lv_obj_0);
    lv_obj_set_name(direction_button, "direction_button");
    lv_obj_set_y(direction_button, 15);
    lv_obj_set_x(direction_button, 430);
    lv_obj_set_width(direction_button, 100);
    lv_obj_set_height(direction_button, 70);
    lv_obj_set_style_pad_all(direction_button, 0, 0);
    lv_obj_set_style_bg_color(direction_button, lv_color_hex(0x2a3c5e), 0);
    lv_obj_set_style_border_color(direction_button, lv_color_hex(0x3b3939), 0);
    lv_obj_set_style_border_width(direction_button, 4, 0);
    lv_obj_set_style_bg_image_src(direction_button, image_rotate_right, 0);
    lv_obj_set_style_shadow_width(direction_button, 0, 0);
    lv_obj_add_event_cb(direction_button, direction_button_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *dropdown_mode = lv_dropdown_create(lv_obj_0);
    lv_obj_set_name(dropdown_mode, "dropdown_mode");
    lv_obj_set_width(dropdown_mode, 200);
    lv_obj_set_height(dropdown_mode, 70);
    lv_obj_set_y(dropdown_mode, 15);
    lv_obj_set_x(dropdown_mode, 580);
    lv_dropdown_set_options(dropdown_mode, "CSP\nCSV");
    lv_dropdown_set_selected(dropdown_mode, 0);
    lv_obj_set_style_radius(dropdown_mode, 15, 0);
    lv_obj_set_style_bg_color(dropdown_mode, lv_color_hex(0x2a3c5e), 0);
    lv_obj_set_style_text_font(dropdown_mode, SemiBold_Font_32, 0);
    lv_obj_set_style_text_color(dropdown_mode, lv_color_hex(0xb0b3b7), 0);
    lv_obj_set_style_text_align(dropdown_mode, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_letter_space(dropdown_mode, 4, 0);
    lv_obj_set_style_border_width(dropdown_mode, 4, 0);
    lv_obj_set_style_border_color(dropdown_mode, lv_color_hex(0x3b3939), 0);
    lv_obj_set_flag(dropdown_mode, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_t *lv_dropdown_list_0 = lv_dropdown_get_list(dropdown_mode);
    lv_obj_set_style_border_width(lv_dropdown_list_0, 2, 0);
    lv_obj_set_style_border_color(lv_dropdown_list_0, lv_color_hex(0x2f5190), 0);
    lv_obj_set_style_bg_color(lv_dropdown_list_0, lv_color_hex(0x2a3c5e), 0);
    lv_obj_set_style_shadow_width(lv_dropdown_list_0, 20, 0);
    lv_obj_set_style_shadow_opa(lv_dropdown_list_0, 150, 0);
    lv_obj_set_style_shadow_spread(lv_dropdown_list_0, 3, 0);
    lv_obj_set_style_text_font(lv_dropdown_list_0, SemiBold_Font_32, 0);
    lv_obj_set_style_text_align(lv_dropdown_list_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_letter_space(lv_dropdown_list_0, 3, 0);
    lv_obj_set_style_text_color(lv_dropdown_list_0, lv_color_hex(0x1be6e2), 0);
    lv_obj_add_event_cb(dropdown_mode, mode_change_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_width(lv_obj_1, 30);
    lv_obj_set_height(lv_obj_1, 50);
    lv_obj_set_x(lv_obj_1, 743);
    lv_obj_set_y(lv_obj_1, 23);
    lv_obj_set_style_border_width(lv_obj_1, 0, 0);
    lv_obj_set_style_bg_color(lv_obj_1, lv_color_hex(0x2a3c5e), 0);

    lv_obj_t *lv_label_0 = lv_label_create(lv_obj_0);
    lv_label_set_text(lv_label_0, "V");
    lv_obj_set_style_text_font(lv_label_0, SemiBold_Font_20, 0);
    lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xe0e0e0), 0);
    lv_obj_set_x(lv_label_0, 745);
    lv_obj_set_y(lv_label_0, 40);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
