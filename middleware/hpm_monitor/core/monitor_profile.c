/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "monitor_profile.h"
#include "monitor_log.h"

// pid vid
__attribute__((section(".rodata")))
const uint32_t d7d75a2a6eebafc3d2ec0a0be089e05322057c4c308d648b374b362ea7eff5fa = (MONITOR_PID << 16) | MONITOR_VID;

// monitor_version
// 20240703122303
// Jul  3 2024
// Jul 13 2024
// 07:03:00
__attribute__((section(".rodata")))
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

static uint16_t monitor_get_value(uint8_t type, uint32_t addr, uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0)
        return RESULT_PAYLOAD_ERR;

    switch (type)
    {
    case TYPE_UINT8:
    {
        if (length != sizeof(uint8_t))
            return RESULT_DATA_LEN_ERR;
        memcpy(data, (void *)addr, sizeof(uint8_t));
    }
    break;
    case TYPE_INT8:
    {
        if (length != sizeof(int8_t))
            return RESULT_DATA_LEN_ERR;
        memcpy(data, (void *)addr, sizeof(int8_t));
    }
    break;
    case TYPE_UINT16:
    {
        if (length != sizeof(uint16_t))
            return RESULT_DATA_LEN_ERR;
        memcpy(data, (void *)addr, sizeof(uint16_t));
    }
    break;
    case TYPE_INT16:
    {
        if (length != sizeof(int16_t))
            return RESULT_DATA_LEN_ERR;
        memcpy(data, (void *)addr, sizeof(int16_t));
    }
    break;
    case TYPE_UINT32:
    {
        if (length != sizeof(uint32_t))
            return RESULT_DATA_LEN_ERR;
        memcpy(data, (void *)addr, sizeof(uint32_t));
    }
    break;
    case TYPE_INT32:
    {
        if (length != sizeof(int32_t))
            return RESULT_DATA_LEN_ERR;
        memcpy(data, (void *)addr, sizeof(int32_t));
    }
    break;
    case TYPE_UINT64:
    {
        if (length != sizeof(uint64_t))
            return RESULT_DATA_LEN_ERR;
        memcpy(data, (void *)addr, sizeof(uint64_t));
    }
    break;
    case TYPE_INT64:
    {
        if (length != sizeof(int64_t))
            return RESULT_DATA_LEN_ERR;
        memcpy(data, (void *)addr, sizeof(int64_t));
    }
    break;
    case TYPE_ARRAY_UINT8:
    case TYPE_ARRAY_INT8:
    case TYPE_ARRAY_UINT16:
    case TYPE_ARRAY_INT16:
    case TYPE_ARRAY_UINT32:
    case TYPE_ARRAY_INT32:
    case TYPE_ARRAY_UINT64:
    case TYPE_ARRAY_INT64:
    case TYPE_ARRAY_STRUCT:
    {
        memcpy(data, (void *)addr, length);
    }
    break;
    case TYPE_FLOAT:
    {
        if (length != sizeof(float))
            return RESULT_DATA_LEN_ERR;
        memcpy(data, (void *)addr, sizeof(float));
    }
    break;
    case TYPE_DOUBLE:
    {
        if (length != sizeof(double))
            return RESULT_DATA_LEN_ERR;
        memcpy(data, (void *)addr, sizeof(double));
    }
    break;
    case TYPE_ARRAY_FLOAT:
    case TYPE_ARRAY_DOUBLE:
    {
        memcpy(data, (void *)addr, length);
    }
    break;
    default:
    {
        return RESULT_DATA_TYPE_ERR;
    }
    }
    return RESULT_SUCCESS;
}

static uint16_t monitor_set_value(uint8_t type, uint32_t addr, uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0)
        return RESULT_PAYLOAD_ERR;

    switch (type)
    {
    case TYPE_UINT8:
    {
        if (length != sizeof(uint8_t))
            return RESULT_DATA_LEN_ERR;
        memcpy((void *)addr, data, sizeof(uint8_t));
    }
    break;
    case TYPE_INT8:
    {
        if (length != sizeof(int8_t))
            return RESULT_DATA_LEN_ERR;
        memcpy((void *)addr, data, sizeof(int8_t));
    }
    break;
    case TYPE_UINT16:
    {
        if (length != sizeof(uint16_t))
            return RESULT_DATA_LEN_ERR;
        memcpy((void *)addr, data, sizeof(uint16_t));
    }
    break;
    case TYPE_INT16:
    {
        if (length != sizeof(int16_t))
            return RESULT_DATA_LEN_ERR;
        memcpy((void *)addr, data, sizeof(int16_t));
    }
    break;
    case TYPE_UINT32:
    {
        if (length != sizeof(uint32_t))
            return RESULT_DATA_LEN_ERR;
        memcpy((void *)addr, data, sizeof(uint32_t));
    }
    break;
    case TYPE_INT32:
    {
        if (length != sizeof(int32_t))
            return RESULT_DATA_LEN_ERR;
        memcpy((void *)addr, data, sizeof(int32_t));
    }
    break;
    case TYPE_UINT64:
    {
        if (length != sizeof(uint64_t))
            return RESULT_DATA_LEN_ERR;
        memcpy((void *)addr, data, sizeof(uint64_t));
    }
    break;
    case TYPE_INT64:
    {
        if (length != sizeof(int64_t))
            return RESULT_DATA_LEN_ERR;
        memcpy((void *)addr, data, sizeof(int64_t));
    }
    break;
    case TYPE_ARRAY_UINT8:
    case TYPE_ARRAY_INT8:
    case TYPE_ARRAY_UINT16:
    case TYPE_ARRAY_INT16:
    case TYPE_ARRAY_UINT32:
    case TYPE_ARRAY_INT32:
    case TYPE_ARRAY_UINT64:
    case TYPE_ARRAY_INT64:
    case TYPE_ARRAY_STRUCT:
    {
        memcpy((void *)addr, data, length);
    }
    break;
    case TYPE_FLOAT:
    {
        if (length != sizeof(float))
            return RESULT_DATA_LEN_ERR;
        memcpy((void *)addr, data, sizeof(float));
    }
    break;
    case TYPE_DOUBLE:
    {
        if (length != sizeof(double))
            return RESULT_DATA_LEN_ERR;
        memcpy((void *)addr, data, sizeof(double));
    }
    break;
    case TYPE_ARRAY_FLOAT:
    case TYPE_ARRAY_DOUBLE:
    {
        memcpy((void *)addr, data, length);
    }
    break;
    default:
    {
        return RESULT_DATA_TYPE_ERR;
    }
    }
    return RESULT_SUCCESS;
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
    monitor_profile->extend2 = 0;
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

void monitor_profile_set_timing(uint8_t *data, uint32_t timing)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    monitor_profile->timing = timing;
}

// timing+extern2
void monitor_profile_set_tickus(uint8_t *data, uint64_t tick_us)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    *(uint64_t *)&monitor_profile->timing = tick_us;
}

uint16_t monitor_profile_get_length(uint8_t *data)
{
    monitor_profile_t *monitor_profile = (monitor_profile_t *)data;
    return monitor_profile->length;
}

void monitor_profile_set_length(uint8_t *data, uint16_t length)
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
    uint16_t i, offset = 0;
    uint16_t result;
    monitor_payload_t *monitor_payload = NULL;
    data_payload_t *data_payload = NULL;
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
    if (monitor_payload->count == 0)
        return RESULT_PAYLOAD_ERR;

    start_point = (uint8_t *)&monitor_payload->data_payload;
    offset = 0;

    for (i = 0; i < monitor_payload->count; i++)
    {
        data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
        if (cmd == CMD_GETVALUE || cmd == CMD_NOTIFY)
        {
            result = monitor_get_value(data_payload->type, data_payload->addr, (uint8_t *)&data_payload->data, data_payload->size);
        }
        else if (cmd == CMD_SETVALUE)
        {
            result = monitor_set_value(data_payload->type, data_payload->addr, (uint8_t *)&data_payload->data, data_payload->size);
            result = monitor_get_value(data_payload->type, data_payload->addr, (uint8_t *)&data_payload->data, data_payload->size);
        }

        offset += data_payload->size + sizeof(data_payload_t) - 4; //-4 is data point
    }
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

    return RESULT_SUCCESS;
}

uint16_t monitor_profile_cmd_process(uint16_t cmd, uint16_t parse_result, uint8_t *data, uint16_t length)
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
    {
        monitor_profile_set_header(data);
        monitor_profile_set_cmd(data, CMD_GETVALUE);
        if (parse_result == RESULT_SUCCESS)
            return monitor_profile_payload_process(CMD_GETVALUE, data, length);
        return parse_result;
    }
    break;
    case CMD_SETVALUE:
    {
        monitor_profile_set_header(data);
        monitor_profile_set_cmd(data, CMD_SETVALUE);
        if (parse_result == RESULT_SUCCESS)
            return monitor_profile_payload_process(CMD_SETVALUE, data, length);
        return parse_result;
    }
    break;
    case CMD_NOTIFY:
    {
        monitor_profile_set_header(data);
        monitor_profile_set_cmd(data, CMD_NOTIFY);
        if (parse_result == RESULT_SUCCESS)
            return monitor_profile_payload_process(CMD_NOTIFY, data, length);
        return parse_result;
    }
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
    if(data == NULL || drop_offset == NULL || expect_length == NULL)
    {
        return RESULT_BAD;
    }
#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_LOG)
    MONITOR_LOG_DBG("len:%d,data:", length);
    for(i = 0; i < length; i++)
    {
        MONITOR_LOG_RAW(" %02x", data[i]);
    }
    MONITOR_LOG_RAW("\r\n");

#endif
    if(length < MONITOR_PROFILE_MIN_SIZE)
    {
        *drop_offset = 0;
        *expect_length = MONITOR_PROFILE_MIN_SIZE - length;
        return RESULT_HEAD_ERR;
    }

    for(i = 0; i < length; i++)
    {
        if(*(uint16_t*)(data+i) == MONITOR_START_CMD && *(uint16_t*)(data+i+2) == MONITOR_MAGIC_CMD)
        {
#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_LOG)
            MONITOR_LOG_DBG("head:[%d]:%04x,[%d]:%04x\r\n", i, *(uint16_t*)(data+i), i+2, *(uint16_t*)(data+i+2));
#endif
            flag = 1;
            break;
        }
    }

    if(flag == 0)
    {
        *drop_offset = length;
        *expect_length = MONITOR_PROFILE_MIN_SIZE;
#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_LOG)
        MONITOR_LOG_DBG("bad!, no head\r\n");
#endif
        return RESULT_HEAD_ERR;
    }

    *drop_offset = i;
    if((length - i) < (MONITOR_PROFILE_MIN_SIZE - 2))
    {
        *expect_length = MONITOR_PROFILE_MIN_SIZE - (length-i);
#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_LOG)
        MONITOR_LOG_DBG("no len, (length-i):%d:%d, expect:%d\r\n", (length - i),(MONITOR_PROFILE_MIN_SIZE - 2), *expect_length);
#endif
        return RESULT_HEAD_ERR;
    }

    monitor_data = (monitor_profile_t *)(data + i);

#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_LOG)
    MONITOR_LOG_DBG("len:0x%04x:%d\r\n", monitor_data->length, monitor_data->length);
#endif

    if(monitor_data->length > MONITOR_PAYLOAD_MAXSIZE)
    {
        *drop_offset += MONITOR_PROFILE_MIN_SIZE;
        *expect_length = ((length - *drop_offset) > MONITOR_PROFILE_MIN_SIZE) ? 0 : (MONITOR_PROFILE_MIN_SIZE - (length - *drop_offset));
        MONITOR_LOG_ERR("Recv data overflow,len:%d\r\n", monitor_data->length);
        // return RESULT_HEAD_ERR;
        return RESULT_PRFILE_OVERFLOW;
    }

    if(length - *drop_offset < monitor_data->length + MONITOR_PROFILE_MIN_SIZE)
    {
        *expect_length = (monitor_data->length + MONITOR_PROFILE_MIN_SIZE) - (length - *drop_offset);
        return RESULT_HEAD_ERR;
    }

#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_LOG)
    MONITOR_LOG_DBG("end,recv:0x%04x,rel:0x%04x\r\n", *(uint16_t *)(data + *drop_offset + MONITOR_PROFILE_MIN_SIZE + monitor_data->length - 2), MONITOR_END_CMD);
#endif
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
