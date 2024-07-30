/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "mp_adc.h"
#include "mp_pwm.h"
#include "mp_timer.h"
#include "mp_api.h"

int hpm_mp_api_get_default(hpm_mp_t* mp_t)
{
    if (NULL == mp_t)
        return -1;
    mp_t->pwm_pair_count = 0;
    mp_t->pwm_sync_time_us = 0;
    mp_t->pwm_pair = NULL;
    return 0;
}

int hpm_mp_pwm_pair_get_default(uint8_t pwm_pair_index, uint32_t freq, hpm_pwm_pair_t* pwm_pair_t)
{
    int i;
    if (NULL == pwm_pair_t)
        return -1;
    pwm_pair_t->pwm_pair_index = pwm_pair_index;
    pwm_pair_t->pwm_map_index = (uint8_t)mp_pwm_pair_get_mapindex(pwm_pair_index);
    pwm_pair_t->pwm_pair_mode = MP_PWM_PAIR_MODE_REVERSE;
    pwm_pair_t->phase_angle = MP_PWM_PAIR_PHASE_ANGLE_0;
    pwm_pair_t->deadzone_start_halfcycle = 0;
    pwm_pair_t->deadzone_end_halfcycle = 0;
    pwm_pair_t->pwm_freq = freq;
    pwm_pair_t->bus_freq = mp_pwm_pair_get_busfreq(pwm_pair_index);
    pwm_pair_t->reload_count = mp_pwm_pair_get_reload(pwm_pair_index, freq);
    pwm_pair_t->duty_count = 0;
    pwm_pair_t->shadow_dutycount = pwm_pair_t->reload_count;
    pwm_pair_t->fault_enable = false;
    pwm_pair_t->trigger_count = 0;
    pwm_pair_t->trigger_enable = 0;
    for (i = 0; i < MP_PWM_TRIGGER_COUNT_MAX; i++)
    {
        pwm_pair_t->trigger_dutycount[i] = pwm_pair_t->reload_count >> 1;
    }
    pwm_pair_t->adc_count = 0;
    pwm_pair_t->adc_pack = NULL;
    pwm_pair_t->adc_over_callback = NULL;
    pwm_pair_t->adc_handle = NULL;
    if (pwm_pair_t->reload_count == 0)
        return -1;
    return 0;
}

int hpm_mp_pwm_get_default(uint8_t pwm_index, uint32_t freq, hpm_pwm_t* pwm_t)
{
    int i;
    if (NULL == pwm_t)
        return -1;
    pwm_t->pwm_index = pwm_index;
    pwm_t->pwm_map_index = (uint8_t)mp_pwm_get_mapindex(pwm_index);
    pwm_t->pwm_freq = freq;
    pwm_t->bus_freq = mp_pwm_get_busfreq(pwm_index);
    pwm_t->reload_count = mp_pwm_get_reload(pwm_index, freq);
    pwm_t->duty_count = 0;
    pwm_t->shadow_dutycount = pwm_t->reload_count;
    pwm_t->fault_enable = false;
    pwm_t->trigger_count = 0;
    pwm_t->trigger_enable = 0;
    for (i = 0; i < MP_PWM_TRIGGER_COUNT_MAX; i++)
    {
        pwm_t->trigger_dutycount[i] = pwm_t->reload_count >> 1;
    }
    pwm_t->adc_count = 0;
    pwm_t->adc_pack = NULL;
    pwm_t->adc_over_callback = NULL;
    pwm_t->adc_handle = NULL;
    if (pwm_t->reload_count == 0)
        return -1;
    return 0;
}

int hpm_mp_adc_get_default(uint8_t adc_index, hpm_adc_t* adc_t)
{
    if (NULL == adc_t)
        return -1;
    mp_adc_get_default(adc_index, adc_t);
    return 0;
}

int hpm_mp_api_init(hpm_mp_t* mp_t)
{
    int i, ret;
    if (NULL == mp_t)
        return -1;
    printf("mp version:%s\r\n", MP_VERSION);

    for (i = 0; i < mp_t->pwm_pair_count; i++)
    {
        ret = mp_pwm_pair_config(&mp_t->pwm_pair[i]);
        if (ret != 0)
        {
            printf("pwm pair config fail:%d\n", ret);
            return -2;
        }
        printf("pwm pair config success, index:%d!\n", i);
    }

    for (i = 0; i < mp_t->pwm_pair_count; i++)
    {
        ret = mp_pwm_fault_mux_config(MP_PWM_PAIR_TYPE, (void*)&mp_t->pwm_pair[i]);
        if (ret != 0)
        {
            printf("pwm fault mux config fail:%d\n", ret);
            return -3;
        }
        printf("pwm fault mux config success, index:%d!\n", i);
    }

    for (i = 0; i < mp_t->pwm_pair_count; i++)
    {
        ret = mp_pwm_trigger_mux_config(MP_PWM_PAIR_TYPE, (void*)&mp_t->pwm_pair[i]);
        if (ret != 0)
        {
            printf("pwm trigger mux config fail:%d\n", ret);
            return -3;
        }
        printf("pwm trigger mux config success, index:%d!\n", i);
    }
    for (i = 0; i < mp_t->pwm_pair_count; i++)
    {
        ret = mp_adc_init(MP_PWM_PAIR_TYPE, (void*)&mp_t->pwm_pair[i]);
        if (ret != 0)
        {
            printf("adc init fail:%d\n", ret);
            return -4;
        }
        printf("adc init success, index:%d!\n", i);
    }
    for (i = 0; i < mp_t->pwm_pair_count; i++)
    {
        ret = mp_adc_trigger_config(MP_PWM_PAIR_TYPE, (void*)&mp_t->pwm_pair[i]);
        if (ret != 0)
        {
            printf("adc trigger fail:%d\n", ret);
            return -5;
        }
        printf("adc trigger config success, index:%d!\n", i);
    }

    ret = mp_pwm_sync_config(mp_t);
    if (ret != 0)
    {
        printf("pwm sync fail:%d\n", ret);
        return -6;
    }
    printf("pwm sync config success!\n");

    for (i = 0; i < mp_t->pwm_pair_count; i++)
    {
        ret = mp_adc_interrupt_enable(MP_PWM_PAIR_TYPE, (void*)&mp_t->pwm_pair[i], true);
        if (ret != 0)
        {
            printf("adc interrupt enable fail:%d\n", ret);
            return -8;
        }
        printf("adc interrupt enable success, index:%d\n", i);
    }

    for (i = 0; i < mp_t->pwm_pair_count; i++)
    {
        ret = mp_pwm_pair_output_enable(&mp_t->pwm_pair[i], true);
        if (ret != 0)
        {
            printf("pwm pair output enable fail:%d\n", ret);
            return -7;
        }
        printf("pwm pair output enable success, index:%d\n", i);
    }
    return 0;
}