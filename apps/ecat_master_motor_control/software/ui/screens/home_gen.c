/**
 * @file home_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "home_gen.h"
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

lv_obj_t * home_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    static bool style_inited = false;

    if (!style_inited) {

        style_inited = true;
    }

    if (home == NULL) home = lv_obj_create(NULL);
    lv_obj_t * lv_obj_0 = home;
    lv_obj_set_name_static(lv_obj_0, "home_#");
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_radius(lv_obj_0, 0, 0);
    lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x152234), 0);

    lv_obj_t * header_0 = header_create(lv_obj_0);
    
    lv_obj_t * monitor_0 = monitor_create(lv_obj_0);
    lv_obj_set_y(monitor_0, 120);
    
    lv_obj_t * speed_slider_0 = speed_slider_create(lv_obj_0);
    lv_obj_set_y(speed_slider_0, 611);
    lv_obj_set_x(speed_slider_0, 0);
    
    lv_obj_t * in_output_0 = in_output_create(lv_obj_0);
    lv_obj_set_y(in_output_0, 810);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

