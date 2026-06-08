/**
 * @file in_output_gen.h
 */

#ifndef IN_OUTPUT_H
#define IN_OUTPUT_H

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

typedef enum {
    IN_OUTPUT_TIMELINE_INPUT_SHOW = 0,
    IN_OUTPUT_TIMELINE_OUTPUT_SHOW = 1,
    _IN_OUTPUT_TIMELINE_CNT = 2
}in_output_timeline_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * in_output_create(lv_obj_t * parent);

/**
 * Get a timeline of a in_output
 * @param obj          pointer to a in_output component
 * @param timeline_id  ID of the the timeline
 * @return             pointer to the timeline or NULL if not found
 */
lv_anim_timeline_t * in_output_get_timeline(lv_obj_t * obj, in_output_timeline_t timeline_id);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*IN_OUTPUT_H*/