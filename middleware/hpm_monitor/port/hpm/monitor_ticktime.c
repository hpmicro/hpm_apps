/*
 * Copyright (c) 2022-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_soc.h"
#include "hpm_clock_drv.h"
#include "hpm_mchtmr_drv.h"
#include "monitor_ticktime.h"

#ifndef HPMICRO_MCHTMR_FREQ
#define HPMICRO_MCHTMR_FREQ 24000000
#endif

static uint64_t ticks_offset = 0;
static uint32_t saved_ms = 0;
static uint32_t mchtmr_freq = HPMICRO_MCHTMR_FREQ;

#define TIME_I2MS(interval) ((interval) / (mchtmr_freq / 1000))
#define TIME_I2US(interval) ((interval) / (mchtmr_freq / 1000000))

void tick_time_init(void)
{
    mchtmr_freq = clock_get_frequency(clock_mchtmr0);
    tick_time_clear();
}

void tick_time_clear(void)
{
    ticks_offset = mchtmr_get_count(HPM_MCHTMR);
}

__attribute__((weak)) void platform_time_save_value(uint32_t value)
{
    saved_ms = value;
}

__attribute__((weak)) uint32_t platform_time_restore_value(void)
{
    return saved_ms;
}

void tick_time_restore(void)
{
    ticks_offset = mchtmr_get_count(HPM_MCHTMR);
}

void tick_time_save(void)
{
    platform_time_save_value(tick_time_read32());
}

uint16_t tick_time_read(void)
{
    return (uint16_t)tick_time_read32();
}

uint32_t tick_time_read32(void)
{
    return (uint32_t)TIME_I2MS((uint64_t)mchtmr_get_count(HPM_MCHTMR) - ticks_offset);
}

uint16_t tick_time_us_read(void)
{
    return (uint16_t)tick_time_us_read32();
}

uint32_t tick_time_us_read32(void)
{
    return (uint32_t)TIME_I2US((uint64_t)mchtmr_get_count(HPM_MCHTMR) - ticks_offset);
}

uint32_t tick_time_ms_read32(void)
{
    return tick_time_read32();
}