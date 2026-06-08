/**
 * @file ui.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "ui.h"
#include "data_common.h"
#include "ec_master.h"
#include "motor_control.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

extern data_pack_t data_pack;
extern ec_master_t g_ec_master;
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void timer_update_cb(lv_timer_t *t);
static void init_all_components(void);
static void bind_all_subject(void);
/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *g_home = NULL;
static lv_obj_t *g_label_speed_value = NULL;
static lv_obj_t *g_desire_position_speed = NULL;
static lv_obj_t *g_slider_control = NULL;
static lv_obj_t *g_start_button = NULL;
static lv_obj_t *g_direction_button = NULL;
static lv_obj_t *g_mode_select = NULL;
static lv_obj_t *g_input_select_button = NULL;
static lv_obj_t *g_label_input_select = NULL;
static lv_obj_t *g_output_select_button = NULL;
static lv_obj_t *g_label_output_select = NULL;
static lv_obj_t *g_label_device = NULL;
static lv_obj_t *g_label_state = NULL;
static lv_obj_t *g_label_address = NULL;
static lv_obj_t *g_label_input_bits = NULL;
static lv_obj_t *g_label_output_bits = NULL;
static lv_obj_t *g_slave_dropdown = NULL;
static lv_obj_t *g_monitor_chart = NULL;
static lv_obj_t *g_label_desire_position_ = NULL;
static lv_obj_t *g_label_current_position = NULL;
static lv_obj_t *g_monitor_position_mode_obj = NULL;
static lv_obj_t *g_monitor_speed_mode_obj = NULL;
static lv_chart_series_t *chart_series_1 = NULL;
static lv_chart_series_t *chart_series_2 = NULL;
static bool chart_initialized = false;
static bool start_button_state = false;
static bool direction_button_state = false;
static int32_t desire_position_value = 0;
static int32_t cur_position_value = 0;
static uint8_t last_slider_value = -1;
static uint32_t last_slave_count = 0;

static lv_obj_t *g_obj_1 = NULL;
static lv_obj_t *g_obj_2 = NULL;
static lv_obj_t *g_obj_3 = NULL;
static lv_obj_t *g_obj_4 = NULL;
static lv_obj_t *g_obj_5 = NULL;
static lv_obj_t *g_obj_6 = NULL;
static lv_obj_t *g_obj_7 = NULL;
static lv_obj_t *g_obj_8 = NULL;
static lv_obj_t *g_obj_9 = NULL;
static lv_obj_t *g_obj_10 = NULL;
static lv_obj_t *g_obj_11 = NULL;
static lv_obj_t *g_obj_12 = NULL;

int8_t input_data_buff[12] = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
static int32_t slider_value = -1;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_init(const char *asset_path)
{
    ui_init_gen(asset_path);
    data_pack.dir = DIR_FORWORD;
    data_pack.mode = MODE_CSP;
    data_pack.speed = 0;
    data_pack.position = 0;
    lv_screen_load(home);
    /* Add your own custom code here if needed */
    lv_timer_create(timer_update_cb, 300, NULL);
}
static void timer_update_cb(lv_timer_t *t)
{
    cur_position_value = get_actual_value();
    if (g_home == NULL || g_slider_control == NULL || g_label_speed_value == NULL || g_start_button == NULL || g_mode_select == NULL || g_input_select_button == NULL || g_desire_position_speed == NULL || g_label_input_select == NULL || g_output_select_button == NULL || g_label_output_select == NULL || g_label_device == NULL || g_label_state == NULL || g_label_address == NULL || g_label_input_bits == NULL || g_label_output_bits == NULL || g_monitor_chart == NULL || g_label_desire_position_ == NULL || g_label_current_position == NULL || g_monitor_position_mode_obj == NULL || g_monitor_speed_mode_obj == NULL || g_slave_dropdown == NULL) {
        LV_UNUSED(t);

        init_all_components();

        bind_all_subject();
        if (g_home != NULL && g_desire_position_speed != NULL && g_slider_control != NULL && g_label_speed_value != NULL && g_start_button != NULL && g_mode_select != NULL && g_input_select_button != NULL && g_label_input_select != NULL && g_output_select_button != NULL && g_label_output_select != NULL && g_label_device != NULL && g_label_state != NULL && g_label_address != NULL && g_label_input_bits != NULL && g_label_output_bits != NULL && g_monitor_chart != NULL && g_label_desire_position_ != NULL && g_label_current_position != NULL && g_monitor_position_mode_obj != NULL && g_monitor_speed_mode_obj != NULL && g_slave_dropdown != NULL) {
            LV_LOG_USER("All UI components found successfully.");
            if (!chart_initialized) {
                lv_obj_set_style_width(g_monitor_chart, 1, LV_PART_INDICATOR);
                chart_series_1 = lv_chart_add_series(g_monitor_chart, lv_color_hex(0x4ade80), LV_CHART_AXIS_PRIMARY_Y);
                chart_series_2 = lv_chart_add_series(g_monitor_chart, lv_color_hex(0xff6b6b), LV_CHART_AXIS_PRIMARY_Y);
                chart_initialized = true;
                LV_LOG_USER("Chart initialized.");
            }
        } else {
            LV_LOG_USER("Waiting for UI components to be created...");
            return;
        }
    }

    uint32_t dropdown_count = g_ec_master.slave_count;

    if (dropdown_count > 0) {
        if (g_slave_dropdown != NULL && dropdown_count != last_slave_count) {
            char opts[256];
            opts[0] = '\0';
            for (uint32_t i = 0; i < dropdown_count; i++) {
                char item[32];
                snprintf(item, sizeof(item), "slave_%u", (unsigned int)(i + 1));
                if (i != 0) {
                    strncat(opts, "\n", sizeof(opts) - strlen(opts) - 1);
                }
                strncat(opts, item, sizeof(opts) - strlen(opts) - 1);
            }
            lv_dropdown_set_options(g_slave_dropdown, opts);
            last_slave_count = dropdown_count;

            if (data_pack.cur_index >= dropdown_count) {
                data_pack.cur_index = 0;
            }
            lv_dropdown_set_selected(g_slave_dropdown, data_pack.cur_index);
        }

        uint32_t idx = data_pack.cur_index;

        if (idx >= g_ec_master.slave_count) {
            idx = 0;
        }

        ec_slave_t *s = &g_ec_master.slaves[idx];

        const char *name = NULL;
        if (s->sii.string_count > 0 &&
            s->sii.general.nameidx > 0 &&
            s->sii.general.nameidx <= s->sii.string_count) {
            name = s->sii.strings[s->sii.general.nameidx - 1];
        }

        if (name == NULL) {
            static char buf[32];
            snprintf(buf, sizeof(buf), "%08X:%08X",
                     (unsigned int)s->sii.vendor_id,
                     (unsigned int)s->sii.product_code);
            lv_label_set_text(g_label_device, buf);
        } else if (strcmp(name, "ECAT_CIA402_FOE_V1000") == 0) {
            lv_label_set_text(g_label_device, "伺服电机");
        } else if (strcmp(name, "ECAT_FOE_CIA402") == 0) {
            lv_label_set_text(g_label_device, "步进电机");
        } else {
            lv_label_set_text(g_label_device, name);
        }

        const char *state_str =
            (s->current_state == EC_SLAVE_STATE_OP)     ? "OP" :
            (s->current_state == EC_SLAVE_STATE_SAFEOP) ? "SAFEOP" :
            (s->current_state == EC_SLAVE_STATE_PREOP)  ? "PREOP" :
            (s->current_state == EC_SLAVE_STATE_INIT)   ? "INIT" :
                                                          "UNKNOWN";
        lv_label_set_text(g_label_state, state_str);
        lv_label_set_text_fmt(g_label_address, "%u", s->station_address);
        lv_label_set_text_fmt(g_label_input_bits, "%u", s->idata_size * 8);
        lv_label_set_text_fmt(g_label_output_bits, "%u", s->odata_size * 8);
        const char *log_name = name ? name : "UNKNOWN";
        lv_obj_add_flag(g_start_button, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_state(g_start_button, LV_STATE_DISABLED);
    } else {
        lv_obj_clear_flag(g_start_button, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_state(g_start_button, LV_STATE_DISABLED);
        start_button_state = false;
        lv_label_set_text(g_label_device, "No slave");
        lv_label_set_text(g_label_state, "DISCONNECT");
        lv_label_set_text(g_label_address, "-");
        lv_label_set_text(g_label_input_bits, "0");
        lv_label_set_text(g_label_output_bits, "0");
        lv_obj_set_style_bg_color(g_start_button, lv_color_hex(0x2a3c5e), 0);
        lv_obj_set_style_border_color(g_start_button, lv_color_hex(0x3b3939), 0);
        lv_obj_set_style_shadow_width(g_start_button, 0, 0);
    }

    // 更新 chart and mode label
    if (chart_initialized) {
        lv_coord_t *ser_array_1 = lv_chart_get_series_y_array(g_monitor_chart, chart_series_1);
        lv_coord_t *ser_array_2 = lv_chart_get_series_y_array(g_monitor_chart, chart_series_2);
        if (data_pack.mode == MODE_CSP) {
            lv_obj_set_flag(g_monitor_speed_mode_obj, LV_OBJ_FLAG_HIDDEN, true);
            lv_obj_clear_flag(g_monitor_position_mode_obj, LV_OBJ_FLAG_HIDDEN);
            if (ser_array_1 != NULL && ser_array_2 != NULL) {
                // 添加新数据点到数组末尾
                ser_array_1[31] = desire_position_value;
                ser_array_2[31] = cur_position_value;
            }
        } else if (data_pack.mode == MODE_CSV) {
            lv_obj_clear_flag(g_monitor_speed_mode_obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_flag(g_monitor_position_mode_obj, LV_OBJ_FLAG_HIDDEN, true);
            if (ser_array_1 != NULL) {
                // 添加新数据点到数组末尾
                ser_array_1[31] = desire_position_value;
            }
        }
        for (int32_t i = 0; i < 31; i++) {
            ser_array_1[i] = ser_array_1[i + 1];
            ser_array_2[i] = ser_array_2[i + 1];
        }
        // 重新绘制 chart
        lv_chart_refresh(g_monitor_chart);
    }
    lv_label_set_text_fmt(g_label_current_position, "%d", cur_position_value);
    lv_label_set_text_fmt(g_label_desire_position_, "%d", desire_position_value);
}
void output_select_button_cb(lv_event_t *e)
{
    lv_obj_set_style_border_color(g_output_select_button, lv_color_hex(0xed6666), 0);
    lv_obj_set_style_bg_color(g_output_select_button, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_border_color(g_input_select_button, lv_color_hex(0x00a0ef), 0);
    lv_obj_set_style_bg_color(g_input_select_button, lv_color_hex(0xb1a9a9), 0);
    lv_obj_set_style_text_color(g_label_output_select, lv_color_hex(0xed6666), 0);
    lv_obj_set_style_text_color(g_label_input_select, lv_color_hex(0x1ab7e4), 0);

    lv_obj_set_flag(g_output_select_button, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_flag(g_input_select_button, LV_OBJ_FLAG_CLICKABLE, true);
}

void input_select_button_cb(lv_event_t *e)
{
    lv_obj_set_style_border_color(g_input_select_button, lv_color_hex(0xed6666), 0);
    lv_obj_set_style_bg_color(g_input_select_button, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_border_color(g_output_select_button, lv_color_hex(0x00a0ef), 0);
    lv_obj_set_style_bg_color(g_output_select_button, lv_color_hex(0xb1a9a9), 0);
    lv_obj_set_style_text_color(g_label_input_select, lv_color_hex(0xed6666), 0);
    lv_obj_set_style_text_color(g_label_output_select, lv_color_hex(0x1ab7e4), 0);

    lv_obj_set_flag(g_input_select_button, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_flag(g_output_select_button, LV_OBJ_FLAG_CLICKABLE, true);
}

void start_button_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    start_button_state = !start_button_state;
    if (start_button_state) {
        ec_stop();
        ec_start();
        motor_control_update();
        motor_control_start();
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x31bd73), 0);
        lv_obj_set_style_border_color(btn, lv_color_hex(0x17cb7f), 0);
        lv_obj_set_style_shadow_width(btn, 12, 0);
        lv_obj_set_style_shadow_spread(btn, 5, 0);
        lv_obj_set_style_shadow_color(btn, lv_color_hex(0x1fcc54), 0);
    } else {
        motor_control_stop();
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2a3c5e), 0);
        lv_obj_set_style_border_color(btn, lv_color_hex(0x3b3939), 0);
        lv_obj_set_style_shadow_width(btn, 0, 0);
    }
}

void direction_button_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    direction_button_state = !direction_button_state;
    if (direction_button_state) {
        motor_control_backward();
        lv_obj_set_style_bg_image_src(btn, image_rotate_left, 0);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x31bd73), 0);
        lv_obj_set_style_border_color(btn, lv_color_hex(0x17cb7f), 0);
        lv_obj_set_style_shadow_width(btn, 12, 0);
        lv_obj_set_style_shadow_spread(btn, 5, 0);
        lv_obj_set_style_shadow_color(btn, lv_color_hex(0x1fcc54), 0);
    } else {
        lv_obj_set_style_bg_image_src(btn, image_rotate_right, 0);
        motor_control_forward();
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2a3c5e), 0);
        lv_obj_set_style_border_color(btn, lv_color_hex(0x3b3939), 0);
        lv_obj_set_style_shadow_width(btn, 0, 0);
    }
}

void speed_slider_change_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);
    // 计算区间
    int32_t current_zone = (value <= 50) ? 0 : (value <= 80) ? 1 :
                                                               2;
    int32_t last_zone = (last_slider_value <= 50) ? 0 : (last_slider_value <= 80) ? 1 :
                                                                                    2;

    // 跨却区间时修改样式
    if (last_slider_value == -1 || current_zone != last_zone) {
        lv_color_t color;
        if (value <= 50) {
            color = lv_color_hex(0x24cad0); // 蓝色
        } else if (value <= 80) {
            color = lv_color_hex(0xd3ab46); // 黄色
        } else {
            color = lv_color_hex(0xe13e3e); // 红色
        }

        lv_obj_set_style_bg_color(slider, color, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(slider, color, LV_PART_KNOB);
        if (g_label_speed_value != NULL) {
            lv_obj_set_style_text_color(g_label_speed_value, color, 0);
        }
    }
    last_slider_value = value;

    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        slider_value = lv_slider_get_value(slider);
        if (data_pack.mode == MODE_CSP) {
            if (data_pack.dir == DIR_FORWORD) {
                data_pack.position = desire_position_value + (slider_value * MODE_POSITION_SIZE);
                desire_position_value = data_pack.position;
            } else {
                int32_t target_pos = desire_position_value - (slider_value * MODE_POSITION_SIZE);
                data_pack.position = (target_pos < 0) ? 0 : target_pos;
                desire_position_value = data_pack.position;
            }
            lv_chart_set_range(g_monitor_chart, LV_CHART_AXIS_PRIMARY_Y, desire_position_value - 100000, cur_position_value + 100000);
        } else if (data_pack.mode == MODE_CSV) {
            data_pack.speed = slider_value;
            desire_position_value = data_pack.speed;
            lv_chart_set_range(g_monitor_chart, LV_CHART_AXIS_PRIMARY_Y, -5, 110);
        } else if (data_pack.mode == MODE_CSP_CSV) {
        }
        motor_control_update();
    }
}

void output_button_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);

    /* 识别按钮 ID (1-12) */
    lv_obj_t *parent = lv_obj_get_parent(btn);
    uint32_t parent_idx = lv_obj_get_index(parent);
    uint32_t btn_idx = lv_obj_get_index(btn);
    int button_id = -1;

    if (parent_idx == 0) {
        button_id = btn_idx + 1;
    } else if (parent_idx == 1) {
        button_id = btn_idx + 7;
    }

    printf("Output button %d clicked\n", button_id);

    // 从 user_data 获取当前状态 (0=关闭, 1=激活)
    intptr_t is_active = (intptr_t)lv_obj_get_user_data(btn);

    if (is_active) {
        // 关闭状态
        lv_obj_set_user_data(btn, (void *)0);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x163b58), 0);
        lv_obj_set_style_border_color(btn, lv_color_hex(0x2b6384), 0);
        lv_obj_set_style_shadow_color(btn, lv_color_hex(0x02a4ff), 0);
        lv_obj_set_style_shadow_spread(btn, 1, 0);
        lv_obj_set_style_shadow_width(btn, 5, 0);
        /* 针对不同按钮执行不同逻辑 (关闭) 共12个按钮*/
        switch (button_id) {
            case 1:
                /* DO_01 关闭逻辑 */
                break;
            case 2:
                break;
                /* ... 其他按钮 ... */
        }
    } else {
        // 激活状态
        lv_obj_set_user_data(btn, (void *)1);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x992020), 0);
        lv_obj_set_style_border_color(btn, lv_color_hex(0xbc2719), 0);
        lv_obj_set_style_shadow_color(btn, lv_color_hex(0xe44141), 0);
        lv_obj_set_style_shadow_spread(btn, 8, 0);
        lv_obj_set_style_shadow_width(btn, 15, 0);
        /* 针对不同按钮执行不同逻辑 (激活) 共12个按钮*/
        switch (button_id) {
            case 1:
                /* DO_01 激活逻辑 */
                break;
            case 2:
                break;
                /* ... 其他按钮 ... */
        }
    }
}

void mode_change_cb(lv_event_t *e)
{
    lv_obj_t *dropdown = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(dropdown);
    data_pack.speed = 0;
    motor_control_stop();
    lv_slider_set_value(g_slider_control, 0, 0);
    lv_obj_set_style_text_color(g_label_speed_value, lv_color_hex(0x00f6ff), 0);
    lv_label_set_text(g_label_speed_value, "0");
    // 根据选择的模式更新 data_pack.mode
    switch (selected) {
        case 0: // CSP
            data_pack.speed = 0;
            data_pack.position = get_actual_value();
            desire_position_value = data_pack.position;
            motor_control_update();
            data_pack.mode = MODE_CSP;
            lv_label_set_text(g_desire_position_speed, "Desire Position");
            break;
        case 1: // CSV
            lv_label_set_text(g_desire_position_speed, "Speed");
            data_pack.mode = MODE_CSV;
            break;
    }

    // 如果电机正在运行，需要重新启动以应用新模式
    if (start_button_state) {
        vTaskDelay(5);
        ec_stop();
        ec_start();
        motor_control_update();
        motor_control_start();
    }
}

void slave_dropdown_change_cb(lv_event_t *e)
{
    lv_obj_t *dropdown = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(dropdown);
    if (g_ec_master.slave_count == 0) {
        data_pack.cur_index = 0;
        lv_dropdown_set_selected(dropdown, 0);
        return;
    }
    if (selected >= g_ec_master.slave_count) {
        selected = 0;
    }
    data_pack.cur_index = selected;
    lv_dropdown_set_selected(dropdown, selected);
}

static void init_all_components(void)
{
    g_home = home ? home : lv_screen_active();
    g_label_speed_value = lv_obj_find_by_name(g_home, "label_speed_value");
    g_slider_control = lv_obj_find_by_name(g_home, "slider_control");
    g_start_button = lv_obj_find_by_name(g_home, "start_button");
    g_direction_button = lv_obj_find_by_name(g_home, "direction_button");
    g_mode_select = lv_obj_find_by_name(g_home, "dropdown_mode");
    g_input_select_button = lv_obj_find_by_name(g_home, "button_input");
    g_label_input_select = lv_obj_find_by_name(g_home, "label_input");
    g_output_select_button = lv_obj_find_by_name(g_home, "button_output");
    g_label_output_select = lv_obj_find_by_name(g_home, "label_output");
    g_label_device = lv_obj_find_by_name(g_home, "label_device_name");
    g_label_state = lv_obj_find_by_name(g_home, "label_state");
    g_label_address = lv_obj_find_by_name(g_home, "label_address");
    g_label_input_bits = lv_obj_find_by_name(g_home, "label_input_bits");
    g_label_output_bits = lv_obj_find_by_name(g_home, "label_output_bits");
    g_monitor_chart = lv_obj_find_by_name(g_home, "monitor_chart");
    g_label_desire_position_ = lv_obj_find_by_name(g_home, "label_desire_position");
    g_label_current_position = lv_obj_find_by_name(g_home, "label_current_position");
    g_monitor_position_mode_obj = lv_obj_find_by_name(g_home, "monitor_position_mode_obj");
    g_monitor_speed_mode_obj = lv_obj_find_by_name(g_home, "monitor_speed_mode_obj");
    g_slave_dropdown = lv_obj_find_by_name(g_home, "slave_dropdown");
    g_desire_position_speed = lv_obj_find_by_name(g_home, "desire_position_speed");

    g_obj_1 = lv_obj_find_by_name(g_home, "obj_1");
    g_obj_2 = lv_obj_find_by_name(g_home, "obj_2");
    g_obj_3 = lv_obj_find_by_name(g_home, "obj_3");
    g_obj_4 = lv_obj_find_by_name(g_home, "obj_4");
    g_obj_5 = lv_obj_find_by_name(g_home, "obj_5");
    g_obj_6 = lv_obj_find_by_name(g_home, "obj_6");
    g_obj_7 = lv_obj_find_by_name(g_home, "obj_7");
    g_obj_8 = lv_obj_find_by_name(g_home, "obj_8");
    g_obj_9 = lv_obj_find_by_name(g_home, "obj_9");
    g_obj_10 = lv_obj_find_by_name(g_home, "obj_10");
    g_obj_11 = lv_obj_find_by_name(g_home, "obj_11");
    g_obj_12 = lv_obj_find_by_name(g_home, "obj_12");
}

static void bind_all_subject(void)
{
    lv_subject_set_int(&subject_input_1, input_data_buff[0]);
    lv_subject_set_int(&subject_input_2, input_data_buff[1]);
    lv_subject_set_int(&subject_input_3, input_data_buff[2]);
    lv_subject_set_int(&subject_input_4, input_data_buff[3]);
    lv_subject_set_int(&subject_input_5, input_data_buff[4]);
    lv_subject_set_int(&subject_input_6, input_data_buff[5]);
    lv_subject_set_int(&subject_input_7, input_data_buff[6]);
    lv_subject_set_int(&subject_input_8, input_data_buff[7]);
    lv_subject_set_int(&subject_input_9, input_data_buff[8]);
    lv_subject_set_int(&subject_input_10, input_data_buff[9]);
    lv_subject_set_int(&subject_input_11, input_data_buff[10]);
    lv_subject_set_int(&subject_input_12, input_data_buff[11]);
}
