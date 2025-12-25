/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "monitor_log.h"
#include "monitor_mem.h"
#include "monitor_timer.h"
#include "monitor_ticktime.h"
#include "monitor_list.h"
#include "monitor_report.h"


#define MONITOR_REPORT_CHANNEL_HEAD_OFFSET ((MONITOR_PROFILE_MIN_SIZE - 2) + (sizeof(monitor_payload_t) - 4) + (sizeof(data_payload_t) - 4))
#define MONITOR_REPORT_PACKET_HEAD_OFFSET(count) ((MONITOR_PROFILE_MIN_SIZE - 2) + (sizeof(monitor_payload_t) - 4) + ((sizeof(data_payload_t) - 4) * count))
#define MONITOR_NOTIFY_JITTER_PERIOD_MAX (4)
#define MONTTOR_NOTIFY_REPORT_INTERVAL_MAX (100000) /*100ms*/

enum
{
    TYPE_NOTIFY = 1,
    TYPE_STREAM,
    TYPE_BUFFER,
    TYPE_TRIGGER,
    TYPE_MAX,
};

enum
{
    REPORT_DATA_FLAG_MULTI_DATA = 0,
    REPORT_DATA_FLAG_COMBINE_DATA,
};

typedef struct
{
    uint8_t type;
    uint8_t rsvd1;
    uint16_t rsvd2;
    uint32_t addr_or_ch;
} __attribute__((packed)) notify_header_t;


typedef struct
{
    uint8_t enable;
    uint8_t first_tick;
    uint8_t data_type;
    montiro_report_data_t packet;
    monitor_trigger_t trigger;
    StaticDataLinkedList data_list;
    uint16_t report_index;
} monitor_report_t;

static MONITOR_NOCACHE_RAM_SECTION MONITOR_ATTR_ALIGN(4) monitor_report_t MONITOR_PRIVATE_VAR_NAME(monitor_report);
static volatile uint8_t MONITOR_PRIVATE_VAR_NAME(report_init) = 0;

#define MONITOR_REPORT_CTX (&MONITOR_PRIVATE_VAR_NAME(monitor_report))

// 通用数值条件判断函数
static bool evaluate_condition(int64_t current, int64_t prev, int64_t target, TRIGGER_MODE mode)
{
    switch (mode)
    {
    case TRIGGER_MODE_EDGE_BOTH:
        return (current >= target && prev < target) ||
               (current <= target && prev > target);
    case TRIGGER_MODE_EDGE_RISING:
        return (current >= target) && (prev < target);
    case TRIGGER_MODE_EDGE_FALLING:
        return (current <= target) && (prev > target);
    case TRIGGER_MODE_LEVEL_LOW:
        return current <= target;
    case TRIGGER_MODE_LEVEL_HIGH:
        return current >= target;
    default:
        return false;
    }
    return false;
}

// 浮点专用条件判断
static bool evaluate_float_condition(float current, float prev, float target, TRIGGER_MODE mode)
{
    switch (mode)
    {
    case TRIGGER_MODE_EDGE_BOTH:
        return (current >= target && prev < target) ||
               (current <= target && prev > target);
    case TRIGGER_MODE_EDGE_RISING:
        return (current >= target) && (prev < target);
    case TRIGGER_MODE_EDGE_FALLING:
        return (current <= target) && (prev > target);
    case TRIGGER_MODE_LEVEL_LOW:
        return current <= target;
    case TRIGGER_MODE_LEVEL_HIGH:
        return current >= target;
    default:
        return false;
    }
    return false;
}

static bool evaluate_double_condition(double current, double prev, double target, TRIGGER_MODE mode)
{
    switch (mode)
    {
    case TRIGGER_MODE_EDGE_BOTH:
        return (current >= target && prev < target) ||
               (current <= target && prev > target);
    case TRIGGER_MODE_EDGE_RISING:
        return (current >= target) && (prev < target);
    case TRIGGER_MODE_EDGE_FALLING:
        return (current <= target) && (prev > target);
    case TRIGGER_MODE_LEVEL_LOW:
        return current <= target;
    case TRIGGER_MODE_LEVEL_HIGH:
        return current >= target;
    default:
        return false;
    }
    return false;
}

static bool monitor_trigger_normal_process(uint32_t addr_or_ch, void *input, uint32_t byte_len)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    int index = -1;
    uint32_t i;
    bool cond_met = false;
    monitor_trigger_t *trigger = (monitor_trigger_t *)&ctx->trigger;
    data_trigger_rsv_t *item;
    if (trigger->count == 0 || trigger->count > MONITOR_TRIGGER_MAXCOUNT || input == NULL || byte_len == 0)
    {
        return cond_met;
    }
    for (i = 0; i < trigger->count; ++i)
    {
        item = &trigger->data[i];
        if (item->addr_or_ch == addr_or_ch)
        {
            index = i;
            break;
        }
    }
    if (index < 0)
        return cond_met;

    if (ctx->trigger.trg_result[index])
        return ctx->trigger.trg_result[index];

    switch (item->type)
    {
    case TYPE_BOOL:
    {
        bool *data = (bool *)input;
        for (i = 0; i < byte_len / sizeof(bool); i++)
        {
            bool current = data[i];
            bool prev = ctx->trigger.trg_prev_vals[index][0] != 0;

            // 首次运行初始化历史值
            if (!ctx->trigger.trg_inited[index])
            {
                ctx->trigger.trg_prev_vals[index][0] = current ? 1 : 0;
                ctx->trigger.trg_inited[index] = true;
                prev = current; // 避免首次触发边沿
            }

            switch (item->mode)
            {
            case TRIGGER_MODE_EDGE_BOTH:
                cond_met = (current && !prev) || (!current && prev);
                break;
            case TRIGGER_MODE_EDGE_RISING:
                cond_met = current && !prev;
                break;
            case TRIGGER_MODE_EDGE_FALLING:
                cond_met = !current && prev;
                break;
            case TRIGGER_MODE_LEVEL_LOW:
                cond_met = !current;
                break;
            case TRIGGER_MODE_LEVEL_HIGH:
                cond_met = current;
                break;
            }
            ctx->trigger.trg_prev_vals[index][0] = current ? 1 : 0; // 更新历史值
            if (cond_met)
                break;
        }
        break;
    }
    case TYPE_UINT8:
    {
        uint8_t *data = (uint8_t *)input;
        for (i = 0; i < byte_len / sizeof(uint8_t); i++)
        {
            uint8_t current = data[i];
            uint8_t target = (uint8_t)item->data[0];
            uint8_t prev = (uint8_t)ctx->trigger.trg_prev_vals[index][0];

            if (!ctx->trigger.trg_inited[index])
            {
                *(uint8_t *)ctx->trigger.trg_prev_vals[index] = current;
                ctx->trigger.trg_inited[index] = true;
                prev = current;
            }

            cond_met = evaluate_condition(current, prev, target, item->mode);
            *(uint8_t *)ctx->trigger.trg_prev_vals[index] = current; // 更新历史值
            if (cond_met)
                break;
        }
        break;
    }

    case TYPE_INT8:
    {
        int8_t *data = (int8_t *)input;
        for (i = 0; i < byte_len / sizeof(int8_t); i++)
        {
            int8_t current = data[i];
            int8_t target = (int8_t)item->data[0];
            int8_t prev = (int8_t)ctx->trigger.trg_prev_vals[index][0];

            if (!ctx->trigger.trg_inited[index])
            {
                *(int8_t *)ctx->trigger.trg_prev_vals[index] = current;
                ctx->trigger.trg_inited[index] = true;
                prev = current;
            }
            cond_met = evaluate_condition(current, prev, target, item->mode);
            *(int8_t *)ctx->trigger.trg_prev_vals[index] = current; // 更新历史值
            if (cond_met)
                break;
        }
        break;
    }

    case TYPE_UINT16:
    {
        uint16_t *data = (uint16_t *)input;
        for (i = 0; i < byte_len / sizeof(uint16_t); i++)
        {
            uint16_t current = data[i];
            uint16_t target = (uint16_t)item->data[0];
            uint16_t prev = (uint16_t)ctx->trigger.trg_prev_vals[index][0];

            if (!ctx->trigger.trg_inited[index])
            {
                *(uint16_t *)ctx->trigger.trg_prev_vals[index] = current;
                ctx->trigger.trg_inited[index] = true;
                prev = current;
            }

            cond_met = evaluate_condition(current, prev, target, item->mode);
            *(uint16_t *)ctx->trigger.trg_prev_vals[index] = current; // 更新历史值
            if (cond_met)
                break;
        }
        break;
    }

    case TYPE_INT16:
    {
        int16_t *data = (int16_t *)input;
        for (i = 0; i < byte_len / sizeof(int16_t); i++)
        {
            int16_t current = data[i];
            int16_t target = (int16_t)item->data[0];
            int16_t prev = (int16_t)ctx->trigger.trg_prev_vals[index][0];

            if (!ctx->trigger.trg_inited[index])
            {
                *(int16_t *)ctx->trigger.trg_prev_vals[index] = current;
                ctx->trigger.trg_inited[index] = true;
                prev = current;
            }

            cond_met = evaluate_condition(current, prev, target, item->mode);
            *(int16_t *)ctx->trigger.trg_prev_vals[index] = current; // 更新历史值
            if (cond_met)
                break;
        }
        break;
    }

    case TYPE_UINT32:
    {
        uint32_t *data = (uint32_t *)input;
        for (i = 0; i < byte_len / sizeof(uint32_t); i++)
        {
            uint32_t current = data[i];
            uint32_t target = (uint32_t)item->data[0];
            uint32_t prev = (uint32_t)ctx->trigger.trg_prev_vals[index][0];

            if (!ctx->trigger.trg_inited[index])
            {
                *(uint32_t *)ctx->trigger.trg_prev_vals[index] = current;
                ctx->trigger.trg_inited[index] = true;
                prev = current;
            }

            cond_met = evaluate_condition(current, prev, target, item->mode);
            *(uint32_t *)ctx->trigger.trg_prev_vals[index] = current; // 更新历史值
            if (cond_met)
                break;
        }
        break;
    }

    case TYPE_INT32:
    {
        int32_t *data = (int32_t *)input;
        for (i = 0; i < byte_len / sizeof(int32_t); i++)
        {
            int32_t current = data[i];
            int32_t target = (int32_t)item->data[0];
            int32_t prev = (int32_t)ctx->trigger.trg_prev_vals[index][0];

            if (!ctx->trigger.trg_inited[index])
            {
                *(int32_t *)ctx->trigger.trg_prev_vals[index] = current;
                ctx->trigger.trg_inited[index] = true;
                prev = current;
            }

            cond_met = evaluate_condition(current, prev, target, item->mode);
            *(int32_t *)ctx->trigger.trg_prev_vals[index] = current; // 更新历史值
            if (cond_met)
                break;
        }

        break;
    }

    case TYPE_UINT64:
    {
        uint64_t *data = (uint64_t *)input;
        for (i = 0; i < byte_len / sizeof(uint64_t); i++)
        {
            uint64_t current = data[i];
            uint64_t target = (uint64_t)item->data[0];
            uint64_t prev = (uint64_t)ctx->trigger.trg_prev_vals[index][0];

            if (!ctx->trigger.trg_inited[index])
            {
                *(uint64_t *)ctx->trigger.trg_prev_vals[index] = current;
                ctx->trigger.trg_inited[index] = true;
                prev = current;
            }

            cond_met = evaluate_condition(current, prev, target, item->mode);
            *(uint64_t *)ctx->trigger.trg_prev_vals[index] = current; // 更新历史值
            if (cond_met)
                break;
        }
        break;
    }

    case TYPE_INT64:
    {
        int64_t *data = (int64_t *)input;
        for (i = 0; i < byte_len / sizeof(int64_t); i++)
        {
            int64_t current = data[i];
            int64_t target = (int64_t)item->data[0];
            int64_t prev = (int64_t)ctx->trigger.trg_prev_vals[index][0];

            if (!ctx->trigger.trg_inited[index])
            {
                *(int64_t *)ctx->trigger.trg_prev_vals[index] = current;
                memcpy(ctx->trigger.trg_prev_vals[index], &current, sizeof(current));
                ctx->trigger.trg_inited[index] = true;
                prev = current;
            }

            cond_met = evaluate_condition(current, prev, target, item->mode);
            *(int64_t *)ctx->trigger.trg_prev_vals[index] = current; // 更新历史值
            if (cond_met)
                break;
        }

        break;
    }

    case TYPE_FLOAT:
    {
        float *data = (float *)input;
        for (i = 0; i < byte_len / sizeof(float); i++)
        {
            float current = data[i];
            float target = (float)item->data[0];
            float prev = (float)ctx->trigger.trg_prev_vals[index][0];
            if (!ctx->trigger.trg_inited[index])
            {
                *(float *)ctx->trigger.trg_prev_vals[index] = current;
                ctx->trigger.trg_inited[index] = true;
                prev = current;
            }
            cond_met = evaluate_float_condition(current, prev, target, item->mode);
            *(float *)ctx->trigger.trg_prev_vals[index] = current;
            if (cond_met)
                break;
        }
        break;
    }

    case TYPE_DOUBLE:
    {
        double *data = (double *)input;
        for (i = 0; i < byte_len / sizeof(double); i++)
        {
            double current = data[i];
            double target = (double)item->data[0];
            double prev = (double)ctx->trigger.trg_prev_vals[index][0];
            if (!ctx->trigger.trg_inited[index])
            {
                *(double *)ctx->trigger.trg_prev_vals[index] = current;
                ctx->trigger.trg_inited[index] = true;
                prev = current;
            }
            cond_met = evaluate_double_condition(current, prev, target, item->mode);
            *(double *)ctx->trigger.trg_prev_vals[index] = current;
            if (cond_met)
                break;
        }
        break;
    }

    default:
        cond_met = false;
        break;
    }
    ctx->trigger.trg_result[index] = cond_met;
    return cond_met;
}

#if 0
static void monitor_trigger_special_process(void *input, uint32_t byte_len)
{
#if 0
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    uint32_t i, j, a, count, offset, packlen;
    bool cond_met = false;
    monitor_trigger_t *trigger = (monitor_trigger_t *)&ctx->trigger;
    montiro_report_data_t *data = (montiro_report_data_t *)&ctx->packet;
    data_trigger_rsv_t *item;
    count = data->sample.data_count;
     if (trigger->count == 0 || trigger->count > MONITOR_TRIGGER_MAXCOUNT || input == NULL || byte_len == 0 ||\
         byte_len < monitor_get_sample_count_size(data, count))
     {
         return;
     }
     packlen = monitor_get_sample_once_size(&ctx->data);
    for (i = 0; i < trigger->count; ++i)
    {
        if (ctx->trigger.trg_result[i])
        {
            continue;
        }
        item = &trigger->data[i];
        for (j = 0; j < data->sample.pkt_count; ++j)
        {
            if (item->addr_or_ch == data->sample_data[j].addr_or_ch)
            {
                offset = 0;
                for (a = 0; a < j; ++a)
                {
                    offset += monitor_type_convert_byte(data->sample_data[j].type);
                }
                switch (item->type)
                {
                case TYPE_BOOL:
                {
                    for (a = 0; a < count; a++)
                    {
                        bool current = *(bool *)(input + offset + (a * packlen));
                        bool prev = ctx->trigger.trg_prev_vals[i][0] != 0;

                        // 首次运行初始化历史值
                        if (!ctx->trigger.trg_inited[i])
                        {
                            ctx->trigger.trg_prev_vals[i][0] = current ? 1 : 0;
                            ctx->trigger.trg_inited[i] = true;
                            prev = current; // 避免首次触发边沿
                        }

                        switch (item->mode)
                        {
                        case TRIGGER_MODE_EDGE_BOTH:
                            cond_met = (current && !prev) || (!current && prev);
                            break;
                        case TRIGGER_MODE_EDGE_RISING:
                            cond_met = current && !prev;
                            break;
                        case TRIGGER_MODE_EDGE_FALLING:
                            cond_met = !current && prev;
                            break;
                        case TRIGGER_MODE_LEVEL_LOW:
                            cond_met = !current;
                            break;
                        case TRIGGER_MODE_LEVEL_HIGH:
                            cond_met = current;
                            break;
                        }
                        ctx->trigger.trg_prev_vals[i][0] = current ? 1 : 0; // 更新历史值
                        if (cond_met)
                            break;
                    }
                    break;
                }
                case TYPE_UINT8:
                {
                    for (a = 0; a < count; a++)
                    {
                        uint8_t current = *(uint8_t *)(input + offset + (a * packlen));
                        uint8_t target = (uint8_t)item->data[0];
                        uint8_t prev = (uint8_t)ctx->trigger.trg_prev_vals[i][0];

                        if (!ctx->trigger.trg_inited[i])
                        {
                            *(uint8_t *)ctx->trigger.trg_prev_vals[i] = current;
                            ctx->trigger.trg_inited[i] = true;
                            prev = current;
                        }

                        cond_met = evaluate_condition(current, prev, target, item->mode);
                        *(uint8_t *)ctx->trigger.trg_prev_vals[i] = current; // 更新历史值
                        if (cond_met)
                            break;
                    }
                    break;
                }

                case TYPE_INT8:
                {
                    for (a = 0; a < count; a++)
                    {
                        int8_t current = *(int8_t *)(input + offset + (a * packlen));
                        int8_t target = (int8_t)item->data[0];
                        int8_t prev = (int8_t)ctx->trigger.trg_prev_vals[i][0];

                        if (!ctx->trigger.trg_inited[i])
                        {
                            *(int8_t *)ctx->trigger.trg_prev_vals[i] = current;
                            ctx->trigger.trg_inited[i] = true;
                            prev = current;
                        }
                        cond_met = evaluate_condition(current, prev, target, item->mode);
                        *(int8_t *)ctx->trigger.trg_prev_vals[i] = current; // 更新历史值
                        if (cond_met)
                            break;
                    }
                    break;
                }

                case TYPE_UINT16:
                {
                    for (a = 0; a < count; a++)
                    {
                        uint16_t current = *(uint16_t *)(input + offset + (a * packlen));
                        uint16_t target = (uint16_t)item->data[0];
                        uint16_t prev = (uint16_t)ctx->trigger.trg_prev_vals[i][0];

                        if (!ctx->trigger.trg_inited[i])
                        {
                            *(uint16_t *)ctx->trigger.trg_prev_vals[i] = current;
                            ctx->trigger.trg_inited[i] = true;
                            prev = current;
                        }

                        cond_met = evaluate_condition(current, prev, target, item->mode);
                        *(uint16_t *)ctx->trigger.trg_prev_vals[i] = current; // 更新历史值
                        if (cond_met)
                            break;
                    }
                    break;
                }

                case TYPE_INT16:
                {
                    for (a = 0; a < count; a++)
                    {
                        int16_t current = *(int16_t *)(input + offset + (a * packlen));
                        int16_t target = (int16_t)item->data[0];
                        int16_t prev = (int16_t)ctx->trigger.trg_prev_vals[i][0];

                        if (!ctx->trigger.trg_inited[i])
                        {
                            *(int16_t *)ctx->trigger.trg_prev_vals[i] = current;
                            ctx->trigger.trg_inited[i] = true;
                            prev = current;
                        }

                        cond_met = evaluate_condition(current, prev, target, item->mode);
                        *(int16_t *)ctx->trigger.trg_prev_vals[i] = current; // 更新历史值
                        if (cond_met)
                            break;
                    }
                    break;
                }

                case TYPE_UINT32:
                {
                    for (a = 0; a < count; a++)
                    {
                        uint32_t current = *(uint32_t *)(input + offset + (a * packlen));
                        uint32_t target = (uint32_t)item->data[0];
                        uint32_t prev = (uint32_t)ctx->trigger.trg_prev_vals[i][0];

                        if (!ctx->trigger.trg_inited[i])
                        {
                            *(uint32_t *)ctx->trigger.trg_prev_vals[i] = current;
                            ctx->trigger.trg_inited[i] = true;
                            prev = current;
                        }

                        cond_met = evaluate_condition(current, prev, target, item->mode);
                        *(uint32_t *)ctx->trigger.trg_prev_vals[i] = current; // 更新历史值
                        if (cond_met)
                            break;
                    }
                    break;
                }

                case TYPE_INT32:
                {
                    for (a = 0; a < count; a++)
                    {
                        int32_t current = *(int32_t *)(input + offset + (a * packlen));
                        int32_t target = (int32_t)item->data[0];
                        int32_t prev = (int32_t)ctx->trigger.trg_prev_vals[i][0];

                        if (!ctx->trigger.trg_inited[i])
                        {
                            *(int32_t *)ctx->trigger.trg_prev_vals[i] = current;
                            ctx->trigger.trg_inited[i] = true;
                            prev = current;
                        }

                        cond_met = evaluate_condition(current, prev, target, item->mode);
                        *(int32_t *)ctx->trigger.trg_prev_vals[i] = current; // 更新历史值
                        if (cond_met)
                            break;
                    }

                    break;
                }

                case TYPE_UINT64:
                {
                    for (a = 0; a < count; a++)
                    {
                        uint64_t current = *(uint64_t *)(input + offset + (a * packlen));
                        uint64_t target = (uint64_t)item->data[0];
                        uint64_t prev = (uint64_t)ctx->trigger.trg_prev_vals[i][0];

                        if (!ctx->trigger.trg_inited[i])
                        {
                            *(uint64_t *)ctx->trigger.trg_prev_vals[i] = current;
                            ctx->trigger.trg_inited[i] = true;
                            prev = current;
                        }

                        cond_met = evaluate_condition(current, prev, target, item->mode);
                        *(uint64_t *)ctx->trigger.trg_prev_vals[i] = current; // 更新历史值
                        if (cond_met)
                            break;
                    }
                    break;
                }

                case TYPE_INT64:
                {
                    for (a = 0; a < count; a++)
                    {
                        int64_t current = *(int64_t *)(input + offset + (a * packlen));
                        int64_t target = (int64_t)item->data[0];
                        int64_t prev = (int64_t)ctx->trigger.trg_prev_vals[i][0];

                        if (!ctx->trigger.trg_inited[i])
                        {
                            *(int64_t *)ctx->trigger.trg_prev_vals[i] = current;
                            memcpy(ctx->trigger.trg_prev_vals[i], &current, sizeof(current));
                            ctx->trigger.trg_inited[i] = true;
                            prev = current;
                        }

                        cond_met = evaluate_condition(current, prev, target, item->mode);
                        *(int64_t *)ctx->trigger.trg_prev_vals[i] = current; // 更新历史值
                        if (cond_met)
                            break;
                    }

                    break;
                }

                case TYPE_FLOAT:
                {
                    for (a = 0; a < count; a++)
                    {
                        float current = *(float *)(input + offset + (a * packlen));
                        float target = (float)item->data[0];
                        float prev = (float)ctx->trigger.trg_prev_vals[i][0];
                        if (!ctx->trigger.trg_inited[i])
                        {
                            *(float *)ctx->trigger.trg_prev_vals[i] = current;
                            ctx->trigger.trg_inited[i] = true;
                            prev = current;
                        }
                        cond_met = evaluate_float_condition(current, prev, target, item->mode);
                        *(float *)ctx->trigger.trg_prev_vals[i] = current;
                        if (cond_met)
                            break;
                    }
                    break;
                }

                case TYPE_DOUBLE:
                {
                    for (a = 0; a < count; a++)
                    {
                        double current = *(double *)(input + offset + (a * packlen));
                        double target = (double)item->data[0];
                        double prev = (double)ctx->trigger.trg_prev_vals[i][0];
                        if (!ctx->trigger.trg_inited[i])
                        {
                            *(double *)ctx->trigger.trg_prev_vals[i] = current;
                            ctx->trigger.trg_inited[i] = true;
                            prev = current;
                        }
                        cond_met = evaluate_double_condition(current, prev, target, item->mode);
                        *(double *)ctx->trigger.trg_prev_vals[i] = current;
                        if (cond_met)
                            break;
                    }
                    break;
                }

                default:
                    cond_met = false;
                    break;
                }
                ctx->trigger.trg_result[i] = cond_met;
            }
        }
    }
#endif
    return;
}

static bool monitor_trigger_check(void)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    uint32_t i;
    if (ctx->trigger.count == 0 || ctx->trigger.count > MONITOR_TRIGGER_MAXCOUNT)
    {
        return false;
    }
    bool final_result = ctx->trigger.trg_result[0];
    for (i = 1; i < ctx->trigger.count; ++i)
    {
        switch (ctx->trigger.data[i].expr)
        {
        case TRIGGER_EXPR_AND:
            final_result &= ctx->trigger.trg_result[i];
            break;
        case TRIGGER_EXPR_OR:
            final_result |= ctx->trigger.trg_result[i];
            break;
        default:
            final_result = false;
        }
    }
    return final_result;
}
#endif

static void monitor_sample_cb(int ret, uint8_t index)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    static int old_result = 0;
#if 0
    static int buf_count = 0;
     if (ctx->enable == TYPE_TRIGGER && ret == 0)
     {
         if (ctx->data.sample_count == 1)
         {
             monitor_trigger_normal_process(ctx->data.sample_data[0].addr_or_ch,
                                            (void *)(ctx->data.sample_buf_addr[index] + MONITOR_PAYLOAD_DATA_OFFSET),
                                            (ctx->data.sample_buf_len[index] - MONITOR_PAYLOAD_DATA_OFFSET));
         }
         else if (ctx->data.sample_count > 1)
         {
             monitor_trigger_special_process((void *)(ctx->data.sample_buf_addr[index] + MONITOR_PAYLOAD_DATA_OFFSET),
                                             (ctx->data.sample_buf_len[index] - MONITOR_PAYLOAD_DATA_OFFSET));
         }

         if (monitor_trigger_check())
         {
             if (ctx->data_type == TYPE_STREAM)
             {
                 ctx->enable = TYPE_STREAM;
             }
             else if (ctx->data_type == TYPE_BUFFER)
             {
                 buf_count = 0;
                 ctx->enable = TYPE_BUFFER;
             }
         }
     }
#endif

    if (ctx->enable == TYPE_STREAM || ctx->enable == TYPE_BUFFER)
    {
        if (data_list_get_of_addr(&ctx->data_list, ctx->packet.mem_addr[ctx->packet.channel.pkt_count + index]) != NULL)
        {
            old_result = -1; // lost data
            return;
        }
        if (ret == 0 && old_result != 0)
        {
            ret = old_result;
        }
        if (data_list_add(&ctx->data_list, ctx->packet.mem_addr[ctx->packet.channel.pkt_count + index], ctx->packet.mem_len[ctx->packet.channel.pkt_count + index], ret, -1, tick_time_us_read32()))
        {
            old_result = 0;
        }
        else
        {
            old_result = -1; // lost data
        }
    }
}

uint32_t monitor_noitfy_get_data_once_size(data_payload_t *data, uint32_t pkt_count)
{
    int type_byte;
    uint32_t i, once_size = 0;
    for (i = 0; i < pkt_count; i++)
    {
        type_byte = monitor_type_convert_byte(data->type);
        if (type_byte <= 0)
        {
            MONITOR_LOG_ERR("BAD, dma type no support!\r\n");
            return 0;
        }
        once_size += type_byte;
    }
    return once_size;
}

static int monitor_channel_add(data_payload_t *data)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    const monitor_var_info_t *monitor_ch_info = NULL;
    data_payload_t *data_payload = data;
    if (data == NULL)
        return -1;

    if (!payload_is_channel_type(data_payload->addr_or_ch))
    {
        return 0;
    }

    monitor_ch_info = monitor_ch_info_find_by_channel(MONITOR_PAYLOAD_CH_DATA(data_payload->addr_or_ch));
    if(monitor_ch_info == NULL)
    {
        MONITOR_LOG_ERR("BAD, notify channel 0x%08x not found!\r\n", MONITOR_PAYLOAD_CH_DATA(data_payload->addr_or_ch));
        return -3;
    }

    if(ctx->packet.channel.pkt_count > MONITOR_REPORT_MAXCOUNT)
    {
        MONITOR_LOG_ERR("BAD, channel maximum limit!\r\n");
        return -2;
    }
    ctx->packet.channel.data_freq[ctx->packet.channel.pkt_count] = monitor_ch_info->freq_t;
    memcpy(&(ctx->packet.data[ctx->packet.channel.pkt_count]), data_payload, (sizeof(data_payload_t) - 4));
    ctx->packet.channel.pkt_count++;
    return 0;
}

static int monitor_notify_add(data_payload_t *data)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    if (data == NULL)
        return -1;

    if (payload_is_channel_type(data->addr_or_ch))
    {
        return 0;
    }

    memcpy(&(ctx->packet.data[ctx->packet.channel.pkt_count + ctx->packet.notify.pkt_count]), data, (sizeof(data_payload_t) - 4));
    ctx->packet.notify.pkt_count++;
    return 0;
}

static int monitor_sample_add(data_payload_t *data)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    if (data == NULL)
        return -1;

    if (payload_is_channel_type(data->addr_or_ch))
    {
        return 0;
    }

    if(ctx->packet.sample.pkt_count > MONITOR_REPORT_MAXCOUNT)
    {
        MONITOR_LOG_ERR("BAD, sample maximum limit!\r\n");
        return -2;
    }

    memcpy(&(ctx->packet.data[ctx->packet.channel.pkt_count + ctx->packet.sample.pkt_count]), data, (sizeof(data_payload_t) - 4));
    ctx->packet.sample.pkt_count++;
    return 0;
}

void monitor_trigger_clear(void)
{
    // monitor_report_t *ctx = MONITOR_REPORT_CTX;
    // ctx->trigger.count = 0;
    // memset(ctx->trigger.trg_inited, 0, sizeof(ctx->trigger.trg_inited));
    // memset(ctx->trigger.trg_prev_vals, 0, sizeof(ctx->trigger.trg_prev_vals));
    // memset(ctx->trigger.trg_result, 0, sizeof(ctx->trigger.trg_result));
}

void monitor_report_clear(void)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    monitor_mem_reset();
    init_static_data_list(&ctx->data_list);
    ctx->data_type = 0;
    ctx->enable = 0;
    ctx->first_tick = 0;
    memset(&(ctx->packet), 0, sizeof(montiro_report_data_t));
    memset(&(ctx->trigger), 0, sizeof(monitor_trigger_t));
}

int monitor_notify_set(uint8_t *input, uint32_t length)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    monitor_payload_t *monitor_payload;
    data_payload_t *data_payload;
    uint8_t *start_point;
    uint32_t offset;
    if (length > MONITOR_PROFILE_MAXSIZE || length <= 0)
        return -1;

    monitor_payload = monitor_profile_get_payload(input);
    start_point = (uint8_t *)&monitor_payload->data_payload;
    offset = 0;

    for (int i = 0; i < monitor_payload->pkt_count; i++)
    {
        data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
        if (payload_is_channel_type(data_payload->addr_or_ch))
        {
            if(monitor_channel_add(data_payload) != 0)
            {
                return -1;
            }
        }
        offset += sizeof(data_payload_t) - 4;
    }

    start_point = (uint8_t *)&monitor_payload->data_payload;
    offset = 0;
    for (int i = 0; i < monitor_payload->pkt_count; i++)
    {
        data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
        if (!payload_is_channel_type(data_payload->addr_or_ch))
        {
            if(monitor_notify_add(data_payload) != 0)
            {
                return -1;
            }
        }
        offset += sizeof(data_payload_t) - 4;
    }
    if(ctx->packet.notify.pkt_count > 0)
    {
        ctx->packet.notify.freq = monitor_payload->freq;
    }

    ctx->data_type = TYPE_NOTIFY;
    return 0;
}

int monitor_stream_set(uint8_t *input, uint32_t length)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    monitor_payload_t *monitor_payload;
    data_payload_t *data_payload;
    uint8_t *start_point;
    uint32_t offset;
    if (length > MONITOR_PROFILE_MAXSIZE || length <= 0)
        return -1;

    monitor_payload = monitor_profile_get_payload(input);
    start_point = (uint8_t *)&monitor_payload->data_payload;
    offset = 0;

    for (int i = 0; i < monitor_payload->pkt_count; i++)
    {
        data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
        if (payload_is_channel_type(data_payload->addr_or_ch))
        {
            if(monitor_channel_add(data_payload) != 0)
            {
                return -1;
            }
        }
        offset += sizeof(data_payload_t) - 4;
    }

    start_point = (uint8_t *)&monitor_payload->data_payload;
    offset = 0;
    for (int i = 0; i < monitor_payload->pkt_count; i++)
    {
        data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
        if (!payload_is_channel_type(data_payload->addr_or_ch))
        {
            if(monitor_sample_add(data_payload) != 0)
            {
                return -1;
            }
        }
        offset += sizeof(data_payload_t) - 4;
    }

    if(ctx->packet.sample.pkt_count > 0)
    {
        ctx->packet.sample.data_freq = monitor_payload->freq;
        ctx->packet.sample.data_count = monitor_payload->dat_count;
    }
    ctx->data_type = TYPE_STREAM;;
    return 0;
}


int monitor_buffer_set(uint8_t *input, uint32_t length)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    monitor_payload_t *monitor_payload;
    data_payload_t *data_payload;
    uint8_t *start_point;
    uint32_t offset;
    if (length > MONITOR_PROFILE_MAXSIZE || length <= 0)
        return -1;

    monitor_payload = monitor_profile_get_payload(input);
    start_point = (uint8_t *)&monitor_payload->data_payload;
    offset = 0;

    for (int i = 0; i < monitor_payload->pkt_count; i++)
    {
        data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
        if (payload_is_channel_type(data_payload->addr_or_ch))
        {
            if(monitor_channel_add(data_payload) != 0)
            {
                return -1;
            }
        }
        offset += sizeof(data_payload_t) - 4;
    }

    start_point = (uint8_t *)&monitor_payload->data_payload;
    offset = 0;
    for (int i = 0; i < monitor_payload->pkt_count; i++)
    {
        data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
        if (!payload_is_channel_type(data_payload->addr_or_ch))
        {
            if(monitor_sample_add(data_payload) != 0)
            {
                return -1;
            }
        }
        offset += sizeof(data_payload_t) - 4;
    }

    if(ctx->packet.sample.pkt_count > 0)
    {
        ctx->packet.sample.data_freq = monitor_payload->freq;
        ctx->packet.sample.data_count = monitor_payload->dat_count;
    }
    ctx->data_type = TYPE_BUFFER;;
    return 0;
}

static int monitor_report_channel_config(void)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    monitor_payload_t *monitor_payload = NULL;
    data_payload_t *data_payload;
    uint8_t *start_point = NULL;
    const monitor_var_info_t *monitor_ch_info = NULL;
    float report_ms = 0; //100ms

    if(ctx->packet.channel.pkt_count <=0)
    {
        return 0;
    }

    for(uint16_t i = 0; i < ctx->packet.channel.pkt_count; i++)
    {
        monitor_ch_info = monitor_ch_info_find_by_channel(MONITOR_PAYLOAD_CH_DATA(ctx->packet.data[i].addr_or_ch));
        if(monitor_ch_info == NULL)
        {
            MONITOR_LOG_ERR("BAD, notify channel 0x%08x not found!\r\n", MONITOR_PAYLOAD_CH_DATA(ctx->packet.data[i].addr_or_ch));
            return -3;
        }

        if(monitor_ch_info->count_t <=0)
        {
    #define CHANNEL_REPORT_STEP   (0.1) //100ms
    #define CHANNLE_REPORT_MAX    (0.5) //500ms
    #define CHANNEL_REPORT_DATACOUNT_MIN_LIMIT    (1000)
            report_ms = 0;
            do
            {
                if(report_ms >= CHANNLE_REPORT_MAX)
                {
                    break;
                }
                report_ms += CHANNEL_REPORT_STEP;
                ctx->packet.channel.data_count[i] = monitor_ch_info->freq_t * report_ms;
            } while(report_ms < CHANNLE_REPORT_MAX && ctx->packet.channel.data_count[i] < CHANNEL_REPORT_DATACOUNT_MIN_LIMIT);
            ctx->packet.channel.is_double_mem[i] = 1;
            MONITOR_LOG_DBG("ch %d, report ms:%d ms, datacount:%d\r\n",MONITOR_PAYLOAD_CH_DATA(ctx->packet.data[i].addr_or_ch),
                (int)(report_ms*1000), ctx->packet.channel.data_count[i]);
        }
        else
        {
            ctx->packet.channel.data_count[i] = monitor_ch_info->count_t;
            ctx->packet.channel.is_double_mem[i] = 0;
        }
    }

    for(uint16_t i = 0; i < ctx->packet.channel.pkt_count; i++)
    {
        ctx->packet.channel.mem_single_len[i] = MONITOR_PAYLOAD_DATA_OFFSET + (ctx->packet.channel.data_count[i] * monitor_type_convert_byte(ctx->packet.data[i].type)) + 2; //+2 is end
        if(ctx->packet.channel.is_double_mem[i])
        {
            ctx->packet.mem_len[i] = ALIGN_4(ctx->packet.channel.mem_single_len[i]) * 2;
            ctx->packet.channel.mem_cur_index[i] = 0;
            ctx->packet.channel.data_index[i] = 0;

        }
        else
        {
            ctx->packet.mem_len[i] = ctx->packet.channel.mem_single_len[i];
        }
        ctx->packet.mem_addr[i] = (uint32_t)monitor_mem_alloc(ctx->packet.mem_len[i]);
        if (ctx->packet.mem_addr[i] == 0)
        {
            MONITOR_LOG_ERR("BAD, channel mem alloc fail!\r\n");
            return -4;
        }
        start_point = (uint8_t *)(ctx->packet.mem_addr[i]);
        monitor_profile_set_header(start_point);
        monitor_profile_set_length(start_point, ctx->packet.channel.mem_single_len[i] - MONITOR_PROFILE_MIN_SIZE);
        monitor_payload = monitor_profile_get_payload(start_point);
        monitor_payload->freq = ctx->packet.channel.data_freq[i];
        monitor_payload->flag = REPORT_DATA_FLAG_COMBINE_DATA;
        monitor_payload->pkt_count = 1;
        monitor_payload->rsvd = 0;
        monitor_payload->dat_count = ctx->packet.channel.data_count[i];
        data_payload = (data_payload_t *)((uint8_t *)&monitor_payload->data_payload);
        memcpy(data_payload, &ctx->packet.data[i], sizeof(data_payload_t) - 4);
        if(ctx->packet.channel.is_double_mem[i])
        {
            memcpy((uint8_t *)(ctx->packet.mem_addr[i] + ALIGN_4(ctx->packet.channel.mem_single_len[i])), start_point, ctx->packet.channel.mem_single_len[i]);
        }
    }
    MONITOR_LOG_INFO("channel config, ch_count:%d\r\n", ctx->packet.channel.pkt_count);
    return 0;
}

static int monitor_report_notify_config(void)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    monitor_payload_t *monitor_payload;
    data_payload_t *data_payload;
    uint8_t *start_point;
    uint32_t offset;

    ctx->packet.notify.buff = (uint8_t *)monitor_mem_alloc(MONITOR_PROFILE_MAXSIZE);
    if(ctx->packet.notify.buff == 0)
    {
        MONITOR_LOG_ERR("BAD, notify buff mem alloc fail!\r\n");
        return -1;
    }
    ctx->packet.notify.timing_us = (uint32_t)(1000000 / ctx->packet.notify.freq);
    ctx->packet.notify.offset = MONITOR_REPORT_PACKET_HEAD_OFFSET(ctx->packet.notify.pkt_count);
    ctx->packet.notify.length = 0; //no use
    ctx->packet.notify.stepsize = 0;

    monitor_payload = monitor_profile_get_payload(ctx->packet.notify.buff);
    monitor_payload->freq = ctx->packet.notify.freq;
    monitor_payload->pkt_count = ctx->packet.notify.pkt_count;
    start_point = (uint8_t *)&monitor_payload->data_payload;
    offset = 0;
    for (uint16_t i = 0; i < ctx->packet.notify.pkt_count; i++)
    {
        data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
        memcpy(data_payload, &ctx->packet.data[ctx->packet.channel.pkt_count + i], (sizeof(data_payload_t) - 4));
        ctx->packet.notify.stepsize += monitor_type_convert_byte(data_payload->type);
        offset += sizeof(data_payload_t) - 4;
    }
    if ((ctx->packet.notify.offset + ctx->packet.notify.stepsize + 2) > (MONITOR_PROFILE_MAXSIZE / 2))
    {
        ctx->packet.notify.buf_count = 1;
        ctx->packet.notify.buf_index = 0;
        ctx->packet.notify.dat_count = (MONITOR_PROFILE_MAXSIZE - ctx->packet.notify.offset - 2) / ctx->packet.notify.stepsize;
        monitor_payload->flag = REPORT_DATA_FLAG_COMBINE_DATA;
        monitor_payload->dat_count = ctx->packet.notify.dat_count;
    }
    else
    {
        ctx->packet.notify.buf_count = 2; // double buf mode
        ctx->packet.notify.cur_count = 0;
        ctx->packet.notify.buf_index = 0;
        ctx->packet.notify.dat_count = ((MONITOR_PROFILE_MAXSIZE / ctx->packet.notify.buf_count) - ctx->packet.notify.offset - 2) / ctx->packet.notify.stepsize;
        monitor_payload->flag = REPORT_DATA_FLAG_COMBINE_DATA;
        monitor_payload->dat_count = ctx->packet.notify.dat_count;
        memcpy(ctx->packet.notify.buff + (MONITOR_PROFILE_MAXSIZE / ctx->packet.notify.buf_count), ctx->packet.notify.buff, ctx->packet.notify.offset);
    }
    ctx->packet.notify.last_sample_us = ctx->packet.notify.last_report_us = 0;
    MONITOR_LOG_INFO("notify config, sample count:%d, ch_count:%d, sample_bufcount:%d\r\n", ctx->packet.notify.pkt_count, ctx->packet.channel.pkt_count, ctx->packet.notify.buf_count);
    return 0;
}

static int monitor_report_sample_config(uint32_t data_count, uint8_t mem_count, uint8_t is_loop)
{
    uint32_t memsize;
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    monitor_payload_t *monitor_payload = NULL;
    uint32_t i, j;
    data_payload_t *data_payload = NULL;
    uint8_t *start_point = NULL;

    if(ctx->packet.sample.pkt_count <=0)
    {
        MONITOR_LOG_ERR("BAD, sample count is 0!\r\n");
        return -1;
    }

    ctx->packet.sample.data_count = data_count;
    ctx->packet.sample.mem_cur_index = 0;
    ctx->packet.sample.mem_count = mem_count;
    memsize = 0;

    for (i = 0; i < ctx->packet.sample.pkt_count; i++)
    {
        ctx->packet.sample.mem_payload_offset[i] = memsize;
        ctx->packet.sample.mem_payload_len[i] = MONITOR_PAYLOAD_DATA_OFFSET + (ctx->packet.sample.data_count * monitor_type_convert_byte(ctx->packet.data[ctx->packet.channel.pkt_count + i].type)) + 2; //+2 is end
        memsize += ctx->packet.sample.mem_payload_len[i];
        memsize = ALIGN_4(memsize);
    }
    MONITOR_LOG_INFO("data_count:%d, sample_count:%d, ,memcount:%d, memsize:%d\r\n", data_count, ctx->packet.sample.pkt_count, mem_count, memsize);
    for (i = 0; i < ctx->packet.sample.mem_count; i++)
    {
        ctx->packet.mem_addr[ctx->packet.channel.pkt_count + i] = (uint32_t)monitor_mem_alloc(memsize);
        if (ctx->packet.mem_addr[ctx->packet.channel.pkt_count + i] == 0)
        {
            MONITOR_LOG_ERR("BAD, mem alloc fail!\r\n");
            return -2;
        }
        ctx->packet.mem_len[ctx->packet.channel.pkt_count + i] = memsize;
        memset((void *)ctx->packet.mem_addr[ctx->packet.channel.pkt_count + i], 0, ctx->packet.mem_len[ctx->packet.channel.pkt_count + i]);
        MONITOR_LOG_INFO("sample config, addr:0x%08x, size:%d\r\n", ctx->packet.mem_addr[ctx->packet.channel.pkt_count + i], ctx->packet.mem_len[ctx->packet.channel.pkt_count + i]);
        for (j = 0; j < ctx->packet.sample.pkt_count; j++)
        {
            start_point = (uint8_t *)(ctx->packet.mem_addr[ctx->packet.channel.pkt_count + i] + ctx->packet.sample.mem_payload_offset[j]);
            monitor_profile_set_header(start_point);
            monitor_profile_set_length(start_point, ctx->packet.sample.mem_payload_len[j] - MONITOR_PROFILE_MIN_SIZE);
            monitor_payload = monitor_profile_get_payload(start_point);
            monitor_payload->freq = ctx->packet.sample.data_freq;
            monitor_payload->flag = REPORT_DATA_FLAG_COMBINE_DATA;
            monitor_payload->pkt_count = 1;
            monitor_payload->rsvd = 0;
            monitor_payload->dat_count = ctx->packet.sample.data_count;
            data_payload = (data_payload_t *)((uint8_t *)&monitor_payload->data_payload);
            memcpy(data_payload, &ctx->packet.data[ctx->packet.channel.pkt_count + j], sizeof(data_payload_t) - 4);
        }
    }

    return monitor_timer_sample_data_config(&ctx->packet, monitor_sample_cb, is_loop, true);
}

static int monitor_report_config(uint8_t type)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    init_static_data_list(&ctx->data_list);
    int ret;

    if(ctx->data_type != type)
    {
        MONITOR_LOG_ERR("BAD, report type mismatch,current:%d,config:%d\r\n", ctx->data_type, type);
        return -3;
    }

    if(ctx->packet.channel.pkt_count > 0)
    {
        ret = monitor_report_channel_config();
        if (ret != 0)
        {
            MONITOR_LOG_ERR("BAD! monitor report channel config fail:%d!\r\n", ret);
            return -2;
        }
    }

    if(type == TYPE_NOTIFY)
    {
        MONITOR_LOG_DBG("notify report config\r\n");
        if(ctx->packet.notify.pkt_count <=0)
        {
            return 0;
        }
        ret = monitor_report_notify_config();
        if (ret != 0)
        {
            MONITOR_LOG_ERR("BAD! monitor report notify config fail:%d!\r\n", ret);
            return -1;
        }
    }
    else if (type == TYPE_STREAM)
    {
        MONITOR_LOG_DBG("stream report config\r\n");
        if(ctx->packet.sample.pkt_count <=0)
        {
            return 0;
        }
        ret = monitor_report_sample_config(ctx->packet.sample.data_count, 2, 1);
        if (ret != 0)
        {
            MONITOR_LOG_ERR("BAD! monitor timer sample config fail:%d!\r\n", ret);
            return -1;
        }
    }
    else if (type == TYPE_BUFFER)
    {
        MONITOR_LOG_DBG("buffer report config\r\n");
        if(ctx->packet.sample.pkt_count <=0)
        {
            return 0;
        }
        ret = monitor_report_sample_config(ctx->packet.sample.data_count, 1, 0);
        if (ret != 0)
        {
            MONITOR_LOG_ERR("BAD! monitor timer sample config fail:%d!\r\n", ret);
            return -1;
        }
    }
    else if (type == TYPE_TRIGGER)
    {
        MONITOR_LOG_DBG("trigger report config\r\n");
        ret = monitor_report_sample_config(ctx->packet.sample.data_count / 4, 4, 1);
        if (ret != 0)
        {
            MONITOR_LOG_ERR("BAD! monitor timer sample config fail:%d!\r\n", ret);
            return -1;
        }
    }
    else
    {
        MONITOR_LOG_ERR("BAD, report type no support:%d!\r\n", type);
        return -4;
    }
    return 0;
}

int monitor_notify_start(void)
{
    int ret;
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    ret = monitor_report_config(TYPE_NOTIFY);
    ctx->enable = TYPE_NOTIFY;
    ctx->first_tick = 1;
    return ret;
}

int monitor_notify_stop(void)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    monitor_report_clear();
    ctx->enable = 0;
    return 0;
}

int monitor_stream_start(void)
{
    int ret;
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    ret = monitor_report_config(TYPE_STREAM);
    ctx->enable = TYPE_STREAM;
    ctx->first_tick = 1;
    return ret;
}

int monitor_stream_stop(void)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    monitor_report_clear();
    monitor_timer_sample_destroy();
    ctx->enable = 0;
    return 0;
}

int monitor_buffer_start(void)
{
    int ret;
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    ret = monitor_report_config(TYPE_BUFFER);
    ctx->enable = TYPE_BUFFER;
    ctx->first_tick = 1;
    return ret;
}

int monitor_buffer_stop(void)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    monitor_report_clear();
    monitor_timer_sample_destroy();
    ctx->enable = 0;
    return 0;
}

int monitor_trigger_start(void)
{
    int ret;
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    ret = monitor_report_config(TYPE_TRIGGER);
    ctx->enable = TYPE_TRIGGER;
    return ret;
}

int monitor_trigger_stop(void)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    monitor_report_clear();
    monitor_timer_sample_destroy();
    ctx->enable = 0;
    return 0;
}

static int monitor_report_packet_process(uint16_t cmd, ListDataNode *Node, uint8_t **output)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    montiro_report_data_t *data = &ctx->packet;
    uint16_t i;
    int ch_index = -1;
    uint8_t *start_point = NULL;
    uint8_t *buffer = (uint8_t *)Node->addr;
    if (Node == NULL)
    {
        return -1;
    }

    if (Node->ch_or_sample < 0) // sample
    {
        if (data->sample.pkt_count<= 0)
        {
            data_list_remove_node(&ctx->data_list, Node);
            Node = NULL;
            return 0;
        }

        for (i = 0; i < data->sample.pkt_count; i++)
        {
            start_point = (uint8_t *)(buffer + data->sample.mem_payload_offset[i]);
            monitor_profile_set_header(start_point);
            monitor_profile_set_cmd(start_point, cmd);
            monitor_profile_set_length(start_point, data->sample.mem_payload_len[i] - MONITOR_PROFILE_MIN_SIZE);
            if (Node->result == 0)
            {
                monitor_profile_set_result(start_point, RESULT_SUCCESS);
            }
            else if (Node->result == -1) // lost
            {
                monitor_profile_set_result(start_point, RESULT_LOSS);
            }
            else
            {
                monitor_profile_set_result(start_point, RESULT_RUNNING_ERR);
            }
            monitor_profile_set_index(start_point, ctx->report_index++);
            monitor_profile_set_timing(start_point, Node->tick_us);
            monitor_profile_set_end(start_point);
            monitor_profile_set_crc32(start_point, monitor_profile_crc32(start_point));
        }
        *output = buffer;
        return (int)Node->len;
    }
    else // channel
    {
        for (i = 0; i < data->channel.pkt_count; i++)
        {
            if (MONITOR_PAYLOAD_CH_DATA(data->data[i].addr_or_ch) == (uint32_t)MONITOR_PAYLOAD_CH_DATA(Node->ch_or_sample))
            {
                ch_index = i;
                break;
            }
        }
        if (ch_index < 0)
        {
            data_list_remove_node(&ctx->data_list, Node);
            Node = NULL;
            return 0;
        }
        start_point = (uint8_t *)(buffer);
        monitor_profile_set_header(start_point);
        monitor_profile_set_cmd(start_point, cmd);
        monitor_profile_set_length(start_point, Node->len - MONITOR_PROFILE_MIN_SIZE);
        if (Node->result == 0)
        {
            monitor_profile_set_result(start_point, RESULT_SUCCESS);
        }
        else if (Node->result == -1) // lost
        {
            monitor_profile_set_result(start_point, RESULT_LOSS);
        }
        else
        {
            monitor_profile_set_result(start_point, RESULT_RUNNING_ERR);
        }
        monitor_profile_set_index(start_point, ctx->report_index++);
        monitor_profile_set_timing(start_point, Node->tick_us);
        monitor_profile_set_end(start_point);
        monitor_profile_set_crc32(start_point, monitor_profile_crc32(start_point));
        *output = buffer;
        return (int)Node->len;
    }
    return -1;
}

static int monitor_nofity_report(uint32_t start_addr, uint32_t tick, uint8_t **output)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    ctx->report_index++;
    monitor_profile_set_header((uint8_t *)start_addr);
    monitor_profile_set_result((uint8_t *)start_addr, RESULT_SUCCESS);
    monitor_profile_set_cmd((uint8_t *)start_addr, CMD_NOTIFY);
    monitor_profile_set_index((uint8_t *)start_addr, ctx->report_index);
    monitor_profile_set_timing((uint8_t *)start_addr, tick);
    monitor_profile_set_length((uint8_t *)start_addr, ctx->packet.notify.offset + 2 - MONITOR_PROFILE_MIN_SIZE);
    monitor_profile_set_end((uint8_t *)start_addr);
    monitor_profile_set_crc32((uint8_t *)start_addr, monitor_profile_crc32((uint8_t *)start_addr));
    *output = (uint8_t *)start_addr;
    ctx->packet.notify.cur_count = 0;
    ctx->packet.notify.offset = MONITOR_REPORT_PACKET_HEAD_OFFSET(ctx->packet.notify.pkt_count);
    if (ctx->packet.notify.buf_index >= ctx->packet.notify.buf_count - 1)
    {
        ctx->packet.notify.buf_index = 0;
    }
    else
    {
        ctx->packet.notify.buf_index++;
    }
    return (MONITOR_PROFILE_MIN_SIZE + monitor_profile_get_length((uint8_t *)start_addr));
}

int monitor_notify_ch_handle(uint8_t **output)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    ListDataNode *Node;
    int ret = 0;
    if (ctx->enable == TYPE_NOTIFY)
    {
        Node = data_list_get_valid(&ctx->data_list);
        if (Node != NULL)
        {
            if(Node->ch_or_sample >= 0)
            {
                ret = monitor_report_packet_process(CMD_NOTIFY, Node, output);
            }
            else
            {
                data_list_remove_node(&ctx->data_list, Node);
                Node = NULL;
            }
        }
    }
    return ret;
}

int monitor_notify_handle(uint8_t **output)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    uint32_t start_addr;
    int ret = 0;
    uint32_t current_time = tick_time_us_read32();

    if (ctx->enable == TYPE_NOTIFY)
    {
        if (ctx->first_tick)
        {
            ctx->first_tick = 0;
            tick_time_clear();
            current_time = 0; // tick_time_us_read32();
            ctx->packet.notify.last_sample_us = current_time;
            ctx->packet.notify.last_report_us = current_time;
        }

        uint32_t time_since_last_report = current_time - ctx->packet.notify.last_report_us;
        uint32_t time_since_last_sample = current_time - ctx->packet.notify.last_sample_us;

        start_addr = (uint32_t)(ctx->packet.notify.buff + ((MONITOR_PROFILE_MAXSIZE / ctx->packet.notify.buf_count) * ctx->packet.notify.buf_index));

        if ((ctx->packet.notify.offset > MONITOR_REPORT_PACKET_HEAD_OFFSET(ctx->packet.notify.pkt_count)) &&
            ((time_since_last_sample >= (ctx->packet.notify.last_sample_us * MONITOR_NOTIFY_JITTER_PERIOD_MAX)) ||
             (time_since_last_report >= MONTTOR_NOTIFY_REPORT_INTERVAL_MAX)))
        {
            monitor_payload_t *monitor_payload;
            monitor_payload = monitor_profile_get_payload((uint8_t *)start_addr);
            monitor_payload->dat_count = ctx->packet.notify.cur_count;
            ret = monitor_nofity_report(start_addr, ctx->packet.notify.last_report_us, output);
            ctx->packet.notify.last_report_us = current_time;
        }
        else if (ctx->packet.notify.timing_us > 0 && time_since_last_sample >= ctx->packet.notify.timing_us)
        {
            ctx->packet.notify.last_sample_us = current_time;
            monitor_payload_t *monitor_payload = monitor_profile_get_payload((uint8_t *)start_addr);
            uint8_t *start_point = (uint8_t *)&monitor_payload->data_payload;
            uint32_t offset = 0;

            // sample data
            for (int i = 0; i < monitor_payload->pkt_count; i++)
            {
                data_payload_t *data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
                monitor_get_value(data_payload->type, data_payload->addr_or_ch, (uint8_t *)(start_addr + ctx->packet.notify.offset));
                ctx->packet.notify.offset += monitor_type_convert_byte(data_payload->type);
                offset += sizeof(data_payload_t) - 4;
            }
            ctx->packet.notify.cur_count++;

            // check if need report
            if (ctx->packet.notify.cur_count >= ctx->packet.notify.dat_count)
            {
                monitor_payload->dat_count = ctx->packet.notify.cur_count;
                ret = monitor_nofity_report(start_addr, ctx->packet.notify.last_report_us, output);
                ctx->packet.notify.last_report_us = current_time;
            }
        }
    }
    return ret;
}

int monitor_stream_handle(uint8_t **output)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    ListDataNode *Node;
    if (ctx->enable == TYPE_STREAM)
    {
        Node = data_list_get_valid(&ctx->data_list);
        if (Node != NULL)
        {
            return monitor_report_packet_process(CMD_STREAM, Node, output);
        }
    }
    return 0;
}

int monitor_buffer_handle(uint8_t **output)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    ListDataNode *Node;
    if (ctx->enable == TYPE_BUFFER)
    {
        Node = data_list_get_valid(&ctx->data_list);
        if (Node != NULL)
        {
            return monitor_report_packet_process(CMD_BUFFER, Node, output);
        }
    }
    return 0;
}

void monitor_report_done(uint32_t free_addr)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    data_list_remove_of_addr(&ctx->data_list, free_addr);
    if (ctx->enable == TYPE_STREAM)
    {
        if (ctx->packet.sample.pkt_count > 0 && !monitor_timer_sample_is_running())
        {
            monitor_timer_sample_restart();
        }
    }
}

int monitor_trigger_handle(void)
{
    return 0;
}

#if 0
void *monitor_custom_mem_alloc(uint8_t ch, uint32_t size)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    uint32_t data_addr;
    uint32_t memsize = MONITOR_REPORT_CHANNEL_HEAD_OFFSET + size + 2;
    uint32_t addr = (uint32_t)monitor_mem_alloc(memsize);
    if (addr == 0)
    {
        MONITOR_LOG_ERR("BAD, mem alloc fail!\r\n");
        return NULL;
    }
    data_addr = addr + MONITOR_REPORT_CHANNEL_HEAD_OFFSET;
    if (!custom_list_add(&ctx->custom_list, addr, memsize, data_addr, size, (int)ch))
    {
        MONITOR_LOG_ERR("BAD,custom list add fail!\r\n");
        monitor_mem_free((void *)addr);
        return NULL;
    }
    return (void *)data_addr;
}

void monitor_custom_mem_free(void *addr)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    if (addr == NULL)
        return;
    custom_list_remove_of_dataaddr(&ctx->custom_list, (uint32_t)addr);
}
#endif

int monitor_channel_add_data(uint8_t ch, void *data)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    StaticDataLinkedList *data_list = &ctx->data_list;
    uint32_t data_addr;
    static uint8_t lost_data = 0;
    int ch_index = -1;

    if(MONITOR_PRIVATE_VAR_NAME(report_init) == 0 || ctx->enable == 0 || ctx->packet.channel.pkt_count <=0)
    {
        return 0;
    }

    for(uint16_t i = 0; i < ctx->packet.channel.pkt_count; i++)
    {
        if(MONITOR_PAYLOAD_CH_DATA(ctx->packet.data[i].addr_or_ch) == ch)
        {
            ch_index = i;
            break;
        }
    }
    if(ch_index < 0)
    {
        return -2;
    }

    if (data_list_get_of_addr(data_list, (ctx->packet.mem_addr[ch_index] + (ctx->packet.channel.mem_cur_index[ch_index] * ALIGN_4(ctx->packet.channel.mem_single_len[ch_index])))) != NULL)
    {
        lost_data = 1; // lost data
        return -3;
    }

    data_addr = ctx->packet.mem_addr[ch_index] + (ctx->packet.channel.mem_cur_index[ch_index] * ALIGN_4(ctx->packet.channel.mem_single_len[ch_index]))
               + MONITOR_REPORT_CHANNEL_HEAD_OFFSET + (ctx->packet.channel.data_index[ch_index] * monitor_type_convert_byte(ctx->packet.data[ch_index].type));

    //printf("index:%d, data_addr:%08x\r\n",ctx->packet.channel.data_index[ch_index], data_addr);
    memcpy((void*) data_addr, data, monitor_type_convert_byte(ctx->packet.data[ch_index].type));

    ctx->packet.channel.data_index[ch_index]++;

    if (ctx->packet.channel.data_index[ch_index] >= ctx->packet.channel.data_count[ch_index])
    {
        ctx->packet.channel.data_index[ch_index] = 0;

        if (ctx->first_tick)
        {
            tick_time_clear();
            ctx->first_tick = 0;
        }
        //printf("add addr:%08x, len:%d\r\n", (ctx->packet.mem_addr[ch_index] + (ctx->packet.channel.mem_cur_index[ch_index] * ALIGN_4(ctx->packet.channel.mem_single_len[ch_index]))),
        //          ctx->packet.channel.mem_single_len[ch_index]);
        if (data_list_add(data_list, (ctx->packet.mem_addr[ch_index] + (ctx->packet.channel.mem_cur_index[ch_index] * ALIGN_4(ctx->packet.channel.mem_single_len[ch_index]))),
                        ctx->packet.channel.mem_single_len[ch_index],
                        lost_data, ch, tick_time_us_read32()))
        {
            lost_data = 0;
        }
        else
        {
            lost_data = 1; // lost data
        }
        ctx->packet.channel.mem_cur_index[ch_index] = ctx->packet.channel.mem_cur_index[ch_index] == 0 ? 1 : 0;
    }
    return lost_data == 0 ? 0 : -1;
}

int monitor_channel_report_array(uint8_t ch, void *array, uint32_t count)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    StaticDataLinkedList *data_list = &ctx->data_list;
    static uint8_t lost_data = 0;

    if(MONITOR_PRIVATE_VAR_NAME(report_init) == 0 || ctx->enable == 0 || ctx->packet.channel.pkt_count <=0)
    {
        return 0;
    }

    for(uint16_t i = 0; i < ctx->packet.channel.pkt_count; i++)
    {
        if(MONITOR_PAYLOAD_CH_DATA(ctx->packet.data[i].addr_or_ch) == ch)
        {
            if(count != ctx->packet.channel.data_count[i])
            {
                MONITOR_LOG_ERR("channel report array count mismatch, ch:%d, config:%d, report:%d\r\n", ch, ctx->packet.channel.data_count[i], count);
                return -1;
            }
            if (data_list_get_of_addr(data_list, ctx->packet.mem_addr[i]) != NULL)
            {
                lost_data = 1; // lost data
                return -1;
            }
            memcpy((void*)(ctx->packet.mem_addr[i] + MONITOR_REPORT_CHANNEL_HEAD_OFFSET), array, count * monitor_type_convert_byte(ctx->packet.data[i].type));
            if (ctx->first_tick)
            {
                tick_time_clear();
                ctx->first_tick = 0;
            }
            if (data_list_add(data_list, ctx->packet.mem_addr[i],
                              ctx->packet.channel.mem_single_len[i],
                              lost_data, ch, tick_time_us_read32()))
            {
                lost_data = 0;
                return 0;
            }
            else
            {
                lost_data = 1; // lost data
                return -1;
            }
        }
    }
    return -1;
}

bool monitor_report_addr_is_released(uint32_t addr)
{
    if (addr == 0)
        return false;
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    if (data_list_get_of_addr(&ctx->data_list, addr) == NULL)
        return true;
    return false;
}

bool monitor_report_ch_is_released(uint8_t ch)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    StaticDataLinkedList *data_list = &ctx->data_list;
    int ch_index = -1;

    if(MONITOR_PRIVATE_VAR_NAME(report_init) == 0 || ctx->enable == 0 || ctx->packet.channel.pkt_count <=0)
    {
        return true;
    }

    for(uint16_t i = 0; i < ctx->packet.channel.pkt_count; i++)
    {
        if(MONITOR_PAYLOAD_CH_DATA(ctx->packet.data[i].addr_or_ch) == ch)
        {
            ch_index = i;
            break;
        }
    }
    if(ch_index < 0)
    {
        return true;
    }

    if(ctx->packet.channel.is_double_mem[ch_index])
    {
        if (data_list_get_of_addr(data_list, (ctx->packet.mem_addr[ch_index] + (ctx->packet.channel.mem_cur_index[ch_index] * ALIGN_4(ctx->packet.channel.mem_single_len[ch_index])))) != NULL)
        {
            return false;
        }
    }
    else
    {
        if (data_list_get_of_addr(data_list, ctx->packet.mem_addr[ch_index]) != NULL)
        {
            return false;
        }
    }
    return true;
}

#if 0
bool monitor_custom_addr_is_released(uint32_t addr)
{
    if (addr == 0)
        return false;
    uint32_t temp_addr = addr;
    ListCustomNode *customnode;
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    customnode = custom_list_get_of_dataaddr(&ctx->custom_list, addr);
    if (customnode != NULL)
    {
        temp_addr = customnode->addr;
    }
    if (data_list_get_of_addr(&ctx->data_list, temp_addr) == NULL)
        return true;
    return false;
}
#endif

void monitor_report_init(void)
{
    monitor_report_t *ctx = MONITOR_REPORT_CTX;
    memset(ctx, 0, sizeof(monitor_report_t));
    init_static_data_list(&ctx->data_list);
    monitor_mem_reset();
    MONITOR_PRIVATE_VAR_NAME(report_init) = 1;
}