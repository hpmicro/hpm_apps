#include "hpm_common.h"
#include "hpm_ticktime.h"
#include "debug_profile.h"
#include "debug_task.h"
#include "debug_config.h"

#define DEBUG_TASK_CTX (&debug_task)

typedef struct
{
    uint8_t init;
    debug_channel_cb output;
    uint16_t ack_cmd;
    uint16_t ack_parse_result;
    uint16_t ack_index;
    uint16_t send_length;
    uint8_t send_buff[DEBUG_PROFILE_MAXSIZE + 1];
    uint8_t notify_enable;
    uint32_t notify_timing;
    uint16_t notify_index;
    uint64_t notify_last_us;
    uint16_t notify_length;
    uint8_t notify_buff[2][DEBUG_PROFILE_MAXSIZE + 1]; // pingpang buffer
    uint8_t notify_pb;
} debug_task_t;

ATTR_RAMFUNC_WITH_ALIGNMENT(8)
debug_task_t debug_task = {0};

void debug_init(void)
{
    debug_task_t *ctx = DEBUG_TASK_CTX;
    memset(ctx, 0, sizeof(debug_task_t));
#ifdef DEBUG_LOG_ENABLE
    printf("debug_pidvid:0x%08x\r\n", d7d75a2a6eebafc3d2ec0a0be089e05322057c4c308d648b374b362ea7eff5fa);
    printf("debug_version:%lld\r\n", be8443a5d67825271edb6f6bee202d6125a38e6aa8f2acbd54652d113af8793b);
#endif
}

void debug_register(debug_channel_cb output_cb)
{
    debug_task_t *ctx = DEBUG_TASK_CTX;
    ctx->output = output_cb;
    ctx->init = 1;
}

int debug_src_process(uint8_t *data, uint32_t length)
{
    debug_task_t *ctx = DEBUG_TASK_CTX;
    debug_payload_t *payload = NULL;
    int result;
    if (data == NULL || length == 0)
        return -1;
    if (ctx->init == 0)
        return -2;
    result = debug_profile_parse(data, length, &payload);
    if (result == RESULT_BAD)
        return -3;
    ctx->ack_cmd = debug_profile_get_cmd(data) & (~DEBUG_CMD_HOST_MASK);
    ctx->ack_index = debug_profile_get_index(data);
    ctx->ack_parse_result = result;
    ctx->send_length = DEBUG_PROFILE_MIN_SIZE;
    if (result < RESULT_SUCCESS)
    {
        return 1;
    }
    if (ctx->ack_cmd == CMD_NOTIFY)
    {
        ctx->notify_enable = 0;
        if (debug_profile_get_result(data) == RESULT_NOTIFY_ON)
        {
            ctx->notify_timing = debug_profile_get_timing(data);
            ctx->notify_index = debug_profile_get_index(data);
            ctx->notify_length = debug_profile_get_length(data) + DEBUG_PROFILE_MIN_SIZE;
            memcpy(ctx->notify_buff[0], data, ctx->notify_length);
            memcpy(ctx->notify_buff[1], data, ctx->notify_length);

            ctx->notify_last_us = clock_get_now_tick_us();
            ctx->notify_enable = 1;
        }
        else
        {
            ctx->notify_enable = 0;
        }
    }
    ctx->send_length = debug_profile_get_length(data) + DEBUG_PROFILE_MIN_SIZE;
    memcpy(ctx->send_buff, data, ctx->send_length);
    return 0;
}

uint32_t test_data;
double test_double;
float test_float = 23.2324;
void debug_handle(void)
{
    debug_task_t *ctx = DEBUG_TASK_CTX;
    uint16_t result;
    uint8_t pd;
    if (ctx->init == 0)
        return;
    if (ctx->ack_cmd > CMD_INVALID)
    {
        result = debug_profile_cmd_process(ctx->ack_cmd, ctx->ack_parse_result, ctx->send_buff, ctx->send_length);
        ctx->send_length = debug_profile_get_length(ctx->send_buff) + DEBUG_PROFILE_MIN_SIZE;
        debug_profile_set_result(ctx->send_buff, \
                ((ctx->ack_parse_result != RESULT_SUCCESS ? ctx->ack_parse_result : result) | DEBUG_RESULT_ACK_MASK));
        debug_profile_set_index(ctx->send_buff, ctx->ack_index);
        debug_profile_set_tickus(ctx->send_buff, clock_get_now_tick_us());
        debug_profile_set_end(ctx->send_buff);
        debug_profile_set_crc32(ctx->send_buff, debug_profile_crc32(ctx->send_buff));
        if (ctx->output != NULL)
        {
#ifdef DEBUG_LOG_ENABLE
            printf("test_data:%ld\r\n", test_data++);
            printf("test_double:%f\r\n", test_double);
            printf("test_float:%f\r\n", test_float);
#endif
            test_double += 1.2343;
            test_float += 1.0032;
            ctx->output(ctx->send_buff, ctx->send_length);
        }
        ctx->ack_cmd = CMD_INVALID;
    }

    if (ctx->notify_enable == 1)
    {
        if (ctx->notify_timing == 0 ||
            clock_get_now_tick_us() - ctx->notify_last_us >= ctx->notify_timing)
        {
            ctx->notify_last_us = clock_get_now_tick_us();
            ctx->notify_index++;
            pd = ctx->notify_pb;
            ctx->notify_pb = (pd == 0) ? 1 : 0;
            result = debug_profile_cmd_process(CMD_NOTIFY, RESULT_SUCCESS, ctx->notify_buff[pd], ctx->notify_length);
            debug_profile_set_result(ctx->notify_buff[pd], (result & ~DEBUG_RESULT_ACK_MASK));
            debug_profile_set_index(ctx->notify_buff[pd], ctx->notify_index);
            // debug_profile_set_timing(ctx->notify_buff[pd], ctx->notify_timing);
            debug_profile_set_tickus(ctx->notify_buff[pd], clock_get_now_tick_us());
            debug_profile_set_end(ctx->notify_buff[pd]);
            debug_profile_set_crc32(ctx->notify_buff[pd], debug_profile_crc32(ctx->notify_buff[pd]));
            if (ctx->output != NULL)
            {
#ifdef DEBUG_LOG_ENABLE
                // printf("nt test_data:%ld\r\n", test_data++);
                // printf("nt test_double:%f\r\n", test_double);
                // printf("nt test_float:%f\r\n", test_float);
#endif
                test_double += 1.2343;
                // test_float += 1.0032;
                srand(ctx->notify_last_us);
                test_float = rand() / 1000;
                ctx->output(ctx->notify_buff[pd], ctx->notify_length);
            }
        }
    }
}