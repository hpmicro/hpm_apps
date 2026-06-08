/**
 * @file ui_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "ui_gen.h"

#if LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/*----------------
 * Translations
 *----------------*/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

lv_obj_t * home = NULL;

/*----------------
 * Fonts
 *----------------*/

lv_font_t * SemiBold_Font_16;
extern lv_font_t SemiBold_Font_16_data;
lv_font_t * SemiBold_Font_20;
extern lv_font_t SemiBold_Font_20_data;
lv_font_t * SemiBold_Font_24;
extern lv_font_t SemiBold_Font_24_data;
lv_font_t * SemiBold_Font_28;
extern lv_font_t SemiBold_Font_28_data;
lv_font_t * SemiBold_Font_32;
extern lv_font_t SemiBold_Font_32_data;
lv_font_t * SemiBold_Font_36;
extern lv_font_t SemiBold_Font_36_data;
lv_font_t * SemiBold_Font_44;
extern lv_font_t SemiBold_Font_44_data;
lv_font_t * NotoSerifCJKsc_SemiBold_28;
extern lv_font_t NotoSerifCJKsc_SemiBold_28_data;

/*----------------
 * Images
 *----------------*/

const void * image_start;
extern const void * image_start_data;
const void * image_HPMicro_Logo;
extern const void * image_HPMicro_Logo_data;
const void * image_device;
extern const void * image_device_data;
const void * image_state;
extern const void * image_state_data;
const void * image_address;
extern const void * image_address_data;
const void * image_input;
extern const void * image_input_data;
const void * image_output;
extern const void * image_output_data;
const void * image_monitor;
extern const void * image_monitor_data;
const void * image_speed;
extern const void * image_speed_data;
const void * image_gear;
extern const void * image_gear_data;
const void * image_rotate_right;
extern const void * image_rotate_right_data;
const void * image_rotate_left;
extern const void * image_rotate_left_data;

/*----------------
 * Global styles
 *----------------*/

lv_style_t style_button_normal;
lv_style_t style_button_pressed;
lv_style_t style_input_normal;
lv_style_t style_input_change;
lv_style_t style_label_normal;

/*----------------
 * Subjects
 *----------------*/

lv_subject_t speed_slider_value_subject;
lv_subject_t current_position_subject;
lv_subject_t subject_input_1;
lv_subject_t subject_input_2;
lv_subject_t subject_input_3;
lv_subject_t subject_input_4;
lv_subject_t subject_input_5;
lv_subject_t subject_input_6;
lv_subject_t subject_input_7;
lv_subject_t subject_input_8;
lv_subject_t subject_input_9;
lv_subject_t subject_input_10;
lv_subject_t subject_input_11;
lv_subject_t subject_input_12;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_init_gen(const char * asset_path)
{
    char buf[256];

    /*----------------
     * Fonts
     *----------------*/

    /* get font 'SemiBold_Font_16' from a C array */
    SemiBold_Font_16 = &SemiBold_Font_16_data;
    /* get font 'SemiBold_Font_20' from a C array */
    SemiBold_Font_20 = &SemiBold_Font_20_data;
    /* get font 'SemiBold_Font_24' from a C array */
    SemiBold_Font_24 = &SemiBold_Font_24_data;
    /* get font 'SemiBold_Font_28' from a C array */
    SemiBold_Font_28 = &SemiBold_Font_28_data;
    /* get font 'SemiBold_Font_32' from a C array */
    SemiBold_Font_32 = &SemiBold_Font_32_data;
    /* get font 'SemiBold_Font_36' from a C array */
    SemiBold_Font_36 = &SemiBold_Font_36_data;
    /* get font 'SemiBold_Font_44' from a C array */
    SemiBold_Font_44 = &SemiBold_Font_44_data;
    /* get font 'NotoSerifCJKsc_SemiBold_28' from a C array */
    NotoSerifCJKsc_SemiBold_28 = &NotoSerifCJKsc_SemiBold_28_data;


    /*----------------
     * Images
     *----------------*/
    image_start = &image_start_data;
    image_HPMicro_Logo = &image_HPMicro_Logo_data;
    image_device = &image_device_data;
    image_state = &image_state_data;
    image_address = &image_address_data;
    image_input = &image_input_data;
    image_output = &image_output_data;
    image_monitor = &image_monitor_data;
    image_speed = &image_speed_data;
    image_gear = &image_gear_data;
    image_rotate_right = &image_rotate_right_data;
    image_rotate_left = &image_rotate_left_data;

    /*----------------
     * Global styles
     *----------------*/

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&style_button_normal);
        lv_style_set_bg_color(&style_button_normal, lv_color_hex(0x1f96e9));

        lv_style_init(&style_button_pressed);
        lv_style_set_bg_color(&style_button_pressed, lv_color_hex(0xff0000));

        lv_style_init(&style_input_normal);
        lv_style_set_width(&style_input_normal, 85);
        lv_style_set_height(&style_input_normal, 80);
        lv_style_set_pad_all(&style_input_normal, 0);
        lv_style_set_bg_color(&style_input_normal, lv_color_hex(0x163b58));
        lv_style_set_border_color(&style_input_normal, lv_color_hex(0x2b6384));
        lv_style_set_border_width(&style_input_normal, 3);
        lv_style_set_shadow_color(&style_input_normal, lv_color_hex(0x02a4ff));
        lv_style_set_shadow_spread(&style_input_normal, 1);
        lv_style_set_shadow_width(&style_input_normal, 5);
        lv_style_set_radius(&style_input_normal, 35);

        lv_style_init(&style_input_change);
        lv_style_set_width(&style_input_change, 85);
        lv_style_set_height(&style_input_change, 80);
        lv_style_set_pad_all(&style_input_change, 0);
        lv_style_set_bg_color(&style_input_change, lv_color_hex(0x42b477));
        lv_style_set_border_color(&style_input_change, lv_color_hex(0x13e78f));
        lv_style_set_border_width(&style_input_change, 3);
        lv_style_set_shadow_color(&style_input_change, lv_color_hex(0x3dbc63));
        lv_style_set_shadow_spread(&style_input_change, 3);
        lv_style_set_shadow_width(&style_input_change, 12);
        lv_style_set_radius(&style_input_change, 35);

        lv_style_init(&style_label_normal);
        lv_style_set_text_color(&style_label_normal, lv_color_hex(0x5290e2));

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_int(&speed_slider_value_subject, 0);
    lv_subject_init_int(&current_position_subject, 0);
    lv_subject_init_int(&subject_input_1, 0);
    lv_subject_init_int(&subject_input_2, 0);
    lv_subject_init_int(&subject_input_3, 0);
    lv_subject_init_int(&subject_input_4, 0);
    lv_subject_init_int(&subject_input_5, 0);
    lv_subject_init_int(&subject_input_6, 0);
    lv_subject_init_int(&subject_input_7, 0);
    lv_subject_init_int(&subject_input_8, 0);
    lv_subject_init_int(&subject_input_9, 0);
    lv_subject_init_int(&subject_input_10, 0);
    lv_subject_init_int(&subject_input_11, 0);
    lv_subject_init_int(&subject_input_12, 0);

    /*----------------
     * Translations
     *----------------*/

#if LV_USE_XML
    /* Register widgets */

    /* Register fonts */
    lv_xml_register_font(NULL, "SemiBold_Font_16", SemiBold_Font_16);
    lv_xml_register_font(NULL, "SemiBold_Font_20", SemiBold_Font_20);
    lv_xml_register_font(NULL, "SemiBold_Font_24", SemiBold_Font_24);
    lv_xml_register_font(NULL, "SemiBold_Font_28", SemiBold_Font_28);
    lv_xml_register_font(NULL, "SemiBold_Font_32", SemiBold_Font_32);
    lv_xml_register_font(NULL, "SemiBold_Font_36", SemiBold_Font_36);
    lv_xml_register_font(NULL, "SemiBold_Font_44", SemiBold_Font_44);
    lv_xml_register_font(NULL, "NotoSerifCJKsc_SemiBold_28", NotoSerifCJKsc_SemiBold_28);

    /* Register subjects */
    lv_xml_register_subject(NULL, "speed_slider_value_subject", &speed_slider_value_subject);
    lv_xml_register_subject(NULL, "current_position_subject", &current_position_subject);
    lv_xml_register_subject(NULL, "subject_input_1", &subject_input_1);
    lv_xml_register_subject(NULL, "subject_input_2", &subject_input_2);
    lv_xml_register_subject(NULL, "subject_input_3", &subject_input_3);
    lv_xml_register_subject(NULL, "subject_input_4", &subject_input_4);
    lv_xml_register_subject(NULL, "subject_input_5", &subject_input_5);
    lv_xml_register_subject(NULL, "subject_input_6", &subject_input_6);
    lv_xml_register_subject(NULL, "subject_input_7", &subject_input_7);
    lv_xml_register_subject(NULL, "subject_input_8", &subject_input_8);
    lv_xml_register_subject(NULL, "subject_input_9", &subject_input_9);
    lv_xml_register_subject(NULL, "subject_input_10", &subject_input_10);
    lv_xml_register_subject(NULL, "subject_input_11", &subject_input_11);
    lv_xml_register_subject(NULL, "subject_input_12", &subject_input_12);

    /* Register callbacks */
    lv_xml_register_event_cb(NULL, "start_button_cb", start_button_cb);
    lv_xml_register_event_cb(NULL, "direction_button_cb", direction_button_cb);
    lv_xml_register_event_cb(NULL, "mode_change_cb", mode_change_cb);
    lv_xml_register_event_cb(NULL, "output_select_button_cb", output_select_button_cb);
    lv_xml_register_event_cb(NULL, "input_select_button_cb", input_select_button_cb);
    lv_xml_register_event_cb(NULL, "output_button_cb", output_button_cb);
    lv_xml_register_event_cb(NULL, "slave_dropdown_change_cb", slave_dropdown_change_cb);
    lv_xml_register_event_cb(NULL, "speed_slider_change_cb", speed_slider_change_cb);
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "image_start", image_start);
    lv_xml_register_image(NULL, "image_HPMicro_Logo", image_HPMicro_Logo);
    lv_xml_register_image(NULL, "image_device", image_device);
    lv_xml_register_image(NULL, "image_state", image_state);
    lv_xml_register_image(NULL, "image_address", image_address);
    lv_xml_register_image(NULL, "image_input", image_input);
    lv_xml_register_image(NULL, "image_output", image_output);
    lv_xml_register_image(NULL, "image_monitor", image_monitor);
    lv_xml_register_image(NULL, "image_speed", image_speed);
    lv_xml_register_image(NULL, "image_gear", image_gear);
    lv_xml_register_image(NULL, "image_rotate_right", image_rotate_right);
    lv_xml_register_image(NULL, "image_rotate_left", image_rotate_left);
#endif

#if LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manaully from XML using lv_xml_create() */
    /* To allow screens to reference each other, create them all before calling the sceen create functions */
    home = lv_obj_create(NULL);

    home_create();
#endif
}

/* Callbacks */
#if defined(LV_EDITOR_PREVIEW)
void __attribute__((weak)) start_button_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("start_button_cb was called\n");
}
void __attribute__((weak)) direction_button_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("direction_button_cb was called\n");
}
void __attribute__((weak)) mode_change_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("mode_change_cb was called\n");
}
void __attribute__((weak)) output_select_button_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("output_select_button_cb was called\n");
}
void __attribute__((weak)) input_select_button_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("input_select_button_cb was called\n");
}
void __attribute__((weak)) output_button_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("output_button_cb was called\n");
}
void __attribute__((weak)) slave_dropdown_change_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("slave_dropdown_change_cb was called\n");
}
void __attribute__((weak)) speed_slider_change_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_LOG("speed_slider_change_cb was called\n");
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/