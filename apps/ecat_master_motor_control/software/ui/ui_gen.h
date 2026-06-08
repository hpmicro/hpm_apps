/**
 * @file ui_gen.h
 */

#ifndef UI_GEN_H
#define UI_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

extern lv_obj_t * home;

/*----------------
 * Global styles
 *----------------*/

extern lv_style_t style_button_normal;
extern lv_style_t style_button_pressed;
extern lv_style_t style_input_normal;
extern lv_style_t style_input_change;
extern lv_style_t style_label_normal;

/*----------------
 * Fonts
 *----------------*/

extern lv_font_t * SemiBold_Font_16;

extern lv_font_t * SemiBold_Font_20;

extern lv_font_t * SemiBold_Font_24;

extern lv_font_t * SemiBold_Font_28;

extern lv_font_t * SemiBold_Font_32;

extern lv_font_t * SemiBold_Font_36;

extern lv_font_t * SemiBold_Font_44;

extern lv_font_t * NotoSerifCJKsc_SemiBold_28;

/*----------------
 * Images
 *----------------*/

extern const void * image_start;
extern const void * image_HPMicro_Logo;
extern const void * image_device;
extern const void * image_state;
extern const void * image_address;
extern const void * image_input;
extern const void * image_output;
extern const void * image_monitor;
extern const void * image_speed;
extern const void * image_gear;
extern const void * image_rotate_right;
extern const void * image_rotate_left;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t speed_slider_value_subject;
extern lv_subject_t current_position_subject;
extern lv_subject_t subject_input_1;
extern lv_subject_t subject_input_2;
extern lv_subject_t subject_input_3;
extern lv_subject_t subject_input_4;
extern lv_subject_t subject_input_5;
extern lv_subject_t subject_input_6;
extern lv_subject_t subject_input_7;
extern lv_subject_t subject_input_8;
extern lv_subject_t subject_input_9;
extern lv_subject_t subject_input_10;
extern lv_subject_t subject_input_11;
extern lv_subject_t subject_input_12;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

void start_button_cb(lv_event_t * e);
void direction_button_cb(lv_event_t * e);
void mode_change_cb(lv_event_t * e);
void output_select_button_cb(lv_event_t * e);
void input_select_button_cb(lv_event_t * e);
void output_button_cb(lv_event_t * e);
void slave_dropdown_change_cb(lv_event_t * e);
void speed_slider_change_cb(lv_event_t * e);

/**
 * Initialize the component library
 */

void ui_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widget and components of this library*/
#include "components/column/column_gen.h"
#include "components/header/header_gen.h"
#include "components/in_output/in_output_gen.h"
#include "components/monitor/monitor_gen.h"
#include "components/row/row_gen.h"
#include "components/speed_slider/speed_slider_gen.h"
#include "screens/home_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_GEN_H*/