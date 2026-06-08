/**
 * @file home_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "home_gen.h"
#include "watch.h"
#include "board.h"
#include "hpm_lcdc_drv.h"
#include "hpm_l1c_drv.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define LAYER2_WIDTH  352
#define LAYER2_HEIGHT 350
#define LAYER2_X      325
#define LAYER2_Y      210

#define LAYER3_WIDTH  352
#define LAYER3_HEIGHT 350
#define LAYER3_X      1245
#define LAYER3_Y      210

#define LAYER4_WIDTH  64
#define LAYER4_HEIGHT 512
#define LAYER4_X      40
#define LAYER4_Y      124

#define LAYER5_WIDTH  64
#define LAYER5_HEIGHT 512
#define LAYER5_X      1824
#define LAYER5_Y      124

#define LAYER6_WIDTH  1408
#define LAYER6_HEIGHT 100
#define LAYER6_X      242
#define LAYER6_Y      620

#define LAYER7_WIDTH  640
#define LAYER7_HEIGHT 64
#define LAYER7_X      630
#define LAYER7_Y      17

#define LAYER8_WIDTH  256
#define LAYER8_HEIGHT 345
#define LAYER8_X      770
#define LAYER8_Y      140

/***********************
 *  STATIC PROTOTYPES
 **********************/

static void layer2_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void init_layer2(void);
static void layer3_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void init_layer3(void);
static void layer4_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void init_layer4(void);
static void layer5_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void init_layer5(void);
static void layer6_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void init_layer6(void);
static void layer7_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void init_layer7(void);
static void layer8_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void init_layer8(void);

/***********************
 *  STATIC FUNCTIONS
 **********************/

static void layer2_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)px_map, LAYER2_WIDTH * LAYER2_HEIGHT * 4);
    }
    lcdc_layer_set_next_buffer(HPM_LCDC, 1, (uint32_t)px_map);
    lv_display_flush_ready(disp);
}

static void init_layer2(void)
{
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer2_buf0[LAYER2_WIDTH * LAYER2_HEIGHT * 4];
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer2_buf1[LAYER2_WIDTH * LAYER2_HEIGHT * 4];
    lcdc_layer_config_t layer;
    lcdc_get_default_layer_config(HPM_LCDC, &layer, display_pixel_format_argb8888, 1);
    layer.position_x = LAYER2_X;
    layer.position_y = LAYER2_Y;
    layer.width = LAYER2_WIDTH;
    layer.height = LAYER2_HEIGHT;
    layer.buffer = (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)layer2_buf0);
    layer.alphablend.src_alpha = 0xFF;
    layer.alphablend.dst_alpha = 0xFF;
    layer.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer.alphablend.mode = display_alphablend_mode_src_over;
    memset(layer2_buf0, 0, sizeof(layer2_buf0));
    memset(layer2_buf1, 0, sizeof(layer2_buf1));
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)layer2_buf0, sizeof(layer2_buf0));
        l1c_dc_writeback((uint32_t)layer2_buf1, sizeof(layer2_buf1));
    }
    lcdc_config_layer(HPM_LCDC, 1, &layer, true);
    lv_display_t *disp = lv_display_create(LAYER2_WIDTH, LAYER2_HEIGHT);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_buffers(disp, layer2_buf0, layer2_buf1, sizeof(layer2_buf0), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, layer2_flush_cb);
    home_layer2 = lv_display_get_screen_active(disp);
    lv_obj_set_style_bg_opa(home_layer2, LV_OPA_TRANSP, 0);
    lv_sysmon_hide_memory(disp);
    lv_sysmon_hide_performance(disp);
}

static void layer3_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)px_map, LAYER3_WIDTH * LAYER3_HEIGHT * 4);
    }
    lcdc_layer_set_next_buffer(HPM_LCDC, 2, (uint32_t)px_map);
    lv_display_flush_ready(disp);
}

static void init_layer3(void)
{
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer3_buf0[LAYER3_WIDTH * LAYER3_HEIGHT * 4];
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer3_buf1[LAYER3_WIDTH * LAYER3_HEIGHT * 4];
    lcdc_layer_config_t layer;
    lcdc_get_default_layer_config(HPM_LCDC, &layer, display_pixel_format_argb8888, 2);
    layer.position_x = LAYER3_X;
    layer.position_y = LAYER3_Y;
    layer.width = LAYER3_WIDTH;
    layer.height = LAYER3_HEIGHT;
    layer.buffer = (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)layer3_buf0);
    layer.alphablend.src_alpha = 0xFF;
    layer.alphablend.dst_alpha = 0xFF;
    layer.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer.alphablend.mode = display_alphablend_mode_src_over;
    memset(layer3_buf0, 0, sizeof(layer3_buf0));
    memset(layer3_buf1, 0, sizeof(layer3_buf1));
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)layer3_buf0, sizeof(layer3_buf0));
        l1c_dc_writeback((uint32_t)layer3_buf1, sizeof(layer3_buf1));
    }
    lcdc_config_layer(HPM_LCDC, 2, &layer, true);
    lv_display_t *disp = lv_display_create(LAYER3_WIDTH, LAYER3_HEIGHT);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_buffers(disp, layer3_buf0, layer3_buf1, sizeof(layer3_buf0), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, layer3_flush_cb);
    home_layer3 = lv_display_get_screen_active(disp);
    lv_obj_set_style_bg_opa(home_layer3, LV_OPA_TRANSP, 0);
    lv_sysmon_hide_memory(disp);
    lv_sysmon_hide_performance(disp);
}
static void layer4_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)px_map, LAYER4_WIDTH * LAYER4_HEIGHT * 4);
    }
    lcdc_layer_set_next_buffer(HPM_LCDC, 3, (uint32_t)px_map);
    lv_display_flush_ready(disp);
}
static void init_layer4(void)
{
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer4_buf0[LAYER4_WIDTH * LAYER4_HEIGHT * 4];
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer4_buf1[LAYER4_WIDTH * LAYER4_HEIGHT * 4];
    lcdc_layer_config_t layer;
    lcdc_get_default_layer_config(HPM_LCDC, &layer, display_pixel_format_argb8888, 3);
    layer.position_x = LAYER4_X;
    layer.position_y = LAYER4_Y;
    layer.width = LAYER4_WIDTH;
    layer.height = LAYER4_HEIGHT;
    layer.buffer = (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)layer4_buf0);
    layer.alphablend.src_alpha = 0xFF;
    layer.alphablend.dst_alpha = 0xFF;
    layer.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer.alphablend.mode = display_alphablend_mode_src_over;
    memset(layer4_buf0, 0, sizeof(layer4_buf0));
    memset(layer4_buf1, 0, sizeof(layer4_buf1));
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)layer4_buf0, sizeof(layer4_buf0));
        l1c_dc_writeback((uint32_t)layer4_buf1, sizeof(layer4_buf1));
    }
    lcdc_config_layer(HPM_LCDC, 3, &layer, true);
    lv_display_t *disp = lv_display_create(LAYER4_WIDTH, LAYER4_HEIGHT);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_buffers(disp, layer4_buf0, layer4_buf1, sizeof(layer4_buf0), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, layer4_flush_cb);
    home_layer4 = lv_display_get_screen_active(disp);
    lv_obj_set_style_bg_opa(home_layer4, LV_OPA_TRANSP, 0);
    lv_sysmon_hide_memory(disp);
    lv_sysmon_hide_performance(disp);
}
static void layer5_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)px_map, LAYER5_WIDTH * LAYER5_HEIGHT * 4);
    }
    lcdc_layer_set_next_buffer(HPM_LCDC, 4, (uint32_t)px_map);
    lv_display_flush_ready(disp);
}
static void init_layer5(void)
{
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer5_buf0[LAYER5_WIDTH * LAYER5_HEIGHT * 4];
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer5_buf1[LAYER5_WIDTH * LAYER5_HEIGHT * 4];
    lcdc_layer_config_t layer;
    lcdc_get_default_layer_config(HPM_LCDC, &layer, display_pixel_format_argb8888, 4);
    layer.position_x = LAYER5_X;
    layer.position_y = LAYER5_Y;
    layer.width = LAYER5_WIDTH;
    layer.height = LAYER5_HEIGHT;
    layer.buffer = (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)layer5_buf0);
    layer.alphablend.src_alpha = 0xFF;
    layer.alphablend.dst_alpha = 0xFF;
    layer.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer.alphablend.mode = display_alphablend_mode_src_over;
    memset(layer5_buf0, 0, sizeof(layer5_buf0));
    memset(layer5_buf1, 0, sizeof(layer5_buf1));
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)layer5_buf0, sizeof(layer5_buf0));
        l1c_dc_writeback((uint32_t)layer5_buf1, sizeof(layer5_buf1));
    }
    lcdc_config_layer(HPM_LCDC, 4, &layer, true);
    lv_display_t *disp = lv_display_create(LAYER5_WIDTH, LAYER5_HEIGHT);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_buffers(disp, layer5_buf0, layer5_buf1, sizeof(layer5_buf0), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, layer5_flush_cb);
    home_layer5 = lv_display_get_screen_active(disp);
    lv_obj_set_style_bg_opa(home_layer5, LV_OPA_TRANSP, 0);
    lv_sysmon_hide_memory(disp);
    lv_sysmon_hide_performance(disp);
}
static void layer6_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)px_map, LAYER6_WIDTH * LAYER6_HEIGHT * 4);
    }
    lcdc_layer_set_next_buffer(HPM_LCDC, 5, (uint32_t)px_map);
    lv_display_flush_ready(disp);
}
static void init_layer6(void)
{
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer6_buf0[LAYER6_WIDTH * LAYER6_HEIGHT * 4];
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer6_buf1[LAYER6_WIDTH * LAYER6_HEIGHT * 4];
    lcdc_layer_config_t layer;
    lcdc_get_default_layer_config(HPM_LCDC, &layer, display_pixel_format_argb8888, 5);
    layer.position_x = LAYER6_X;
    layer.position_y = LAYER6_Y;
    layer.width = LAYER6_WIDTH;
    layer.height = LAYER6_HEIGHT;
    layer.buffer = (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)layer6_buf0);
    layer.alphablend.src_alpha = 0xFF;
    layer.alphablend.dst_alpha = 0xFF;
    layer.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer.alphablend.mode = display_alphablend_mode_src_over;
    memset(layer6_buf0, 0, sizeof(layer6_buf0));
    memset(layer6_buf1, 0, sizeof(layer6_buf1));
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)layer6_buf0, sizeof(layer6_buf0));
        l1c_dc_writeback((uint32_t)layer6_buf1, sizeof(layer6_buf1));
    }
    lcdc_config_layer(HPM_LCDC, 5, &layer, true);
    lv_display_t *disp = lv_display_create(LAYER6_WIDTH, LAYER6_HEIGHT);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_buffers(disp, layer6_buf0, layer6_buf1, sizeof(layer6_buf0), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, layer6_flush_cb);
    home_layer6 = lv_display_get_screen_active(disp);
    lv_obj_set_style_bg_opa(home_layer6, LV_OPA_TRANSP, 0);
    lv_sysmon_hide_memory(disp);
    lv_sysmon_hide_performance(disp);
}
static void layer7_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)px_map, LAYER7_WIDTH * LAYER7_HEIGHT * 4);
    }
    lcdc_layer_set_next_buffer(HPM_LCDC, 6, (uint32_t)px_map);
    lv_display_flush_ready(disp);
}
static void init_layer7(void)
{
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer7_buf0[LAYER7_WIDTH * LAYER7_HEIGHT * 4];
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer7_buf1[LAYER7_WIDTH * LAYER7_HEIGHT * 4];
    lcdc_layer_config_t layer;
    lcdc_get_default_layer_config(HPM_LCDC, &layer, display_pixel_format_argb8888, 6);
    layer.position_x = LAYER7_X;
    layer.position_y = LAYER7_Y;
    layer.width = LAYER7_WIDTH;
    layer.height = LAYER7_HEIGHT;
    layer.buffer = (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)layer7_buf0);
    layer.alphablend.src_alpha = 0xFF;
    layer.alphablend.dst_alpha = 0xFF;
    layer.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer.alphablend.mode = display_alphablend_mode_src_over;
    memset(layer7_buf0, 0, sizeof(layer7_buf0));
    memset(layer7_buf1, 0, sizeof(layer7_buf1));
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)layer7_buf0, sizeof(layer7_buf0));
        l1c_dc_writeback((uint32_t)layer7_buf1, sizeof(layer7_buf1));
    }
    lcdc_config_layer(HPM_LCDC, 6, &layer, true);
    lv_display_t *disp = lv_display_create(LAYER7_WIDTH, LAYER7_HEIGHT);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_buffers(disp, layer7_buf0, layer7_buf1, sizeof(layer7_buf0), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, layer7_flush_cb);
    home_layer7 = lv_display_get_screen_active(disp);
    lv_obj_set_style_bg_opa(home_layer7, LV_OPA_TRANSP, 0);
    lv_sysmon_hide_memory(disp);
    lv_sysmon_hide_performance(disp);
}
static void layer8_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)px_map, LAYER8_WIDTH * LAYER8_HEIGHT * 4);
    }
    lcdc_layer_set_next_buffer(HPM_LCDC, 7, (uint32_t)px_map);
    lv_display_flush_ready(disp);
}
static void init_layer8(void)
{
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer8_buf0[LAYER8_WIDTH * LAYER8_HEIGHT * 4];
    static uint8_t __attribute__((section(".framebuffer"), aligned(HPM_L1C_CACHELINE_SIZE))) layer8_buf1[LAYER8_WIDTH * LAYER8_HEIGHT * 4];
    lcdc_layer_config_t layer;
    lcdc_get_default_layer_config(HPM_LCDC, &layer, display_pixel_format_argb8888, 7);
    layer.position_x = LAYER8_X;
    layer.position_y = LAYER8_Y;
    layer.width = LAYER8_WIDTH;
    layer.height = LAYER8_HEIGHT;
    layer.buffer = (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)layer8_buf0);
    layer.alphablend.src_alpha = 0xFF;
    layer.alphablend.dst_alpha = 0xFF;
    layer.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer.alphablend.mode = display_alphablend_mode_src_over;
    memset(layer8_buf0, 0, sizeof(layer8_buf0));
    memset(layer8_buf1, 0, sizeof(layer8_buf1));
    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)layer8_buf0, sizeof(layer8_buf0));
        l1c_dc_writeback((uint32_t)layer8_buf1, sizeof(layer8_buf1));
    }
    lcdc_config_layer(HPM_LCDC, 7, &layer, true);
    lv_display_t *disp = lv_display_create(LAYER8_WIDTH, LAYER8_HEIGHT);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    lv_display_set_buffers(disp, layer8_buf0, layer8_buf1, sizeof(layer8_buf0), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, layer8_flush_cb);
    home_layer8 = lv_display_get_screen_active(disp);
    lv_obj_set_style_bg_opa(home_layer8, LV_OPA_TRANSP, 0);
    lv_sysmon_hide_memory(disp);
    lv_sysmon_hide_performance(disp);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *home_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t roller_selected;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&roller_selected);
        lv_style_set_text_font(&roller_selected, SemiBold_50);
        lv_style_set_text_color(&roller_selected, lv_color_hex(0xffffff));
        lv_style_set_bg_color(&roller_selected, lv_color_hex(0x157c43));
        lv_style_set_bg_opa(&roller_selected, 0);

        style_inited = true;
    }

    if (home == NULL)
        home = lv_obj_create(NULL);
    lv_obj_t *lv_obj_0 = home;
    lv_obj_set_name_static(lv_obj_0, "home_#");
    lv_obj_set_style_bg_image_src(lv_obj_0, bk, 0);

    if (home_layer2 == NULL)
        init_layer2();
    if (home_layer3 == NULL)
        init_layer3();
    if (home_layer4 == NULL)
        init_layer4();
    if (home_layer5 == NULL)
        init_layer5();
    if (home_layer6 == NULL)
        init_layer6();
    if (home_layer7 == NULL)
        init_layer7();
    if (home_layer8 == NULL)
        init_layer8();

    lv_obj_t *lv_image_10 = lv_image_create(lv_obj_0);
    lv_obj_set_x(lv_image_10, 890);
    lv_obj_set_y(lv_image_10, 500);
    lv_image_set_src(lv_image_10, youliang);

    lv_obj_t *lv_image_1 = lv_image_create(home_layer2);
    lv_image_set_src(lv_image_1, needle);
    lv_obj_set_x(lv_image_1, 160);
    lv_obj_set_y(lv_image_1, 3);
    lv_obj_set_name_static(lv_image_1, "speed_needle");

    lv_obj_t *lv_image_2 = lv_image_create(home_layer3);
    lv_image_set_src(lv_image_2, needle);
    lv_obj_set_x(lv_image_2, 160);
    lv_obj_set_y(lv_image_2, 3);
    lv_obj_set_name_static(lv_image_2, "rotational_needle");

    lv_obj_t *lv_label_0 = lv_label_create(home_layer8);
    lv_obj_set_x(lv_label_0, 110);
    lv_obj_set_y(lv_label_0, 5);
    lv_obj_set_width(lv_label_0, 144);
    lv_label_bind_text(lv_label_0, &subject_speed_value, NULL);
    lv_obj_set_style_text_color(lv_label_0, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(lv_label_0, SemiBold_70, 0);
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *mode_inner_roller = lv_roller_create(home_layer8);
    lv_obj_set_x(mode_inner_roller, 10);
    lv_obj_set_y(mode_inner_roller, 110);
    lv_obj_set_name(mode_inner_roller, "mode_inner_roller");
    lv_obj_set_width(mode_inner_roller, 100);
    lv_obj_set_height(mode_inner_roller, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(mode_inner_roller, 0, 0);
    lv_roller_set_options(mode_inner_roller, "P\nR\nN\nD\nS", LV_ROLLER_MODE_INFINITE);
    lv_obj_set_style_text_font(mode_inner_roller, SemiBold_50, 0);
    lv_obj_set_style_text_color(mode_inner_roller, lv_color_hex(0x424242), 0);
    lv_obj_set_style_bg_opa(mode_inner_roller, 0, 0);
    lv_obj_set_style_border_width(mode_inner_roller, 0, 0);
    lv_obj_set_style_text_line_space(mode_inner_roller, 30, 0);
    lv_roller_set_visible_row_count(mode_inner_roller, 3);
    lv_obj_add_style(mode_inner_roller, &roller_selected, LV_PART_SELECTED);

    lv_obj_t *lv_label_1 = lv_label_create(home_layer8);
    lv_obj_set_x(lv_label_1, 140);
    lv_obj_set_y(lv_label_1, 190);
    lv_obj_set_name(lv_label_1, "gear_value_label");
    lv_obj_set_width(lv_label_1, 100);
    lv_label_set_text(lv_label_1, "P");
    lv_obj_set_style_text_color(lv_label_1, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(lv_label_1, SemiBold_90, 0);
    lv_obj_set_style_radius(lv_label_1, 10, 0);
    lv_obj_set_style_border_width(lv_label_1, 3, 0);
    lv_obj_set_style_bg_color(lv_label_1, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_color(lv_label_1, lv_color_hex3(0xfff), 0);
    lv_obj_set_style_bg_grad_dir(lv_label_1, LV_GRAD_DIR_HOR, 0);
    lv_obj_set_style_bg_grad_color(lv_label_1, lv_color_hex(0x3255ef), 0);
    lv_obj_set_style_bg_opa(lv_label_1, 100, 0);
    lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *lv_image_3 = lv_image_create(home_layer6);
    lv_obj_set_x(lv_image_3, 420);
    lv_obj_set_y(lv_image_3, 10);
    lv_image_set_src(lv_image_3, anquandai);
    lv_obj_set_name(lv_image_3, "anquandai_left");

    lv_obj_t *lv_image_4 = lv_image_create(home_layer6);
    lv_obj_set_x(lv_image_4, 950);
    lv_obj_set_y(lv_image_4, 10);
    lv_image_set_src(lv_image_4, anquandai);
    lv_obj_set_name(lv_image_4, "anquandai_right");

    lv_obj_t *lv_image_5 = lv_image_create(home_layer6);
    lv_obj_set_x(lv_image_5, 1310);
    lv_obj_set_y(lv_image_5, 10);
    lv_image_set_src(lv_image_5, shousha);
    lv_obj_set_name(lv_image_5, "shousha");

    lv_obj_t *lv_image_6 = lv_image_create(home_layer6);
    lv_obj_set_x(lv_image_6, 180);
    lv_obj_set_y(lv_image_6, 15);
    lv_image_set_src(lv_image_6, jinguangdeng);
    lv_obj_set_name(lv_image_6, "jinguangdeng");

    lv_obj_t *lv_image_7 = lv_image_create(home_layer6);
    lv_obj_set_x(lv_image_7, 100);
    lv_obj_set_y(lv_image_7, 15);
    lv_image_set_src(lv_image_7, yuanguangdeng);
    lv_obj_set_name(lv_image_7, "yuanguangdeng");

    lv_obj_t *lv_image_8 = lv_image_create(home_layer6);
    lv_obj_set_x(lv_image_8, 10);
    lv_obj_set_y(lv_image_8, 10);
    lv_image_set_src(lv_image_8, wudeng);
    lv_obj_set_name(lv_image_8, "wudeng");

    lv_obj_t *lv_image_9 = lv_image_create(lv_obj_0);
    lv_obj_set_x(lv_image_9, 1778);
    lv_obj_set_y(lv_image_9, 617);
    lv_image_set_src(lv_image_9, oil_normal);

    lv_obj_t *lv_image_12 = lv_image_create(lv_obj_0);
    lv_obj_set_x(lv_image_12, 110);
    lv_obj_set_y(lv_image_12, 617);
    lv_image_set_src(lv_image_12, shuiwen_low);

    lv_obj_t *lv_label_2 = lv_label_create(lv_obj_0);
    lv_obj_set_x(lv_label_2, 950);
    lv_obj_set_y(lv_label_2, 510);
    lv_label_set_text(lv_label_2, "360km");
    lv_obj_set_style_text_color(lv_label_2, lv_color_hex3(0xfff), 0);
    lv_obj_set_style_text_font(lv_label_2, SemiBold_30, 0);

    lv_obj_t *lv_label_3 = lv_label_create(lv_obj_0);
    lv_obj_set_x(lv_label_3, 860);
    lv_obj_set_y(lv_label_3, 640);
    lv_obj_set_width(lv_label_3, 200);
    lv_obj_set_style_text_align(lv_label_3, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_3, "2026 . 3 . 11");
    lv_obj_set_style_text_color(lv_label_3, lv_color_hex3(0xfff), 0);
    lv_obj_set_style_text_font(lv_label_3, NotoSeri_30, 0);

    lv_obj_t *lv_label_4 = lv_label_create(lv_obj_0);
    lv_obj_set_x(lv_label_4, 885);
    lv_obj_set_y(lv_label_4, 580);
    lv_obj_set_width(lv_label_4, 150);
    lv_label_set_text(lv_label_4, "19:23");
    lv_obj_set_style_text_color(lv_label_4, lv_color_hex3(0xfff), 0);
    lv_obj_set_style_text_font(lv_label_4, NotoSeri_50, 0);
    lv_obj_set_style_text_align(lv_label_4, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *lv_label_5 = lv_label_create(lv_obj_0);
    lv_obj_set_x(lv_label_5, 1600);
    lv_obj_set_y(lv_label_5, 30);
    lv_label_set_text(lv_label_5, "18.0°C");
    lv_obj_set_style_text_color(lv_label_5, lv_color_hex3(0xfff), 0);
    lv_obj_set_style_text_font(lv_label_5, NotoSeri_50, 0);

    lv_obj_t *lv_image_14 = lv_image_create(home_layer7);
    lv_obj_set_x(lv_image_14, 540);
    lv_obj_set_y(lv_image_14, 3);
    lv_image_set_src(lv_image_14, youzhuan);
    lv_obj_set_name(lv_image_14, "youzhuan");

    lv_obj_t *lv_image_15 = lv_image_create(home_layer7);
    lv_obj_set_x(lv_image_15, 10);
    lv_obj_set_y(lv_image_15, 3);
    lv_image_set_src(lv_image_15, zuozhuan);
    lv_obj_set_name(lv_image_15, "zuozhuan");

    lv_obj_t *left_bar_clip = lv_obj_create(home_layer4);
    lv_obj_set_name(left_bar_clip, "left_bar_clip");
    lv_obj_set_size(left_bar_clip, 60, 512);
    lv_obj_set_style_pad_all(left_bar_clip, 0, 0);
    lv_obj_set_style_border_width(left_bar_clip, 0, 0);
    lv_obj_set_style_bg_opa(left_bar_clip, 0, 0);
    lv_obj_set_style_radius(left_bar_clip, 6, 0);
    lv_obj_clear_flag(left_bar_clip, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_clip_corner(left_bar_clip, true, 0);

    lv_obj_t *left_bar = lv_obj_create(left_bar_clip);
    lv_obj_set_name(left_bar, "left_bar");
    lv_obj_set_pos(left_bar, 0, 0);
    lv_obj_set_size(left_bar, 60, 512);
    lv_obj_set_style_pad_all(left_bar, 0, 0);
    lv_obj_set_style_border_width(left_bar, 0, 0);
    lv_obj_set_style_radius(left_bar, 6, 0);
    lv_obj_set_style_bg_opa(left_bar, 150, 0);
    lv_obj_set_style_bg_color(left_bar, lv_color_hex(0x0062e7), 0);

    lv_obj_t *right_bar_clip = lv_obj_create(home_layer5);
    lv_obj_set_name(right_bar_clip, "right_bar_clip");
    lv_obj_set_size(right_bar_clip, 60, 512);
    lv_obj_set_style_pad_all(right_bar_clip, 0, 0);
    lv_obj_set_style_border_width(right_bar_clip, 0, 0);
    lv_obj_set_style_bg_opa(right_bar_clip, 0, 0);
    lv_obj_set_style_radius(right_bar_clip, 6, 0);
    lv_obj_clear_flag(right_bar_clip, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_clip_corner(right_bar_clip, true, 0);

    lv_obj_t *right_bar = lv_obj_create(right_bar_clip);
    lv_obj_set_name(right_bar, "right_bar");
    lv_obj_set_pos(right_bar, 0, -460);
    lv_obj_set_size(right_bar, 60, 512);
    lv_obj_set_style_pad_all(right_bar, 0, 0);
    lv_obj_set_style_border_width(right_bar, 0, 0);
    lv_obj_set_style_radius(right_bar, 6, 0);
    lv_obj_set_style_bg_opa(right_bar, 150, 0);
    lv_obj_set_style_bg_color(right_bar, lv_color_hex(0x0062e7), 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}
