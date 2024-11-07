/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __MONITOR_PROFILE_H
#define __MONITOR_PROFILE_H

#include <stdio.h>
#include <stdint.h>
#include "monitor_kconfig.h"

#define MONITOR_PROFILE_VERSION (1)

// START:    0x4850                 起始位
// MAGIC:   0x4D44                 标识位
// VER:       0x0001                 版本号，可扩展
// EXTEND1: 0x00000000         协议扩展预留
// CRC:       0x00000000          范围为后续所有的内容
// INDEX:    0x0001                 序号，用于同步，范围0~65535，当无ACK响应时会有重发机制存在；
// CMD:       0x8001/0x0001     最高位：1为上位机， 0为MCU
// RESULT:   0x0001                当CMD最高位是1 上位机时，RESULT为收到MCU的结果反馈；
//                                          当CMD最高位是0 MCU时， RESULT为收到上位机的结果反馈；
// TIMING:  0x00000000          上报间隔，用于控制主动上报数据的频率；
// EXTEND2: 0x00000000         协议扩展预留
// LENGH:   0x0001                 PAYLOAD长度，可以是0(可以没有payload，只是ack响应)；
// PAYLOAD: []                       数据内容
// END：     0xA55A                结束符

#define MONITOR_START_CMD (0x4850)
#define MONITOR_MAGIC_CMD (0x4D44)
#define MONITOR_END_CMD (0xA55A)

#define MONITOR_CMD_HOST_MASK (0x8000)

#define MONITOR_PAYLOAD_MAXSIZE (MONITOR_PROFILE_MAXSIZE - sizeof(monitor_profile_t))

#define MONITOR_PROFILE_MIN_SIZE (sizeof(monitor_profile_t) - 4) // 4 is payload point

#define MONITOR_RESULT_ACK_MASK (0x8000)

// uint16_t start;
// uint16_t magic;
// uint16_t version;
// uint32_t extend1;
// uint32_t crc32;
#define MONITOR_PROFILE_CRC_OFFSET (14)
// COUNT:         多包个数
// TYPE：          数据类型
// ADDRESS:     对应内存地址
// SIZE:            数据长度
// DATA:            数据值
// 。。。：        下一包数据，TYPE:SIZE:ADDRESS:DATA

typedef struct
{
    uint8_t type;
    uint32_t addr;
    uint16_t size;
    uint8_t *data;
} __attribute__((packed)) data_payload_t;

typedef struct
{
    uint16_t count;
    data_payload_t *data_payload;
} __attribute__((packed)) monitor_payload_t;

typedef struct
{
    uint16_t vid;
    uint16_t pid;
    uint64_t monitor_version;
    uint32_t maxsize;
} __attribute__((packed)) monitor_info_t;

typedef struct
{
    uint16_t start;
    uint16_t magic;
    uint16_t version;
    uint32_t extend1;
    uint32_t crc32;
    uint16_t index;
    uint16_t cmd;
    uint16_t result;
    uint32_t timing;
    uint32_t extend2;
    uint16_t length;
    monitor_payload_t *monitor_payload;
    uint16_t end;
} __attribute__((packed)) monitor_profile_t;

typedef enum
{
    CMD_INVALID = 0x0000,
    CMD_CONNECT = 0x0001,
    CMD_GETINFO = 0x0002,

    CMD_GETVALUE = 0x00A0,
    CMD_SETVALUE = 0x00A1,

    CMD_NOTIFY = 0x0A00,

} DEVICE_CMD;

typedef enum
{
    TYPE_BOOL = 0x0,
    TYPE_UINT8 = 0x01,
    TYPE_INT8 = 0x02,
    TYPE_UINT16 = 0x03,
    TYPE_INT16 = 0x04,
    TYPE_UINT32 = 0x05,
    TYPE_INT32 = 0x06,
    TYPE_UINT64 = 0x07,
    TYPE_INT64 = 0x08,
    TYPE_FLOAT = 0x09,
    TYPE_DOUBLE = 0x0A,
    TYPE_STRUCT = 0x0B,
    TYPE_ARRAY_STRUCT = 0x0C,
    TYPE_ARRAY_UINT8 = 0x0D,
    TYPE_ARRAY_INT8 = 0x0E,
    TYPE_ARRAY_UINT16 = 0x0F,
    TYPE_ARRAY_INT16 = 0x10,
    TYPE_ARRAY_UINT32 = 0x11,
    TYPE_ARRAY_INT32 = 0x12,
    TYPE_ARRAY_UINT64 = 0x13,
    TYPE_ARRAY_INT64 = 0x14,
    TYPE_ARRAY_FLOAT = 0x15,
    TYPE_ARRAY_DOUBLE = 0x16,
    TYPE_UNKNOWN = 0xA0,
} DATA_TYPE;

typedef enum
{
    RESULT_BAD = 0x0001,
    RESULT_HEAD_ERR = 0x0002,
    RESULT_VER_ERR = 0x0003,
    RESULT_CRC_ERR = 0x0004,
    RESULT_PAYLOAD_ERR = 0x0005,
    RESULT_GET_ERR = 0x0006,
    RESULT_SET_ERR = 0x0007,
    RESULT_DATA_LEN_ERR = 0x0008,
    RESULT_DATA_TYPE_ERR = 0x0009,
    RESULT_PRFILE_OVERFLOW = 0x000A,
    RESULT_CMD_ERR = 0x000B,

    RESULT_SUCCESS = 0x00FF,
    RESULT_NOTIFY_ON = 0x02FF,
    RESULT_NOTIFY_OFF = 0x01FF,
} MONITOR_RESULT;

extern const uint32_t d7d75a2a6eebafc3d2ec0a0be089e05322057c4c308d648b374b362ea7eff5fa;

extern const uint64_t be8443a5d67825271edb6f6bee202d6125a38e6aa8f2acbd54652d113af8793b;

// function

uint32_t monitor_profile_crc32(uint8_t *data);

void monitor_profile_set_header(uint8_t *data);

void monitor_profile_set_end(uint8_t *data);

void monitor_profile_set_crc32(uint8_t *data, uint32_t crc32);

uint16_t monitor_profile_get_index(uint8_t *data);

void monitor_profile_set_index(uint8_t *data, uint16_t index);

uint16_t monitor_profile_get_cmd(uint8_t *data);

void monitor_profile_set_cmd(uint8_t *data, uint16_t cmd);

uint16_t monitor_profile_get_result(uint8_t *data);

void monitor_profile_set_result(uint8_t *data, uint16_t result);

uint32_t monitor_profile_get_timing(uint8_t *data);

void monitor_profile_set_timing(uint8_t *data, uint32_t timing);

void monitor_profile_set_tickus(uint8_t *data, uint64_t tick_us);

uint16_t monitor_profile_get_length(uint8_t *data);

void monitor_profile_set_length(uint8_t *data, uint16_t length);

monitor_payload_t *monitor_profile_get_payload(uint8_t *data);

monitor_info_t *monitor_profile_get_monitorinfo(uint8_t *data);

uint16_t monitor_profile_payload_process(uint16_t cmd, uint8_t *data, uint32_t length);

uint16_t monitor_profile_info_process(uint8_t *data);

uint16_t monitor_profile_cmd_process(uint16_t cmd, uint16_t parse_result, uint8_t *data, uint16_t length);

uint16_t monitor_profile_parse(uint8_t *data, uint32_t length, uint32_t *drop_offset, uint32_t *expect_length);

#endif //__MONITOR_PROFILE_H