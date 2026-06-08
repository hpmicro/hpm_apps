#include "user_lvgl_port.h"
#include "lv_app_conf.h"
#include "src/draw/lv_draw_buf_private.h"
#include "src/display/lv_display_private.h"
#include <string.h>
#include <stdio.h>
#include <board.h>
#include <hpm_soc.h>
#include <hpm_csr_regs.h>
#include <hpm_interrupt.h>
#include <hpm_lcdc_drv.h>
#include <hpm_l1c_drv.h>
#include <hpm_pdma_drv.h>
#include <hpm_mchtmr_drv.h>

#if (LV_USE_OS != LV_OS_NONE)
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#endif

#ifndef LV_INV_BUF_SIZE
#define LV_INV_BUF_SIZE 32
#endif

#define USER_LVGL_RUNNING_CORE      HPM_CORE0
#define USER_LVGL_LCDC_BASE         BOARD_LCD_BASE
#define USER_LVGL_LCDC_LAYER_INDEX  (0)
#define USER_LVGL_LCDC_IRQ_NUM      BOARD_LCD_IRQ
#define USER_LVGL_LCDC_IRQ_PRIORITY 7

#define USER_LVGL_PIXEL_SIZE (LV_COLOR_DEPTH / 8)
#define USER_LVGL_LCD_WIDTH  (BOARD_LCD_WIDTH)
#define USER_LVGL_LCD_HEIGHT (BOARD_LCD_HEIGHT)
#define USER_LVGL_LCD_STRIDE ((USER_LVGL_LCD_WIDTH * USER_LVGL_PIXEL_SIZE + LV_DRAW_BUF_STRIDE_ALIGN - 1) & ~(LV_DRAW_BUF_STRIDE_ALIGN - 1))
#define USER_LVGL_FB_SIZE    HPM_L1C_CACHELINE_ALIGN_UP(USER_LVGL_LCD_STRIDE *USER_LVGL_LCD_HEIGHT)

#define USER_LVGL_MCHTMR     HPM_MCHTMR
#define USER_LVGL_MCHTMR_CLK clock_mchtmr0

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
#ifndef IRQn_PDMA_D0
#define USER_LVGL_PDMA_IRQ_NUM IRQn_PDMA
#else
#define USER_LVGL_PDMA_IRQ_NUM IRQn_PDMA_D0
#endif
#define USER_LVGL_PDMA_IRQ_PRIORITY 6
#ifdef BOARD_PDMA_BASE
#define USER_LVGL_PDMA_BASE BOARD_PDMA_BASE
#else
#define USER_LVGL_PDMA_BASE HPM_PDMA
#endif
#define LVGL_PDMA_CLOCK clock_pdma

typedef void (*user_lvgl_pdma_finish_cb_t)(void *cb_data);

typedef struct user_lvgl_pdma_flush_context {
    struct {
        uint32_t pixel_size;
        uint32_t stride;
        pdma_plane_config_t plane_src;
        pdma_output_config_t output;
    } cfg;
    void *pdma_base;
    uint32_t dst_addr;
    uint32_t src_addr;
    lv_area_t areas[LV_INV_BUF_SIZE];
    volatile uint16_t area_num;
    volatile uint16_t area_num_cnt;
    user_lvgl_pdma_finish_cb_t finish_cb;
    void *finish_cb_data;
    void *user_data;
} user_lvgl_pdma_flush_context_t;
#endif

typedef struct user_lvgl_context {
#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
    user_lvgl_pdma_flush_context_t pdma_ctx;
#endif
    lv_display_t *disp;
    volatile int lcdc_vsync_flag;
    int render_mode;
#if (LV_USE_OS != LV_OS_NONE)
    SemaphoreHandle_t sync;
#endif
} user_lvgl_context_t;

static user_lvgl_context_t user_lvgl_context;

#define FB_SECTION ".framebuffer"

static uint8_t __attribute__((section(FB_SECTION), aligned(HPM_L1C_CACHELINE_SIZE))) user_lvgl_fb0[USER_LVGL_FB_SIZE];
static uint8_t __attribute__((section(FB_SECTION), aligned(HPM_L1C_CACHELINE_SIZE))) user_lvgl_fb1[USER_LVGL_FB_SIZE];
#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
static uint8_t __attribute__((section(FB_SECTION), aligned(HPM_L1C_CACHELINE_SIZE))) user_lvgl_lcdc_fb[USER_LVGL_FB_SIZE];
#endif

static inline uint32_t user_lvgl_irq_lock(void)
{
    return disable_global_irq(CSR_MSTATUS_MIE_MASK);
}

static inline void user_lvgl_irq_unlock(uint32_t state)
{
    restore_global_irq(state);
}

#if (LV_USE_OS != LV_OS_NONE)
static int user_lvgl_lcdc_vsync_init(user_lvgl_context_t *ctx)
{
    ctx->sync = xSemaphoreCreateBinary();
    return 0;
}

ATTR_RAMFUNC static int user_lvgl_lcdc_vsync_wait(user_lvgl_context_t *ctx)
{
    xSemaphoreTake(ctx->sync, portMAX_DELAY);
    return 0;
}

ATTR_RAMFUNC static int user_lvgl_lcdc_vsync_signal(user_lvgl_context_t *ctx)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(ctx->sync, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken != pdFALSE) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    return 0;
}

ATTR_RAMFUNC static uint32_t user_lvgl_tick_get_cb(void)
{
    return xTaskGetTickCount();
}

ATTR_RAMFUNC static void user_lvgl_delay_cb(uint32_t ms)
{
    vTaskDelay(ms);
}

static void user_lvgl_tick_init(void)
{
    lv_tick_set_cb(user_lvgl_tick_get_cb);
    lv_delay_set_cb(user_lvgl_delay_cb);
}
#else
static int user_lvgl_lcdc_vsync_wait(user_lvgl_context_t *ctx)
{
    (void)ctx;
    return 0;
}

ATTR_RAMFUNC static int user_lvgl_lcdc_vsync_signal(user_lvgl_context_t *ctx)
{
    (void)ctx;
    return 0;
}

ATTR_RAMFUNC static int user_lvgl_lcdc_vsync_init(user_lvgl_context_t *ctx)
{
    (void)ctx;
    return 0;
}

ATTR_RAMFUNC static uint32_t user_lvgl_tick_get_cb(void)
{
    static uint32_t mchtmr_freq_in_khz = 0;

    if (!mchtmr_freq_in_khz) {
        clock_add_to_group(USER_LVGL_MCHTMR_CLK, 0);
        mchtmr_freq_in_khz = clock_get_frequency(USER_LVGL_MCHTMR_CLK) / 1000;
    }

    return mchtmr_get_count(USER_LVGL_MCHTMR) / mchtmr_freq_in_khz;
}

static void user_lvgl_tick_init(void)
{
    lv_tick_set_cb(user_lvgl_tick_get_cb);
}
#endif

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
static void user_lvgl_pdma_init(user_lvgl_pdma_flush_context_t *ctx, void *pdma_base, uint32_t pixel_size)
{
    pdma_config_t config;
    display_pixel_format_t pixel_format;
    pdma_plane_config_t *plane_src_cfg = &ctx->cfg.plane_src;
    pdma_output_config_t *output_cfg = &ctx->cfg.output;
    PDMA_Type *pdma_ptr = (PDMA_Type *)pdma_base;

    ctx->pdma_base = pdma_base;
    ctx->cfg.pixel_size = pixel_size;
    clock_add_to_group(LVGL_PDMA_CLOCK, USER_LVGL_RUNNING_CORE);
    pixel_format = (pixel_size == 4U) ? display_pixel_format_argb8888 : display_pixel_format_rgb565;
    pdma_get_default_config(pdma_ptr, &config, pixel_format);

    config.enable_plane = pdma_plane_src;
    config.block_size = pdma_blocksize_16x16;
    pdma_init(pdma_ptr, &config);

    pdma_get_default_plane_config(pdma_ptr, plane_src_cfg, pixel_format);
    pdma_get_default_output_config(pdma_ptr, output_cfg, pixel_format);
    intc_m_enable_irq_with_priority(USER_LVGL_PDMA_IRQ_NUM, USER_LVGL_PDMA_IRQ_PRIORITY);
}

/* 配置并启动一次 PDMA 区域搬运（供 copy_start 和 copy_done 共用） */
ATTR_RAMFUNC static void user_lvgl_pdma_submit_area(user_lvgl_pdma_flush_context_t *ctx, uint32_t dst, uint32_t src)
{
    PDMA_Type *pdma_ptr = (PDMA_Type *)ctx->pdma_base;
    lv_area_t *area = &ctx->areas[ctx->area_num_cnt];
    pdma_plane_config_t *plane_src_cfg = &ctx->cfg.plane_src;
    pdma_output_config_t *output_cfg = &ctx->cfg.output;
    user_lvgl_context_t *lvgl_ctx = ctx->user_data;
    lv_display_t *disp = lvgl_ctx->disp;
    lv_display_rotation_t lvgl_rotation = lv_display_get_rotation(disp);
    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t src_stride;
    uint32_t dst_stride;
    lv_area_t dst_area;

    if (lvgl_rotation == LV_DISPLAY_ROTATION_90) {
        plane_src_cfg->rotate = pdma_rotate_270_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = lv_draw_buf_width_to_stride(lv_display_get_vertical_resolution(disp), cf);
    } else if (lvgl_rotation == LV_DISPLAY_ROTATION_270) {
        plane_src_cfg->rotate = pdma_rotate_90_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = lv_draw_buf_width_to_stride(lv_display_get_vertical_resolution(disp), cf);
    } else if (lvgl_rotation == LV_DISPLAY_ROTATION_180) {
        plane_src_cfg->rotate = pdma_rotate_180_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = src_stride;
    } else {
        plane_src_cfg->rotate = pdma_rotate_0_degree;
        src_stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        dst_stride = src_stride;
    }

    lv_area_copy(&dst_area, area);
    lv_display_rotate_area(disp, &dst_area);

    /* DIRECT 模式：src 指向全帧缓冲，需按区域偏移；PARTIAL 模式：px_map 已是区域起始地址 */
    if (lvgl_ctx->render_mode == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        plane_src_cfg->buffer = (uint32_t)src;
        src_stride = lv_draw_buf_width_to_stride(lv_area_get_width(area), cf);
    } else {
        plane_src_cfg->buffer = (uint32_t)src + (area->y1 * src_stride + area->x1 * ctx->cfg.pixel_size);
    }
    plane_src_cfg->width = lv_area_get_width(area);
    plane_src_cfg->height = lv_area_get_height(area);
    plane_src_cfg->pitch = src_stride;
    plane_src_cfg->background = 0x0;
    pdma_config_planes(pdma_ptr, plane_src_cfg, NULL, NULL);

    output_cfg->plane[pdma_plane_src].x = 0;
    output_cfg->plane[pdma_plane_src].y = 0;
    output_cfg->plane[pdma_plane_src].width = lv_area_get_width(&dst_area);
    output_cfg->plane[pdma_plane_src].height = lv_area_get_height(&dst_area);
    output_cfg->alphablend.src_alpha = 0xFF;
    output_cfg->alphablend.src_alpha_op = display_alpha_op_override;
    output_cfg->alphablend.mode = display_alphablend_mode_clear;
    output_cfg->buffer = (uint32_t)dst + (dst_area.y1 * dst_stride + dst_area.x1 * ctx->cfg.pixel_size);
    output_cfg->width = lv_area_get_width(&dst_area);
    output_cfg->height = lv_area_get_height(&dst_area);
    output_cfg->pitch = dst_stride;
    pdma_config_output(pdma_ptr, output_cfg);

    pdma_start(pdma_ptr);
    pdma_enable_irq(pdma_ptr, PDMA_CTRL_PDMA_DONE_IRQ_EN_MASK, true);
}

ATTR_RAMFUNC static int user_lvgl_pdma_copy_start(user_lvgl_pdma_flush_context_t *ctx, uint32_t dst, uint32_t src)
{
    if (ctx->area_num < 1U) {
        return -1;
    }

    ctx->area_num_cnt = 0;
    ctx->dst_addr = dst;
    ctx->src_addr = src;

    pdma_stop((PDMA_Type *)ctx->pdma_base);
    user_lvgl_pdma_submit_area(ctx, dst, src);
    return 0;
}

ATTR_RAMFUNC static void user_lvgl_pdma_copy_done(user_lvgl_pdma_flush_context_t *ctx)
{
    if (ctx->area_num < 1U) {
        return;
    }

    ctx->area_num_cnt++;
    if (ctx->area_num == ctx->area_num_cnt) {
        ctx->area_num = 0;
        if (ctx->finish_cb != NULL) {
            ctx->finish_cb(ctx->finish_cb_data);
        }
        return;
    }

    user_lvgl_pdma_submit_area(ctx, ctx->dst_addr, ctx->src_addr);
}

ATTR_RAMFUNC static void user_lvgl_pdma_copy_register_finish_cb(user_lvgl_pdma_flush_context_t *ctx, user_lvgl_pdma_finish_cb_t cb, void *cb_data)
{
    ctx->finish_cb = cb;
    ctx->finish_cb_data = cb_data;
}

ATTR_RAMFUNC static int user_lvgl_pdma_add_area(user_lvgl_pdma_flush_context_t *ctx, const lv_area_t *area)
{
    lv_area_copy(&ctx->areas[ctx->area_num], area);
    ctx->area_num++;
    return 0;
}

SDK_DECLARE_EXT_ISR_M(USER_LVGL_PDMA_IRQ_NUM, user_lvgl_pdma_isr)
void user_lvgl_pdma_isr(void)
{
    user_lvgl_pdma_flush_context_t *ctx = &user_lvgl_context.pdma_ctx;
    PDMA_Type *pdma_ptr = (PDMA_Type *)ctx->pdma_base;

    pdma_ptr->STAT = PDMA_STAT_PDMA_DONE_MASK;
    pdma_stop(pdma_ptr);
    pdma_enable_irq(pdma_ptr, PDMA_CTRL_PDMA_DONE_IRQ_EN_MASK, false);
    user_lvgl_pdma_copy_done(ctx);
}
#endif

ATTR_RAMFUNC static void user_lvgl_lcdc_vsync_flag_set(user_lvgl_context_t *ctx, int val)
{
    uint32_t state = user_lvgl_irq_lock();
    ctx->lcdc_vsync_flag = val;
    user_lvgl_irq_unlock(state);
}

ATTR_RAMFUNC static int user_lvgl_lcdc_vsync_flag_get(user_lvgl_context_t *ctx)
{
    return ctx->lcdc_vsync_flag;
}

static void user_lvgl_wait_lcdc_vsync(user_lvgl_context_t *ctx)
{
    user_lvgl_lcdc_vsync_flag_set(ctx, 0);
    while (!user_lvgl_lcdc_vsync_flag_get(ctx)) {
        user_lvgl_lcdc_vsync_wait(ctx);
    }
}

SDK_DECLARE_EXT_ISR_M(USER_LVGL_LCDC_IRQ_NUM, user_lvgl_lcdc_vsync_isr)
void user_lvgl_lcdc_vsync_isr(void)
{
    user_lvgl_context_t *ctx = &user_lvgl_context;

    lcdc_clear_status(USER_LVGL_LCDC_BASE, LCDC_ST_VS_BLANK_MASK);
    user_lvgl_lcdc_vsync_flag_set(ctx, 1);
    user_lvgl_lcdc_vsync_signal(ctx);
}

static void user_lvgl_lcdc_enable(uint32_t fb_buffer)
{
    display_pixel_format_t pixel_format;
    lcdc_config_t config = { 0 };
    lcdc_layer_config_t layer;

    lcdc_get_default_config(USER_LVGL_LCDC_BASE, &config);
    board_panel_para_to_lcdc(&config);

#if LV_COLOR_DEPTH == 32
    pixel_format = display_pixel_format_argb8888;
#elif LV_COLOR_DEPTH == 16
    pixel_format = display_pixel_format_rgb565;
#else
#error only support 16 or 32 color depth
#endif

    lcdc_init(USER_LVGL_LCDC_BASE, &config);

    lcdc_get_default_layer_config(USER_LVGL_LCDC_BASE, &layer, pixel_format, USER_LVGL_LCDC_LAYER_INDEX);
    layer.position_x = 0;
    layer.position_y = 0;
    layer.width = USER_LVGL_LCD_WIDTH;
    layer.height = USER_LVGL_LCD_HEIGHT;
    layer.buffer = fb_buffer;
    layer.stride = USER_LVGL_LCD_STRIDE;
    layer.background.u = 0;
    layer.max_bytes = lcdc_layer_max_bytes_512;
    layer.alphablend.src_alpha = 0xFF;
    layer.alphablend.dst_alpha = 0xFF;
    layer.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer.alphablend.mode = display_alphablend_mode_src_over;

    if (status_success != lcdc_config_layer(USER_LVGL_LCDC_BASE, USER_LVGL_LCDC_LAYER_INDEX, &layer, true)) {
        printf("failed to configure LVGL layer\n");
        while (1) {
        }
    }

    lcdc_turn_on_display(USER_LVGL_LCDC_BASE);
    lcdc_enable_interrupt(USER_LVGL_LCDC_BASE, LCDC_INT_EN_VS_BLANK_MASK);
    intc_m_enable_irq_with_priority(USER_LVGL_LCDC_IRQ_NUM, USER_LVGL_LCDC_IRQ_PRIORITY);
}

static void user_lvgl_display_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    user_lvgl_context_t *ctx = (user_lvgl_context_t *)lv_display_get_user_data(disp);

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
    user_lvgl_pdma_add_area(&ctx->pdma_ctx, area);
#endif

    if ((ctx->render_mode == LV_DISPLAY_RENDER_MODE_DIRECT) && (lv_display_flush_is_last(disp) == 0)) {
        lv_display_flush_ready(disp);
        return;
    }

    if (l1c_dc_is_enabled() && ctx->pdma_ctx.area_num > 0) {
        /* 将所有脏区域合并为一次 writeback，减少 cache 操作次数 */
        uint32_t stride = USER_LVGL_LCD_STRIDE;
        uint32_t merge_start = UINT32_MAX;
        uint32_t merge_end = 0;
        for (uint16_t i = 0; i < ctx->pdma_ctx.area_num; i++) {
            lv_area_t *a = &ctx->pdma_ctx.areas[i];
            uint32_t s = (uint32_t)px_map + a->y1 * stride;
            uint32_t e = (uint32_t)px_map + (a->y2 + 1) * stride;
            if (s < merge_start)
                merge_start = s;
            if (e > merge_end)
                merge_end = e;
        }
        merge_start &= ~(HPM_L1C_CACHELINE_SIZE - 1U);
        uint32_t merge_size = HPM_L1C_CACHELINE_ALIGN_UP(merge_end - merge_start);
        l1c_dc_writeback(merge_start, merge_size);
    }

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
#if defined(LV_USE_HPM_PDMA_WAIT_VSYNC) && LV_USE_HPM_PDMA_WAIT_VSYNC
    if (lv_display_flush_is_last(disp)) {
        user_lvgl_wait_lcdc_vsync(ctx);
    }
#endif
    user_lvgl_pdma_copy_start(&ctx->pdma_ctx, (uint32_t)user_lvgl_lcdc_fb, (uint32_t)px_map);
#else
    do {
        lcdc_layer_set_next_buffer(USER_LVGL_LCDC_BASE, USER_LVGL_LCDC_LAYER_INDEX, (uint32_t)px_map);
        user_lvgl_wait_lcdc_vsync(ctx);
    } while (!lcdc_layer_control_shadow_loaded(USER_LVGL_LCDC_BASE, USER_LVGL_LCDC_LAYER_INDEX));

    lv_display_flush_ready(disp);
#endif
}

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
ATTR_RAMFUNC static void user_lvgl_pdma_finish_callback(void *cb_data)
{
    user_lvgl_context_t *ctx = (user_lvgl_context_t *)cb_data;
    lv_display_flush_ready(ctx->disp);
}
#endif

static void user_lvgl_display_init(void)
{
    lv_display_render_mode_t render_mode;
    lv_display_t *disp;
    uint32_t lcdc_fb_buffer;
    user_lvgl_context_t *ctx = &user_lvgl_context;

#if defined(LV_USE_HPM_MODE_DIRECT) && LV_USE_HPM_MODE_DIRECT
    render_mode = LV_DISPLAY_RENDER_MODE_DIRECT;
#else
    render_mode = LV_DISPLAY_RENDER_MODE_PARTIAL;
#endif

    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)user_lvgl_fb0, USER_LVGL_FB_SIZE);
        l1c_dc_writeback((uint32_t)user_lvgl_fb1, USER_LVGL_FB_SIZE);
    }

    lcdc_fb_buffer = (uint32_t)user_lvgl_fb1;
    disp = lv_display_create(USER_LVGL_LCD_WIDTH, USER_LVGL_LCD_HEIGHT);

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);
#endif
    lv_display_set_buffers(disp, user_lvgl_fb0, user_lvgl_fb1, USER_LVGL_FB_SIZE, render_mode);
    lv_display_set_flush_cb(disp, user_lvgl_display_flush_cb);
    lv_display_set_user_data(disp, ctx);

    lv_sysmon_show_performance(disp);
    lv_sysmon_show_memory(disp);

    ctx->render_mode = render_mode;
    ctx->disp = disp;

#if defined(LV_USE_HPM_PDMA_FLUSH) && LV_USE_HPM_PDMA_FLUSH
    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t stride = lv_draw_buf_width_to_stride(USER_LVGL_LCD_WIDTH, cf);
    ctx->pdma_ctx.cfg.stride = stride;
    lcdc_fb_buffer = (uint32_t)user_lvgl_lcdc_fb;

    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)user_lvgl_lcdc_fb, USER_LVGL_FB_SIZE);
    }

    user_lvgl_pdma_init(&ctx->pdma_ctx, USER_LVGL_PDMA_BASE, LV_COLOR_DEPTH / 8U);
    user_lvgl_pdma_copy_register_finish_cb(&ctx->pdma_ctx, user_lvgl_pdma_finish_callback, ctx);
    ctx->pdma_ctx.user_data = ctx;
#endif

    user_lvgl_lcdc_vsync_init(ctx);
    user_lvgl_lcdc_enable(lcdc_fb_buffer);
}

/**
 * D-Cache flush callback for LVGL draw buffers.
 * Called before GPU/DMA reads CPU-written data (e.g. font glyphs, decoded images).
 * Writes back dirty cache lines to main memory so GPU can see the latest data.
 */
static void user_lvgl_draw_buf_flush_cache(const lv_draw_buf_t *draw_buf, const lv_area_t *area)
{
    LV_UNUSED(area);
    if (l1c_dc_is_enabled() && draw_buf && draw_buf->data) {
        uint32_t size = HPM_L1C_CACHELINE_ALIGN_UP(draw_buf->header.stride * draw_buf->header.h);
        l1c_dc_writeback((uint32_t)draw_buf->data, size);
    }
}

/**
 * D-Cache invalidate callback for LVGL draw buffers.
 * Called after GPU/DMA writes data that CPU needs to read.
 * Invalidates cache lines so CPU reads fresh data from main memory.
 */
static void user_lvgl_draw_buf_invalidate_cache(const lv_draw_buf_t *draw_buf, const lv_area_t *area)
{
    LV_UNUSED(area);
    if (l1c_dc_is_enabled() && draw_buf && draw_buf->data) {
        uint32_t size = HPM_L1C_CACHELINE_ALIGN_UP(draw_buf->header.stride * draw_buf->header.h);
        l1c_dc_invalidate((uint32_t)draw_buf->data, size);
    }
}

/**
 * Register D-Cache callbacks to a draw_buf handler set.
 */
static void user_lvgl_register_cache_cb(lv_draw_buf_handlers_t *handlers)
{
    handlers->flush_cache_cb = user_lvgl_draw_buf_flush_cache;
    handlers->invalidate_cache_cb = user_lvgl_draw_buf_invalidate_cache;
}

void user_lvgl_init(void)
{
    lv_init();

    /* Register D-Cache callbacks for GPU/DMA coherency.
     * VG-Lite GPU accesses memory bypassing CPU D-Cache.
     * Without these callbacks, GPU reads stale data (labels, decoded images).
     */
    user_lvgl_register_cache_cb(lv_draw_buf_get_handlers());       /* default draw bufs */
    user_lvgl_register_cache_cb(lv_draw_buf_get_font_handlers());  /* font glyph draw bufs */
    user_lvgl_register_cache_cb(lv_draw_buf_get_image_handlers()); /* image cache draw bufs */

    user_lvgl_tick_init();
    user_lvgl_display_init();
}

void user_lvgl_wait_vsync(void)
{
    user_lvgl_wait_lcdc_vsync(&user_lvgl_context);
}
