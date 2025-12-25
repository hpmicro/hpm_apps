/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include <math.h>
#include "hpm_common.h"
#include "monitor_report.h"
#include "monitor.h"
#include "monitor_ticktime.h"
#include "adc_16_pmt.h"

#define ADC_SAMPLE_FREQ             (100000UL)
MONITOR_DEFINE_GLOBAL_VAR(adc_ch, 0, uint16_t, ADC_SAMPLE_FREQ, APP_ADC16_DMA_BUFF_LEN_IN_BYTES);

static void monitor_adc_handle(void)
{
    if (adc_get_done(0) && adc_get_done(1))
    {
        printf("adc dual buff full up!\r\n");
    }
    if (adc_get_done(0) && monitor_report_ch_is_released(0))
    {
        adc_clear_done(0);
        monitor_channel_report_array(0, adc_get_buf_addr_point(0), APP_ADC16_DMA_BUFF_LEN_IN_BYTES);
    }
    else if (adc_get_done(1) && monitor_report_ch_is_released(0))
    {
        adc_clear_done(1);
        monitor_channel_report_array(0, adc_get_buf_addr_point(1), APP_ADC16_DMA_BUFF_LEN_IN_BYTES);
    }
}

int main(void)
{
    board_init();
    printf("monitor adc demo!\r\n");
    printf("__DATE__:%s, __TIME__:%s\r\n", __DATE__, __TIME__);

    monitor_init();

    adc_pmt_auto_config(ADC_SAMPLE_FREQ);
    while (1)
    {
        monitor_adc_handle();
        monitor_handle();
    }
    return 0;
}
