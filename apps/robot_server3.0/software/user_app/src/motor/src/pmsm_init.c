/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "pmsm_init.h"
#include "pmsm_speedctrl.h"
#include "hpm_pwmv2_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_gptmr_drv.h"
#include "pmsm_currentctrl.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_adc.h"
#include "hpm_synt_drv.h"
#include "pmsm_define.h"
#include "bldc_foc_cfg.h"
#include "pmsm_currentctrl_svpwm.h"
#include "parm_global.h"
#include "hpm_qeiv2_drv.h"
#include "bldc_foc_callback.h"
#include "hpm_gpio_drv.h"
#include "pmsm_currentctrl_3p3z.h"


MOTOR_CONTROL_Global Motor_Control_Global;
CMDGENE_PARA CMDGENEObj;
MOTOR_PARA motor;
qei_CalObj qeiCalObj;
Para_Obj paraObj[1] = {0};
Para_Obj* pt_para = paraObj;
#if defined (CONFIG_USE_FUNCTION) && (CONFIG_USE_FUNCTION)
INERTIA_CAL_Obj INTERIA_CalHdl;
#endif
function_par func_par;

/**********************************************
 * *void param_init(MOTOR_CONTROL_Global* global, qei_CalObj* qeiCalHdl, MOTOR_PARA* motor)
 * * Important intermediate variable initialization
 * *@Input: Motor status, 1ms_isr_flag, QEI max cnt in 1 r, pmsm ploes
 * *#Output：None
 * ***************************************/
void param_init(MOTOR_CONTROL_Global* global, qei_CalObj* qeiCalHdl, MOTOR_PARA* motor)
{
    global->motor_status = 0x10;
    global->flag_1ms = 1;
    qeiCalHdl->maxph = BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV;//BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV  BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV
    qeiCalHdl->motor_pole = motor->foc_para.motorpar.i_poles_n;
    global->Motor_Interia = 0;
}
/**********************************************
 * *void adc_module_cfg(adc_type* adc_typ,uint8_t adc_module,ADC16_Type* HPM_ADC_BASE)
 * * Important intermediate variable initialization
 * *@Input: Adc base
 * *#Output：None
 * ***************************************/
void adc_module_cfg(adc_type* adc_typ,uint8_t adc_module,ADC16_Type* HPM_ADC_BASE)
{
    adc_typ->adc_base.adc16 = HPM_ADC_BASE;
    adc_typ->module = adc_module;
}
/**********************************************
 * *void pi_param_init(BLDC_CONTRL_PID_PARA *par, float KP, float KI, float MAX)
 * * position, and speed loop pi params init
 * *@Input: PID structure, kp, ki, max init
 * *#Output：None
 * ***************************************/
void pi_param_init(BLDC_CONTRL_PID_PARA *par, float KP, float KI, float MAX)
{
    par->func_pid  = bldc_foc_pi_contrl;
    par->i_kp      = HPM_MOTOR_MATH_FL_MDF(KP);
    par->i_ki      = HPM_MOTOR_MATH_FL_MDF(KI);
    par->i_max     = HPM_MOTOR_MATH_FL_MDF(MAX);
}
/**********************************************
 * *void motor_param_int(MOTOR_PARA *motor_par, uint8_t MOTOR_ID)
 * * pmsm params init
 * *@Input: Motor structure
 * *#Output：None
 * ***************************************/
void motor_param_int(MOTOR_PARA *motor_par, uint8_t MOTOR_ID)
{
#if USE_MOTOR_DM
    motor_par->foc_para.motorpar.i_lstator_h = 0.000305;
    motor_par->foc_para.motorpar.i_maxspeed_rs = 50;
    motor_par->foc_para.motorpar.i_phasecur_a = 3;
    motor_par->foc_para.motorpar.i_phasevol_v = 24;
    motor_par->foc_para.motorpar.i_poles_n = PMSM_ENCODER_POLES;
    motor_par->foc_para.motorpar.i_rstator_ohm = 0.33;
    motor_par->foc_para.motorpar.i_samplingper_s = 0.00005;//  1/PWM_FREQUENCY

    motor_par->foc_para.speedcalpar.i_speedacq = 20;
    motor_par->foc_para.speedcalpar.i_speedfilter = 0.1;
    motor_par->foc_para.speedcalpar.i_speedlooptime_s = HPM_MOTOR_MATH_FL_MDF(0.00005*20);
    motor_par->foc_para.speedcalpar.i_motorpar = &motor_par->foc_para.motorpar;
    motor_par->foc_para.speedcalpar.func_getspd = bldc_foc_al_speed;

    motor_par->foc_para.currentdpipar.i_kp = PMSM_CURRENT_KP;
    motor_par->foc_para.currentdpipar.i_ki = PMSM_CURRENT_KI;
    motor_par->foc_para.currentdpipar.i_kd = 0;
    motor_par->foc_para.currentdpipar.i_max = PMSM_CURRENT_D_OUT_MAX;
    motor_par->foc_para.currentdpipar.func_pid = bldc_foc_pi_contrl;

    motor_par->foc_para.currentqpipar.i_kp = PMSM_CURRENT_KP;
    motor_par->foc_para.currentqpipar.i_ki = PMSM_CURRENT_KI;
    motor_par->foc_para.currentqpipar.i_kd = 0;
    motor_par->foc_para.currentqpipar.i_max = PMSM_CURRENT_Q_OUT_MAX;
    motor_par->foc_para.currentqpipar.func_pid = bldc_foc_pi_contrl;
#else
    motor_par->foc_para.motorpar.i_lstator_h = 0.003;
    motor_par->foc_para.motorpar.i_maxspeed_rs = 50;
    motor_par->foc_para.motorpar.i_phasecur_a = 3;
    motor_par->foc_para.motorpar.i_phasevol_v = 24;
    motor_par->foc_para.motorpar.i_poles_n = 14;
    motor_par->foc_para.motorpar.i_rstator_ohm = 1.1;//1.1
    motor_par->foc_para.motorpar.i_samplingper_s = 0.00005;

    motor_par->foc_para.speedcalpar.i_speedacq = 20;
    motor_par->foc_para.speedcalpar.i_speedfilter = 0.1;
    motor_par->foc_para.speedcalpar.i_speedlooptime_s = HPM_MOTOR_MATH_FL_MDF(0.00005*20);
    motor_par->foc_para.speedcalpar.i_motorpar = &motor_par->foc_para.motorpar;
    motor_par->foc_para.speedcalpar.func_getspd = bldc_foc_al_speed;

    motor_par->foc_para.currentdpipar.i_kp = PMSM_CURRENT_KP;//0.5
    motor_par->foc_para.currentdpipar.i_ki = PMSM_CURRENT_KI;//0.02
    motor_par->foc_para.currentdpipar.i_kd = 0;
    motor_par->foc_para.currentdpipar.i_max = HPM_MOTOR_MATH_FL_MDF(4000);
    motor_par->foc_para.currentdpipar.func_pid = bldc_foc_pi_contrl;

    motor_par->foc_para.currentqpipar.i_kp = PMSM_CURRENT_KP;
    motor_par->foc_para.currentqpipar.i_ki = PMSM_CURRENT_KI;
    motor_par->foc_para.currentqpipar.i_kd = 0;
    motor_par->foc_para.currentqpipar.i_max = HPM_MOTOR_MATH_FL_MDF(7000);
    motor_par->foc_para.currentqpipar.func_pid = bldc_foc_pi_contrl;
#endif
    motor_par->foc_para.pwmpar.func_spwm = bldc_foc_svpwm;
    motor_par->foc_para.pwmpar.i_pwm_reload_max = PWM_RELOAD*0.95;
    motor_par->foc_para.pwmpar.pwmout.func_set_pwm = bldc_foc_pwmset;
    motor_par->foc_para.pwmpar.pwmout.i_pwm_reload = PWM_RELOAD;
    motor_par->foc_para.pwmpar.pwmout.i_motor_id = MOTOR_ID;

    motor_par->foc_para.samplcurpar.func_sampl = bldc_foc_current_cal;
    motor_par->foc_para.func_dqsvpwm =  bldc_foc_ctrl_dq_to_pwm;
    
    motor_par->adc_trig_event_callback = &motor_highspeed_loop;
}
/**********************************************
 * *void pwmv2_duty_init(PWMV2_Type *ptr, uint32_t PWM_PRD, uint8_t CMP_SHADOW_REGISTER_UPDATE_TYPE, uint8_t CMP_PWM_REGISTER_UPDATE_TYPE, uint8_t CMP_SOURCE)
 * * pmsm params init
 * *@Input: Pwm structure, pwm reload, pwm shadow and cmp update type, pwm shadow update source
 * *#Output：None
 * ***************************************/
void pwmv2_duty_init(PWMV2_Type *ptr, uint32_t PWM_PRD, uint8_t CMP_SHADOW_REGISTER_UPDATE_TYPE, uint8_t CMP_PWM_REGISTER_UPDATE_TYPE, uint8_t CMP_SOURCE)
{
    pwmv2_cmp_config_t cmp_cfg[2];
    pwmv2_pair_config_t pwm_cfg;

    cmp_cfg[0].cmp = PWM_PRD;
    cmp_cfg[0].enable_half_cmp = false;
    cmp_cfg[0].enable_hrcmp = false;
    cmp_cfg[0].cmp_source = CMP_SOURCE;
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(1);
    cmp_cfg[0].update_trigger = CMP_SHADOW_REGISTER_UPDATE_TYPE;
    
    cmp_cfg[1].cmp = PWM_PRD;
    cmp_cfg[1].enable_half_cmp = false;
    cmp_cfg[1].enable_hrcmp = false;
    cmp_cfg[1].cmp_source = CMP_SOURCE;
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(2);
    cmp_cfg[1].update_trigger = CMP_SHADOW_REGISTER_UPDATE_TYPE;

    pwm_cfg.pwm[0].enable_output = true;
    pwm_cfg.pwm[0].enable_async_fault = false;
    pwm_cfg.pwm[0].enable_sync_fault = false;
    pwm_cfg.pwm[0].invert_output = false;
    pwm_cfg.pwm[0].enable_four_cmp = false;
    pwm_cfg.pwm[0].update_trigger = CMP_PWM_REGISTER_UPDATE_TYPE;
    pwm_cfg.pwm[0].dead_zone_in_half_cycle = PWM_DEAD_AREA_TICK;
    
    pwm_cfg.pwm[1].enable_output = true;
    pwm_cfg.pwm[1].enable_async_fault = false;
    pwm_cfg.pwm[1].enable_sync_fault = false;
    pwm_cfg.pwm[1].invert_output = false;
    pwm_cfg.pwm[1].enable_four_cmp = false;
    pwm_cfg.pwm[1].update_trigger = CMP_PWM_REGISTER_UPDATE_TYPE;
    pwm_cfg.pwm[1].dead_zone_in_half_cycle = PWM_DEAD_AREA_TICK;

    pwmv2_disable_counter(ptr, pwm_counter_0);
    pwmv2_reset_counter(ptr, pwm_counter_0);

    pwmv2_shadow_register_unlock(ptr);
    pwmv2_set_shadow_val(ptr, PWMV2_SHADOW_INDEX(0), PWM_PRD, 0, false);
    pwmv2_set_shadow_val(ptr, PWMV2_SHADOW_INDEX(10), 1, 0, false);
    pwmv2_shadow_register_lock(ptr);

    pwmv2_setup_waveform_in_pair(ptr, pwm_channel_0, &pwm_cfg, PWMV2_CMP_INDEX(0), &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(3);
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(4);
    pwmv2_setup_waveform_in_pair(ptr, pwm_channel_2, &pwm_cfg, PWMV2_CMP_INDEX(4), &cmp_cfg[0], 2);
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(5);
    cmp_cfg[1].cmp_source_index = PWMV2_SHADOW_INDEX(6);
    pwmv2_setup_waveform_in_pair(ptr, pwm_channel_4, &pwm_cfg, PWMV2_CMP_INDEX(8), &cmp_cfg[0], 2);

    pwmv2_counter_select_data_offset_from_shadow_value(BOARD_PMSM0PWM, pwm_counter_0, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(ptr, pwm_counter_0);
    pwmv2_set_reload_update_time(ptr, pwm_counter_0, CMP_PWM_REGISTER_UPDATE_TYPE);

    pwmv2_counter_select_data_offset_from_shadow_value(BOARD_PMSM0PWM, pwm_counter_1, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(ptr, pwm_counter_1);
    pwmv2_set_reload_update_time(ptr, pwm_counter_1, CMP_PWM_REGISTER_UPDATE_TYPE);

    pwmv2_counter_select_data_offset_from_shadow_value(BOARD_PMSM0PWM, pwm_counter_2, PWMV2_SHADOW_INDEX(0));
    pwmv2_counter_burst_disable(ptr, pwm_counter_2);
    pwmv2_set_reload_update_time(ptr, pwm_counter_2, CMP_PWM_REGISTER_UPDATE_TYPE);
}
/**********************************************
 * *void pwmv2_trigfor_adc_init()
 * * pwm channel trig for adc
 * *@Input: Pwm structure, pwm reload, pwm channel trig point, pwm shadow and cmp update type, pwm shadow update source, adc trig source, pwm trig channel
 * *#Output：None
 * ***************************************/
void pwmv2_trigfor_adc_init(PWMV2_Type *ptr, uint32_t PWM_PRD, uint32_t PWM_CNT, uint8_t CMP_SHADOW_REGISTER_UPDATE_TYPE, uint8_t PWM_TRIGOUT_CH_ADC, uint8_t CMP_SOURCE, uint8_t PWM_CH_TRIG_ADC)
{
    
    pwmv2_cmp_config_t cmp_cfg[2];
    
    pwmv2_shadow_register_unlock(ptr);
    cmp_cfg[0].cmp = PWM_PRD - 1;
    cmp_cfg[0].enable_half_cmp = false;
    cmp_cfg[0].enable_hrcmp = false;
    cmp_cfg[0].cmp_source = CMP_SOURCE;
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(9);
    cmp_cfg[0].update_trigger = CMP_SHADOW_REGISTER_UPDATE_TYPE;
    pwmv2_config_cmp(ptr, PWM_CH_TRIG_ADC, &cmp_cfg[0]);

    pwmv2_select_cmp_source(ptr, PWM_CH_TRIG_ADC, CMP_SOURCE, PWMV2_SHADOW_INDEX(9));
    pwmv2_set_trigout_cmp_index(ptr, PWM_TRIGOUT_CH_ADC, PWM_CH_TRIG_ADC);
    pwmv2_cmp_select_counter(ptr, PWM_CH_TRIG_ADC, PWM_CNT);
    pwmv2_shadow_register_lock(ptr);
}
/**********************************************
 * *void pwmv2_trigfor_currentctrl_init()
 * * pwm channel trig for current ctrl isr
 * *@Input: Pwm structure, pwm reload, pwm channel trig point, pwm shadow and cmp update type, pwm shadow update source, current ctrl trig source, pwm trig channel
 * *#Output：None
 * ***************************************/
void pwmv2_trigfor_currentctrl_init(PWMV2_Type *ptr,uint32_t PWM_CNT, uint8_t CMP_SHADOW_REGISTER_UPDATE_TYPE,uint8_t PWM_TRIGOUT_CH_CUREENTCTRL, uint8_t CMP_SOURCE, uint8_t PWM_CH_TRIG_CUREENTCTRL)
{
    
    pwmv2_cmp_config_t cmp_cfg[2];
    
    pwmv2_shadow_register_unlock(ptr);
    cmp_cfg[0].cmp = 200;
    cmp_cfg[0].enable_half_cmp = false;
    cmp_cfg[0].enable_hrcmp = false;
    cmp_cfg[0].cmp_source = CMP_SOURCE;
    cmp_cfg[0].cmp_source_index = PWMV2_SHADOW_INDEX(10);
    cmp_cfg[0].update_trigger = CMP_SHADOW_REGISTER_UPDATE_TYPE;
    pwmv2_config_cmp(ptr, PWM_CH_TRIG_CUREENTCTRL, &cmp_cfg[0]);

    pwmv2_select_cmp_source(ptr, PWM_CH_TRIG_CUREENTCTRL, CMP_SOURCE, PWMV2_SHADOW_INDEX(10));
    pwmv2_set_trigout_cmp_index(ptr, PWM_TRIGOUT_CH_CUREENTCTRL, PWM_CH_TRIG_CUREENTCTRL);
    pwmv2_cmp_select_counter(ptr, PWM_CH_TRIG_CUREENTCTRL, PWM_CNT);
    pwmv2_shadow_register_lock(ptr);

    /* start counter0,counter1,counter2 */
    pwmv2_enable_multi_counter_sync(ptr, 0x07);
    pwmv2_start_pwm_output_sync(ptr, 0x07); 
}
/**********************************************
 * *void timer_init()
 * * timer init for 1ms isr
 * *@Input: None
 * *#Output：None
 * ***************************************/
void timer_init(void)
{
    gptmr_channel_config_t config;
    clock_add_to_group(clock_gptmr2, 0);
    gptmr_channel_get_default_config(BOARD_BLDC_TMR_1MS, &config);
    config.cmp[0] = BOARD_BLDC_TMR_RELOAD;
    config.debug_mode = 0;
    config.reload = BOARD_BLDC_TMR_RELOAD+1;

    gptmr_enable_irq(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
    gptmr_channel_config(BOARD_BLDC_TMR_1MS, BOARD_BLDC_TMR_CH, &config, true);
    intc_m_enable_irq_with_priority(BOARD_BLDC_TMR_IRQ, 6);
}
/**********************************************
 * *void init_trigger_mux()
 * * trgm init for pwm channel trig to adc
 * *@Input: Trgm structure, trgm input, trgm out for adc
 * *#Output：None
 * ***************************************/
void init_trigger_mux(TRGM_Type * ptr, uint8_t TRAG_INPUT, uint8_t TRAG_INPUT_FOR_ADC)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = TRAG_INPUT;
    trgm_output_config(ptr, TRAG_INPUT_FOR_ADC, &trgm_output_cfg);
}
/**********************************************
 * *void init_trigger_cfg()
 * * trgm cfg for preempt to adc
 * *@Input: Trgm structure, trgm channel, adc structure and preempt length
 * *#Output：None
 * ***************************************/
void init_trigger_cfg(ADC16_Type *ptr, uint8_t trig_ch, uint8_t channel, bool inten, uint32_t ADC_MODULE, uint8_t ADC_PREEMPT_TRIG_LEN)
{
    adc_pmt_config_t pmt_cfg = {0};     
    pmt_cfg.module = ADC_MODULE;

    pmt_cfg.config.adc16.trig_ch   = trig_ch;
    pmt_cfg.config.adc16.trig_len  = ADC_PREEMPT_TRIG_LEN;
    pmt_cfg.config.adc16.adc_ch[0] = channel;
    pmt_cfg.config.adc16.inten[0] = inten;
    pmt_cfg.adc_base.adc16 = ptr;
    hpm_adc_set_preempt_config(&pmt_cfg);
}
/**********************************************
 * *void adc_cfg_init()
 * * adc init, include adc clock, adc sample time, adc channel
 * *@Input: adc structure, adc channel, adc sample cycle
 * *#Output：None
 * ***************************************/
void adc_cfg_init(ADC16_Type *ptr, uint8_t channel, uint32_t sample_cycle, uint32_t ADC_MODULE)
{
    adc_config_t cfg;
    adc_channel_config_t ch_cfg;
    cfg.module = ADC_MODULE;
    hpm_adc_init_default_config(&cfg);

    cfg.config.adc16.res            = adc16_res_16_bits;
    cfg.config.adc16.conv_mode      = adc16_conv_mode_preemption;
    cfg.config.adc16.adc_clk_div    = adc16_clock_divider_4;
    cfg.config.adc16.sel_sync_ahb   = false;
    cfg.config.adc16.adc_ahb_en = true;
    cfg.adc_base.adc16 = ptr;
    hpm_adc_init(&cfg);

    ch_cfg.module = ADC_MODULE;
    hpm_adc_init_channel_default_config(&ch_cfg);

    ch_cfg.config.adc16_ch.sample_cycle  = sample_cycle;
    ch_cfg.adc_base.adc16                = ptr;
    ch_cfg.config.adc16_ch.ch            = channel;
    hpm_adc_channel_init(&ch_cfg);   
}
/**********************************************
 * *void adc_pins_init()
 * * adc pin init
 * *@Input: pwm structure
 * *#Output：None
 * ***************************************/
void adc_pins_init(PWMV2_Type *ptr)
{
     if  (ptr == BOARD_PMSM0PWM)
    {  
#if defined (CONFIG_USE_HPM5E00)&& CONFIG_USE_HPM5E00

#if USE_MOTOR_DM
    HPM_IOC->PAD[IOC_PAD_PF28].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  //temp 
    HPM_IOC->PAD[IOC_PAD_PF29].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  //temp 
    HPM_IOC->PAD[IOC_PAD_PF30].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  //temp 
#else
        HPM_IOC->PAD[IOC_PAD_PF17].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;     
        HPM_IOC->PAD[IOC_PAD_PF18].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  
        HPM_IOC->PAD[IOC_PAD_PF31].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK; //udc_24v      
        HPM_IOC->PAD[IOC_PAD_PF26].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  //temp 
#endif
#elif defined (CONFIG_USE_HPM6E00)&& CONFIG_USE_HPM6E00
#if USE_MOTOR_DM
    HPM_IOC->PAD[IOC_PAD_PF28].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  //temp 
    HPM_IOC->PAD[IOC_PAD_PF29].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  //temp 
    HPM_IOC->PAD[IOC_PAD_PF30].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  //temp 
#else
        HPM_IOC->PAD[IOC_PAD_PF06].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;       
        HPM_IOC->PAD[IOC_PAD_PF08].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;  
        HPM_IOC->PAD[IOC_PAD_PF17].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;       
        HPM_IOC->PAD[IOC_PAD_PF18].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK; 
#endif

#endif    
    } 
}



/**********************************************
 * *void pwm_pins_init()
 * * pwm init
 * *@Input: pwm structure
 * *#Output：None
 * ***************************************/
void pwm_pins_init(PWMV2_Type *ptr)
{
     if  (ptr == BOARD_PMSM0PWM)
    {
        HPM_IOC->PAD[IOC_PAD_PE00].FUNC_CTL = IOC_PE00_FUNC_CTL_PWM0_P_0; 
        HPM_IOC->PAD[IOC_PAD_PE01].FUNC_CTL = IOC_PE01_FUNC_CTL_PWM0_P_1;
        HPM_IOC->PAD[IOC_PAD_PE02].FUNC_CTL = IOC_PE02_FUNC_CTL_PWM0_P_2;
        HPM_IOC->PAD[IOC_PAD_PE03].FUNC_CTL = IOC_PE03_FUNC_CTL_PWM0_P_3;
        HPM_IOC->PAD[IOC_PAD_PE04].FUNC_CTL = IOC_PE04_FUNC_CTL_PWM0_P_4;
        HPM_IOC->PAD[IOC_PAD_PE05].FUNC_CTL = IOC_PE05_FUNC_CTL_PWM0_P_5;
    }
}
/**********************************************
 * *void qeiv2_pins_init()
 * * qeiv2 pins init
 * *@Input: Trgm structure
 * *#Output：None
 * ***************************************/
void qeiv2_pins_init(PWMV2_Type *ptr)
{
    if (ptr == BOARD_PMSM0PWM)
    {
        HPM_IOC->PAD[IOC_PAD_PB07].FUNC_CTL = IOC_PB07_FUNC_CTL_QEI0_A;
        HPM_IOC->PAD[IOC_PAD_PB06].FUNC_CTL = IOC_PB06_FUNC_CTL_QEI0_B;
    }
}
/**********************************************
 * *void qeiv2_cfg_init()
 * * qeiv2 cfg init
 * *@Input: None
 * *#Output：None
 * ***************************************/
void qeiv2_cfg_init(void)
{
    qeiv2_reset_counter(BOARD_PMSM0_QEI_BASE);

    qeiv2_set_work_mode(BOARD_PMSM0_QEI_BASE, qeiv2_work_mode_abz);

    qeiv2_config_z_phase_counter_mode(BOARD_PMSM0_QEI_BASE, qeiv2_z_count_inc_on_phase_count_max);
    qeiv2_config_phmax_phparam(BOARD_PMSM0_QEI_BASE, BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV);


    intc_m_enable_irq_with_priority(BOARD_PMSM0_QEI_IRQ, 1);

    qeiv2_set_phcnt_cmp_value(BOARD_PMSM0_QEI_BASE, 4);

    qeiv2_set_cmp2_match_option(BOARD_PMSM0_QEI_BASE, true, false, true, true, true, true, true);
    qeiv2_enable_load_read_trigger_event(BOARD_PMSM0_QEI_BASE, QEIV2_EVENT_POSITION_COMPARE_FLAG_MASK);

    qeiv2_release_counter(BOARD_PMSM0_QEI_BASE);
}


/**********************************************
 * *void lv_set_adval_middle()
 * * phase current zero bias
 * *@Input: Motor structure
 * *#Output：None
 * ***************************************/
void lv_set_adval_middle(BLDC_CONTROL_FOC_PARA *par, uint32_t ADC_TRG)
{  

    par->pwmpar.pwmout.pwm_u = PWM_RELOAD >> 1;
    par->pwmpar.pwmout.pwm_v = PWM_RELOAD >> 1;
    par->pwmpar.pwmout.pwm_w = PWM_RELOAD >> 1;
    par->pwmpar.pwmout.func_set_pwm(&par->pwmpar.pwmout);

    par->samplcurpar.adc_u_middle = ((adc_buff[0][ADC_TRG*4]&0xffff)>>4)&0xfff;
    par->samplcurpar.adc_v_middle = ((adc_buff[1][ADC_TRG*4]&0xffff)>>4)&0xfff;
    par->samplcurpar.adc_w_middle = ((adc_buff[2][ADC_TRG*4]&0xffff)>>4)&0xfff;
}
/**********************************************
 * *void encoder_spi_init()
 * * encoder use spi init
 * *@Input: spi structure
 * *#Output：None
 * ***************************************/
void encoder_spi_init(SPI_Type *ptr, uint32_t CLK, uint8_t DATA_BITS, uint8_t MODE, uint8_t CPOL, uint8_t CPHA, uint8_t TRANS_MODE)
{

    spi_timing_config_t timing_config = {0};
    spi_format_config_t format_config = {0};
    spi_control_config_t control_config = {0};
    uint32_t spi_clcok;

    /* bsp initialization */
    spi_clcok = board_init_spi_clock(ptr);
    board_init_spi_pins(ptr);
    printf("SPI-Master Polling Transfer Example\n");

    /* set SPI sclk frequency for master */
    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.clk_src_freq_in_hz = spi_clcok;
    timing_config.master_config.sclk_freq_in_hz = CLK;
    if (status_success != spi_master_timing_init(ptr, &timing_config)) {
        printf("SPI master timming init failed\n");
        while (1) {
        }
    }
    printf("SPI-Master transfer timing is configured.\n");
    printf("SPI-Master transfer source clock frequency: %dHz\n", timing_config.master_config.clk_src_freq_in_hz);
    printf("SPI-Master transfer sclk frequency: %dHz\n", timing_config.master_config.sclk_freq_in_hz);

    /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = DATA_BITS;
    format_config.common_config.mode = MODE;
    format_config.common_config.cpol = CPOL;
    format_config.common_config.cpha = CPHA;
    spi_format_init(ptr, &format_config);

    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;  /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode = TRANS_MODE;//spi_trans_write_read_together;

}
/**********************************************
 * *void adc_init_udc_temp()
 * * bus udc n and tempture period sample init
 * *@Input: Adc16 structure, udc adc channel, tempture adc channel, sample cycle
 * *#Output：None
 * ***************************************/
void adc_init_udc_temp(ADC16_Type *ptr, uint8_t udc_channel, uint8_t temp_channel, uint32_t sample_cycle)
{
    board_init_adc16_pins();
    board_init_adc_clock(ptr, true);
    adc16_config_t cfg;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);
    cfg.res            = adc16_res_16_bits;
    cfg.conv_mode      = adc16_conv_mode_period;
    cfg.adc_clk_div    = adc16_clock_divider_4;
    cfg.sel_sync_ahb   = true;
    /* adc16 initialization */
    adc16_init(ptr, &cfg);

    adc16_channel_config_t ch_cfg;
    adc16_prd_config_t prd_cfg;
    /* get a default channel config */
    adc16_get_channel_default_config(&ch_cfg);

    /* initialize an udc ADC channel */
    ch_cfg.ch           = udc_channel;
    ch_cfg.sample_cycle = sample_cycle;
    adc16_init_channel(ptr, &ch_cfg);

    prd_cfg.ch           = udc_channel;
    prd_cfg.prescale     = 22;    /* Set divider: 2^22 clocks */
    prd_cfg.period_count = 5;     /* 104.86ms when AHB clock at 200MHz is ADC clock source */
    adc16_set_prd_config(ptr, &prd_cfg);

       /* initialize an temp ADC channel */
    ch_cfg.ch           = temp_channel;
    ch_cfg.sample_cycle = sample_cycle;
    adc16_init_channel(ptr, &ch_cfg);

    prd_cfg.ch           = temp_channel;
    prd_cfg.prescale     = 22;    /* Set divider: 2^22 clocks */
    prd_cfg.period_count = 5;     /* 104.86ms when AHB clock at 200MHz is ADC clock source */
    adc16_set_prd_config(ptr, &prd_cfg);

}
void init_adc_bus_voltag(void)
{
    board_init_adc16_pins();
    board_init_adc_clock(BOARD_APP_ADC16_BASE, true);
    adc16_config_t cfg;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);
    cfg.res            = adc16_res_16_bits;
    cfg.conv_mode      = adc16_conv_mode_period;
    cfg.adc_clk_div    = adc16_clock_divider_4;
    cfg.sel_sync_ahb   = true;
    /* adc16 initialization */
    adc16_init(BOARD_APP_ADC16_BASE, &cfg);

    adc16_channel_config_t ch_cfg;
    adc16_prd_config_t prd_cfg;
    /* get a default channel config */
    adc16_get_channel_default_config(&ch_cfg);

    /* initialize an udc ADC channel */
    ch_cfg.ch           = BOARD_APP_ADC16_UDC_CH;
    ch_cfg.sample_cycle = 20;
    adc16_init_channel(BOARD_APP_ADC16_BASE, &ch_cfg);

    prd_cfg.ch           = BOARD_APP_ADC16_UDC_CH;
    prd_cfg.prescale     = 22;    /* Set divider: 2^22 clocks */
    prd_cfg.period_count = 5;     /* 104.86ms when AHB clock at 200MHz is ADC clock source */
    adc16_set_prd_config(BOARD_APP_ADC16_BASE, &prd_cfg);
    
}
/**********************************************
 * *void led_motor_operation_init()
 * * led gpio init
 * *@Input: None
 * *#Output：None
 * ***************************************/
void led_motor_operation_init(void)
{
    HPM_IOC->PAD[IOC_PAD_PC18].FUNC_CTL = IOC_PC18_FUNC_CTL_GPIO_C_18;
    gpio_set_pin_output(HPM_GPIO0, GPIO_DO_GPIOC, 18);
    HPM_IOC->PAD[IOC_PAD_PC24].FUNC_CTL = IOC_PC24_FUNC_CTL_GPIO_C_24;
    gpio_set_pin_output(HPM_GPIO0, GPIO_DO_GPIOC, 24);
}

/**********************************************
 * *void para_index_init()
 * * intermediate variable init
 * *@Input: None
 * *#Output：None
 * ***************************************/
void para_index_init(Para_Obj* paraIdx)
{
   for(uint8_t i = 0;i<1;i++)
   {
      paraIdx[i].MotorObj.MotorParaObj.Motor_EncType.index = 0x4007;
      paraIdx[i].MotorObj.MotorParaObj.Motor_EncType.value = 0; 
      paraIdx[i].CurrentObj.Kp.index = 0x5000;
      paraIdx[i].CurrentObj.Kp.dotbit = 3;
      paraIdx[i].CurrentObj.Kp.value = 0.5;
      paraIdx[i].CurrentObj.Ki.index = 0x5001;
      paraIdx[i].CurrentObj.Ki.dotbit = 3;
      paraIdx[i].CurrentObj.Ki.value = 0.02;
      paraIdx[i].CurrentObj.Current_ref.index = 0x5002;
      paraIdx[i].CurrentObj.Current_Fdk.index = 0x5003;  
      paraIdx[i].VelObj.Kp.index = 0x6000;
      paraIdx[i].VelObj.Kp.dotbit = 0;
      paraIdx[i].VelObj.Kp.value = 60;
      paraIdx[i].VelObj.Ki.index = 0x6001;
      paraIdx[i].VelObj.Ki.dotbit = 3;
      paraIdx[i].VelObj.Ki.value = 0.01;
      paraIdx[i].VelObj.Vel_ForWard_Gain.index = 0x6002;
      paraIdx[i].VelObj.Vel_ForWard_Gain.dotbit = 3;
      paraIdx[i].VelObj.LowPassWn.index = 0x6003;
      paraIdx[i].VelObj.LowPassWn.dotbit = 3;
      paraIdx[i].VelObj.LowPassWn.value = 0.1;
      paraIdx[i].VelObj.Vel_Ref.index = 0x6004;
#if  MOTORCONTROL_EC_OR_STUDIO
      paraIdx[i].VelObj.Vel_Ref.value = 5;  // 上位机使能后设定速度默认10r/s
#endif
#if  !MOTORCONTROL_EC_OR_STUDIO
      paraIdx[i].VelObj.Vel_Ref.value = 0; // twincat使能后设定速度默认0
#endif
      paraIdx[i].VelObj.Acc_Ref.index = 0x6005;
      paraIdx[i].VelObj.Acc_Ref.value = 100;
      paraIdx[i].VelObj.Jerk_Ref.index = 0x6006;
      paraIdx[i].VelObj.Jerk_Ref.value = 1000;
      
      paraIdx[i].VelObj.Dec_Ref.index = 0x6007;
      paraIdx[i].VelObj.Vel_Fdk.index = 0x6008;
      paraIdx[i].VelObj.Vel_err.index = 0x6009;
      paraIdx[i].VelObj.Vel_IntrErr.index = 0x600A;
      paraIdx[i].VelObj.Vel_IntrFdk.index = 0X600B;
      paraIdx[i].VelObj.Vel_IntrCmd.index = 0X600C;
      paraIdx[i].VelObj.VelCnstTime.index = 0X600D;
      paraIdx[i].VelObj.VelCnstTime.value = 1;
      paraIdx[i].VelObj.VelCnstTime.dotbit = 0;
      paraIdx[i].PosObj.Kp.index = 0x7000;
      paraIdx[i].PosObj.Kp.dotbit = 5;
      paraIdx[i].PosObj.Kp.value = 0.001;
      paraIdx[i].PosObj.Pos_ref.index = 0x7001;
      paraIdx[i].PosObj.Pos_ref.value = 50;
      paraIdx[i].PosObj.Pos_Fdk.index = 0x7002;
      paraIdx[i].PosObj.Pos_err.index = 0x7003;
      paraIdx[i].PosObj.Pos_IntrErr.index = 0X7004;
      paraIdx[i].PosObj.Pos_IntrFdk.index = 0X7005;
      paraIdx[i].PosObj.Pos_IntrCmd.index = 0X7006;
      paraIdx[i].PosObj.CycleType.index = 0X7007;
      paraIdx[i].PosObj.CmdType.index = 0X7008;
      paraIdx[i].PosObj.DwellTime.index = 0X7009;
      paraIdx[i].PosObj.DwellTime.value = 500;
      paraIdx[i].PosObj.CycleCount.index = 0X700A;
      paraIdx[i].PosObj.CycleCount.value = 1;
      paraIdx[i].ScopeObj.Scp_SampeCnt.index = 0x8000;
      paraIdx[i].ScopeObj.Scp_SampeCnt.value = 200;
      paraIdx[i].ScopeObj.Scp_SampleGap.index = 0x8001;
      paraIdx[i].ScopeObj.Scp_SampleGap.value = 10;
      paraIdx[i].ScopeObj.Scp_channelCfg1.index = 0x8002;
      paraIdx[i].ScopeObj.Scp_channelCfg2.index = 0x8003;
      paraIdx[i].ScopeObj.Scp_channelCfg3.index = 0x8004;
      paraIdx[i].ScopeObj.Scp_channelCfg4.index = 0x8005;
      paraIdx[i].ScopeObj.Scp_channelCfg5.index = 0x8006;
      paraIdx[i].ScopeObj.Scp_channelCfg6.index = 0x8007;
      paraIdx[i].CmdObj.ControlWord.index = 0x9000;
      paraIdx[i].CmdObj.OpMode.index = 0x9001;
      paraIdx[i].CmdObj.zero_ControlWord.index = 0x9002;
      paraIdx[i].CmdObj.status.index = 0x9003;
   }
}
/**********************************************
 * *void pmsm_motor1_init()
 * * init for pmsm control
 * *@Input: None
 * *#Output：None
 * ***************************************/
void pmsm_motor1_init(void)
{
#if !USE_MOTOR_DM
    led_motor_operation_init();
#endif
    adc_pins_init(BOARD_PMSM0PWM);

    pwm_pins_init(BOARD_PMSM0PWM);

#if !USE_MOTOR_DM
    qeiv2_pins_init(BOARD_PMSM0PWM);
    qeiv2_cfg_init();
    adc_init_udc_temp(BOARD_APP_ADC16_BASE, BOARD_APP_ADC16_UDC_CH, BOARD_APP_ADC16_TEMP_CH, 20);
    encoder_spi_init(BOARD_APP_SPI_BASE, BOARD_APP_SPI_SCLK_FREQ, BOARD_APP_SPI_DATA_LEN_IN_BITS, spi_master_mode, spi_sclk_low_idle, spi_sclk_sampling_even_clk_edges, spi_trans_write_read_together);

#else
    init_encoder_mt6701();
    init_current_protect();
    init_adc_bus_voltag();
#endif  
    
#if defined  (CONFIG_USE_HPM6E00) &&(CONFIG_USE_FUNCTION)&&(BOARD_PMSM0_CLC_3P3Z) 
    clc_3p3z_init();
#endif

    board_init_adc_clock(BOARD_PMSM0_ADC_U_BASE, true);
    board_init_adc_clock(BOARD_PMSM0_ADC_V_BASE, true);

    adc_cfg_init(BOARD_PMSM0_ADC_U_BASE, BOARD_PMSM0_ADC_CH_U, 15, BOARD_PMSM0_ADC_MODULE);
    adc_cfg_init(BOARD_PMSM0_ADC_V_BASE, BOARD_PMSM0_ADC_CH_V, 15, BOARD_PMSM0_ADC_MODULE);

    init_trigger_mux(BOARD_PMSM0PWM_TRGM, BOARD_PMSM0_TRIGMUX_IN_NUM, BOARD_PMSM0_TRG_NUM);
    
    init_trigger_cfg(BOARD_PMSM0_ADC_U_BASE, BOARD_PMSM0_ADC_TRG, BOARD_PMSM0_ADC_CH_U, true, BOARD_PMSM0_ADC_MODULE, BOARD_PMSM0_ADC_PREEMPT_TRIG_LEN);
    init_trigger_cfg(BOARD_PMSM0_ADC_V_BASE, BOARD_PMSM0_ADC_TRG, BOARD_PMSM0_ADC_CH_V, true, BOARD_PMSM0_ADC_MODULE, BOARD_PMSM0_ADC_PREEMPT_TRIG_LEN);

#if BOARD_PMSM0_ADC_MODULE == ADCX_MODULE_ADC16
    adc16_set_pmt_queue_enable(BOARD_PMSM0_ADC_U_BASE, BOARD_PMSM0_ADC_TRG, true);
    adc16_set_pmt_queue_enable(BOARD_PMSM0_ADC_V_BASE, BOARD_PMSM0_ADC_TRG, true);
#endif

    adc_type hpm_adc_motor0_a;
    adc_type  hpm_adc_motor0_b;
    adc_module_cfg(&hpm_adc_motor0_a,BOARD_PMSM0_ADC_MODULE,BOARD_PMSM0_ADC_U_BASE);
    adc_module_cfg(&hpm_adc_motor0_b,BOARD_PMSM0_ADC_MODULE,BOARD_PMSM0_ADC_V_BASE);
    hpm_adc_init_pmt_dma(&hpm_adc_motor0_a, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[0]));
    hpm_adc_init_pmt_dma(&hpm_adc_motor0_b, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[1]));

    pwmv2_duty_init(BOARD_PMSM0PWM, PWM_RELOAD, pwm_shadow_register_update_on_reload, pwm_reload_update_on_reload, cmp_value_from_shadow_val);
    pwmv2_trigfor_adc_init(BOARD_PMSM0PWM, PWM_RELOAD, PWM_CNT0, pwm_shadow_register_update_on_reload,BOARD_PMSM0_PWM_TRIGOUT_CH_ADC, cmp_value_from_shadow_val, BOARD_PMSM0_PWM_TRIG_CMP_INDEX);
    pwmv2_trigfor_currentctrl_init(BOARD_PMSM0PWM,PWM_CNT0, pwm_shadow_register_update_on_reload,BOARD_PMSM0_PWM_TRIGOUT_CH_CURRENTLOOP, cmp_value_from_shadow_val, BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP);

    para_index_init(pt_para);
    motor_param_int(&motor, BLDC_MOTOR0_INDEX);
#if !USE_MOTOR_DM
    pi_param_init(&motor.speedloop_para, PMSM_SPEED_LOOP_KP,PMSM_SPEED_LOOP_KI,500);
    pi_param_init(&motor.position_para, PMSM_POSITION_LOOP_KP, PMSM_POSITION_LOOP_KI, 50*4000);
#else
    pi_param_init(&motor.speedloop_para, PMSM_SPEED_LOOP_KP,PMSM_SPEED_LOOP_KI,PMSM_SPEED_OUT_MAX);
    pi_param_init(&motor.position_para, PMSM_POSITION_LOOP_KP, PMSM_POSITION_LOOP_KI, PMSM_POSITION_OUT_MAX);
#endif
    param_init(&Motor_Control_Global,&qeiCalObj,&motor);
    board_delay_ms(1000);//电流采样避免采样不到
    lv_set_adval_middle(&motor.foc_para, BOARD_PMSM0_ADC_TRG);
    
    timer_init();
    motor_encoder_spi_elecinit(&Motor_Control_Global);
    motor_protect_param_init(&motor);
#if defined (CONFIG_USE_FUNCTION) && (CONFIG_USE_FUNCTION)
    interia_param_init(&paraObj[0]);//比库的初始化优先
    interia_init(&paraObj[0], &INTERIA_CalHdl);
    INTERIA_CalHdl.INTERIA_InObj.poles = motor.foc_para.motorpar.i_poles_n;
    motor_param_detection_cfg(&func_par.detection_par,&motor);
#endif
}

