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

ATTR_PLACE_AT_FAST_RAM_INIT_WITH_ALIGNMENT(4) int test_square_array[1024];
ATTR_PLACE_AT_FAST_RAM_INIT_WITH_ALIGNMENT(4) float test_sine_array[1024];

ATTR_PLACE_AT_FAST_RAM_INIT_WITH_ALIGNMENT(4) float test_triangule_wave = 0;
ATTR_PLACE_AT_FAST_RAM_INIT_WITH_ALIGNMENT(4) float test_sine_wave = 0;
ATTR_PLACE_AT_FAST_RAM_INIT_WITH_ALIGNMENT(4) int test_square_ware = 0;
ATTR_PLACE_AT_FAST_RAM_INIT_WITH_ALIGNMENT(4) float test_sawtooth_ware = 0;

#define SAMPLE_RATE 44100  // 采样率
#define FREQUENCY 440      // 频率
#define AMPLITUDE 500      // 振幅

MONITOR_DEFINE_GLOBAL_VAR(ch_signal_float_triangule, 0, float, 100, 0);
MONITOR_DEFINE_GLOBAL_VAR(ch_signal_float_sawtooth, 1, float, 100, 0);
MONITOR_DEFINE_GLOBAL_VAR(ch_array_int_square, 2, int32_t, 100, 1024);
MONITOR_DEFINE_GLOBAL_VAR(ch_array_float_sine, 3, float, 100, 1024);

//三角波
void triangule_wave_handle(void)
{
    static int i = 0;
    float t;

    if(i > SAMPLE_RATE)
        i = 0;
    t = (float)((float)i / SAMPLE_RATE);
    test_triangule_wave = (float)(AMPLITUDE * (2.0 * fabs(2.0 * t * FREQUENCY - floor(2.0 * t * FREQUENCY + 0.5)) - 1.0));
    i++;
}

//正玄波
void sine_wave_handle(void)
{
    static int i = 0;
    float t;

    if(i > SAMPLE_RATE)
        i = 0;

    t = (float)((float)i / SAMPLE_RATE);
    test_sine_wave = (float)(AMPLITUDE * sin(2.0 * M_PI * FREQUENCY * t));
    i++;
}

//方波
void square_ware_handle(void)
{
    static int i = 0;
    float t;

    if(i > SAMPLE_RATE)
        i = 0;
    t = (float)((float)i / SAMPLE_RATE);
    if (fmod(t * FREQUENCY, 1.0) < 0.5) {
        test_square_ware = AMPLITUDE;
    } else {
        test_square_ware = -AMPLITUDE;
    }
    i++;
}

//锯齿波
void sawtooth_ware_handle(void)
{
    static int i = 0;
    float t;

    if(i > SAMPLE_RATE)
        i = 0;

    t = (float)((float)i / SAMPLE_RATE);
    test_sawtooth_ware = (float)(AMPLITUDE * (t * FREQUENCY - floor(t * FREQUENCY)));
    i++;
}

void timer_cb(void)
{
    monitor_channel_add_data(0, &test_triangule_wave);
    monitor_channel_add_data(1, &test_sawtooth_ware);
    board_led_toggle();
}

int main(void)
{
    uint64_t time = 0;
    uint32_t index1, index2;
    board_init();
    board_init_led_pins();
    printf("general debug demo!\r\n");
    printf("__DATE__:%s, __TIME__:%s\r\n", __DATE__, __TIME__);

    monitor_init();
    board_timer_create(10, timer_cb);

    index1 = 0;
    index2 = 0;
    while (1)
    {
        if(tick_time_ms_read32() - time >= 10)
        {
            time = tick_time_ms_read32();
            triangule_wave_handle();
            sine_wave_handle();
            square_ware_handle();
            sawtooth_ware_handle();
            test_square_array[index1++] = test_square_ware;
            test_sine_array[index2++] = test_sine_wave;
            if(index1 >= 1024)
            {
                index1 = 0;
                monitor_channel_report_array(2, test_square_array, 1024);
            }
            if(index2 >= 1024)
            {
                index2 = 0;
                monitor_channel_report_array(3, test_sine_array, 1024);
            }
        }
        monitor_handle();
    }
    return 0;
}
