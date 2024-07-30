/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_gptmr_drv.h"
#include "mp_timer.h"
#include "mp_api.h"

static mp_timer_cb timer_cb;

void mp_timer_isr(void)
{
    if (gptmr_check_status(MP_CALLBACK_TIMER, GPTMR_CH_RLD_STAT_MASK(MP_CALLBACK_TIMER_CH)))
    {
        gptmr_clear_status(MP_CALLBACK_TIMER, GPTMR_CH_RLD_STAT_MASK(MP_CALLBACK_TIMER_CH));
        timer_cb();
    }
}
SDK_DECLARE_EXT_ISR_M(MP_CALLBACK_TIMER_IRQ, mp_timer_isr);

int hpm_mp_timer_create(uint32_t us, mp_timer_cb cb)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    if (cb == NULL)
        return -1;

    timer_cb = cb;
    gptmr_channel_get_default_config(MP_CALLBACK_TIMER, &config);

    clock_add_to_group(MP_CALLBACK_TIMER_CLK_NAME, 0);
    gptmr_freq = clock_get_frequency(MP_CALLBACK_TIMER_CLK_NAME);

    config.reload = gptmr_freq / (1000*1000) * us -1;
    gptmr_channel_config(MP_CALLBACK_TIMER, MP_CALLBACK_TIMER_CH, &config, false);
    gptmr_enable_irq(MP_CALLBACK_TIMER, GPTMR_CH_RLD_IRQ_MASK(MP_CALLBACK_TIMER_CH));
    intc_m_enable_irq_with_priority(MP_CALLBACK_TIMER_IRQ, 1);

    gptmr_start_counter(MP_CALLBACK_TIMER, MP_CALLBACK_TIMER_CH);
    return 0;
}
