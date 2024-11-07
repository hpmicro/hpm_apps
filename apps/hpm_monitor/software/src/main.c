/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include <math.h>
#include "monitor.h"
#include "monitor_ticktime.h"

float test_triangule_wave = 0;
float test_sine_wave = 0;
int test_square_ware = 0;
float test_sawtooth_ware = 0;

#define SAMPLE_RATE 44100  // 采样率
#define FREQUENCY 440      // 频率
#define AMPLITUDE 500      // 振幅

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


int main(void)
{
    uint64_t time = 0;
    board_init();
    printf("general debug demo!\r\n");
    printf("__DATE__:%s, __TIME__:%s\r\n", __DATE__, __TIME__);

    monitor_init();

    while (1)
    {
        if(clock_get_now_tick_ms() - time >= 10)
        {
            time = clock_get_now_tick_ms();
            triangule_wave_handle();
            sine_wave_handle();
            square_ware_handle();
            sawtooth_ware_handle();
        }
        monitor_handle();
    }
    return 0;
}
