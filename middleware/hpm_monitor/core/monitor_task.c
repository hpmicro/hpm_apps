/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "monitor_ticktime.h"
#include "monitor_profile.h"
#include "monitor_timer.h"
#include "monitor_report.h"
#include "monitor_task.h"
#include "monitor_log.h"

#define MONITOR_TASK_CTX (&MONITOR_PRIVATE_VAR_NAME(monitor_task))

#ifndef BIT
#define BIT(n)                                      (1UL << (n))
#endif

typedef struct
{
    monitor_channel_cb output;
    uint16_t ack_cmd;
    uint16_t ack_result;
    uint16_t ack_index;
    uint16_t send_length;
    MONITOR_ATTR_ALIGN(4) uint8_t send_buff[MONITOR_PROFILE_MAXSIZE + 1];
} monitor_task_t;

MONITOR_NOCACHE_RAM_SECTION MONITOR_ATTR_ALIGN(4) monitor_task_t MONITOR_PRIVATE_VAR_NAME(monitor_task) = {0};

void monitor_task_init(void)
{
    monitor_task_t *ctx = MONITOR_TASK_CTX;
    memset(ctx, 0, sizeof(monitor_task_t));

    MONITOR_LOG_INFO("profile_pidvid:0x%08x\r\n", d7d75a2a6eebafc3d2ec0a0be089e05322057c4c308d648b374b362ea7eff5fa);
    MONITOR_LOG_INFO("profile_version:%lld\r\n", be8443a5d67825271edb6f6bee202d6125a38e6aa8f2acbd54652d113af8793b);
    MONITOR_LOG_INFO("VERSION:%s\r\n", MONITOR_VERSION_STR);

#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_INFO)
    monitor_ch_info_print_registry();
#endif
    tick_time_init();
    monitor_report_init();
    monitor_timer_init();
}

void monitor_task_output_register(monitor_channel_cb output_cb)
{
    monitor_task_t *ctx = MONITOR_TASK_CTX;
    ctx->output = output_cb;
}

bool monitor_priority_send(void)
{
    monitor_task_t *ctx = MONITOR_TASK_CTX;
    if (ctx->output != NULL && ctx->send_length > 0)
    {
        if(ctx->output(ctx->send_buff, ctx->send_length) < 0)
        {
            return false;
        }
        ctx->send_length = 0;
    }
    return true;
}

void monitor_ack_handle(void)
{
    monitor_task_t *ctx = MONITOR_TASK_CTX;
    uint16_t result;
    if (ctx->ack_cmd > CMD_INVALID)
    {
        result = monitor_profile_cmd_process(ctx->ack_cmd, ctx->ack_result, ctx->send_buff, ctx->send_length);
        ctx->send_length = monitor_profile_get_length(ctx->send_buff) + MONITOR_PROFILE_MIN_SIZE;
        monitor_profile_set_result(ctx->send_buff, \
                ((ctx->ack_result != RESULT_SUCCESS ? ctx->ack_result : result) | MONITOR_RESULT_ACK_MASK));
        monitor_profile_set_index(ctx->send_buff, ctx->ack_index);
        monitor_profile_set_timing(ctx->send_buff, tick_time_us_read32());
        monitor_profile_set_end(ctx->send_buff);
        monitor_profile_set_crc32(ctx->send_buff, monitor_profile_crc32(ctx->send_buff));
        monitor_priority_send();
        ctx->ack_cmd = CMD_INVALID;
    }

}

int monitor_task_input_process(uint8_t *data, uint32_t length, uint32_t *drop_offset, uint32_t *expect_length)
{
    monitor_task_t *ctx = MONITOR_TASK_CTX;
    int result;
    if (data == NULL || length == 0)
        return -1;
    result = monitor_profile_parse(data, length, drop_offset, expect_length);
    if (result <= RESULT_HEAD_ERR)
        return -3;
    ctx->ack_cmd = monitor_profile_get_cmd(data) & (~MONITOR_CMD_HOST_MASK);
    ctx->ack_index = monitor_profile_get_index(data);
    ctx->ack_result = result;
    ctx->send_length = MONITOR_PROFILE_MIN_SIZE;
    if (result < RESULT_SUCCESS)
    {
        memcpy(ctx->send_buff, data, ctx->send_length);
        result = monitor_profile_cmd_process(ctx->ack_cmd, ctx->ack_result, ctx->send_buff, ctx->send_length);
        ctx->send_length = monitor_profile_get_length(ctx->send_buff) + MONITOR_PROFILE_MIN_SIZE;
        monitor_profile_set_result(ctx->send_buff, \
                ((ctx->ack_result != RESULT_SUCCESS ? ctx->ack_result : result) | MONITOR_RESULT_ACK_MASK));
        monitor_profile_set_index(ctx->send_buff, ctx->ack_index);
        monitor_profile_set_timing(ctx->send_buff, tick_time_us_read32());
        monitor_profile_set_end(ctx->send_buff);
        monitor_profile_set_crc32(ctx->send_buff, monitor_profile_crc32(ctx->send_buff));
        monitor_priority_send();
        ctx->ack_cmd = CMD_INVALID;
        return 1;
    }

    ctx->send_length = monitor_profile_get_length(data) + MONITOR_PROFILE_MIN_SIZE;
    memcpy(ctx->send_buff, data, ctx->send_length);
    result = monitor_profile_cmd_process(ctx->ack_cmd, ctx->ack_result, ctx->send_buff, ctx->send_length);
    if (ctx->ack_cmd >= CMD_NOTIFY)
    {
        if (monitor_profile_get_result(data) == RESULT_ON)
        {
            switch(ctx->ack_cmd)
            {
                case CMD_NOTIFY:
                {
                    if(monitor_notify_start() != 0)
                    {
                        monitor_notify_stop();
                        ctx->ack_result = RESULT_CONFIG_ERR;
                    }
                    else
                    {
                        ctx->ack_result = RESULT_SUCCESS;
                    }
                }
                break;
                case CMD_STREAM:
                {
                    if(monitor_stream_start() != 0)
                    {
                        monitor_stream_stop();
                        ctx->ack_result = RESULT_CONFIG_ERR;
                    }
                    else
                    {
                        ctx->ack_result = RESULT_SUCCESS;
                    }
                }
                break;
                case CMD_BUFFER:
                {
                    if(monitor_buffer_start() != 0)
                    {
                        monitor_buffer_stop();
                        ctx->ack_result = RESULT_CONFIG_ERR;
                    }
                    else
                    {
                        ctx->ack_result = RESULT_SUCCESS;
                    }
                }
                break;
                case CMD_TRIGGER:
                {
                    if(monitor_trigger_start() != 0)
                    {
                        monitor_trigger_stop();
                        ctx->ack_result = RESULT_CONFIG_ERR;
                    }
                    else
                    {
                        ctx->ack_result = RESULT_SUCCESS;
                    }
                }
                break;
            }
        }
        else if(monitor_profile_get_result(data) == RESULT_OFF)
        {
            switch(ctx->ack_cmd)
            {
                case CMD_NOTIFY:
                {
                    monitor_notify_stop();
                }
                break;
                case CMD_STREAM:
                {
                    monitor_stream_stop();
                }
                break;
                case CMD_BUFFER:
                {
                    monitor_buffer_stop();
                }
                break;
                case CMD_TRIGGER:
                {
                    monitor_trigger_stop();
                }
                break;
            }
        }
    }

    MONITOR_LOG_DBG("ack, cmd:%04X, result:%04X\r\n", ctx->ack_cmd, ctx->ack_result);
    ctx->send_length = monitor_profile_get_length(ctx->send_buff) + MONITOR_PROFILE_MIN_SIZE;
    monitor_profile_set_result(ctx->send_buff, \
            ((ctx->ack_result != RESULT_SUCCESS ? ctx->ack_result : result) | MONITOR_RESULT_ACK_MASK));
    monitor_profile_set_index(ctx->send_buff, ctx->ack_index);
    monitor_profile_set_timing(ctx->send_buff, tick_time_us_read32());
    monitor_profile_set_end(ctx->send_buff);
    monitor_profile_set_crc32(ctx->send_buff, monitor_profile_crc32(ctx->send_buff));
    monitor_priority_send();
    ctx->ack_cmd = CMD_INVALID;

    return 0;
}

void monitor_report_notify_handle(void)
{
    monitor_task_t *ctx = MONITOR_TASK_CTX;
    static uint32_t old_addr = 0, old_len = 0;
    static uint32_t free_addr = 0;
    uint8_t *output;
    uint32_t len;

    if(monitor_send_is_idle())
    {
        if(free_addr != 0)
        {
            monitor_report_done(free_addr);
            free_addr = 0;
        }
        len = monitor_notify_ch_handle(&output);
        if(len > 0)
        {
            ctx->output(output, len);
            free_addr = (uint32_t)output;
        }
        else if(old_len > 0)
        {
            ctx->output((uint8_t*)old_addr, old_len);
            old_len = 0;
        }
    }

    len = monitor_notify_handle(&output);
    if(len > 0)
    {
        if(monitor_send_is_idle())
        {
            ctx->output(output, len);
        }
        else
        {
            old_addr = (uint32_t)output;
            old_len = len;
        }
    }
}

void monitor_report_stream_handle(void)
{
    monitor_task_t *ctx = MONITOR_TASK_CTX;
    static uint32_t free_addr = 0;
    uint8_t *output;
    uint32_t len;
    if(monitor_send_is_idle())
    {
        if(free_addr != 0)
        {
            monitor_report_done(free_addr);
            free_addr = 0;
        }
        len = monitor_stream_handle(&output);
        if(len > 0)
        {
            ctx->output(output, len);
            free_addr = (uint32_t)output;
        }
    }
}

void monitor_report_buffer_handle(void)
{
    monitor_task_t *ctx = MONITOR_TASK_CTX;
    static uint32_t free_addr = 0;
    uint8_t *output;
    uint32_t len;
    if(monitor_send_is_idle())
    {
        if(free_addr != 0)
        {
            monitor_report_done(free_addr);
            free_addr = 0;
        }
        len = monitor_buffer_handle(&output);
        if(len > 0)
        {
            ctx->output(output, len);
            free_addr = (uint32_t)output;
        }
    }
}

void monitor_report_trigger_handle(void)
{
    monitor_trigger_handle();
}

void monitor_report_task_handle(void)
{
    monitor_report_notify_handle();
    monitor_report_trigger_handle();
    monitor_report_buffer_handle();
    monitor_report_stream_handle();
}

void monitor_task_handle(void)
{
    if (!monitor_priority_send())
        return;

    monitor_report_task_handle();
}