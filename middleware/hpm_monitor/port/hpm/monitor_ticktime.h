/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HPM_TICKTIME_H
#define __HPM_TICKTIME_H

#include <stdint.h>

void tick_time_init(void);

void tick_time_clear(void);

void tick_time_restore(void);

void tick_time_save(void);

uint16_t tick_time_read(void);

uint32_t tick_time_read32(void);

uint32_t tick_time_us_read32(void);

uint32_t tick_time_ms_read32(void);

#endif //__HPM_TICKTIME_H