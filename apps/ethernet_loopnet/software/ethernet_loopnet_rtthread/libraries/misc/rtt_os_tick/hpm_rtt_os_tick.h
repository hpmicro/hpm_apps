/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-02-24     HPMicro      the first version
 */

#ifndef _RTT_OS_TICK_H
#define _RTT_OS_TICK_H
#include "hpm_common.h"
#include "hpm_soc.h"

/* mchtimer section */
#define BOARD_MCHTMR_FREQ_IN_HZ (24000000UL)

/***************************************************************
 *
 * RT-Thread related definitions
 *
 **************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
#ifdef HPM_USING_VECTOR_PREEMPTED_MODE

void enable_rtt_plic_feature(void);

#endif

void os_tick_config(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _RTT_OS_TICK_H */
