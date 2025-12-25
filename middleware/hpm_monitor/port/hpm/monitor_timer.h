#ifndef __MONITOR_TIMER_H
#define __MONITOR_TIMER_H

#include "monitor_report.h"

typedef void (*sample_result_cb)(int ret, uint8_t index);

void monitor_timer_config(uint32_t freq_hz, uint32_t count);

void monitor_timer_stop(void);

void monitor_timer_start(void);

int monitor_sample_dma_config(void);

void monitor_sample_dma_start(void);

bool monitor_timer_sample_is_running(void);

int monitor_timer_sample_data_config(montiro_report_data_t *data, sample_result_cb cb, uint8_t is_loop, bool enable);

void monitor_timer_sample_restart(void);

void monitor_timer_sample_stop(void);

void monitor_timer_sample_destroy(void);

void monitor_timer_init(void);

#endif //__MONITOR_TIMER_H