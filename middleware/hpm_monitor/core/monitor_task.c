/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "monitor_ticktime.h"
#include "monitor_profile.h"
#include "monitor_task.h"
#include "monitor_log.h"

#define MONITOR_TASK_CTX (&monitor_task)

typedef struct
{
    monitor_channel_cb output;
    uint16_t ack_cmd;
    uint16_t ack_parse_result;
    uint16_t ack_index;
    uint16_t send_length;
    MONITOR_ATTR_ALIGN(4) uint8_t send_buff[MONITOR_PROFILE_MAXSIZE + 1];
    uint8_t notify_enable;
    uint32_t notify_timing;
    uint16_t notify_index;
    uint64_t notify_last_us;
    uint16_t notify_length;
    MONITOR_ATTR_ALIGN(4) uint8_t notify_buff[MONITOR_PROFILE_MAXSIZE + 1];
} monitor_task_t;

MONITOR_NOCACHE_RAM_SECTION MONITOR_ATTR_ALIGN(4) monitor_task_t monitor_task = {0};

void monitor_task_init(void)
{
    monitor_task_t *ctx = MONITOR_TASK_CTX;
    memset(ctx, 0, sizeof(monitor_task_t));

    MONITOR_LOG_INFO("profile_pidvid:0x%08x\r\n", d7d75a2a6eebafc3d2ec0a0be089e05322057c4c308d648b374b362ea7eff5fa);
    MONITOR_LOG_INFO("profile_version:%lld\r\n", be8443a5d67825271edb6f6bee202d6125a38e6aa8f2acbd54652d113af8793b);
    MONITOR_LOG_INFO("VERSION:%s\r\n", MONITOR_VERSION_STR);
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
        result = monitor_profile_cmd_process(ctx->ack_cmd, ctx->ack_parse_result, ctx->send_buff, ctx->send_length);
        ctx->send_length = monitor_profile_get_length(ctx->send_buff) + MONITOR_PROFILE_MIN_SIZE;
        monitor_profile_set_result(ctx->send_buff, \
                ((ctx->ack_parse_result != RESULT_SUCCESS ? ctx->ack_parse_result : result) | MONITOR_RESULT_ACK_MASK));
        monitor_profile_set_index(ctx->send_buff, ctx->ack_index);
        monitor_profile_set_tickus(ctx->send_buff, clock_get_now_tick_us());
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
    ctx->ack_parse_result = result;
    ctx->send_length = MONITOR_PROFILE_MIN_SIZE;
    if (result < RESULT_SUCCESS)
    {
        memcpy(ctx->send_buff, data, ctx->send_length);
        monitor_ack_handle();
        return 1;
    }
    if (ctx->ack_cmd == CMD_NOTIFY)
    {
        ctx->notify_enable = 0;
        if (monitor_profile_get_result(data) == RESULT_NOTIFY_ON)
        {
            ctx->notify_timing = monitor_profile_get_timing(data);
            ctx->notify_index = monitor_profile_get_index(data);
            ctx->notify_length = monitor_profile_get_length(data) + MONITOR_PROFILE_MIN_SIZE;
            memcpy(ctx->notify_buff, data, ctx->notify_length);

            ctx->notify_last_us = clock_get_now_tick_us();
            ctx->notify_enable = 1;
        }
        else
        {
            ctx->notify_enable = 0;
        }
    }
    ctx->send_length = monitor_profile_get_length(data) + MONITOR_PROFILE_MIN_SIZE;
    memcpy(ctx->send_buff, data, ctx->send_length);
    monitor_ack_handle();
    return 0;
}

void monitor_task_handle(void)
{
    monitor_task_t *ctx = MONITOR_TASK_CTX;
    uint16_t result;

    if (!monitor_priority_send())
        return;

    if (ctx->notify_enable == 1)
    {
        if (ctx->notify_timing == 0 ||
            clock_get_now_tick_us() - ctx->notify_last_us >= ctx->notify_timing)
        {
            ctx->notify_last_us = clock_get_now_tick_us();
            ctx->notify_index++;
            result = monitor_profile_cmd_process(CMD_NOTIFY, RESULT_SUCCESS, ctx->notify_buff, ctx->notify_length);
            monitor_profile_set_result(ctx->notify_buff, (result & ~MONITOR_RESULT_ACK_MASK));
            monitor_profile_set_index(ctx->notify_buff, ctx->notify_index);
            // monitor_profile_set_timing(ctx->notify_buff[pd], ctx->notify_timing);
            monitor_profile_set_tickus(ctx->notify_buff, clock_get_now_tick_us());
            monitor_profile_set_end(ctx->notify_buff);
            monitor_profile_set_crc32(ctx->notify_buff, monitor_profile_crc32(ctx->notify_buff));
            if (ctx->output != NULL)
            {
                ctx->output(ctx->notify_buff, ctx->notify_length);
            }
        }
    }
}