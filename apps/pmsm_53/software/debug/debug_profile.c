#include <string.h>
#include "debug_profile.h"


// pid vid
uint32_t d7d75a2a6eebafc3d2ec0a0be089e05322057c4c308d648b374b362ea7eff5fa = (DEBUG_PID << 16) | DEBUG_VID;

//debug_version
//20240703122303
//Jul  3 2024
//Jul 13 2024
//07:03:00
uint64_t be8443a5d67825271edb6f6bee202d6125a38e6aa8f2acbd54652d113af8793b = (
    ((__DATE__[7u]-'0')                                  * 10000000000000ul) + \
    ((__DATE__[8u]-'0')                                  * 1000000000000ul) + \
    ((__DATE__[9u]-'0')                                  * 100000000000ul) + \
    ((__DATE__[10u]-'0')                                 * 10000000000ul) + \
    (((__DATE__[2u] == 'n' && __DATE__[1u] == 'a')      ? 1u  /*Jan*/
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
                                                       : 12u /*Dec*/)
                                                         * 100000000ul) + \
    ((__DATE__[4u] == ' ' ? 0 : __DATE__[4u] - '0')      * 10000000ul) + \
    ((__DATE__[5u] - '0')                                * 1000000ul) + \
    ((__TIME__[0u] - '0')                                * 100000ul) + \
    ((__TIME__[1u] - '0')                                * 10000ul) + \
    ((__TIME__[3u] - '0')                                * 1000ul) + \
    ((__TIME__[4u] - '0')                                * 100ul) + \
    ((__TIME__[6u] - '0')                                * 10ul) + \
    ((__TIME__[7u] - '0')                                * 1ul)
);

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

static uint16_t debug_get_value(uint8_t type, uint32_t addr, uint8_t *data, uint16_t length)
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

static uint16_t debug_set_value(uint8_t type, uint32_t addr, uint8_t *data, uint16_t length)
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

uint32_t debug_profile_crc32(uint8_t *data)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    uint32_t length = debug_profile->length + DEBUG_PROFILE_MIN_SIZE;
    return private_crc32((uint8_t *)&debug_profile->index, length - DEBUG_PROFILE_CRC_OFFSET);
}

void debug_profile_set_header(uint8_t *data)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    debug_profile->start = DEBUG_START_CMD;
    debug_profile->magic = DEBUG_MAGIC_CMD;
    debug_profile->version = DEBUG_VERSION;
    debug_profile->extend1 = 0;
    debug_profile->extend2 = 0;
}

void debug_profile_set_end(uint8_t *data)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    *(uint16_t *)(data + DEBUG_PROFILE_MIN_SIZE + debug_profile->length - 2) = DEBUG_END_CMD;
}

void debug_profile_set_crc32(uint8_t *data, uint32_t crc32)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    debug_profile->crc32 = crc32;
}

uint16_t debug_profile_get_index(uint8_t *data)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    return debug_profile->index;
}

void debug_profile_set_index(uint8_t *data, uint16_t index)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    debug_profile->index = index;
}

uint16_t debug_profile_get_cmd(uint8_t *data)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    return debug_profile->cmd;
}

void debug_profile_set_cmd(uint8_t *data, uint16_t cmd)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    debug_profile->cmd = cmd;
}

uint16_t debug_profile_get_result(uint8_t *data)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    return debug_profile->result;
}

void debug_profile_set_result(uint8_t *data, uint16_t result)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    debug_profile->result = result;
}

uint32_t debug_profile_get_timing(uint8_t *data)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    return debug_profile->timing;
}

void debug_profile_set_timing(uint8_t *data, uint32_t timing)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    debug_profile->timing = timing;
}

//timing+extern2
void debug_profile_set_tickus(uint8_t *data, uint64_t tick_us)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    *(uint64_t*)&debug_profile->timing = tick_us;
}

uint16_t debug_profile_get_length(uint8_t *data)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    return debug_profile->length;
}

void debug_profile_set_length(uint8_t *data, uint16_t length)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    debug_profile->length = length;
}

debug_payload_t *debug_profile_get_payload(uint8_t *data)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    return (debug_payload_t *)&debug_profile->debug_payload;
}

debug_info_t *debug_profile_get_debuginfo(uint8_t *data)
{
    debug_profile_t *debug_profile = (debug_profile_t *)data;
    return (debug_info_t *)&debug_profile->debug_payload;
}

uint16_t debug_profile_payload_process(uint16_t cmd, uint8_t *data, uint32_t length)
{
    uint16_t i, offset = 0;
    uint16_t result;
    debug_payload_t *debug_payload = NULL;
    data_payload_t *data_payload = NULL;
    uint8_t *start_point;

    if (data == NULL || length == 0)
    {
        return RESULT_PAYLOAD_ERR;
    }

    if (debug_profile_get_length(data) == 0)
    {
        return RESULT_SUCCESS;
    }

    debug_payload = debug_profile_get_payload(data);
    if (debug_payload->count == 0)
        return RESULT_PAYLOAD_ERR;

    start_point = (uint8_t *)&debug_payload->data_payload;
    offset = 0;

    for (i = 0; i < debug_payload->count; i++)
    {
        data_payload = (data_payload_t *)((uint8_t *)start_point + offset);
        if (cmd == CMD_GETVALUE || cmd == CMD_NOTIFY)
        {
            result = debug_get_value(data_payload->type, data_payload->addr, (uint8_t *)&data_payload->data, data_payload->size);
        }
        else if (cmd == CMD_SETVALUE)
        {
            result = debug_set_value(data_payload->type, data_payload->addr, (uint8_t *)&data_payload->data, data_payload->size);
            result = debug_get_value(data_payload->type, data_payload->addr, (uint8_t *)&data_payload->data, data_payload->size);
        }

        offset += data_payload->size + sizeof(data_payload_t) - 4; //-4 is data point
    }
    return result;
}

uint16_t debug_profile_info_process(uint8_t *data)
{
    debug_info_t *debug_info = NULL;

    if (data == NULL)
    {
        return RESULT_PAYLOAD_ERR;
    }

    debug_info = debug_profile_get_debuginfo(data);
    debug_info->vid = (uint16_t)(d7d75a2a6eebafc3d2ec0a0be089e05322057c4c308d648b374b362ea7eff5fa & 0xFFFF);
    debug_info->pid = (uint16_t)(d7d75a2a6eebafc3d2ec0a0be089e05322057c4c308d648b374b362ea7eff5fa >> 16);
    debug_info->debug_version = be8443a5d67825271edb6f6bee202d6125a38e6aa8f2acbd54652d113af8793b;
    debug_info->payload_maxsize = DEBUG_PAYLOAD_MAXSIZE;

    return RESULT_SUCCESS;
}

uint16_t debug_profile_cmd_process(uint16_t cmd, uint16_t parse_result, uint8_t *data, uint16_t length)
{
    switch (cmd)
    {
    case CMD_CONNECT:
    {
        debug_profile_set_header(data);
        debug_profile_set_cmd(data, CMD_CONNECT);
        return RESULT_SUCCESS;
    }
    break;
    case CMD_GETINFO:
    {
        debug_profile_set_header(data);
        debug_profile_set_cmd(data, CMD_GETINFO);
        debug_profile_set_length(data, sizeof(debug_info_t));
        if (parse_result == RESULT_SUCCESS)
            return debug_profile_info_process(data);
        return parse_result;
    }
    break;
    case CMD_GETVALUE:
    {
        debug_profile_set_header(data);
        debug_profile_set_cmd(data, CMD_GETVALUE);
        if (parse_result == RESULT_SUCCESS)
            return debug_profile_payload_process(CMD_GETVALUE, data, length);
        return parse_result;
    }
    break;
    case CMD_SETVALUE:
    {
        debug_profile_set_header(data);
        debug_profile_set_cmd(data, CMD_SETVALUE);
        if (parse_result == RESULT_SUCCESS)
            return debug_profile_payload_process(CMD_SETVALUE, data, length);
        return parse_result;
    }
    break;
    case CMD_NOTIFY:
    {
        debug_profile_set_header(data);
        debug_profile_set_cmd(data, CMD_NOTIFY);
        if (parse_result == RESULT_SUCCESS)
            return debug_profile_payload_process(CMD_NOTIFY, data, length);
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

uint16_t debug_profile_parse(uint8_t *data, uint32_t length, debug_payload_t **payload)
{
    debug_profile_t *debug_data = (debug_profile_t *)data;

#ifdef DEBUG_LOG_ENABLE
    printf("recv len:%d, value:", length);
    for (uint32_t i = 0; i < length; i++)
    {
        printf(" 0x%02X", data[i]);
    }
    printf("\r\n");
#endif

    if (data == NULL || length < DEBUG_PROFILE_MIN_SIZE || debug_data->length + DEBUG_PROFILE_MIN_SIZE > length)
    {
        return RESULT_BAD;
    }

    if (debug_data->start != DEBUG_START_CMD ||
        debug_data->magic != DEBUG_MAGIC_CMD ||
        *(uint16_t *)(data + DEBUG_PROFILE_MIN_SIZE + debug_data->length - 2) != DEBUG_END_CMD)
    {
        printf("ER: start[0x%04X], magic[0x%04X], end[0x%04x]\r\n",
               debug_data->start, debug_data->magic, *(uint16_t *)(data + DEBUG_PROFILE_MIN_SIZE + debug_data->length - 2));
        return RESULT_HEAD_ERR;
    }

    if (debug_data->version != DEBUG_VERSION)
    {
        printf("WR: version no match!\r\n");
        return RESULT_VER_ERR;
    }

    if (debug_data->length > 0 && debug_data->length + DEBUG_PROFILE_MIN_SIZE > DEBUG_PROFILE_MAXSIZE)
    {
        printf("WR: data overflow!\r\n");
        return RESULT_PRFILE_OVERFLOW;
    }

    if (debug_data->crc32 != debug_profile_crc32(data))
    {
        printf("ER: CRC32 no match!\r\n");
        return RESULT_CRC_ERR;
    }

    if ((debug_data->cmd & DEBUG_CMD_HOST_MASK) == 0)
    {
        printf("ER: data cmd is error!,cmd:0x%04X\r\n", debug_data->cmd);
        return RESULT_CMD_ERR;
    }

    if (debug_data->length > 0)
    {
        if (payload == NULL)
        {
            return RESULT_BAD;
        }
        else
        {
            *payload = (debug_payload_t *)data + DEBUG_PROFILE_MIN_SIZE - sizeof(uint16_t); // uint16_t is end
        }
    }
    return RESULT_SUCCESS;
}
