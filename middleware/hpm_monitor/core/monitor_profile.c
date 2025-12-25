/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "monitor_report.h"
#include "monitor_profile.h"
#include "monitor_log.h"

// pid vid)
__attribute__ ((section(".rodata"), used))
const uint32_t d7d75a2a6eebafc3d2ec0a0be089e05322057c4c308d648b374b362ea7eff5fa = (MONITOR_PID << 16) | MONITOR_VID;

// monitor_version
// 20240703122303
// Jul  3 2024
// Jul 13 2024
// 07:03:00
__attribute__ ((section(".rodata"), used))
const uint64_t be8443a5d67825271edb6f6bee202d6125a38e6aa8f2acbd54652d113af8793b = (((__DATE__[7u] - '0') * 10000000000000ul) +
                                                                                   ((__DATE__[8u] - '0') * 1000000000000ul) +
                                                                                   ((__DATE__[9u] - '0') * 100000000000ul) +
                                                                                   ((__DATE__[10u] - '0') * 10000000000ul) +
                                                                                   (((__DATE__[2u] == 'n' && __DATE__[1u] == 'a')   ? 1u  /*Jan*/
                                                                                     : (__DATE__[2u] == 'b')                        ? 2u  /*Feb*/
                                                                                     : (__DATE__[2u] == 'r' && __DATE__[1u] == 'a') ? 3u  /*Mar*/
                                                                                     : (__DATE__[2u] == 'r')                        ? 4u  /*Apr*/
                                                                                     : (__DATE__[2u] == 'y')                        ? 5u  /*May*/
                                                                                     : (__DATE__[2u] == 'n')                        ? 6u  /*Jun*/
                                                                                     : (__DATE__[2u] == 'l')                        ? 7u  /*Jul*/
                                                                                     : (__DATE__[2u] == 'g')                        ? 8u  /*Aug*/
                                                                                     : (__DATE__[2u] == 'p')                        ? 9u  /*Sep*/
                                                                                     : (__DATE__[2u] == 't')                        ? 10u /*Oct*/
                                                                                     : (__DATE__[2u] == 'v')                        ? 11u /*Nov*/
                                                                                                                                    : 12u /*Dec*/) *
                                                                                    100000000ul) +
                                                                                   ((__DATE__[4u] == ' ' ? 0 : __DATE__[4u] - '0') * 10000000ul) +
                                                                                   ((__DATE__[5u] - '0') * 1000000ul) +
                                                                                   ((__TIME__[0u] - '0') * 100000ul) +
                                                                                   ((__TIME__[1u] - '0') * 10000ul) +
                                                                                   ((__TIME__[3u] - '0') * 1000ul) +
                                                                                   ((__TIME__[4u] - '0') * 100ul) +
                                                                                   ((__TIME__[6u] - '0') * 10ul) +
                                                                                   ((__TIME__[7u] - '0') * 1ul));

monitor_var_info_t MONITOR_PRIVATE_VAR_NAME(monitor_registry)[MONITOR_CHANNEL_MAXCOUNT];
uint32_t MONITOR_PRIVATE_VAR_NAME(monitor_registry_count);

static uint32_t private_crc32(uint8_t *data, uint32_t length)
{
    const uint32_t polynomial = 0xbeaf5aa5;
    uint32_t crc = 0xFFFFFFFF;
    for (uint32_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (int j = 0; j < 8; j++)
        {
            crc = (crc >> 1) ^ (polynomial & ~((crc & 1) - 1));
        }
    }
    return crc;
}

uint16_t monitor_get_value(uint8_t type, uint32_t addr, uint8_t *data)
{
    if (data == NULL)
        return RESULT_PAYLOAD_ERR;
    switch (type)
    {
    case TYPE_BOOL:
    {
        *(bool *)data = *(bool *)addr;
    }
    break;
    case TYPE_UINT8:
    {
        *(uint8_t *)data = *(uint8_t *)addr;
    }
    break;
    case TYPE_INT8:
    {
        *(int8_t *)data = *(int8_t *)addr;
    }
    break;
    case TYPE_UINT16:
    {
        *(uint16_t *)data = *(uint16_t *)addr;
    }
    break;
    case TYPE_INT16:
    {
        *(int16_t *)data = *(int16_t *)addr;
    }
    break;
    case TYPE_UINT32:
    {
        *(uint32_t *)data = *(uint32_t *)addr;
    }
    break;
    case TYPE_INT32:
    {
        *(int32_t *)data = *(int32_t *)addr;
    }
    break;
    case TYPE_UINT64:
    {
        *(uint64_t *)data = *(uint64_t *)addr;
    }
    break;
    case TYPE_INT64:
    {
        *(int64_t *)data = *(int64_t *)addr;
    }
    break;
    case TYPE_FLOAT:
    {
        *(float *)data = *(float *)addr;
    }
    break;
    case TYPE_DOUBLE:
    {
        *(double *)data = *(double *)addr;
    }
    break;
    default:
    {
        return RESULT_PAYLOAD_ERR;
    }
    break;
    }
    return RESULT_SUCCESS;
}

uint16_t monitor_set_value(uint8_t type, uint32_t addr, uint8_t *data)
{
    if (data == NULL)
        return RESULT_PAYLOAD_ERR;
    switch (type)
    {
    case TYPE_BOOL:
    {
        *(bool *)addr = *(bool *)data;
    }
    break;
    case TYPE_UINT8:
    {
        *(uint8_t *)addr = *(uint8_t *)data;
    }
    break;
    case TYPE_INT8:
    {
        *(int8_t *)addr = *(int8_t *)data;
    }
    break;
    case TYPE_UINT16:
    {
        *(uint16_t *)addr = *(uint16_t *)data;
    }
    break;
    case TYPE_INT16:
    {
        *(int16_t *)addr = *(int16_t *)data;
    }
    break;
    case TYPE_UINT32:
    {
        *(uint32_t *)addr = *(uint32_t *)data;
    }
    break;
    case TYPE_INT32:
    {
        *(int32_t *)addr = *(int32_t *)data;
    }
    break;
    case TYPE_UINT64:
    {
        *(uint64_t *)addr = *(uint64_t *)data;
    }
    break;
    case TYPE_INT64:
    {
        *(int64_t *)addr = *(int64_t *)data;
    }
    break;
    case TYPE_FLOAT:
    {
        *(float *)addr = *(float *)data;
    }
    break;
    case TYPE_DOUBLE:
    {
        *(double *)addr = *(double *)data;
    }
    break;
    default:
    {
        return RESULT_PAYLOAD_ERR;
    }
    break;
    }
    return RESULT_SUCCESS;
}

bool payload_is_channel_type(uint32_t value)
{
    return ((value & MONITOR_PAYLOAD_CH_MASK) == MONITOR_PAYLOAD_CH_MASK);
}

int monitor_type_convert_byte(DATA_TYPE type)
{
    int ret;
    switch (type)
    {
    case TYPE_BOOL:
    {
        ret = sizeof(bool);
    }
    break;
    case TYPE_UINT8:
    {
        ret = sizeof(uint8_t);
    }
    break;
    case TYPE_INT8:
    {
        ret = sizeof(int8_t);
    }
    break;
    case TYPE_UINT16:
    {
        ret = sizeof(uint16_t);
    }
    break;
    case TYPE_INT16:
    {
        ret = sizeof(int16_t);
    }
    break;
    case TYPE_UINT32:
    {
        ret = sizeof(uint32_t);
    }
    break;
    case TYPE_INT32:
    {
        ret = sizeof(int32_t);
    }
    break;
    case TYPE_UINT64:
    {
        ret = sizeof(uint64_t);
    }
    break;
    case TYPE_INT64:
    {
        ret = sizeof(int64_t);
    }
    break;
    case TYPE_FLOAT:
    {
        ret = sizeof(float);
    }
    break;
    case TYPE_DOUBLE:
    {
        ret = sizeof(double);
    }
    break;
    default:
    {
        ret = -1;
    }
    break;
    }
    return ret;
}

uint32_t monitor_profile_crc32(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    uint32_t length = monitor_profile->length + MONITOR_PROFILE_MIN_SIZE;
    return private_crc32((uint8_t *)&monitor_profile->index, length - MONITOR_PROFILE_CRC_OFFSET);
}

void monitor_profile_set_header(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    monitor_profile->start = MONITOR_START_CMD;
    monitor_profile->magic = MONITOR_MAGIC_CMD;
    monitor_profile->version = MONITOR_PROFILE_VERSION;
    monitor_profile->extend1 = 0;
}

void monitor_profile_set_end(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    *(uint16_t *)(data + MONITOR_PROFILE_MIN_SIZE + monitor_profile->length - 2) = MONITOR_END_CMD;
}

void monitor_profile_set_crc32(uint8_t *data, uint32_t crc32)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    monitor_profile->crc32 = crc32;
}

uint32_t monitor_profile_get_crc32(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    return monitor_profile->crc32;
}

uint16_t monitor_profile_get_index(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    return monitor_profile->index;
}

void monitor_profile_set_index(uint8_t *data, uint16_t index)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    monitor_profile->index = index;
}

uint16_t monitor_profile_get_cmd(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    return monitor_profile->cmd;
}

void monitor_profile_set_cmd(uint8_t *data, uint16_t cmd)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    monitor_profile->cmd = cmd;
}

uint16_t monitor_profile_get_result(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    return monitor_profile->result;
}

void monitor_profile_set_result(uint8_t *data, uint16_t result)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    monitor_profile->result = result;
}

uint32_t monitor_profile_get_timing(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    return monitor_profile->timing;
}

void monitor_profile_set_timing(uint8_t *data, uint32_t tick_us)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    monitor_profile->timing = tick_us;
}

uint32_t monitor_profile_get_length(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    return monitor_profile->length;
}

void monitor_profile_set_length(uint8_t *data, uint32_t length)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    monitor_profile->length = length;
}

monitor_payload_t *monitor_profile_get_payload(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    return (monitor_payload_t *)&monitor_profile->monitor_payload;
}

monitor_info_t *monitor_profile_get_monitorinfo(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    return (monitor_info_t *)&monitor_profile->monitor_payload;
}

uint16_t monitor_profile_payload_process(uint16_t cmd, uint8_t *data, uint32_t length)
{
    uint32_t i, offset = 0;
    uint16_t result;
    monitor_payload_t *monitor_payload = NULL;
    data_payload_t *data_payload = NULL;
    //data_trigger_t *data_trigger = NULL;
    uint8_t *start_point;

    if (data == NULL || length == 0)
    {
        return RESULT_PAYLOAD_ERR;
    }

    if (monitor_profile_get_length(data) == 0)
    {
        return RESULT_SUCCESS;
    }

    monitor_payload = monitor_profile_get_payload(data);
    if (monitor_payload->pkt_count == 0)
        return RESULT_PAYLOAD_ERR;

    MONITOR_LOG_DBG("cmd:%04x\r\n", cmd);
    MONITOR_LOG_DBG("result:%04x\r\n", monitor_profile_get_result(data));
    MONITOR_LOG_DBG("pkt_count:%d\r\n", monitor_payload->pkt_count);
    MONITOR_LOG_DBG("flag:%d\r\n", monitor_payload->flag);
    MONITOR_LOG_DBG("dat_count:%d\r\n", monitor_payload->dat_count);
    MONITOR_LOG_DBG("freq:%d\r\n", monitor_payload->freq);

    switch(cmd)
    {
        case CMD_GETVALUE:
        {
            start_point = (uint8_t *)&monitor_payload->data_payload;
            offset = 0;
            for (i = 0; i < monitor_payload->pkt_count; i++)
            {
                data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
                result = monitor_get_value(data_payload->type, data_payload->addr_or_ch, (uint8_t *)&data_payload->data);
                if (result != RESULT_SUCCESS)
                    goto profile_payload_process_end;
                offset += monitor_type_convert_byte(data_payload->type) + sizeof(data_payload_t) - 4; //-4 is data point
            }
        }
        break;
        case CMD_SETVALUE:
        {
            start_point = (uint8_t *)&monitor_payload->data_payload;
            offset = 0;
            for (i = 0; i < monitor_payload->pkt_count; i++)
            {
                data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
                result = monitor_set_value(data_payload->type, data_payload->addr_or_ch, (uint8_t *)&data_payload->data);
                if (result != RESULT_SUCCESS)
                    goto profile_payload_process_end;
                result = monitor_get_value(data_payload->type, data_payload->addr_or_ch, (uint8_t *)&data_payload->data);
                if (result != RESULT_SUCCESS)
                    goto profile_payload_process_end;
                offset += monitor_type_convert_byte(data_payload->type) + sizeof(data_payload_t) - 4; //-4 is data point
            }
        }
        break;
        case CMD_NOTIFY:
        {
            monitor_notify_stop();
            if (monitor_notify_set(data, length) != 0)
            {
                MONITOR_LOG_ERR("BAD, notify set fail!\r\n");
                return RESULT_PAYLOAD_ERR;
            }
            return RESULT_SUCCESS;
        }
        break;
        case CMD_STREAM:
        {
            monitor_stream_stop();
            if (monitor_stream_set(data, length) != 0)
            {
                MONITOR_LOG_ERR("BAD, stream set fail!\r\n");
                return RESULT_PAYLOAD_ERR;
            }
            return RESULT_SUCCESS;
        }
        break;
        case CMD_BUFFER:
        {
            monitor_buffer_stop();
            if (monitor_buffer_set(data, length) != 0)
            {
                MONITOR_LOG_ERR("BAD, buffer set fail!\r\n");
                return RESULT_PAYLOAD_ERR;
            }
            return RESULT_SUCCESS;
        }
        break;
        case CMD_TRIGGER:
        {
            //monitor_trigger_stop();
            // data_trigger = (data_trigger_t *)((uint8_t *)start_point + offset);
            // result = monitor_trigger_add(data_trigger);
            // if (result != RESULT_SUCCESS)
            //     goto profile_payload_process_end;
            // offset += monitor_type_convert_byte(data_trigger->type) + sizeof(data_trigger_t) - 4; //-4 is data point
        }
        break;
        default:
            return RESULT_PAYLOAD_ERR;
    }

profile_payload_process_end:
    return result;
}

uint16_t monitor_profile_info_process(uint8_t *data)
{
    monitor_info_t *monitor_info = NULL;

    if (data == NULL)
    {
        return RESULT_PAYLOAD_ERR;
    }

    monitor_info = monitor_profile_get_monitorinfo(data);
    monitor_info->vid = (uint16_t)(d7d75a2a6eebafc3d2ec0a0be089e05322057c4c308d648b374b362ea7eff5fa & 0xFFFF);
    monitor_info->pid = (uint16_t)(d7d75a2a6eebafc3d2ec0a0be089e05322057c4c308d648b374b362ea7eff5fa >> 16);
    monitor_info->monitor_version = be8443a5d67825271edb6f6bee202d6125a38e6aa8f2acbd54652d113af8793b;
    monitor_info->maxsize = MONITOR_PROFILE_MAXSIZE;
    monitor_info->memsize = MONITOR_MEM_SIZE;
    monitor_info->max_count = MONITOR_REPORT_MAXCOUNT;
    monitor_info->threshold = 0;

    return RESULT_SUCCESS;
}

uint16_t monitor_profile_cmd_process(uint16_t cmd, uint16_t parse_result, uint8_t *data, uint32_t length)
{
    switch (cmd)
    {
    case CMD_CONNECT:
    {
        monitor_profile_set_header(data);
        monitor_profile_set_cmd(data, CMD_CONNECT);
        return RESULT_SUCCESS;
    }
    break;
    case CMD_GETINFO:
    {
        monitor_profile_set_header(data);
        monitor_profile_set_cmd(data, CMD_GETINFO);
        monitor_profile_set_length(data, sizeof(monitor_info_t));
        if (parse_result == RESULT_SUCCESS)
            return monitor_profile_info_process(data);
        return parse_result;
    }
    break;
    case CMD_GETVALUE:
    case CMD_SETVALUE:
    case CMD_NOTIFY:
    case CMD_STREAM:
    case CMD_BUFFER:
    case CMD_TRIGGER:
    {
        monitor_profile_set_header(data);
        monitor_profile_set_cmd(data, cmd);
        if (parse_result == RESULT_SUCCESS)
            return monitor_profile_payload_process(cmd, data, length);
        return parse_result;
    }
    break;
    default:
    {
        return RESULT_CMD_ERR;
    }
    break;
    }
    return RESULT_BAD;
}

uint16_t monitor_profile_parse(uint8_t *data, uint32_t length, uint32_t *drop_offset, uint32_t *expect_length)
{
    uint32_t i;
    uint8_t flag = 0;
    monitor_profile_t *monitor_data = NULL;
    if (data == NULL || drop_offset == NULL || expect_length == NULL)
    {
        return RESULT_BAD;
    }
#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_LOG)
    MONITOR_LOG_DBG("len:%d,data:", length);
    for (i = 0; i < length; i++)
    {
        MONITOR_LOG_RAW(" %02x", data[i]);
    }
    MONITOR_LOG_RAW("\r\n");
#endif
    if (length < MONITOR_PROFILE_MIN_SIZE)
    {
        *drop_offset = 0;
        *expect_length = MONITOR_PROFILE_MIN_SIZE - length;
        return RESULT_HEAD_ERR;
    }

    for (i = 0; i < length; i++)
    {
        if (*(uint16_t *)(data + i) == MONITOR_START_CMD && *(uint16_t *)(data + i + 2) == MONITOR_MAGIC_CMD)
        {
            MONITOR_LOG_DBG("head:[%d]:%04x,[%d]:%04x\r\n", i, *(uint16_t *)(data + i), i + 2, *(uint16_t *)(data + i + 2));
            flag = 1;
            break;
        }
    }

    if (flag == 0)
    {
        *drop_offset = length;
        *expect_length = MONITOR_PROFILE_MIN_SIZE;
        MONITOR_LOG_DBG("bad!, no head\r\n");
        return RESULT_HEAD_ERR;
    }

    *drop_offset = i;
    if ((length - i) < (MONITOR_PROFILE_MIN_SIZE - 2))
    {
        *expect_length = MONITOR_PROFILE_MIN_SIZE - (length - i);
        MONITOR_LOG_DBG("no len, (length-i):%d:%d, expect:%d\r\n", (length - i), (MONITOR_PROFILE_MIN_SIZE - 2), *expect_length);
        return RESULT_HEAD_ERR;
    }

    monitor_data = (monitor_profile_t *)(data + i);

    MONITOR_LOG_DBG("len:0x%04x:%d\r\n", monitor_data->length, monitor_data->length);

    if (monitor_data->length > MONITOR_PAYLOAD_MAXSIZE)
    {
        *drop_offset += MONITOR_PROFILE_MIN_SIZE;
        *expect_length = ((length - *drop_offset) > MONITOR_PROFILE_MIN_SIZE) ? 0 : (MONITOR_PROFILE_MIN_SIZE - (length - *drop_offset));
        MONITOR_LOG_ERR("Recv data overflow,len:%d\r\n", monitor_data->length);
        return RESULT_PRFILE_OVERFLOW;
    }

    if (length - *drop_offset < monitor_data->length + MONITOR_PROFILE_MIN_SIZE)
    {
        *expect_length = (monitor_data->length + MONITOR_PROFILE_MIN_SIZE) - (length - *drop_offset);
        return RESULT_HEAD_ERR;
    }

    MONITOR_LOG_DBG("end,recv:0x%04x,rel:0x%04x\r\n", *(uint16_t *)(data + *drop_offset + MONITOR_PROFILE_MIN_SIZE + monitor_data->length - 2), MONITOR_END_CMD);

    if (*(uint16_t *)(data + *drop_offset + MONITOR_PROFILE_MIN_SIZE + monitor_data->length - 2) != MONITOR_END_CMD)
    {
        *drop_offset += MONITOR_PROFILE_MIN_SIZE + monitor_data->length;
        *expect_length = ((length - *drop_offset) > MONITOR_PROFILE_MIN_SIZE) ? 0 : (MONITOR_PROFILE_MIN_SIZE - (length - *drop_offset));
        MONITOR_LOG_ERR("The End no match\r\n");
        return RESULT_HEAD_ERR;
    }

    if (monitor_data->version != MONITOR_PROFILE_VERSION)
    {
        *drop_offset += MONITOR_PROFILE_MIN_SIZE + monitor_data->length;
        *expect_length = ((length - *drop_offset) > MONITOR_PROFILE_MIN_SIZE) ? 0 : (MONITOR_PROFILE_MIN_SIZE - (length - *drop_offset));
        MONITOR_LOG_ERR("The protocol version does not match\r\n");
        return RESULT_VER_ERR;
    }

    if (monitor_data->crc32 != monitor_profile_crc32(data))
    {
        *drop_offset += MONITOR_PROFILE_MIN_SIZE + monitor_data->length;
        *expect_length = ((length - *drop_offset) > MONITOR_PROFILE_MIN_SIZE) ? 0 : (MONITOR_PROFILE_MIN_SIZE - (length - *drop_offset));
        MONITOR_LOG_ERR("CRC32 no match!\r\n");
        return RESULT_CRC_ERR;
    }

    if ((monitor_data->cmd & MONITOR_CMD_HOST_MASK) == 0)
    {
        *drop_offset += MONITOR_PROFILE_MIN_SIZE + monitor_data->length;
        *expect_length = ((length - *drop_offset) > MONITOR_PROFILE_MIN_SIZE) ? 0 : (MONITOR_PROFILE_MIN_SIZE - (length - *drop_offset));
        MONITOR_LOG_ERR("Cmd is error!,cmd:0x%04X\r\n", monitor_data->cmd);
        return RESULT_CMD_ERR;
    }

    *drop_offset += MONITOR_PROFILE_MIN_SIZE + monitor_data->length;
    *expect_length = ((length - *drop_offset) > MONITOR_PROFILE_MIN_SIZE) ? 0 : (MONITOR_PROFILE_MIN_SIZE - (length - *drop_offset));

    return RESULT_SUCCESS;
}

const monitor_var_info_t *monitor_ch_info_find_by_name(const char *name)
{
    for (uint32_t i = 0; i < MONITOR_PRIVATE_VAR_NAME(monitor_registry_count); i++)
    {
        if (strcmp(MONITOR_PRIVATE_VAR_NAME(monitor_registry)[i].name_t, name) == 0)
        {
            return &MONITOR_PRIVATE_VAR_NAME(monitor_registry)[i];
        }
    }
    return NULL;
}

const monitor_var_info_t *monitor_ch_info_find_by_channel(uint8_t channel)
{
    for (uint32_t i = 0; i < MONITOR_PRIVATE_VAR_NAME(monitor_registry_count); i++)
    {
        if (MONITOR_PRIVATE_VAR_NAME(monitor_registry)[i].channel_t == channel)
        {
            return &MONITOR_PRIVATE_VAR_NAME(monitor_registry)[i];
        }
    }
    return NULL;
}

uint32_t monitor_ch_info_get_registry_count(void)
{
    return MONITOR_PRIVATE_VAR_NAME(monitor_registry_count);
}

void monitor_ch_info_print_registry(void)
{
    printf("Monitor Registry (%zu/%d entries):\n", MONITOR_PRIVATE_VAR_NAME(monitor_registry_count), MONITOR_CHANNEL_MAXCOUNT);
    printf("%20s %8s %8s %8s %12s %18s\n",
           "Name", "Channel", "Type", "Count", "Frequency", "Encoded Value");
    printf("--------------------------------------------------------------------------------\n");

    for (size_t i = 0; i < MONITOR_PRIVATE_VAR_NAME(monitor_registry_count); i++)
    {
        const monitor_var_info_t *entry = &MONITOR_PRIVATE_VAR_NAME(monitor_registry)[i];
        printf("%20s %8d 0x%06X %8d %12d 0x%016lX\n",
               entry->name_t,
               entry->channel_t,
               entry->type_index_t,
               entry->count_t,
               entry->freq_t,
               entry->encoded_value);
    }
    if (MONITOR_PRIVATE_VAR_NAME(monitor_registry_count) == MONITOR_CHANNEL_MAXCOUNT)
    {
        printf("WARNING: Registry is full!\n");
    }
}