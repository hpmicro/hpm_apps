/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __MONITOR_TASK_H
#define __MONITOR_TASK_H

#include <stdint.h>

typedef int (*monitor_channel_cb)(uint8_t *src, uint16_t length);

void monitor_task_init(void);

void monitor_task_output_register(monitor_channel_cb output_cb);

/*
* result: < 0, the data incomplete; == 0, parse success; > 0, parse error;
*/
int monitor_task_input_process(uint8_t *data, uint32_t length, uint32_t *drop_offset, uint32_t *expect_length);


void monitor_task_handle(void);

#endif //__MONITOR_TASK_H