/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_synt_drv.h"
#include "hpm_clock_drv.h"
#include "mp_api.h"
#include "mp_pwm.h"

#define MP_PWM_SHADOW_CMP        (15)
#define MP_PWM_FAULT_CMP         (14)


#define MP_PWM_PAIR_MAP_CTX       pwm_pair_map_t
#define MP_PWM_PAIR_MAP_COUNT     pwm_pair_map_count

#define PWM_PAIR_INDEX            pwm_pair_t->pwm_pair_index
#define PWM_PAIR_MAP_INDEX        pwm_pair_t->pwm_map_index
#define PWM_PAIR_BASE             MP_PWM_PAIR_MAP_CTX[PWM_PAIR_MAP_INDEX].pwm_x
#define PWM_PAIR_CMP              MP_PWM_PAIR_MAP_CTX[PWM_PAIR_MAP_INDEX].cmp
#define PWM_PAIR_PIN              MP_PWM_PAIR_MAP_CTX[PWM_PAIR_MAP_INDEX].pin
#define PWM_PAIR_TRIG_CMP         MP_PWM_PAIR_MAP_CTX[PWM_PAIR_MAP_INDEX].pwm_trigger_map[MP_PWM_TRIGGER_ADC_BIT].trgm_cmp
#define PWM_PAIR_TRIG_PIN         MP_PWM_PAIR_MAP_CTX[PWM_PAIR_MAP_INDEX].pwm_trigger_map[MP_PWM_TRIGGER_ADC_BIT].trgm_pin

#define MP_PWM_MAP_CTX       pwm_map_t
#define MP_PWM_MAP_COUNT     pwm_map_count

#define PWM_INDEX            pwm_t->pwm_index
#define PWM_MAP_INDEX        pwm_t->pwm_map_index
#define PWM_BASE             MP_PWM_MAP_CTX[PWM_MAP_INDEX].pwm_x
#define PWM_CMP              MP_PWM_MAP_CTX[PWM_MAP_INDEX].cmp
#define PWM_PIN              MP_PWM_MAP_CTX[PWM_MAP_INDEX].pin
#define PWM_TRIG_CMP         MP_PWM_MAP_CTX[PWM_MAP_INDEX].pwm_trigger_map[MP_PWM_TRIGGER_ADC_BIT].trgm_cmp
#define PWM_TRIG_PIN         MP_PWM_MAP_CTX[PWM_MAP_INDEX].pwm_trigger_map[MP_PWM_TRIGGER_ADC_BIT].trgm_pin

static const mp_pwm_pair_map_t* pwm_pair_map_t = NULL;
static uint32_t pwm_pair_map_count = 0;
static volatile uint8_t mp_sync_done = 0;
static uint32_t sync_time_max = 0;

static const mp_pwm_map_t* pwm_map_t = NULL;
static uint32_t pwm_map_count = 0;

static void isr_pwm0(void)
{

    uint32_t mask;
    mask = pwm_get_status(HPM_PWM0);

    if (PWM_IRQ_RELOAD == (mask & PWM_IRQ_RELOAD))
    {
        pwm_clear_status(HPM_PWM0, PWM_IRQ_RELOAD);
        if (HPM_SYNT->CNT > sync_time_max)
        {
            synt_enable_counter(HPM_SYNT, false);
            pwm_disable_irq(HPM_PWM0, PWM_IRQ_RELOAD);
            mp_sync_done = 1;
        }
    }
}

static void isr_pwm1(void)
{
    uint32_t mask;
    mask = pwm_get_status(HPM_PWM1);

    if (PWM_IRQ_RELOAD == (mask & PWM_IRQ_RELOAD))
    {
        pwm_clear_status(HPM_PWM1, PWM_IRQ_RELOAD);
        if (HPM_SYNT->CNT > sync_time_max)
        {
            synt_enable_counter(HPM_SYNT, false);
            pwm_disable_irq(HPM_PWM1, PWM_IRQ_RELOAD);
            mp_sync_done = 1;
        }
    }
}

#ifdef IRQn_PWM2
static void isr_pwm2(void)
{
    uint32_t mask;
    mask = pwm_get_status(HPM_PWM2);

    if (PWM_IRQ_RELOAD == (mask & PWM_IRQ_RELOAD))
    {
        pwm_clear_status(HPM_PWM2, PWM_IRQ_RELOAD);
        if (HPM_SYNT->CNT > sync_time_max)
        {
            synt_enable_counter(HPM_SYNT, false);
            pwm_disable_irq(HPM_PWM2, PWM_IRQ_RELOAD);
            mp_sync_done = 1;
        }
    }
}
#endif

#ifdef IRQn_PWM3
static void isr_pwm3(void)
{
    uint32_t mask;
    mask = pwm_get_status(HPM_PWM3);

    if (PWM_IRQ_RELOAD == (mask & PWM_IRQ_RELOAD))
    {
        pwm_clear_status(HPM_PWM3, PWM_IRQ_RELOAD);
        if (HPM_SYNT->CNT > sync_time_max)
        {
            synt_enable_counter(HPM_SYNT, false);
            pwm_disable_irq(HPM_PWM3, PWM_IRQ_RELOAD);
            mp_sync_done = 1;
        }
    }
}
#endif
SDK_DECLARE_EXT_ISR_M(IRQn_PWM0, isr_pwm0)
SDK_DECLARE_EXT_ISR_M(IRQn_PWM1, isr_pwm1)
#ifdef IRQn_PWM2
SDK_DECLARE_EXT_ISR_M(IRQn_PWM2, isr_pwm2)
#endif
#ifdef IRQn_PWM3
SDK_DECLARE_EXT_ISR_M(IRQn_PWM3, isr_pwm3)
#endif

uint8_t mp_pwm_pair_get_mapindex(uint8_t pair_index)
{
    uint32_t i;
    for (i = 0; i < MP_PWM_PAIR_MAP_COUNT; i++)
    {
        if (MP_PWM_PAIR_MAP_CTX[i].pair_index == pair_index)
        {
            return (uint8_t)i;
        }
    }
    return -1;
}

uint8_t mp_pwm_get_mapindex(uint8_t index)
{
    uint32_t i;
    for (i = 0; i < MP_PWM_MAP_COUNT; i++)
    {
        if (MP_PWM_MAP_CTX[i].index == index)
        {
            return (uint8_t)i;
        }
    }
    return -1;
}

uint32_t mp_pwm_pair_get_busfreq(uint8_t pair_index)
{
    uint8_t map_index;
    uint32_t bus_freq = 0;

    map_index = mp_pwm_pair_get_mapindex(pair_index);

    if (MP_PWM_PAIR_MAP_CTX[map_index].pwm_x == HPM_PWM0)
    {
        bus_freq = clock_get_frequency(clock_mot0);
        printf("pwm0 freq:%d\n", bus_freq);
    }
    else if (MP_PWM_PAIR_MAP_CTX[map_index].pwm_x == HPM_PWM1)
    {
#ifdef clock_mot1
        bus_freq = clock_get_frequency(clock_mot1);
        printf("pwm1 freq:%d\n", bus_freq);
#else
        bus_freq = clock_get_frequency(clock_mot0);
        printf("pwm1-0 freq:%d\n", bus_freq);
#endif
    }
#ifdef HPM_PWM2
    else if (MP_PWM_PAIR_MAP_CTX[map_index].pwm_x == HPM_PWM2)
    {
        bus_freq = clock_get_frequency(clock_mot2);
        printf("pwm2 freq:%d\n", bus_freq);
    }
#endif
#ifdef HPM_PWM3
    else if (MP_PWM_PAIR_MAP_CTX[map_index].pwm_x == HPM_PWM3)
    {
        bus_freq = clock_get_frequency(clock_mot3);
        printf("pwm3 freq:%d\n", bus_freq);
    }
#endif

    return bus_freq;
}

uint32_t mp_pwm_get_busfreq(uint8_t index)
{
    uint8_t map_index;
    uint32_t bus_freq = 0;

    map_index = mp_pwm_get_mapindex(index);

    if (MP_PWM_MAP_CTX[map_index].pwm_x == HPM_PWM0)
    {
        bus_freq = clock_get_frequency(clock_mot0);
        printf("pwm0 freq:%d\n", bus_freq);
    }
    else if (MP_PWM_MAP_CTX[map_index].pwm_x == HPM_PWM1)
    {
#ifdef clock_mot1
        bus_freq = clock_get_frequency(clock_mot1);
        printf("pwm1 freq:%d\n", bus_freq);
#endif
    }
#ifdef HPM_PWM2
    else if (MP_PWM_MAP_CTX[map_index].pwm_x == HPM_PWM2)
    {
        bus_freq = clock_get_frequency(clock_mot2);
        printf("pwm2 freq:%d\n", bus_freq);
    }
#endif
#ifdef HPM_PWM3
    else if (MP_PWM_MAP_CTX[map_index].pwm_x == HPM_PWM3)
    {
        bus_freq = clock_get_frequency(clock_mot3);
        printf("pwm3 freq:%d\n", bus_freq);
    }
#endif

    return bus_freq;
}

uint32_t mp_pwm_pair_get_reload(uint8_t pair_index, uint32_t freq)
{
    uint32_t bus_freq = 0, reload;

    bus_freq = mp_pwm_pair_get_busfreq(pair_index);

    if (bus_freq == 0 || freq > bus_freq)
        return 0;
    reload = (bus_freq / freq) - 1;
    return reload;
}

uint32_t mp_pwm_get_reload(uint8_t index, uint32_t freq)
{
    uint32_t bus_freq = 0, reload;

    bus_freq = mp_pwm_get_busfreq(index);

    if (bus_freq == 0 || freq > bus_freq)
        return 0;
    reload = bus_freq / freq - 1;
    return reload;
}

int mp_pwm_pair_config(hpm_pwm_pair_t* pwm_pair_t)
{
    int i;
    uint8_t cmp_index, pin_index, map_index;

    pwm_cmp_config_t cmp_config[4] = { 0 };
    pwm_pair_config_t pwm_pair_config = { 0 };
    pwm_config_t pwm_config[2] = { 0 };
    pwm_output_channel_t pwm_output_ch_cfg;
    pwm_fault_source_config_t pwm_fault_src_cfg;
    TRGM_Type *trgm_t = NULL;

    if (NULL == pwm_pair_t)
    {
        return -1;
    }
    map_index = pwm_pair_t->pwm_map_index;

    pwm_stop_counter(PWM_PAIR_BASE);

    pwm_enable_reload_at_synci(PWM_PAIR_BASE);
    if (PWM_PAIR_BASE == HPM_PWM0)
    {
        clock_add_to_group(clock_mot0, 0);
        trgm_t = HPM_TRGM0;
    }
#ifdef HPM_PWM1
    else if (PWM_PAIR_BASE == HPM_PWM1)
    {
        clock_add_to_group(clock_mot1, 0);
#ifdef HPM_TRGM1
        trgm_t = HPM_TRGM1;
#else
        trgm_t = HPM_TRGM0;
#endif
    }
#endif
#ifdef HPM_PWM2
    else if (PWM_PAIR_BASE == HPM_PWM2)
    {
        clock_add_to_group(clock_mot2, 0);
        trgm_t = HPM_TRGM2;
    }
#endif
#ifdef HPM_PWM3
    else if (PWM_PAIR_BASE == HPM_PWM3)
    {
        clock_add_to_group(clock_mot3, 0);
        trgm_t = HPM_TRGM3;
    }
#endif
    else
    {
        return -2;
    }
#ifdef TRGM_TRGOCFG_PWM_SYNCI
    trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM_SYNCI, 1);
    trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM_SYNCI, 0);
#else
    if (PWM_PAIR_BASE == HPM_PWM0)
    {
        trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM0_SYNCI, 1);
        trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM0_SYNCI, 0);
    }
    else if (PWM_PAIR_BASE == HPM_PWM1)
    {
        trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM1_SYNCI, 1);
        trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM1_SYNCI, 0);
    }
    else
        return -5;
#endif
    /*
    * reload and start counter
    */
    pwm_set_reload(PWM_PAIR_BASE, 0, pwm_pair_t->reload_count);
    pwm_set_start_count(PWM_PAIR_BASE, 0, 0);

    /*
    * config cmp1 and cmp2
    */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = pwm_pair_t->duty_count;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = pwm_pair_t->duty_count;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_hw_event;

    if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_REVERSE)
    {
        if(pwm_pair_t->duty_count != 0 && pwm_pair_t->duty_count != pwm_pair_t->reload_count && pwm_pair_t->duty_count < pwm_pair_t->reload_count)
        {
            cmp_config[0].cmp = (pwm_pair_t->reload_count - pwm_pair_t->duty_count) >> 1;
            cmp_config[1].cmp = (pwm_pair_t->reload_count + pwm_pair_t->duty_count) >> 1;
        }
        pwm_get_default_pwm_pair_config(PWM_PAIR_BASE, &pwm_pair_config);
        pwm_pair_config.pwm[0].enable_output = false;
        pwm_pair_config.pwm[0].dead_zone_in_half_cycle = pwm_pair_t->deadzone_start_halfcycle;
        pwm_pair_config.pwm[0].invert_output = false;

        pwm_pair_config.pwm[1].enable_output = false;
        pwm_pair_config.pwm[1].dead_zone_in_half_cycle = pwm_pair_t->deadzone_end_halfcycle;
        pwm_pair_config.pwm[1].invert_output = false;
    }
    else
    {
        pwm_get_default_pwm_config(PWM_PAIR_BASE, &pwm_config[0]);

        pwm_config[0].enable_output = false;
        pwm_config[0].dead_zone_in_half_cycle = pwm_pair_t->deadzone_start_halfcycle;
        pwm_config[0].invert_output = false;

        pwm_get_default_pwm_config(PWM_PAIR_BASE, &pwm_config[1]);

        pwm_config[1].enable_output = false;
        pwm_config[1].dead_zone_in_half_cycle = pwm_pair_t->deadzone_end_halfcycle;
        pwm_config[1].invert_output = false;
    }


    if (pwm_pair_t->fault_enable)
    {
        if (pwm_pair_t->pwm_fault[0].fault_level == 0)
            pwm_pair_config.pwm[0].fault_mode = pwm_fault_mode_force_output_0;
        else if (pwm_pair_t->pwm_fault[0].fault_level == 1)
            pwm_pair_config.pwm[0].fault_mode = pwm_fault_mode_force_output_1;
        else if (pwm_pair_t->pwm_fault[0].fault_level == 2)
            pwm_pair_config.pwm[0].fault_mode = pwm_fault_mode_force_output_highz;

        if (pwm_pair_t->pwm_fault[0].recovery_trigger == 0)
            pwm_pair_config.pwm[0].fault_recovery_trigger = pwm_fault_recovery_immediately;
        else if (pwm_pair_t->pwm_fault[0].recovery_trigger == 1)
            pwm_pair_config.pwm[0].fault_recovery_trigger = pwm_fault_recovery_on_reload;
        else if (pwm_pair_t->pwm_fault[0].recovery_trigger == 2)
            pwm_pair_config.pwm[0].fault_recovery_trigger = pwm_fault_recovery_on_hw_event;
        else if (pwm_pair_t->pwm_fault[0].recovery_trigger == 3)
            pwm_pair_config.pwm[0].fault_recovery_trigger = pwm_fault_recovery_on_fault_clear;

        if (pwm_pair_t->pwm_fault[1].fault_level == 0)
            pwm_pair_config.pwm[1].fault_mode = pwm_fault_mode_force_output_0;
        else if (pwm_pair_t->pwm_fault[1].fault_level == 1)
            pwm_pair_config.pwm[1].fault_mode = pwm_fault_mode_force_output_1;
        else if (pwm_pair_t->pwm_fault[1].fault_level == 2)
            pwm_pair_config.pwm[1].fault_mode = pwm_fault_mode_force_output_highz;

        if (pwm_pair_t->pwm_fault[1].recovery_trigger == 0)
            pwm_pair_config.pwm[1].fault_recovery_trigger = pwm_fault_recovery_immediately;
        else if (pwm_pair_t->pwm_fault[1].recovery_trigger == 1)
            pwm_pair_config.pwm[1].fault_recovery_trigger = pwm_fault_recovery_on_reload;
        else if (pwm_pair_t->pwm_fault[1].recovery_trigger == 2)
            pwm_pair_config.pwm[1].fault_recovery_trigger = pwm_fault_recovery_on_hw_event;
        else if (pwm_pair_t->pwm_fault[1].recovery_trigger == 3)
            pwm_pair_config.pwm[1].fault_recovery_trigger = pwm_fault_recovery_on_fault_clear;

        pwm_fault_src_cfg.source_mask = MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.fault_mask;

        pwm_fault_src_cfg.fault_recover_at_rising_edge =  MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.ex_fault.fault_level;
        pwm_fault_src_cfg.fault_output_recovery_trigger = MP_PWM_FAULT_CMP; //hw_event, 比较器选择

        pwm_config_fault_source(PWM_PAIR_BASE, &pwm_fault_src_cfg);
    }

    cmp_index = MP_PWM_PAIR_MAP_CTX[map_index].cmp;
    pin_index = MP_PWM_PAIR_MAP_CTX[map_index].pin;

    if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_REVERSE)
    {
        if((cmp_index + 1) >= MP_PWM_FAULT_CMP || (cmp_index + 1) >= MP_PWM_SHADOW_CMP)
        {
            printf("Error: cmp index+1 >= fault cmp or shadow cmp!(cmp index + 1 must < 14)\n");
            return -3;
        }
        if (status_success != pwm_setup_waveform_in_pair(PWM_PAIR_BASE, pin_index, &pwm_pair_config, cmp_index, &cmp_config[0], 2))
        {
            printf("failed to setup waveform 1\n");
            return -3;
        }
    }
    else if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_CENTRAL)
    {
        if((cmp_index + 3) >= MP_PWM_FAULT_CMP || (cmp_index + 3) >= MP_PWM_SHADOW_CMP)
        {
            printf("Error: cmp index+3 >= fault cmp or shadow cmp!(cmp index + 3 must < 14)\n");
            return -3;
        }
        if (status_success != pwm_setup_waveform(PWM_PAIR_BASE, pin_index, &pwm_config[0], cmp_index, &cmp_config[0], 2))
        {
            printf("failed to setup waveform 4\n");
            return -3;
        }

        if (status_success != pwm_setup_waveform(PWM_PAIR_BASE, pin_index + 1, &pwm_config[1], cmp_index + 2, &cmp_config[0], 2))
        {
            printf("failed to setup waveform 5\n");
            return -3;
        }
    }
    else if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_EDGE)
    {
        if((cmp_index + 1) >= MP_PWM_FAULT_CMP || (cmp_index + 1) >= MP_PWM_SHADOW_CMP)
        {
            printf("Error: cmp index+1 >= fault cmp or shadow cmp!(cmp index + 1 must < 14)\n");
            return -3;
        }
        if (status_success != pwm_setup_waveform(PWM_PAIR_BASE, pin_index, &pwm_config[0], cmp_index, &cmp_config[0], 1))
        {
            printf("failed to setup waveform 6\n");
            return -3;
        }
        if (status_success != pwm_setup_waveform(PWM_PAIR_BASE, pin_index + 1, &pwm_config[1], cmp_index + 1, &cmp_config[0], 1))
        {
            printf("failed to setup waveform 7\n");
            return -3;
        }
    }

    for (i = 0; i < pwm_pair_t->trigger_count; i++)
    {
        if (pwm_pair_t->trigger_enable & (1 << i))
        {
            cmp_config[2].mode = pwm_cmp_mode_output_compare;
            cmp_config[2].cmp = pwm_pair_t->trigger_dutycount[i];
            cmp_config[2].update_trigger = pwm_shadow_register_update_on_modify;

            cmp_index = MP_PWM_PAIR_MAP_CTX[map_index].pwm_trigger_map[i].trgm_cmp;
            pin_index = MP_PWM_PAIR_MAP_CTX[map_index].pwm_trigger_map[i].trgm_pin;
            if((cmp_index) >= MP_PWM_FAULT_CMP || (cmp_index) >= MP_PWM_SHADOW_CMP)
            {
                printf("Error:trigger cmp index >= fault cmp or shadow cmp!(cmp index must < 14)\n");
                return -4;
            }
            pwm_config_cmp(PWM_PAIR_BASE, cmp_index, &cmp_config[2]);
            /* Set comparator channel for trigger a */
            pwm_output_ch_cfg.cmp_start_index = cmp_index;
            pwm_output_ch_cfg.cmp_end_index = cmp_index;
            pwm_output_ch_cfg.invert_output = false;
            pwm_config_output_channel(PWM_PAIR_BASE, pin_index, &pwm_output_ch_cfg);
        }
    }

    cmp_config[3].mode = pwm_cmp_mode_output_compare;
    cmp_config[3].cmp = pwm_pair_t->shadow_dutycount;
    cmp_config[3].update_trigger = pwm_shadow_register_update_on_modify;
    cmp_index = MP_PWM_SHADOW_CMP;
    pwm_load_cmp_shadow_on_match(PWM_PAIR_BASE, cmp_index, &cmp_config[3]);

    pwm_set_load_counter_shadow_register_trigger(PWM_PAIR_BASE, pwm_shadow_register_update_on_hw_event, cmp_index);

    pwm_start_counter(PWM_PAIR_BASE);
    // pwm_issue_shadow_register_lock_event(PWM_PAIR_BASE);
    return 0;
}

int mp_pwm_config(hpm_pwm_t* pwm_t)
{
    int i;
    uint8_t cmp_index, pin_index, map_index;

    pwm_cmp_config_t cmp_config[4] = { 0 };
    pwm_config_t pwm_config = { 0 };
    pwm_output_channel_t pwm_output_ch_cfg;
    pwm_fault_source_config_t pwm_fault_src_cfg;
    TRGM_Type *trgm_t = NULL;

    if (NULL == pwm_t)
    {
        return -1;
    }
    map_index = pwm_t->pwm_map_index;

    pwm_stop_counter(PWM_BASE);

    pwm_enable_reload_at_synci(PWM_BASE);
    if (PWM_BASE == HPM_PWM0)
    {
        clock_add_to_group(clock_mot0, 0);
        trgm_t = HPM_TRGM0;
    }
#ifdef HPM_TRGM1
    else if (PWM_BASE == HPM_PWM1)
    {
        clock_add_to_group(clock_mot1, 0);
        trgm_t = HPM_TRGM1;
    }
#endif
#ifdef HPM_PWM2
    else if (PWM_BASE == HPM_PWM2)
    {
        clock_add_to_group(clock_mot2, 0);
        trgm_t = HPM_TRGM2;
    }
#endif
#ifdef HPM_PWM3
    else if (PWM_BASE == HPM_PWM3)
    {
        clock_add_to_group(clock_mot3, 0);
        trgm_t = HPM_TRGM3;
    }
#endif
    else
    {
        return -2;
    }
#ifdef TRGM_TRGOCFG_PWM_SYNCI
    trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM_SYNCI, 1);
    trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM_SYNCI, 0);
#else
    if (PWM_BASE == HPM_PWM0)
    {
        trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM0_SYNCI, 1);
        trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM0_SYNCI, 0);
    }
    else if (PWM_BASE == HPM_PWM1)
    {
        trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM1_SYNCI, 1);
        trgm_output_update_source(trgm_t, TRGM_TRGOCFG_PWM1_SYNCI, 0);
    }
    else
        return -5;
#endif
    /*
    * reload and start counter
    */
    pwm_set_reload(PWM_BASE, 0, pwm_t->reload_count);
    pwm_set_start_count(PWM_BASE, 0, 0);

    /*
    * config cmp1 and cmp2
    */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = pwm_t->duty_count;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    pwm_get_default_pwm_config(PWM_BASE, &pwm_config);

    pwm_config.enable_output = false;
    pwm_config.invert_output = false;

    if (pwm_t->fault_enable)
    {
        if (pwm_t->pwm_fault.fault_level == 0)
            pwm_config.fault_mode = pwm_fault_mode_force_output_0;
        else if (pwm_t->pwm_fault.fault_level == 1)
            pwm_config.fault_mode = pwm_fault_mode_force_output_1;
        else if (pwm_t->pwm_fault.fault_level == 2)
            pwm_config.fault_mode = pwm_fault_mode_force_output_highz;

        if (pwm_t->pwm_fault.recovery_trigger == 0)
            pwm_config.fault_recovery_trigger = pwm_fault_recovery_immediately;
        else if (pwm_t->pwm_fault.recovery_trigger == 1)
            pwm_config.fault_recovery_trigger = pwm_fault_recovery_on_reload;
        else if (pwm_t->pwm_fault.recovery_trigger == 2)
            pwm_config.fault_recovery_trigger = pwm_fault_recovery_on_hw_event;
        else if (pwm_t->pwm_fault.recovery_trigger == 3)
            pwm_config.fault_recovery_trigger = pwm_fault_recovery_on_fault_clear;

        pwm_fault_src_cfg.source_mask = MP_PWM_MAP_CTX[map_index].pwm_fault_map.fault_mask;

        pwm_fault_src_cfg.fault_recover_at_rising_edge = MP_PWM_MAP_CTX[map_index].pwm_fault_map.ex_fault.fault_level;
        pwm_fault_src_cfg.fault_output_recovery_trigger = MP_PWM_FAULT_CMP; //hw_event, 比较器选择，默认19

        pwm_config_fault_source(PWM_BASE, &pwm_fault_src_cfg);
    }

    cmp_index = MP_PWM_MAP_CTX[map_index].cmp;
    pin_index = MP_PWM_MAP_CTX[map_index].pin;
    if((cmp_index) >= MP_PWM_FAULT_CMP || (cmp_index) >= MP_PWM_SHADOW_CMP)
    {
        printf("Error: cmp index >= fault cmp or shadow cmp!(cmp index must < 14)\n");
        return -2;
    }

    if (status_success != pwm_setup_waveform(PWM_BASE, pin_index, &pwm_config, cmp_index, &cmp_config[0], 1))
    {
        printf("failed to setup waveform\n");
        return -2;
    }

    for (i = 0; i < pwm_t->trigger_count; i++)
    {
        if (pwm_t->trigger_enable & (1 << i))
        {
            cmp_config[1].mode = pwm_cmp_mode_output_compare;
            cmp_config[1].cmp = pwm_t->trigger_dutycount[i];
            cmp_config[1].update_trigger = pwm_shadow_register_update_on_modify;

            cmp_index = MP_PWM_MAP_CTX[map_index].pwm_trigger_map[i].trgm_cmp;
            pin_index = MP_PWM_MAP_CTX[map_index].pwm_trigger_map[i].trgm_pin;
            if((cmp_index) >= MP_PWM_FAULT_CMP || (cmp_index) >= MP_PWM_SHADOW_CMP)
            {
                printf("Error: cmp index >= fault cmp or shadow cmp!(cmp index must < 14)\n");
                return -3;
            }
            pwm_config_cmp(PWM_BASE, cmp_index, &cmp_config[1]);
            /* Set comparator channel for trigger a */
            pwm_output_ch_cfg.cmp_start_index = cmp_index;
            pwm_output_ch_cfg.cmp_end_index = cmp_index;
            pwm_output_ch_cfg.invert_output = false;
            pwm_config_output_channel(PWM_BASE, pin_index, &pwm_output_ch_cfg);
        }
    }

    cmp_config[2].mode = pwm_cmp_mode_output_compare;
    cmp_config[2].cmp = pwm_t->shadow_dutycount;
    cmp_config[2].update_trigger = pwm_shadow_register_update_on_modify;
    cmp_index = MP_PWM_SHADOW_CMP;
    pwm_load_cmp_shadow_on_match(PWM_BASE, cmp_index, &cmp_config[2]);

    pwm_set_load_counter_shadow_register_trigger(PWM_BASE, pwm_shadow_register_update_on_hw_event, cmp_index);

    pwm_start_counter(PWM_BASE);
    // pwm_issue_shadow_register_lock_event(PWM_BASE);
    return 0;
}

int mp_pwm_pair_output_enable(hpm_pwm_pair_t* pwm_pair_t, bool enable)
{
    if (NULL == pwm_pair_t)
    {
        return -1;
    }
    if (enable)
    {
        pwm_enable_output(PWM_PAIR_BASE, PWM_PAIR_PIN);
        pwm_enable_output(PWM_PAIR_BASE, PWM_PAIR_PIN + 1);
    }
    else
    {
        pwm_disable_output(PWM_PAIR_BASE, PWM_PAIR_PIN);
        pwm_disable_output(PWM_PAIR_BASE, PWM_PAIR_PIN + 1);
    }
    return 0;
}

int mp_pwm_output_enable(hpm_pwm_t* pwm_t, bool enable)
{
    if (NULL == pwm_t)
    {
        return -1;
    }
    if (enable)
    {
        pwm_enable_output(PWM_BASE, PWM_PIN);
    }
    else
    {
        pwm_disable_output(PWM_BASE, PWM_PIN);
    }
    return 0;
}

int mp_pwm_enable_counter(hpm_pwm_t* pwm_t, bool enable)
{
    if (NULL == pwm_t)
    {
        return -1;
    }
    uint8_t map_index;
    TRGM_Type* trgm_x;

    map_index = pwm_t->pwm_map_index;
    trgm_x = MP_PWM_MAP_CTX[map_index].pwm_trigger_map[MP_PWM_TRIGGER_ADC_BIT].trgm_base;
    if (enable)
    {
        pwm_start_counter(PWM_BASE);
    }
    else
    {
        pwm_stop_counter(PWM_BASE);
        pwm_enable_reload_at_synci(PWM_BASE);
#ifdef TRGM_TRGOCFG_PWM_SYNCI
        trgm_output_update_source(trgm_x, TRGM_TRGOCFG_PWM_SYNCI, 1);
        trgm_output_update_source(trgm_x, TRGM_TRGOCFG_PWM_SYNCI, 0);
#else
        if (PWM_BASE == HPM_PWM0)
        {
            trgm_output_update_source(trgm_x, TRGM_TRGOCFG_PWM0_SYNCI, 1);
            trgm_output_update_source(trgm_x, TRGM_TRGOCFG_PWM0_SYNCI, 0);
        }
        else if (PWM_BASE == HPM_PWM1)
        {
            trgm_output_update_source(trgm_x, TRGM_TRGOCFG_PWM1_SYNCI, 1);
            trgm_output_update_source(trgm_x, TRGM_TRGOCFG_PWM1_SYNCI, 0);
        }
        else
            return -2;
#endif
    }
    return 0;
}

int hpm_mp_api_pwmpair_force_config(hpm_pwm_pair_t* pwm_pair_t, uint8_t index, uint8_t level)
{
    if (NULL == pwm_pair_t)
    {
        return -1;
    }

    pwm_config_force_cmd_timing(PWM_PAIR_BASE, pwm_force_at_reload);
    pwm_disable_pwm_sw_force_output(PWM_PAIR_BASE, PWM_PAIR_PIN + index);
    pwm_set_force_output(PWM_PAIR_BASE,
        PWM_FORCE_OUTPUT((PWM_PAIR_PIN + index), level ? pwm_output_1 : pwm_output_0));
    pwm_enable_sw_force(PWM_PAIR_BASE);
    return 0;
}

int hpm_mp_api_pwm_force_config(hpm_pwm_t* pwm_t, uint8_t level)
{
    if (NULL == pwm_t)
    {
        return -1;
    }

    pwm_config_force_cmd_timing(PWM_BASE, pwm_force_at_reload);
    pwm_disable_pwm_sw_force_output(PWM_BASE, PWM_PIN);
    pwm_set_force_output(PWM_BASE, PWM_FORCE_OUTPUT((PWM_PIN), level ? pwm_output_1 : pwm_output_0));
    pwm_enable_sw_force(PWM_BASE);
    return 0;
}

void hpm_mp_api_pwmpair_force_enable_output(hpm_pwm_pair_t* pwm_pair_t, uint8_t index)
{
    pwm_enable_pwm_sw_force_output(PWM_PAIR_BASE, PWM_PAIR_PIN + index);
}

void hpm_mp_api_pwm_force_enable_output(hpm_pwm_t* pwm_t)
{
    pwm_enable_pwm_sw_force_output(PWM_BASE, PWM_PIN);
}

void hpm_mp_api_pwmpair_force_disable_output(hpm_pwm_pair_t* pwm_pair_t, uint8_t index)
{
    pwm_disable_pwm_sw_force_output(PWM_PAIR_BASE, PWM_PAIR_PIN + index);
}

void hpm_mp_api_pwm_force_disable_output(hpm_pwm_t* pwm_t)
{
    pwm_disable_pwm_sw_force_output(PWM_BASE, PWM_PIN);
}

int mp_pwm_fault_mux_config(uint8_t type, void* handle)
{
    hpm_pwm_pair_t* pwm_pair_t = NULL;
    hpm_pwm_t* pwm_t = NULL;
    uint8_t map_index, input_ch, output_ch;
    TRGM_Type* trgm_x;
    trgm_output_t trgm_output_cfg;
    bool invert = false;
    if (NULL == handle)
        return -1;
    if (type == MP_PWM_SINGLE_TYPE)
    {
        pwm_t = (hpm_pwm_t*)handle;
        if (!pwm_t->fault_enable)
            return 0;
        map_index = pwm_t->pwm_map_index;
        if (MP_PWM_MAP_CTX[map_index].pwm_fault_map.fault_mask == pwm_fault_source_external_0 ||
            MP_PWM_MAP_CTX[map_index].pwm_fault_map.fault_mask == pwm_fault_source_external_1)
            return 0;
        
        trgm_x = MP_PWM_MAP_CTX[map_index].pwm_fault_map.in_fault.trgm_base;
        input_ch = MP_PWM_MAP_CTX[map_index].pwm_fault_map.in_fault.trgm_input;
        output_ch = MP_PWM_MAP_CTX[map_index].pwm_fault_map.in_fault.trgm_output;
        invert = MP_PWM_MAP_CTX[map_index].pwm_fault_map.in_fault.fault_level == 1 ? false : true;
    }
    else if (type == MP_PWM_PAIR_TYPE)
    {
        pwm_pair_t = (hpm_pwm_pair_t*)handle;
        if (!pwm_pair_t->fault_enable)
            return 0;
        map_index = pwm_pair_t->pwm_map_index;
        if (MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.fault_mask == pwm_fault_source_external_0 ||
            MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.fault_mask == pwm_fault_source_external_1)
            return 0;
        trgm_x = MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.in_fault.trgm_base;
        input_ch = MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.in_fault.trgm_input;
        output_ch = MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.in_fault.trgm_output;
        invert = MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.in_fault.fault_level == 1 ? false : true;
    }
    else
        return -2;

    trgm_output_cfg.invert = invert;
    trgm_output_cfg.type = trgm_output_same_as_input;
    trgm_output_cfg.input = input_ch;
    trgm_output_config(trgm_x, output_ch, &trgm_output_cfg);
    return 0;
}

void hpm_mp_api_pwmpair_fault_software_clear(hpm_pwm_pair_t* pwm_pair_t)
{
    pwm_clear_fault(MP_PWM_PAIR_MAP_CTX[pwm_pair_t->pwm_map_index].pwm_x);
}

void hpm_mp_api_pwm_fault_software_clear(hpm_pwm_t* pwm_t)
{
    pwm_clear_fault(MP_PWM_MAP_CTX[pwm_t->pwm_map_index].pwm_x);
}

int mp_pwm_trigger_mux_config(uint8_t type, void* handle)
{
    int i;
    hpm_pwm_pair_t* pwm_pair_t = NULL;
    hpm_pwm_t* pwm_t = NULL;
    uint8_t map_index, input_ch, output_ch;
    TRGM_Type* trgm_x;
    trgm_output_t trgm_output_cfg;
    if (NULL == handle)
        return -1;
    if (type == MP_PWM_SINGLE_TYPE)
    {
        pwm_t = (hpm_pwm_t*)handle;
        if (pwm_t->trigger_enable == 0 || pwm_t->trigger_count == 0)
            return 0;
        map_index = pwm_t->pwm_map_index;
        for (i = 0; i < pwm_t->trigger_count; i++)
        {
            if (pwm_t->trigger_enable & (1 << i))
            {
                trgm_x = MP_PWM_MAP_CTX[map_index].pwm_trigger_map[i].trgm_base;
                input_ch = MP_PWM_MAP_CTX[map_index].pwm_trigger_map[i].trgm_input;
                output_ch = MP_PWM_MAP_CTX[map_index].pwm_trigger_map[i].trgm_output;
                if (MP_PWM_MAP_CTX[map_index].pwm_trigger_map[i].mode == MP_PWM_TRIGGER_MODE_OUTPUT)
                {
                    trgm_output_cfg.invert = false;
                    trgm_output_cfg.type = trgm_output_pulse_at_input_falling_edge;
                    trgm_output_cfg.input = input_ch;
                    trgm_output_config(trgm_x, output_ch, &trgm_output_cfg);
                }
                else if (MP_PWM_MAP_CTX[map_index].pwm_trigger_map[i].mode == MP_PWM_TRIGGER_MODE_DMA)
                {
                    trgm_dma_request_config(trgm_x, input_ch, output_ch);
                }
                else
                {
                    return -2;
                }
            }
        }
    }
    else if (type == MP_PWM_PAIR_TYPE)
    {
        pwm_pair_t = (hpm_pwm_pair_t*)handle;
        if (pwm_pair_t->trigger_enable == 0 || pwm_pair_t->trigger_count == 0)
            return 0;
        map_index = pwm_pair_t->pwm_map_index;
        for (i = 0; i < pwm_pair_t->trigger_count; i++)
        {
            if (pwm_pair_t->trigger_enable & (1 << i))
            {
                trgm_x = MP_PWM_PAIR_MAP_CTX[map_index].pwm_trigger_map[i].trgm_base;
                input_ch = MP_PWM_PAIR_MAP_CTX[map_index].pwm_trigger_map[i].trgm_input;
                output_ch = MP_PWM_PAIR_MAP_CTX[map_index].pwm_trigger_map[i].trgm_output;
                if (MP_PWM_PAIR_MAP_CTX[map_index].pwm_trigger_map[i].mode == MP_PWM_TRIGGER_MODE_OUTPUT)
                {
                    trgm_output_cfg.invert = false;
                    trgm_output_cfg.type = trgm_output_pulse_at_input_falling_edge;
                    trgm_output_cfg.input = input_ch;
                    trgm_output_config(trgm_x, output_ch, &trgm_output_cfg);
                }
                else if (MP_PWM_PAIR_MAP_CTX[map_index].pwm_trigger_map[i].mode == MP_PWM_TRIGGER_MODE_DMA)
                {
                    trgm_dma_request_config(trgm_x, input_ch, output_ch);
                }
                else
                {
                    return -2;
                }
            }
        }
    }
    else
    {
        return -3;
    }
    return 0;
}

const mp_pwm_trigger_map_t* mp_pwm_get_trigger_map(uint8_t type, uint8_t index)
{
    uint32_t i;
    const mp_pwm_trigger_map_t* map_t = NULL;
    if (type == MP_PWM_SINGLE_TYPE)
    {
        for (i = 0; i < MP_PWM_MAP_COUNT; i++)
        {
            if (MP_PWM_MAP_CTX[i].index == index)
            {
                map_t = MP_PWM_MAP_CTX[i].pwm_trigger_map;
                break;
            }
        }
    }
    else if (type == MP_PWM_PAIR_TYPE)
    {
        for (i = 0; i < MP_PWM_PAIR_MAP_COUNT; i++)
        {
            if (MP_PWM_PAIR_MAP_CTX[i].pair_index == index)
            {
                map_t = MP_PWM_PAIR_MAP_CTX[i].pwm_trigger_map;
                break;
            }
        }
    }
        
    return map_t;
}

//注意，PWM相位同步只支持不同的PWM控制器，
int mp_pwm_sync_config(hpm_mp_t* mp_t)
{
    int i = 0, pwm_index=0;
    PWM_Type* pwm_x;
    TRGM_Type* trgm_x;
    trgm_output_t config;
    if (NULL == mp_t)
        return -1;
    if (mp_t->pwm_pair_count <= 1)//mp_t->pwm_sync_time_us == 0 || 
        return 0;

    synt_enable_counter(HPM_SYNT, false);
    synt_reset_counter(HPM_SYNT);
    synt_set_reload(HPM_SYNT, 0xffffffff);

    for (i = 0; i < mp_t->pwm_pair_count; i++)
    {
        pwm_x = MP_PWM_PAIR_MAP_CTX[mp_t->pwm_pair[i].pwm_map_index].pwm_x;
        pwm_enable_reload_at_synci(pwm_x);
        if (MP_PWM_PAIR_MAP_CTX[mp_t->pwm_pair[i].pwm_map_index].pwm_x == HPM_PWM0)
        {
            pwm_index = 0;
            config.invert = false;
#ifdef HPM_TRGM0_INPUT_SRC_SYNT0_CH0
            config.input = HPM_TRGM0_INPUT_SRC_SYNT0_CH0;
#else
            config.input = HPM_TRGM0_INPUT_SRC_SYNT_CH0;
#endif
            config.type = trgm_output_same_as_input;

#ifdef TRGM_TRGOCFG_PWM_SYNCI
            trgm_output_config(HPM_TRGM0, TRGM_TRGOCFG_PWM_SYNCI, &config);
#else
            trgm_output_config(HPM_TRGM0, TRGM_TRGOCFG_PWM0_SYNCI, &config);
#endif
            if(mp_t->pwm_sync_time_us == 0)
            {
                synt_set_comparator(HPM_SYNT, SYNT_CMP_0, 1);
            }
            else
            {
                synt_set_comparator(HPM_SYNT, SYNT_CMP_0, mp_t->pwm_sync_time_us);
            }
        }
        else if (MP_PWM_PAIR_MAP_CTX[mp_t->pwm_pair[i].pwm_map_index].pwm_x == HPM_PWM1)
        {
            pwm_index = 1;
            if(mp_t->pwm_sync_time_us == 0)
            {
                sync_time_max = 1;
            }
            else
            {
                sync_time_max = mp_t->pwm_sync_time_us * 2;
            }

            config.invert = false;
#ifdef HPM_TRGM1

            trgm_x = HPM_TRGM1;
#ifdef HPM_TRGM1_INPUT_SRC_SYNT1_CH0
            config.input = HPM_TRGM1_INPUT_SRC_SYNT1_CH0;
#elif HPM_TRGM1_INPUT_SRC_SYNT0_CH0
            config.input = HPM_TRGM1_INPUT_SRC_SYNT0_CH0;
#else
            config.input = HPM_TRGM1_INPUT_SRC_SYNT_CH0;
#endif

#else

           trgm_x = HPM_TRGM0;
           config.input = HPM_TRGM0_INPUT_SRC_SYNT0_CH1;

#endif

            config.type = trgm_output_same_as_input;
#ifdef TRGM_TRGOCFG_PWM_SYNCI
            trgm_output_config(trgm_x, TRGM_TRGOCFG_PWM_SYNCI, &config);
#else
            trgm_output_config(trgm_x, TRGM_TRGOCFG_PWM1_SYNCI, &config);
#endif
            synt_set_comparator(HPM_SYNT, SYNT_CMP_1, sync_time_max);
        }
#ifdef HPM_PWM2
        else if (MP_PWM_PAIR_MAP_CTX[mp_t->pwm_pair[i].pwm_map_index].pwm_x == HPM_PWM2)
        {
            pwm_index = 2;
            if(mp_t->pwm_sync_time_us == 0)
            {
                sync_time_max = 1;
            }
            else
            {
                sync_time_max = mp_t->pwm_sync_time_us * 3;
            }
            config.invert = false;
            config.input = HPM_TRGM2_INPUT_SRC_SYNT_CH0;
            config.type = trgm_output_same_as_input;
            trgm_output_config(HPM_TRGM2, TRGM_TRGOCFG_PWM_SYNCI, &config);

            synt_set_comparator(HPM_SYNT, SYNT_CMP_2, sync_time_max);
        }
#endif
#ifdef HPM_PWM3
        else if (MP_PWM_PAIR_MAP_CTX[mp_t->pwm_pair[i].pwm_map_index].pwm_x == HPM_PWM3)
        {
            pwm_index = 3;
            if(mp_t->pwm_sync_time_us == 0)
            {
                sync_time_max = 1;
            }
            else
            {
                sync_time_max = mp_t->pwm_sync_time_us * 4;
            }
            config.invert = false;
            config.input = HPM_TRGM3_INPUT_SRC_SYNT_CH0;
            config.type = trgm_output_same_as_input;
            trgm_output_config(HPM_TRGM3, TRGM_TRGOCFG_PWM_SYNCI, &config);

            synt_set_comparator(HPM_SYNT, SYNT_CMP_3, sync_time_max);
        }
#endif
    }

    mp_sync_done = 0;

    if(pwm_index == 0)
    {
        intc_m_enable_irq_with_priority(IRQn_PWM0, 1);
        pwm_enable_irq(HPM_PWM0, PWM_IRQ_RELOAD);
    }
    else if(pwm_index == 1)
    {
        intc_m_enable_irq_with_priority(IRQn_PWM1, 1);
        pwm_enable_irq(HPM_PWM1, PWM_IRQ_RELOAD);
    }
#ifdef HPM_PWM2
    else if(pwm_index == 2)
    {
        intc_m_enable_irq_with_priority(IRQn_PWM2, 1);
        pwm_enable_irq(HPM_PWM2, PWM_IRQ_RELOAD);
    }
#endif
#ifdef HPM_PWM3
    else if(pwm_index == 3)
    {
        intc_m_enable_irq_with_priority(IRQn_PWM3, 1);
        pwm_enable_irq(HPM_PWM3, PWM_IRQ_RELOAD);
    }
#endif
    synt_enable_counter(HPM_SYNT, true);
    
    while (mp_sync_done == 0) {}

    printf("sync done!\n");

    return 0;
}

static hpm_stat_t pwm_update_raw_cmp_central_aligned_t(PWM_Type* pwm_x, uint8_t cmp1_index,
    uint8_t cmp2_index, uint32_t target_cmp1, uint32_t target_cmp2)
{
    //uint32_t reload = PWM_RLD_RLD_GET(pwm_x->RLD);
    //if (!target_cmp1)
    //{
    //    target_cmp1 = reload + 1;
    //}
    //if (!target_cmp2)
    //{
    //    target_cmp2 = reload + 1;
    //}
    // pwm_shadow_register_unlock(pwm_x);
    pwm_cmp_update_cmp_value(pwm_x, cmp1_index, target_cmp1, 0);
    pwm_cmp_update_cmp_value(pwm_x, cmp2_index, target_cmp2, 0);
    return status_success;
}

static int mp_pwm_pair_duty_convert(hpm_pwm_pair_t* pwm_pair_t, uint32_t o_duty_array[4])
{
    if (NULL == pwm_pair_t)
    {
        return -1;
    }
    if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_CENTRAL)
    {
        if (pwm_pair_t->phase_angle == MP_PWM_PAIR_PHASE_ANGLE_0)
        {
            o_duty_array[0] = o_duty_array[2] = (pwm_pair_t->reload_count - pwm_pair_t->duty_count) >> 1;
            o_duty_array[1] = o_duty_array[3] = (pwm_pair_t->reload_count + pwm_pair_t->duty_count) >> 1;
        }
        else if (pwm_pair_t->phase_angle == MP_PWM_PAIR_PHASE_ANGLE_180 || pwm_pair_t->phase_angle == MP_PWM_PAIR_PHASE_ANGLE_120)
        {
            o_duty_array[0] = (uint32_t)(((float)(pwm_pair_t->reload_count - pwm_pair_t->duty_count) / 2) + 0.5) - \
                (uint32_t)(((float)pwm_pair_t->reload_count / 4) + 0.5);
            if ((int32_t)o_duty_array[0] < 1)
                o_duty_array[0] = 1;
            o_duty_array[1] = (uint32_t)(((float)(pwm_pair_t->reload_count + pwm_pair_t->duty_count) / 2) + 0.5) - \
                (uint32_t)(((float)pwm_pair_t->reload_count / 4) + 0.5);
            o_duty_array[2] = (o_duty_array[0] + (pwm_pair_t->reload_count / 2));
            if (o_duty_array[1] != o_duty_array[2] && abs((int)(o_duty_array[1] - o_duty_array[2])) <= 1)
            {
                o_duty_array[1] = o_duty_array[2];
            }
            o_duty_array[3] = (o_duty_array[1] + (pwm_pair_t->reload_count / 2));
            if (o_duty_array[3] >= pwm_pair_t->reload_count)
                o_duty_array[3] = pwm_pair_t->reload_count - 1;
        }
        else
        {
            o_duty_array[0] = (pwm_pair_t->reload_count - pwm_pair_t->duty_count) >> 1;
            o_duty_array[1] = (pwm_pair_t->reload_count + pwm_pair_t->duty_count) >> 1;
            o_duty_array[2] = (o_duty_array[0] + (pwm_pair_t->reload_count / pwm_pair_t->phase_angle));
            o_duty_array[3] = (o_duty_array[1] + (pwm_pair_t->reload_count / pwm_pair_t->phase_angle));
        }
    }
    else if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_EDGE)
    {
        if (pwm_pair_t->phase_angle == MP_PWM_PAIR_PHASE_ANGLE_0)
        {
            o_duty_array[0] = pwm_pair_t->duty_count;
        }
        else
        {
            o_duty_array[0] = pwm_pair_t->duty_count + (pwm_pair_t->duty_count / pwm_pair_t->phase_angle);
        }
    }
    return 0;
}

uint32_t hpm_mp_api_pwmpair_get_reload_of_freq(hpm_pwm_pair_t* pwm_pair_t, uint32_t freq)
{
    if (NULL == pwm_pair_t || freq == 0)
    {
        return 0;
    }
    return (uint32_t)(pwm_pair_t->bus_freq / freq - 1);
}

int hpm_mp_api_pwmpair_set_reload_and_duty(hpm_pwm_pair_t* pwm_pair_t, uint32_t reload_count, uint32_t duty_count)
{
    uint32_t temp_duty[4] = {0};
    if (NULL == pwm_pair_t)
    {
        return -1;
    }
    pwm_pair_t->duty_count = duty_count;
    pwm_pair_t->reload_count = reload_count;
    mp_pwm_pair_duty_convert(pwm_pair_t, temp_duty);
    //pwm_shadow_register_lock(PWM_PAIR_BASE);
    pwm_shadow_register_unlock(PWM_PAIR_BASE);

    if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_REVERSE)
    {
        pwm_update_raw_cmp_central_aligned_t(PWM_PAIR_BASE, PWM_PAIR_CMP, PWM_PAIR_CMP + 1, \
            (pwm_pair_t->reload_count - pwm_pair_t->duty_count) >> 1, (pwm_pair_t->reload_count + pwm_pair_t->duty_count) >> 1);
    }
    else if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_CENTRAL)
    {
        pwm_update_raw_cmp_central_aligned_t(PWM_PAIR_BASE, PWM_PAIR_CMP, PWM_PAIR_CMP + 1, \
            temp_duty[0], temp_duty[1]);
        pwm_update_raw_cmp_central_aligned_t(PWM_PAIR_BASE, PWM_PAIR_CMP + 2, PWM_PAIR_CMP + 3, \
            temp_duty[2], temp_duty[3]);
    }
    else if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_EDGE)
    {
        pwm_cmp_update_cmp_value(PWM_PAIR_BASE, PWM_PAIR_CMP, pwm_pair_t->duty_count, 0);
        pwm_cmp_update_cmp_value(PWM_PAIR_BASE, PWM_PAIR_CMP + 1, temp_duty[0], 0);
    }
    PWM_PAIR_BASE->RLD = PWM_RLD_XRLD_SET(0)
        | PWM_RLD_RLD_SET(pwm_pair_t->reload_count);
    //pwm_issue_shadow_register_lock_event(PWM_PAIR_BASE);
    // pwm_set_start_count(PWM_PAIR_BASE, 0, 0);
    return 0;
}

int hpm_mp_api_pwmpair_freq(hpm_pwm_pair_t* pwm_pair_t, uint32_t freq)
{
    uint32_t new_reload, new_duty;
    if (NULL == pwm_pair_t)
    {
        return -1;
    }
    new_reload = hpm_mp_api_pwmpair_get_reload_of_freq(pwm_pair_t, freq);
    new_duty = (uint32_t)(((float)pwm_pair_t->duty_count * (float)new_reload / (float)pwm_pair_t->reload_count) + 0.5);

    return hpm_mp_api_pwmpair_set_reload_and_duty(pwm_pair_t, new_reload, new_duty);
}

int hpm_mp_api_pwmpair_freq_of_half_duty(hpm_pwm_pair_t* pwm_pair_t, uint32_t freq)
{
    uint32_t new_reload, new_cmp;
    if (NULL == pwm_pair_t)
    {
        return -1;
    }

    new_reload = pwm_pair_t->bus_freq / freq - 1;
    pwm_pair_t->duty_count = new_reload / 2;
    pwm_pair_t->reload_count = new_reload;
    //pwm_shadow_register_lock(PWM_PAIR_BASE);
    pwm_shadow_register_unlock(PWM_PAIR_BASE);
    if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_REVERSE)
    {
        pwm_update_raw_cmp_central_aligned_t(PWM_PAIR_BASE, PWM_PAIR_CMP, PWM_PAIR_CMP + 1, \
            (pwm_pair_t->reload_count - pwm_pair_t->duty_count) >> 1, (pwm_pair_t->reload_count + pwm_pair_t->duty_count) >> 1);
    }
    else
    {
        if (pwm_pair_t->phase_angle == MP_PWM_PAIR_PHASE_ANGLE_0)
        {
            pwm_update_raw_cmp_central_aligned_t(PWM_PAIR_BASE, PWM_PAIR_CMP, PWM_PAIR_CMP + 1, \
                (pwm_pair_t->reload_count - pwm_pair_t->duty_count) >> 1, (pwm_pair_t->reload_count + pwm_pair_t->duty_count) >> 1);
            pwm_update_raw_cmp_central_aligned_t(PWM_PAIR_BASE, PWM_PAIR_CMP + 2, PWM_PAIR_CMP + 3, \
                (pwm_pair_t->reload_count - pwm_pair_t->duty_count) >> 1, (pwm_pair_t->reload_count + pwm_pair_t->duty_count) >> 1);
        }
        else if (pwm_pair_t->phase_angle == MP_PWM_PAIR_PHASE_ANGLE_180)
        {
            new_cmp = pwm_pair_t->reload_count / pwm_pair_t->phase_angle;
            pwm_update_raw_cmp_central_aligned_t(PWM_PAIR_BASE, PWM_PAIR_CMP, PWM_PAIR_CMP + 1, \
                1, new_cmp);
            pwm_update_raw_cmp_central_aligned_t(PWM_PAIR_BASE, PWM_PAIR_CMP + 2, PWM_PAIR_CMP + 3, \
                new_cmp, pwm_pair_t->reload_count - 1);
        }
        else if (pwm_pair_t->phase_angle == MP_PWM_PAIR_PHASE_ANGLE_120)
        {
            //do something
        }
        else
        {
            new_cmp = pwm_pair_t->reload_count / pwm_pair_t->phase_angle;
            pwm_update_raw_cmp_central_aligned_t(PWM_PAIR_BASE, PWM_PAIR_CMP, PWM_PAIR_CMP + 1, \
                (pwm_pair_t->reload_count - pwm_pair_t->duty_count) >> 1, (pwm_pair_t->reload_count + pwm_pair_t->duty_count) >> 1);
            pwm_update_raw_cmp_central_aligned_t(PWM_PAIR_BASE, PWM_PAIR_CMP + 2, PWM_PAIR_CMP + 3, \
                ((pwm_pair_t->reload_count - pwm_pair_t->duty_count) >> 1) + new_cmp, \
                ((pwm_pair_t->reload_count + pwm_pair_t->duty_count) >> 1) + new_cmp);
        }
    }

    // pwm_set_reload(PWM_PAIR_BASE, 0, pwm_pair_t->reload_count);
    PWM_PAIR_BASE->RLD = PWM_RLD_XRLD_SET(0)
        | PWM_RLD_RLD_SET(pwm_pair_t->reload_count);
    //pwm_issue_shadow_register_lock_event(PWM_PAIR_BASE);
    // pwm_set_start_count(PWM_PAIR_BASE, 0, 0);
    return 0;
}

int hpm_mp_api_pwm_freq(hpm_pwm_t* pwm_t, uint32_t freq)
{
    uint32_t new_reload;
    if (NULL == pwm_t)
    {
        return -1;
    }

    new_reload = pwm_t->bus_freq / freq - 1;
    pwm_t->duty_count = (float)pwm_t->duty_count * (float)new_reload / (float)pwm_t->reload_count;

    pwm_t->reload_count = new_reload;

    pwm_update_raw_cmp_edge_aligned(PWM_BASE, PWM_CMP, pwm_t->duty_count);

    pwm_set_reload(PWM_BASE, 0, pwm_t->reload_count);
    // pwm_set_start_count(PWM_PAIR_BASE, 0, 0);
    return 0;
}

int hpm_mp_api_pwm_freq_of_half_duty(hpm_pwm_t* pwm_t, uint32_t freq)
{
    uint32_t new_reload;
    if (NULL == pwm_t)
    {
        return -1;
    }

    new_reload = pwm_t->bus_freq / freq - 1;
    pwm_t->duty_count = new_reload >> 1;

    pwm_t->reload_count = new_reload;

    pwm_update_raw_cmp_edge_aligned(PWM_BASE, PWM_CMP, pwm_t->duty_count);

    pwm_set_reload(PWM_BASE, 0, pwm_t->reload_count);
    // pwm_set_start_count(PWM_PAIR_BASE, 0, 0);
    return 0;
}

int hpm_mp_api_pwmpair_duty(hpm_pwm_pair_t* pwm_pair_t, uint32_t duty)
{
    uint32_t temp_duty[4];
    if (NULL == pwm_pair_t)
    {
        return -1;
    }

    pwm_pair_t->duty_count = duty;

    mp_pwm_pair_duty_convert(pwm_pair_t, temp_duty);

    if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_REVERSE)
    {
        pwm_update_raw_cmp_central_aligned(PWM_PAIR_BASE, PWM_PAIR_CMP, PWM_PAIR_CMP + 1, \
            (pwm_pair_t->reload_count - pwm_pair_t->duty_count) >> 1, (pwm_pair_t->reload_count + pwm_pair_t->duty_count) >> 1);
    }
    else if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_CENTRAL)
    {
        pwm_update_raw_cmp_central_aligned(PWM_PAIR_BASE, PWM_PAIR_CMP, PWM_PAIR_CMP + 1, \
            temp_duty[0], temp_duty[1]);
        pwm_update_raw_cmp_central_aligned(PWM_PAIR_BASE, PWM_PAIR_CMP + 2, PWM_PAIR_CMP + 3, \
            temp_duty[2], temp_duty[3]);
    }
    else if (pwm_pair_t->pwm_pair_mode == MP_PWM_PAIR_MODE_EDGE)
    {
        pwm_update_raw_cmp_edge_aligned(PWM_PAIR_BASE, PWM_PAIR_CMP, pwm_pair_t->duty_count);
        pwm_update_raw_cmp_edge_aligned(PWM_PAIR_BASE, PWM_PAIR_CMP + 1, temp_duty[0]);
    }
    return 0;
}

int hpm_mp_api_pwm_duty(hpm_pwm_t* pwm_t, uint32_t duty)
{
    if (NULL == pwm_t)
    {
        return -1;
    }

    pwm_t->duty_count = duty;

    pwm_update_raw_cmp_edge_aligned(PWM_BASE, PWM_CMP, pwm_t->duty_count);

    return 0;
}

int hpm_mp_api_pwm_pair_trig_set_duty(hpm_pwm_pair_t* pwm_pair_t, uint32_t duty)
{
    if (NULL == pwm_pair_t)
    {
        return -1;
    }

    pwm_update_raw_cmp_edge_aligned(PWM_PAIR_BASE, PWM_PAIR_TRIG_CMP, duty);

    return 0;
}

int hpm_mp_api_pwm_trig_set_duty(hpm_pwm_t* pwm_t, uint32_t duty)
{
    if (NULL == pwm_t)
    {
        return -1;
    }

    pwm_update_raw_cmp_edge_aligned(PWM_BASE, PWM_TRIG_CMP, duty);

    return 0;
}


int hpm_mp_api_pwm_pair_set_deadzone(hpm_pwm_pair_t* pwm_pair_t, uint32_t start_halfcycle, uint32_t end_halfcycle)
{
    if (NULL == pwm_pair_t)
    {
        return -1;
    }
    pwm_pair_t->deadzone_start_halfcycle = start_halfcycle;
    pwm_pair_t->deadzone_end_halfcycle = end_halfcycle;

    PWM_PAIR_BASE->PWMCFG[PWM_PAIR_PIN] |= \
        PWM_PWMCFG_DEADAREA_SET(pwm_pair_t->deadzone_start_halfcycle);

    PWM_PAIR_BASE->PWMCFG[PWM_PAIR_PIN + 1] |= \
        PWM_PWMCFG_DEADAREA_SET(pwm_pair_t->deadzone_end_halfcycle);


    return 0;
}


int hpm_mp_api_dma_request_enable(bool enable, uint8_t type, void* handle)
{
    int i;
    hpm_pwm_pair_t* pwm_pair_t = NULL;
    hpm_pwm_t* pwm_t = NULL;
    uint8_t map_index;
    if (NULL == handle)
        return -1;
    if (type == MP_PWM_SINGLE_TYPE)
    {
        pwm_t = (hpm_pwm_t*)handle;
        map_index = pwm_t->pwm_map_index;
        if (pwm_t->trigger_enable == 0 || pwm_t->trigger_count == 0)
            return 0;
        for (i = 0; i < pwm_t->trigger_count; i++)
        {
            if (MP_PWM_MAP_CTX[map_index].pwm_trigger_map[i].mode == MP_PWM_TRIGGER_MODE_DMA)
            {
                if (enable)
                {
                    pwm_enable_dma_request(MP_PWM_MAP_CTX[map_index].pwm_x, \
                        PWM_IRQ_CMP(MP_PWM_MAP_CTX[map_index].pwm_trigger_map[i].trgm_cmp));
                }
                else
                {
                    pwm_disable_dma_request(MP_PWM_MAP_CTX[map_index].pwm_x, \
                        PWM_IRQ_CMP(MP_PWM_MAP_CTX[map_index].pwm_trigger_map[i].trgm_cmp));
                }
            }
        }
    }
    else if (type == MP_PWM_PAIR_TYPE)
    {
        pwm_pair_t = (hpm_pwm_pair_t*)handle;
        map_index = pwm_pair_t->pwm_map_index;
        if (pwm_pair_t->trigger_enable == 0 || pwm_pair_t->trigger_count == 0)
            return 0;
        for (i = 0; i < pwm_pair_t->trigger_count; i++)
        {
            if (MP_PWM_PAIR_MAP_CTX[map_index].pwm_trigger_map[i].mode == MP_PWM_TRIGGER_MODE_DMA)
            {
                if (enable)
                {
                    pwm_enable_dma_request(MP_PWM_PAIR_MAP_CTX[map_index].pwm_x, \
                        PWM_IRQ_CMP(MP_PWM_PAIR_MAP_CTX[map_index].pwm_trigger_map[i].trgm_cmp));
                }
                else
                {
                    pwm_disable_dma_request(MP_PWM_PAIR_MAP_CTX[map_index].pwm_x, \
                        PWM_IRQ_CMP(MP_PWM_PAIR_MAP_CTX[map_index].pwm_trigger_map[i].trgm_cmp));
                }
            }
        }
    }
    else
        return -2;
    return 0;
}

void hpm_mp_api_pair_pwm_io_init(uint8_t pair_index)
{
    uint8_t map_index = mp_pwm_pair_get_mapindex(pair_index);
    HPM_IOC->PAD[MP_PWM_PAIR_MAP_CTX[map_index].io[0].pin].FUNC_CTL = MP_PWM_PAIR_MAP_CTX[map_index].io[0].mux;
    HPM_IOC->PAD[MP_PWM_PAIR_MAP_CTX[map_index].io[1].pin].FUNC_CTL = MP_PWM_PAIR_MAP_CTX[map_index].io[1].mux;
    if(MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.fault_mask == pwm_fault_source_external_0 ||
       MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.fault_mask == pwm_fault_source_external_1)
    {
        HPM_IOC->PAD[MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.ex_fault.pin].FUNC_CTL = MP_PWM_PAIR_MAP_CTX[map_index].pwm_fault_map.ex_fault.mux;
    }
}

void hpm_mp_api_pwm_io_init(uint8_t index)
{
    uint8_t map_index = mp_pwm_get_mapindex(index);
    HPM_IOC->PAD[MP_PWM_MAP_CTX[map_index].io.pin].FUNC_CTL = MP_PWM_MAP_CTX[map_index].io.mux;
}

int hpm_mp_api_pwm_pair_set_map(const mp_pwm_pair_map_t* map_t, int count)
{
    if (map_t == NULL)
        return -1;

    if (MP_PWM_PAIR_MAP_CTX != NULL)
        return -2;

    MP_PWM_PAIR_MAP_CTX = map_t;
    
    MP_PWM_PAIR_MAP_COUNT = count;

    return 0;
}

int hpm_mp_api_pwm_set_map(const mp_pwm_map_t* map_t, int count)
{
    if (map_t == NULL)
        return -1;

    if (MP_PWM_MAP_CTX != NULL)
        return -2;

    MP_PWM_MAP_CTX = map_t;

    MP_PWM_MAP_COUNT = count;

    return 0;
}