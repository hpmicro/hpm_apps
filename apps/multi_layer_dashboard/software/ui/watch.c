/**
 * @file watch.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "watch.h"

/* 将高频动画回调放入 ITCM，避免每帧从 Flash XIP 取指造成 cache miss */
#ifndef WATCH_RAMFUNC
#ifdef ATTR_RAMFUNC
#define WATCH_RAMFUNC ATTR_RAMFUNC
#else
#define WATCH_RAMFUNC __attribute__((section(".itcm")))
#endif
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/* --- 仪表盘指针 --- */
static lv_obj_t *s_speed_needle = NULL;
static lv_obj_t *s_rpm_needle = NULL;

/* --- 警告图标 --- */
static lv_obj_t *s_turn_left_icon = NULL;
static lv_obj_t *s_turn_right_icon = NULL;
static lv_obj_t *s_fog_icon = NULL;
static lv_obj_t *s_low_beam_icon = NULL;
static lv_obj_t *s_high_beam_icon = NULL;
static lv_obj_t *s_seatbelt_left_icon = NULL;
static lv_obj_t *s_seatbelt_right_icon = NULL;
static lv_obj_t *s_brake_icon = NULL;
static lv_obj_t *s_engine_icon = NULL;

/* --- 档位控件 --- */
static lv_obj_t *s_gear_roller = NULL;
static lv_obj_t *s_gear_value_label = NULL;

/* --- 开机动画状态 --- */
/* 转向灯组亮灭状态 */
static bool s_turn_icon_visible = false;
/* 其余图标组亮灭状态 */
static bool s_warn_icon_visible = false;

/* --- 速度演示计数器 --- */
static int32_t s_speed_value = 0;
static int32_t s_rpm_value = 0;

/* --- 侧边条动画 --- */
static lv_obj_t *s_left_bar_obj = NULL;
static lv_obj_t *s_right_bar_obj = NULL;
static const uint32_t s_left_bar_anim_duration = 2000;
static const uint32_t s_right_bar_anim_duration = 1000;
/* 当前颜色状态：true=红色，false=蓝色 */
static bool s_left_bar_is_red = false;
static bool s_right_bar_is_red = false;
/* 颜色动画是否正在进行中（防止阈值附近重复触发） */
static bool s_left_color_anim_running = false;
static bool s_right_color_anim_running = false;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/* 指针旋转动画 */
static void needle_rotate_anim_init(void);
static int32_t value_to_rotation(int32_t value, int32_t value_min, int32_t value_max, int32_t rotation_min, int32_t rotation_max);
static int32_t speed_to_rotation(int32_t speed);
static int32_t rpm_to_rotation(int32_t rpm);
WATCH_RAMFUNC static void needle_rotate_to(lv_obj_t *needle, int32_t target_rot);
WATCH_RAMFUNC static void rotate_anim_exec_cb(void *obj, int32_t v);
WATCH_RAMFUNC static void speed_subject_changed_cb(lv_observer_t *observer, lv_subject_t *subject);
WATCH_RAMFUNC static void rpm_needle_update(void);

/* 图标显示控制 */
static void icon_set_visible(lv_obj_t *obj, bool visible);
static void icons_set_visible(lv_obj_t **icons, uint32_t count, bool visible);
static lv_obj_t *find_image_by_src(lv_obj_t *parent, const void *src, uint32_t occurrence);

/* 档位动画 */
static void gear_label_update(uint32_t selected);
static void gear_label_sync_from_roller(void);
static void gear_anim_timer_cb(lv_timer_t *timer);

/* 开机动画 */
static void turn_blink_timer_cb(lv_timer_t *timer);
static void warn_blink_timer_cb(lv_timer_t *timer);

/* 侧边条动画 */
WATCH_RAMFUNC static void sidebar_color_anim_ready_cb(lv_anim_t *a);
WATCH_RAMFUNC static void sidebar_color_anim_exec_cb(void *obj, int32_t v);
WATCH_RAMFUNC static void sidebar_trigger_color_anim(lv_obj_t *bar);
WATCH_RAMFUNC static void sidebar_height_anim_exec_cb(void *obj, int32_t v);
static void sidebar_start_anim(lv_obj_t *bar, int32_t from_y, int32_t to_y, uint32_t duration);
static void sidebar_anim_init(void);

/* 速度演示定时器 */
WATCH_RAMFUNC static void speed_demo_timer_cb(lv_timer_t *timer);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void watch_init(const char *asset_path)
{
    watch_init_gen(asset_path);

    lv_screen_load(home);

    /* 指针旋转动画初始化 */
    needle_rotate_anim_init();

    /* 侧边能量条动画初始化 */
    sidebar_anim_init();

    /* 查找并缓存所有警告图标 */
    s_turn_left_icon = find_image_by_src(home_layer7, zuozhuan, 0);
    s_turn_right_icon = find_image_by_src(home_layer7, youzhuan, 0);
    s_fog_icon = find_image_by_src(home_layer6, wudeng, 0);
    s_low_beam_icon = find_image_by_src(home_layer6, jinguangdeng, 0);
    s_high_beam_icon = find_image_by_src(home_layer6, yuanguangdeng, 0);
    s_seatbelt_left_icon = find_image_by_src(home_layer6, anquandai, 0);
    s_seatbelt_right_icon = find_image_by_src(home_layer6, anquandai, 1);
    s_brake_icon = find_image_by_src(home_layer6, shousha, 0);
    s_engine_icon = find_image_by_src(home, fadongji, 0);

    /* 查找并缓存档位控件 */
    s_gear_roller = lv_obj_get_child_by_name(home_layer8, "mode_inner_roller");
    s_gear_value_label = lv_obj_get_child_by_name(home_layer8, "gear_value_label");
    gear_label_sync_from_roller();

    /* 开机前隐藏所有警告图标 */
    lv_obj_t *all_icons[] = { s_turn_left_icon,     s_turn_right_icon,     s_fog_icon,   s_low_beam_icon, s_high_beam_icon,
                              s_seatbelt_left_icon, s_seatbelt_right_icon, s_brake_icon, s_engine_icon };
    icons_set_visible(all_icons, sizeof(all_icons) / sizeof(all_icons[0]), false);

    /* 启动开机动画定时器：转向灯组 1s 亮灭，其余图标组 2s 亮灭 */
    s_turn_icon_visible = false;
    s_warn_icon_visible = false;
    lv_timer_create(turn_blink_timer_cb, 1000, NULL);
    lv_timer_create(warn_blink_timer_cb, 700, NULL);

    /* 档位滚动演示定时器 */
    lv_timer_create(gear_anim_timer_cb, 3000, NULL);

    // /* 速度演示定时器 */
    lv_timer_create(speed_demo_timer_cb, 200, NULL);
}

/* ================================================================
 * 速度演示定时器：周期性驱动速度值在 0~240 之间往返
 * ================================================================ */
WATCH_RAMFUNC static void speed_demo_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    static bool speed_increasing = true;
    static int speed_pause_counter = 0;
    static bool rpm_increasing = true;
    static int rpm_pause_counter = 0;

    if ((s_speed_value == 0 || s_speed_value == 240) && speed_pause_counter < 4) {
        speed_pause_counter++;
    } else {
        speed_pause_counter = 0;

        if (speed_increasing) {
            s_speed_value += 10;
            if (s_speed_value >= 240) {
                s_speed_value = 240;
                speed_increasing = false;
            }
        } else {
            s_speed_value -= 10;
            if (s_speed_value <= 0) {
                s_speed_value = 0;
                speed_increasing = true;
            }
        }

        lv_subject_set_int(&subject_speed_value, s_speed_value);
    }

    if ((s_rpm_value == 0 || s_rpm_value == 16) && rpm_pause_counter < 4) {
        rpm_pause_counter++;
    } else {
        rpm_pause_counter = 0;

        if (rpm_increasing) {
            s_rpm_value++;
            if (s_rpm_value >= 16) {
                s_rpm_value = 16;
                rpm_increasing = false;
            }
        } else {
            s_rpm_value--;
            if (s_rpm_value <= 0) {
                s_rpm_value = 0;
                rpm_increasing = true;
            }
        }

        rpm_needle_update();
    }
}

/* ================================================================
 * 指针旋转动画模块
 * ================================================================ */
WATCH_RAMFUNC static void rotate_anim_exec_cb(void *obj, int32_t v)
{
    lv_obj_set_style_transform_rotation((lv_obj_t *)obj, v, 0);
}

static int32_t speed_to_rotation(int32_t speed)
{
    return value_to_rotation(speed, 0, 240, -1350, 1350);
}

static int32_t rpm_to_rotation(int32_t rpm)
{
    return value_to_rotation(rpm, 0, 16, -1350, 1350);
}

static int32_t value_to_rotation(int32_t value, int32_t value_min, int32_t value_max, int32_t rotation_min, int32_t rotation_max)
{
    int32_t clamped_value = value;
    int32_t value_span = value_max - value_min;
    int32_t rotation_span = rotation_max - rotation_min;

    if (clamped_value < value_min)
        clamped_value = value_min;
    if (clamped_value > value_max)
        clamped_value = value_max;
    if (value_span <= 0)
        return rotation_min;

    return rotation_min + ((clamped_value - value_min) * rotation_span) / value_span;
}

WATCH_RAMFUNC static void needle_rotate_to(lv_obj_t *needle, int32_t target_rot)
{
    if (needle == NULL)
        return;

    int32_t current_rot = lv_obj_get_style_transform_rotation(needle, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, needle);
    lv_anim_set_values(&a, current_rot, target_rot);
    lv_anim_set_duration(&a, 600);
    lv_anim_set_exec_cb(&a, rotate_anim_exec_cb);
    lv_anim_start(&a);
}

static void needle_rotate_anim_init(void)
{
    s_speed_needle = find_image_by_src(home_layer2, needle, 0);
    if (s_speed_needle == NULL)
        return;

    lv_obj_set_style_transform_pivot_x(s_speed_needle, 14, 0);
    lv_obj_set_style_transform_pivot_y(s_speed_needle, 166, 0);
    lv_subject_add_observer_obj(&subject_speed_value, speed_subject_changed_cb, s_speed_needle, NULL);

    s_rpm_needle = find_image_by_src(home_layer3, needle, 0);
    if (s_rpm_needle != NULL) {
        lv_obj_set_style_transform_pivot_x(s_rpm_needle, 14, 0);
        lv_obj_set_style_transform_pivot_y(s_rpm_needle, 166, 0);
    }

    speed_subject_changed_cb(NULL, &subject_speed_value);
    rpm_needle_update();
}

WATCH_RAMFUNC static void speed_subject_changed_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);

    if (s_speed_needle == NULL || subject == NULL)
        return;

    int32_t speed = lv_subject_get_int(subject);
    needle_rotate_to(s_speed_needle, speed_to_rotation(speed));
}

WATCH_RAMFUNC static void rpm_needle_update(void)
{
    if (s_rpm_needle == NULL)
        return;

    needle_rotate_to(s_rpm_needle, rpm_to_rotation(s_rpm_value));
}

/* ================================================================
 * 警告图标显示控制模块
 * ================================================================ */
static void icon_set_visible(lv_obj_t *obj, bool visible)
{
    if (obj == NULL)
        return;

    if (visible)
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

static void icons_set_visible(lv_obj_t **icons, uint32_t count, bool visible)
{
    for (uint32_t i = 0; i < count; i++)
        icon_set_visible(icons[i], visible);
}

static lv_obj_t *find_image_by_src(lv_obj_t *parent, const void *src, uint32_t occurrence)
{
    if (parent == NULL || src == NULL)
        return NULL;

    uint32_t match_index = 0;
    uint32_t child_count = lv_obj_get_child_count(parent);

    for (uint32_t i = 0; i < child_count; i++) {
        lv_obj_t *child = lv_obj_get_child(parent, i);
        if (child == NULL)
            continue;
        if (!lv_obj_check_type(child, &lv_image_class))
            continue;
        if (lv_image_get_src(child) == src) {
            if (match_index == occurrence)
                return child;
            match_index++;
        }
    }

    return NULL;
}

/* ================================================================
 * 档位滚动与标签同步模块
 * ================================================================ */
static void gear_label_update(uint32_t selected)
{
    static const char *gear_texts[] = { "P", "R", "N", "D", "S" };

    if (s_gear_value_label == NULL)
        return;

    lv_label_set_text(s_gear_value_label, gear_texts[selected % 5]);
}

static void gear_label_sync_from_roller(void)
{
    if (s_gear_roller == NULL || s_gear_value_label == NULL)
        return;

    gear_label_update(lv_roller_get_selected(s_gear_roller));
}

static void gear_anim_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    if (s_gear_roller == NULL)
        return;

    uint32_t selected = lv_roller_get_selected(s_gear_roller);
    uint32_t next_selected = (selected + 1) % 5;

    lv_roller_set_selected(s_gear_roller, next_selected, LV_ANIM_ON);
    gear_label_update(next_selected);
}

/* ================================================================
 * 开机图标闪烁动画模块
 *
 * 转向灯组（左转、右转）：上电后每隔 1s 同时亮灭，持续循环。
 * 其余图标组：上电后每隔 2s 同时亮灭，持续循环。
 * ================================================================ */
static void turn_blink_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    s_turn_icon_visible = !s_turn_icon_visible;
    icon_set_visible(s_turn_left_icon, s_turn_icon_visible);
    icon_set_visible(s_turn_right_icon, s_turn_icon_visible);
}

static void warn_blink_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    s_warn_icon_visible = !s_warn_icon_visible;
    icon_set_visible(s_fog_icon, s_warn_icon_visible);
    icon_set_visible(s_low_beam_icon, s_warn_icon_visible);
    icon_set_visible(s_high_beam_icon, s_warn_icon_visible);
    icon_set_visible(s_seatbelt_left_icon, s_warn_icon_visible);
    icon_set_visible(s_seatbelt_right_icon, s_warn_icon_visible);
    icon_set_visible(s_brake_icon, s_warn_icon_visible);
    icon_set_visible(s_engine_icon, s_warn_icon_visible);
}

/* ================================================================
 * 侧边能量条动画模块
 *
 * 颜色动画：将混合比 v 编码到 0~255，用 lv_color_mix 插值
 *   v=0   → 纯蓝(0x0062e7)
 *   v=255 → 纯红(0xca1616)
 *
 * 高度动画阈值（y=230 ≈ 条高一半）：
 *   左侧：y <= 230（条高 > 50%）→ 红色；否则蓝色
 *   右侧：y >  230（条高 < 50%）→ 红色；否则蓝色
 * ================================================================ */
WATCH_RAMFUNC static void sidebar_color_anim_ready_cb(lv_anim_t *a)
{
    if ((lv_obj_t *)a->var == s_left_bar_obj)
        s_left_color_anim_running = false;
    else
        s_right_color_anim_running = false;
}

WATCH_RAMFUNC static void sidebar_color_anim_exec_cb(void *obj, int32_t v)
{
    /* 颜色常量声明为 static const，编译器会将其放入只读数据段，避免每帧在栈上重复初始化 */
    static const lv_color_t s_blue = LV_COLOR_MAKE(0x00, 0x62, 0xe7);
    static const lv_color_t s_red = LV_COLOR_MAKE(0xca, 0x16, 0x16);
    lv_color_t mixed = lv_color_mix(s_red, s_blue, (uint8_t)v);
    lv_obj_set_style_bg_color((lv_obj_t *)obj, mixed, 0);
}

WATCH_RAMFUNC static void sidebar_trigger_color_anim(lv_obj_t *bar)
{
    bool *is_red = (bar == s_left_bar_obj) ? &s_left_bar_is_red : &s_right_bar_is_red;
    bool *running = (bar == s_left_bar_obj) ? &s_left_color_anim_running : &s_right_color_anim_running;

    /* 如果颜色动画正在运行，不重复触发 */
    if (*running)
        return;

    bool going_red = !(*is_red);
    int32_t from_v = going_red ? 0 : 255;
    int32_t to_v = going_red ? 255 : 0;

    /* 先删除该对象上可能残留的旧颜色动画 */
    lv_anim_delete(bar, sidebar_color_anim_exec_cb);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, from_v, to_v);
    lv_anim_set_exec_cb(&a, sidebar_color_anim_exec_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_ready_cb(&a, sidebar_color_anim_ready_cb);
    lv_anim_start(&a);

    *is_red = going_red;
    *running = true;
}

WATCH_RAMFUNC static void sidebar_height_anim_exec_cb(void *obj, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)obj, v);

    const int32_t left_threshold = 230;
    const int32_t right_threshold = -230;

    if ((lv_obj_t *)obj == s_left_bar_obj) {
        bool should_red = (v <= left_threshold);
        if (should_red != s_left_bar_is_red)
            sidebar_trigger_color_anim(s_left_bar_obj);
    } else {
        bool should_red = (v >= right_threshold);
        if (should_red != s_right_bar_is_red)
            sidebar_trigger_color_anim(s_right_bar_obj);
    }
}

static void sidebar_start_anim(lv_obj_t *bar, int32_t from_y, int32_t to_y, uint32_t duration)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, from_y, to_y);
    lv_anim_set_duration(&a, duration);
    lv_anim_set_exec_cb(&a, sidebar_height_anim_exec_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_playback_duration(&a, duration); /* 满→空 */
    lv_anim_set_playback_delay(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&a, 1000);
    lv_anim_start(&a);
}

static void sidebar_anim_init(void)
{
    static const lv_color_t s_blue = LV_COLOR_MAKE(0x00, 0x62, 0xe7);
    static const lv_color_t s_red = LV_COLOR_MAKE(0xca, 0x16, 0x16);
    const int32_t left_start_y = 0;
    const int32_t left_end_y = 460;
    const int32_t right_start_y = -460;
    const int32_t right_end_y = 0;

    lv_obj_t *left_clip = NULL;
    lv_obj_t *right_clip = NULL;

    if (home_layer4)
        left_clip = lv_obj_get_child_by_name(home_layer4, "left_bar_clip");
    if (home_layer5)
        right_clip = lv_obj_get_child_by_name(home_layer5, "right_bar_clip");

    if (left_clip == NULL || right_clip == NULL)
        return;

    s_left_bar_obj = lv_obj_get_child(left_clip, 0);
    s_right_bar_obj = lv_obj_get_child(right_clip, 0);

    if (s_left_bar_obj == NULL || s_right_bar_obj == NULL)
        return;

    lv_obj_set_y(s_left_bar_obj, left_start_y);
    lv_obj_set_y(s_right_bar_obj, right_start_y);

    s_left_bar_is_red = (left_start_y <= 230);
    s_right_bar_is_red = (right_start_y < right_end_y ? (right_start_y < -230) : (right_start_y > 230));
    lv_obj_set_style_bg_color(s_left_bar_obj, s_left_bar_is_red ? s_red : s_blue, 0);
    lv_obj_set_style_bg_color(s_right_bar_obj, s_right_bar_is_red ? s_red : s_blue, 0);

    s_left_color_anim_running = false;
    s_right_color_anim_running = false;

    sidebar_start_anim(s_left_bar_obj, left_start_y, left_end_y, s_left_bar_anim_duration);
    sidebar_start_anim(s_right_bar_obj, right_start_y, right_end_y, s_right_bar_anim_duration);
}
