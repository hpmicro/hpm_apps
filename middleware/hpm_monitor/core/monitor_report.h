#ifndef __MONITOR_REPORT_H
#define __MONITOR_REPORT_H

#include "monitor_profile.h"

typedef struct
{
    uint32_t timing_us;      // 周期数
    uint32_t last_sample_us; // 上次触发或采样时间
    uint32_t last_report_us; // 上次上报时间
    uint32_t length;         // 数据长度
    uint32_t pkt_count;      // 数据包数量
    uint32_t dat_count;      // 数据点数量
    uint32_t cur_count;      // 当前数据点数量
    uint8_t buf_count;       // 缓冲区数量
    uint8_t buf_index;       // 当前缓冲区索引
    uint32_t offset;         // 当前缓冲区偏移
    uint32_t stepsize;       // 数据步进长度
    uint8_t *buff;           // 数据缓冲区
    uint32_t buf_size;       // 数据缓冲区大小
} monitor_notify_t;

typedef struct
{
    uint8_t type;
    uint8_t mode;
    uint8_t expr;
    uint8_t rsvd;
    uint32_t addr_or_ch;
    uint8_t data[8];
} __attribute__((packed)) data_trigger_rsv_t;

typedef struct
{
    uint16_t count;
    data_trigger_rsv_t data[MONITOR_TRIGGER_MAXCOUNT];
    uint8_t trg_prev_vals[MONITOR_TRIGGER_MAXCOUNT][8];
    bool trg_inited[MONITOR_TRIGGER_MAXCOUNT];
    bool trg_result[MONITOR_TRIGGER_MAXCOUNT];
} monitor_trigger_t;

typedef struct
{
    uint16_t pkt_count;
    uint32_t data_count;
    uint32_t data_freq;
    uint8_t mem_cur_index;
    uint8_t mem_count;
    uint32_t mem_payload_offset[MONITOR_REPORT_MAXCOUNT];
    uint32_t mem_payload_len[MONITOR_REPORT_MAXCOUNT];
} sample_private_t;

typedef struct
{
    uint16_t pkt_count;
    uint8_t mem_cur_index[MONITOR_REPORT_MAXCOUNT];
    uint8_t is_double_mem[MONITOR_REPORT_MAXCOUNT];
    uint32_t data_freq[MONITOR_REPORT_MAXCOUNT];
    uint32_t data_count[MONITOR_REPORT_MAXCOUNT];
    uint32_t mem_single_len[MONITOR_REPORT_MAXCOUNT];
    uint32_t data_index[MONITOR_REPORT_MAXCOUNT];
} channel_private_t;

typedef struct
{
    uint32_t timing_us;      // 周期数
    uint32_t last_sample_us; // 上次触发或采样时间
    uint32_t last_report_us; // 上次上报时间
    uint32_t length;         // 数据长度
    uint32_t pkt_count;      // 数据包数量
    uint32_t freq;          // 数据频率
    uint32_t dat_count;      // 数据点数量
    uint32_t cur_count;      // 当前数据点数量
    uint8_t buf_count;       // 缓冲区数量
    uint8_t buf_index;       // 当前缓冲区索引
    uint32_t offset;         // 当前缓冲区偏移
    uint32_t stepsize;       // 数据步进长度
    uint8_t *buff;           // 数据缓冲区
    uint32_t buf_size;       // 数据缓冲区大小
} notify_private_t;

typedef struct
{
    sample_private_t sample;
    channel_private_t channel;
    notify_private_t notify;
    uint32_t mem_addr[MONITOR_REPORT_MAXCOUNT];
    uint32_t mem_len[MONITOR_REPORT_MAXCOUNT];
    data_payload_t data[MONITOR_REPORT_MAXCOUNT];
} montiro_report_data_t;

// void monitor_stream_buffer_clear(void);

void monitor_trigger_clear(void);

void monitor_report_clear(void);

int monitor_notify_set(uint8_t *input, uint32_t length);

int monitor_stream_set(uint8_t *input, uint32_t length);

int monitor_buffer_set(uint8_t *input, uint32_t length);

int monitor_notify_start(void);

int monitor_notify_stop(void);

int monitor_stream_start(void);

int monitor_stream_stop(void);

int monitor_buffer_start(void);

int monitor_buffer_stop(void);

int monitor_trigger_start(void);

int monitor_trigger_stop(void);

int monitor_notify_handle(uint8_t** output);

int monitor_notify_ch_handle(uint8_t **output);

int monitor_stream_handle(uint8_t** output);

int monitor_buffer_handle(uint8_t** output);

void monitor_report_done(uint32_t free_addr);

int monitor_trigger_handle(void);

int monitor_channel_report_array(uint8_t ch, void *array, uint32_t count);

int monitor_channel_add_data(uint8_t ch, void *data);

void* monitor_custom_mem_alloc(uint8_t ch, uint32_t size);

void monitor_custom_mem_free(void* addr);

bool monitor_report_addr_is_released(uint32_t addr);

bool monitor_report_ch_is_released(uint8_t ch);

// bool monitor_custom_addr_is_released(uint32_t addr);

void monitor_report_init(void);

#endif //__MONITOR_REPORT_H