/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-02-27     HPMicro      the first version
 */

#include "SEGGER_SYSVIEW.h"

#include "hpm_csr_drv.h"
#include "hpm_rtt_interrupt_util.h"
#include "hpm_clock_drv.h"

U32  SEGGER_SYSVIEW_X_GetTimestamp                (void)
{
    return hpm_csr_get_core_cycle();
}

U32  SEGGER_SYSVIEW_X_GetInterruptId              (void)
{
    return hpm_rtt_interrupt_get();
}

#ifdef RT_USING_SYSTEMVIEW
static int rtt_show_cpu_freq(void)
{
    rt_kprintf("cpu0 frequency:\t\t %luHz\n", clock_get_frequency(clock_cpu0));
    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rtt_show_cpu_freq);
#endif

