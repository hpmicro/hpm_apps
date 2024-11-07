/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HPM_TICKTIME_H
#define __HPM_TICKTIME_H

#include <stdint.h>

uint64_t clock_get_now_tick_us(void);

uint64_t clock_get_now_tick_ms(void);

uint64_t clock_get_now_tick_s(void);

#endif //__HPM_TICKTIME_H