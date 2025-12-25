/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <stddef.h>
#include "monitor_kconfig.h"
#include "monitor_mem.h"

#define ALIGN_SIZE 4

#define ALIGN(n) (((n) + ALIGN_SIZE - 1) & ~(ALIGN_SIZE - 1))
#define HEADER_SIZE sizeof(uint32_t)
#define MIN_BLOCK_SIZE (HEADER_SIZE + ALIGN_SIZE)

MONITOR_NOCACHE_RAM_SECTION MONITOR_ATTR_ALIGN(4) static uint8_t MONITOR_PRIVATE_VAR_NAME(data_pool)[MONITOR_MEM_SIZE];
#define MONITOR_DATA_POOL MONITOR_PRIVATE_VAR_NAME(data_pool)

static inline uint32_t get_header(const uint8_t *addr) {
    return *(const volatile uint32_t *)addr;
}

static inline void set_header(uint8_t *addr, uint32_t val) {
    *(volatile uint32_t *)addr = val;
}

void monitor_mem_reset(void)
{
    set_header(MONITOR_DATA_POOL, MONITOR_MEM_SIZE - HEADER_SIZE);
}

void *monitor_mem_alloc(size_t size)
{
    const size_t need_size = ALIGN(size);
    if (need_size == 0 || need_size > MONITOR_MEM_SIZE - HEADER_SIZE)
        return NULL;

    uint8_t *current = MONITOR_DATA_POOL;
    while (current < MONITOR_DATA_POOL + MONITOR_MEM_SIZE)
    {
        const uint32_t header = get_header(current);
        const size_t block_size = header & 0x7FFFFFFF;
        const int used = header >> 31;

        if (!used && block_size >= need_size)
        {
            const size_t remain = block_size - need_size;

            if (remain >= MIN_BLOCK_SIZE)
            {
                set_header(current, (need_size | 0x80000000));
                uint8_t *next = current + HEADER_SIZE + need_size;
                set_header(next, remain - HEADER_SIZE);
            }
            else
            {
                set_header(current, (block_size | 0x80000000));
            }
            return current + HEADER_SIZE;
        }
        current += HEADER_SIZE + block_size;
    }
    return NULL;
}

void monitor_mem_free(void *ptr)
{
    if (!ptr || (uint8_t *)ptr < MONITOR_DATA_POOL + HEADER_SIZE || (uint8_t *)ptr >= MONITOR_DATA_POOL + MONITOR_MEM_SIZE)
        return;

    uint8_t *current = (uint8_t *)ptr - HEADER_SIZE;
    uint32_t header = get_header(current);
    if (!(header >> 31))
        return;

    size_t total_size = header & 0x7FFFFFFF;
    set_header(current, total_size); // Removal of signs of use

    // Backward merge
    uint8_t *next_block = current + HEADER_SIZE + total_size;
    if (next_block < MONITOR_DATA_POOL + MONITOR_MEM_SIZE)
    {
        const uint32_t next_header = get_header(next_block);
        if (!(next_header >> 31))
        {
            total_size += HEADER_SIZE + (next_header & 0x7FFFFFFF);
            set_header(current, total_size);
        }
    }

    // Forward merger
    uint8_t *prev = MONITOR_DATA_POOL;
    while (prev < current)
    {
        const uint32_t prev_header = get_header(prev);
        const size_t prev_size = prev_header & 0x7FFFFFFF;
        uint8_t *prev_end = prev + HEADER_SIZE + prev_size;

        if (prev_end == current && !(prev_header >> 31))
        {
            total_size += HEADER_SIZE + prev_size;
            set_header(prev, total_size);
            current = prev;
        }
        prev += HEADER_SIZE + prev_size;
    }
}
