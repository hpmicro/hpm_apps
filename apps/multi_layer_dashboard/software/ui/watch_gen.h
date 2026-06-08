/**
 * @file watch_gen.h
 */

#ifndef WATCH_GEN_H
#define WATCH_GEN_H

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

extern lv_obj_t *home;
extern lv_obj_t *home_layer2;
extern lv_obj_t *home_layer3;
extern lv_obj_t *home_layer4;
extern lv_obj_t *home_layer5;
extern lv_obj_t *home_layer6;
extern lv_obj_t *home_layer7;
extern lv_obj_t *home_layer8;

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Fonts
 *----------------*/

extern lv_font_t *SemiBold_90;

extern lv_font_t *SemiBold_70;

extern lv_font_t *SemiBold_50;

extern lv_font_t *NotoSeri_50;

extern lv_font_t *NotoSeri_30;

extern lv_font_t *SemiBold_30;

/*----------------
 * Images
 *----------------*/

extern const void *bk;
extern const void *needle;
extern const void *oil_normal;
extern const void *oil_low;
extern const void *anquandai;
extern const void *fadongji;
extern const void *shousha;
extern const void *jinguangdeng;
extern const void *yuanguangdeng;
extern const void *wudeng;
extern const void *youliang;
extern const void *youzhuan;
extern const void *zuozhuan;
extern const void *shuiwen_high;
extern const void *shuiwen_low;

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t subject_speed_value;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void watch_init_gen(const char *asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widget and components of this library*/
#include "screens/home_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*WATCH_GEN_H*/
