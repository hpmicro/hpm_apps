/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "pmsm_init.h"

adc_type hpm_adc_motor_a;
adc_type hpm_adc_motor_b;
adc_type hpm_adc_motor_c;

MOTOR_PARA motor0;
MOTOR_PARA motor1;
MOTOR_PARA motor2;
MOTOR_PARA motor3;

MOTOR_CONTROL_Global Motor_Control_Global;
uint32_t motor0_clock_hz;

void parm_global_init(void)
{
    #if BOARD_PMSM0_ADC_MODULE == ADCX_MODULE_ADC12
    hpm_adc_motor_a.adc_base.adc12 = BOARD_PMSM0_ADC_U_BASE,
    #else
    hpm_adc_motor_a.adc_base.adc16 = BOARD_PMSM0_ADC_U_BASE,
    #endif
    hpm_adc_motor_a.module = BOARD_PMSM0_ADC_MODULE;
    #if BOARD_PMSM0_ADC_MODULE == ADCX_MODULE_ADC12
    hpm_adc_motor_b.adc_base.adc12 = BOARD_PMSM0_ADC_V_BASE,
    #else
    hpm_adc_motor_b.adc_base.adc16 = BOARD_PMSM0_ADC_V_BASE,
    #endif
    hpm_adc_motor_b.module = BOARD_PMSM0_ADC_MODULE;
    #if BOARD_PMSM0_ADC_MODULE == ADCX_MODULE_ADC12
    hpm_adc_motor_c.adc_base.adc12 = BOARD_PMSM0_ADC_W_BASE,
    #else
    hpm_adc_motor_c.adc_base.adc16 = BOARD_PMSM0_ADC_W_BASE,
    #endif
    hpm_adc_motor_c.module = BOARD_PMSM0_ADC_MODULE;
}


void pi_para_init(BLDC_CONTRL_PID_PARA *par, float KP, float KI, float MAX)
{
    par->func_pid  = hpm_mcl_bldc_foc_pi_contrl;

    par->i_kp      = HPM_MOTOR_MATH_FL_MDF(KP);
    par->i_ki      = HPM_MOTOR_MATH_FL_MDF(KI);
    par->i_max     = HPM_MOTOR_MATH_FL_MDF(MAX);

}

void pmsm_param_init(BLDC_CONTROL_FOC_PARA *par, uint8_t MOTOR_ID)
{
    par->motorpar.i_lstator_h = 0.003;
    par->motorpar.i_maxspeed_rs = 50;
    par->motorpar.i_phasecur_a = 0.125;
    par->motorpar.i_phasevol_v = 24;
    par->motorpar.i_poles_n = 2;
    par->motorpar.i_rstator_ohm = 1.1;
    par->motorpar.i_samplingper_s = 0.00005;

    par->speedcalpar.i_speedacq = 20;
    par->speedcalpar.i_speedfilter = 0.02;
    par->speedcalpar.i_speedlooptime_s = HPM_MOTOR_MATH_FL_MDF(0.00005*20);
    par->speedcalpar.i_motorpar = &par->motorpar;
    par->speedcalpar.func_getspd = hpm_mcl_bldc_foc_al_speed;

    par->currentdpipar.i_kp = 0.2;//0.4;
    par->currentdpipar.i_ki = 0.01;
    par->currentdpipar.i_max = HPM_MOTOR_MATH_FL_MDF(4000);
    par->currentdpipar.func_pid = hpm_mcl_bldc_foc_pi_contrl;

    par->currentqpipar.i_kp = 0.2;//0.4;
    par->currentqpipar.i_ki = 0.01;
    par->currentqpipar.i_max = HPM_MOTOR_MATH_FL_MDF(4000);
    par->currentqpipar.func_pid = hpm_mcl_bldc_foc_pi_contrl;

    par->pwmpar.func_spwm = hpm_mcl_bldc_foc_svpwm;
    par->pwmpar.i_pwm_reload_max = PWM_RELOAD*0.95;
    par->pwmpar.pwmout.func_set_pwm = bldc_foc_pwmset;
    par->pwmpar.pwmout.i_pwm_reload = PWM_RELOAD;
    par->pwmpar.pwmout.i_motor_id = MOTOR_ID;

    par->samplcurpar.func_sampl = hpm_mcl_bldc_foc_current_cal;
    par->func_dqsvpwm =  hpm_mcl_bldc_foc_ctrl_dq_to_pwm;
    
    motor0.adc_trig_event_callback = &motor0_highspeed_loop;
    motor1.adc_trig_event_callback = &motor1_highspeed_loop;
    motor2.adc_trig_event_callback = &motor2_highspeed_loop;
    motor3.adc_trig_event_callback = &motor3_highspeed_loop;

    

}


void pwm_duty_init(PWM_Type *ptr, uint32_t PWM_PRD, uint8_t CMP_SHADOW_REGISTER_UPDATE_TYPE, uint8_t CMP_COMPARE)
{
    uint8_t cmp_index = BOARD_PMSM0PWM_CMP_INDEX_0;
    pwm_cmp_config_t cmp_config[4] = {0};
    pwm_pair_config_t pwm_pair_config = {0};

    pwm_stop_counter(ptr);
    pwm_set_reload(ptr, 0, PWM_PRD);
    pwm_set_start_count(ptr, 0, 0);
    cmp_config[0].mode = CMP_COMPARE;
    cmp_config[0].cmp = PWM_PRD + 1;
    cmp_config[0].update_trigger = CMP_SHADOW_REGISTER_UPDATE_TYPE;

    cmp_config[1].mode = CMP_COMPARE;
    cmp_config[1].cmp = PWM_PRD + 1;
    cmp_config[1].update_trigger = CMP_SHADOW_REGISTER_UPDATE_TYPE;
    

    pwm_get_default_pwm_pair_config(ptr, &pwm_pair_config);
    pwm_pair_config.pwm[0].enable_output = true;
    pwm_pair_config.pwm[0].dead_zone_in_half_cycle = 100;
    pwm_pair_config.pwm[0].invert_output = false;

    pwm_pair_config.pwm[1].enable_output = true;
    pwm_pair_config.pwm[1].dead_zone_in_half_cycle = 100;
    pwm_pair_config.pwm[1].invert_output = false;

    
    if (status_success != pwm_setup_waveform_in_pair(ptr, BOARD_PMSM0_UH_PWM_OUTPIN, &pwm_pair_config, cmp_index, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform_in_pair(ptr, BOARD_PMSM0_VH_PWM_OUTPIN, &pwm_pair_config, cmp_index+2, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform_in_pair(ptr, BOARD_PMSM0_WH_PWM_OUTPIN, &pwm_pair_config, cmp_index+4, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    
    pwm_enable_output(ptr, BOARD_PMSM0_UH_PWM_OUTPIN);
    pwm_enable_output(ptr, BOARD_PMSM0_UL_PWM_OUTPIN);
    pwm_enable_output(ptr, BOARD_PMSM0_VH_PWM_OUTPIN);
    pwm_enable_output(ptr, BOARD_PMSM0_VL_PWM_OUTPIN);
    pwm_enable_output(ptr, BOARD_PMSM0_WH_PWM_OUTPIN);
    pwm_enable_output(ptr, BOARD_PMSM0_WL_PWM_OUTPIN);

}


void pwm_trigfor_adc_init(PWM_Type *ptr, uint32_t PWM_PRD, uint8_t CMP_SHADOW_REGISTER_UPDATE_TYPE, uint8_t CMP_COMPARE, uint8_t PWM_CH_TRIG_ADC)
{
    
    pwm_cmp_config_t cmp_config[4] = {0};
    pwm_output_channel_t pwm_output_ch_cfg;


    cmp_config[2].enable_ex_cmp  = false;
    cmp_config[2].mode           = CMP_COMPARE;
    cmp_config[2].cmp = (PWM_PRD - 1);
    cmp_config[2].update_trigger = CMP_SHADOW_REGISTER_UPDATE_TYPE;
    pwm_config_cmp(ptr, PWM_CH_TRIG_ADC, &cmp_config[2]);

    pwm_output_ch_cfg.cmp_start_index = PWM_CH_TRIG_ADC;
    pwm_output_ch_cfg.cmp_end_index   = PWM_CH_TRIG_ADC;
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(ptr, PWM_CH_TRIG_ADC, &pwm_output_ch_cfg);
    
}

void pwm_trigfor_currentctrl_init(PWM_Type *ptr, uint32_t PWM_PRD, uint8_t CMP_SHADOW_REGISTER_UPDATE_TYPE, uint8_t CMP_COMPARE, uint8_t PWM_CH_TRIG_CURRENTCtrl)
{
    
    pwm_cmp_config_t pwm_trig_currentloop = {0};
    pwm_output_channel_t pwm_output_ch_cfg;
 
    memset(&pwm_trig_currentloop, 0x00, sizeof(pwm_cmp_config_t));
    pwm_trig_currentloop.enable_ex_cmp  = false;
    pwm_trig_currentloop.mode = CMP_COMPARE;
    pwm_trig_currentloop.cmp = 200;
    pwm_trig_currentloop.update_trigger = CMP_SHADOW_REGISTER_UPDATE_TYPE;
    pwm_config_cmp(ptr, PWM_CH_TRIG_CURRENTCtrl, &pwm_trig_currentloop);
   
    pwm_output_ch_cfg.cmp_start_index = PWM_CH_TRIG_CURRENTCtrl; 
    pwm_output_ch_cfg.cmp_end_index   = PWM_CH_TRIG_CURRENTCtrl;  
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(ptr, PWM_CH_TRIG_CURRENTCtrl, &pwm_output_ch_cfg);
 
    pwm_start_counter(ptr);
    pwm_issue_shadow_register_lock_event(ptr);
    
}


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
    intc_m_enable_irq_with_priority(BOARD_BLDC_TMR_IRQ, 2);
    

}

void init_trigger_mux(TRGM_Type * ptr, uint8_t TRAG_INPUT, uint8_t TRAG_INPUT_FOR_ADC)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
    trgm_output_cfg.input  = TRAG_INPUT;
    trgm_output_config(ptr, TRAG_INPUT_FOR_ADC, &trgm_output_cfg);

}

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


void adc_cfg_init(ADC16_Type *ptr, uint8_t channel, uint32_t sample_cycle, uint32_t ADC_MODULE)
{
    adc_config_t cfg;
    adc_channel_config_t ch_cfg;
    cfg.module = ADC_MODULE;
    hpm_adc_init_default_config(&cfg);
    cfg.config.adc16.res            = adc16_res_16_bits;
    cfg.config.adc16.conv_mode      = adc16_conv_mode_preemption;
    cfg.config.adc16.adc_clk_div    = 3;
    cfg.config.adc16.sel_sync_ahb   = true;
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

void init_pwm_sync(SYNT_Type *ptr)
{
    synt_enable_counter(ptr, false);
    synt_reset_counter(ptr);
    synt_set_reload(ptr, 0xffffffff);
    synt_set_comparator(ptr, SYNT_CMP_0, SYNC_TIME_0);
    synt_set_comparator(ptr, SYNT_CMP_1, SYNC_TIME_1);
    synt_set_comparator(ptr, SYNT_CMP_2, SYNC_TIME_2);
    synt_set_comparator(ptr, SYNT_CMP_3, SYNC_TIME_3);
    synt_enable_counter(ptr, true);
}


void pwm_sync_init(void)
{
    trgm_output_t config;

    config.invert = false;
    config.input = HPM_TRGM0_INPUT_SRC_SYNT_CH0;
    config.type = trgm_output_same_as_input;
    trgm_output_config(BOARD_PMSM0PWM_TRGM, TRGM_TRGOCFG_PWM_SYNCI, &config);

    config.invert = false;
    config.input = HPM_TRGM1_INPUT_SRC_SYNT_CH1;
    config.type = trgm_output_same_as_input;
    trgm_output_config(BOARD_PMSM1PWM_TRGM, TRGM_TRGOCFG_PWM_SYNCI, &config);

    config.invert = false;
    config.input = HPM_TRGM2_INPUT_SRC_SYNT_CH2;
    config.type = trgm_output_same_as_input;
    trgm_output_config(BOARD_PMSM2PWM_TRGM, TRGM_TRGOCFG_PWM_SYNCI, &config);

    config.invert = false;
    config.input = HPM_TRGM3_INPUT_SRC_SYNT_CH3;
    config.type = trgm_output_same_as_input;
    trgm_output_config(BOARD_PMSM3PWM_TRGM, TRGM_TRGOCFG_PWM_SYNCI, &config);

    init_pwm_sync(MOTOR_PMSM_PWM_SYNC);
}


void reset_pwm_counter(PWM_Type *ptr)
{
    pwm_enable_reload_at_synci(ptr);
}


void pwm_sync(void)
{
    reset_pwm_counter(BOARD_PMSM0PWM);
    reset_pwm_counter(BOARD_PMSM1PWM);
    reset_pwm_counter(BOARD_PMSM2PWM);
    reset_pwm_counter(BOARD_PMSM3PWM);
    pwm_sync_init();
}

 
void adc_pins_init(PWM_Type *ptr)
{
    if (ptr == HPM_PWM2) 
    {
      HPM_IOC->PAD[IOC_PAD_PC14].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
      HPM_IOC->PAD[IOC_PAD_PC10].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
      HPM_IOC->PAD[IOC_PAD_PC18].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    }
    else if  (ptr == HPM_PWM3)
    {
      HPM_IOC->PAD[IOC_PAD_PC15].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
      HPM_IOC->PAD[IOC_PAD_PC11].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
      HPM_IOC->PAD[IOC_PAD_PC19].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    }  
    else if  (ptr == HPM_PWM0)
    {
     
      HPM_IOC->PAD[IOC_PAD_PC08].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
      HPM_IOC->PAD[IOC_PAD_PC21].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
      HPM_IOC->PAD[IOC_PAD_PC25].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    } 
    else if  (ptr == HPM_PWM1)
    {
      HPM_IOC->PAD[IOC_PAD_PC23].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
      HPM_IOC->PAD[IOC_PAD_PC09].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
      HPM_IOC->PAD[IOC_PAD_PC27].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
    } 

}



void lv_set_adval_middle(BLDC_CONTROL_FOC_PARA *par, uint32_t ADC_TRG)
{   
    uint32_t adc_u_sum = 0;
    uint32_t adc_v_sum = 0;
    uint32_t adc_w_sum = 0;
    uint8_t times = 0;
    par->pwmpar.pwmout.pwm_u = PWM_RELOAD >> 1;
    par->pwmpar.pwmout.pwm_v = PWM_RELOAD >> 1;
    par->pwmpar.pwmout.pwm_w = PWM_RELOAD >> 1;
    par->pwmpar.pwmout.func_set_pwm(&par->pwmpar.pwmout);
    do{
        if(Motor_Control_Global.flag_1ms == 1)
        {

            adc_u_sum += ((adc_buff[0][ADC_TRG*4]&0xffff)>>4)&0xfff;
            adc_v_sum += ((adc_buff[1][ADC_TRG*4]&0xffff)>>4)&0xfff;
            adc_w_sum += ((adc_buff[2][ADC_TRG*4]&0xffff)>>4)&0xfff;
            times++;
            if(times >= BLDC_CURRENT_SET_TIME_MS){
                break;
            }
            Motor_Control_Global.flag_1ms = 0;
        }
    }while(1);
    par->samplcurpar.adc_u_middle = adc_u_sum/ BLDC_CURRENT_SET_TIME_MS;
    par->samplcurpar.adc_v_middle = adc_v_sum/ BLDC_CURRENT_SET_TIME_MS;
    par->samplcurpar.adc_w_middle = adc_w_sum/ BLDC_CURRENT_SET_TIME_MS;
}



void pwm_pins_init(PWM_Type *ptr)
{

    if (ptr == HPM_PWM2) 
    {

       HPM_IOC->PAD[IOC_PAD_PB24].FUNC_CTL = IOC_PB24_FUNC_CTL_PWM2_P_00;   
       HPM_IOC->PAD[IOC_PAD_PB25].FUNC_CTL = IOC_PB25_FUNC_CTL_PWM2_P_01;   
       HPM_IOC->PAD[IOC_PAD_PB26].FUNC_CTL = IOC_PB26_FUNC_CTL_PWM2_P_02;   
       HPM_IOC->PAD[IOC_PAD_PB27].FUNC_CTL = IOC_PB27_FUNC_CTL_PWM2_P_03;   
       HPM_IOC->PAD[IOC_PAD_PB28].FUNC_CTL = IOC_PB28_FUNC_CTL_PWM2_P_04;   
       HPM_IOC->PAD[IOC_PAD_PB29].FUNC_CTL = IOC_PB29_FUNC_CTL_PWM2_P_05;   

    }
    else if  (ptr == HPM_PWM3)
    {
        
       HPM_IOC->PAD[IOC_PAD_PA20].FUNC_CTL = IOC_PA20_FUNC_CTL_PWM3_P_00;  
       HPM_IOC->PAD[IOC_PAD_PA21].FUNC_CTL = IOC_PA21_FUNC_CTL_PWM3_P_01;   
       HPM_IOC->PAD[IOC_PAD_PA22].FUNC_CTL = IOC_PA22_FUNC_CTL_PWM3_P_02;    
       HPM_IOC->PAD[IOC_PAD_PA23].FUNC_CTL = IOC_PA23_FUNC_CTL_PWM3_P_03;   
       HPM_IOC->PAD[IOC_PAD_PA24].FUNC_CTL = IOC_PA24_FUNC_CTL_PWM3_P_04;  
       HPM_IOC->PAD[IOC_PAD_PA25].FUNC_CTL = IOC_PA25_FUNC_CTL_PWM3_P_05;   

    }  
    else if  (ptr == HPM_PWM0)
    {
   
      HPM_IOC->PAD[IOC_PAD_PB12].FUNC_CTL = IOC_PB12_FUNC_CTL_PWM0_P_0;  
      HPM_IOC->PAD[IOC_PAD_PB13].FUNC_CTL = IOC_PB13_FUNC_CTL_PWM0_P_1;   
      HPM_IOC->PAD[IOC_PAD_PB14].FUNC_CTL = IOC_PB14_FUNC_CTL_PWM0_P_2;   
      HPM_IOC->PAD[IOC_PAD_PB15].FUNC_CTL = IOC_PB15_FUNC_CTL_PWM0_P_3;   
      HPM_IOC->PAD[IOC_PAD_PB16].FUNC_CTL = IOC_PB16_FUNC_CTL_PWM0_P_4;   
      HPM_IOC->PAD[IOC_PAD_PB17].FUNC_CTL = IOC_PB17_FUNC_CTL_PWM0_P_5;   
    } 
    else if  (ptr == HPM_PWM1)
    {
    
      HPM_IOC->PAD[IOC_PAD_PB00].FUNC_CTL = IOC_PB00_FUNC_CTL_PWM1_P_0;   
      HPM_IOC->PAD[IOC_PAD_PB01].FUNC_CTL = IOC_PB01_FUNC_CTL_PWM1_P_1;   
      HPM_IOC->PAD[IOC_PAD_PB03].FUNC_CTL = IOC_PB03_FUNC_CTL_PWM1_P_3;    
      HPM_IOC->PAD[IOC_PAD_PB02].FUNC_CTL = IOC_PB02_FUNC_CTL_PWM1_P_2;   
      HPM_IOC->PAD[IOC_PAD_PB05].FUNC_CTL = IOC_PB05_FUNC_CTL_PWM1_P_5;    
      HPM_IOC->PAD[IOC_PAD_PB04].FUNC_CTL = IOC_PB04_FUNC_CTL_PWM1_P_4;        
    } 
}

void qei_cfg_init(TRGM_Type *ptr, QEI_Type *ptr_qei, uint8_t trag_in_A, uint8_t trag_out_A, uint8_t trag_in_B, uint8_t trag_out_B, uint32_t QEI_IRQ, uint32_t MAX_cnt)
{
    

    trgm_output_t config = {0};
    config.invert = false;
    
    config.input = trag_in_A;
    trgm_output_config(ptr, trag_out_A, &config);
    config.input = trag_in_B;
    trgm_output_config(ptr, trag_out_B, &config);

    intc_m_enable_irq_with_priority(QEI_IRQ, 1);

    qei_counter_reset_assert(ptr_qei);
    
    qei_phase_config(ptr_qei, MAX_cnt, qei_z_count_inc_on_phase_count_max, true);
    qei_phase_cmp_set(ptr_qei, 4, false, qei_rotation_dir_cmp_ignore);
    qei_load_read_trigger_event_enable(ptr_qei, QEI_EVENT_POSITIVE_COMPARE_FLAG_MASK);
    qei_counter_reset_release(ptr_qei);
    

}



void pmsm0_foc_angle_align(void)
{
    uint16_t run_times = 0;
    motor0.adc_trig_event_callback = &motor0_angle_align_loop;
    do{             
        if(Motor_Control_Global.flag_1ms == 1)
        {/*1ms timer*/
            enable_all_pwm_output(BOARD_PMSM0PWM);
            Motor_Control_Global.flag_1ms = 0;          
            run_times++;     
                if((Motor_Control_Global.commu_IdRef<100)&&(run_times<=5))
                {
                    Motor_Control_Global.commu_IdRef = 20 + Motor_Control_Global.commu_IdRef;
                    Motor_Control_Global.commu_theta = 90;                          
                }
                else 
                {
                    if(Motor_Control_Global.commu_theta>0)
                    {
                        Motor_Control_Global.commu_theta =   Motor_Control_Global.commu_theta - 0.1 ;   
                   
                    }
                    else
                    {
                        if(Motor_Control_Global.commu_IdRef>0)
                        {
                            Motor_Control_Global.commu_IdRef = Motor_Control_Global.commu_IdRef - 10;                         
                        }
                        else
                        {
                            qei_cfg_init(BOARD_PMSM0_QEI_TRGM, BOARD_PMSM0_QEI_BASE, BOARD_PMSM0_QEI_TRGM_QEI_A_SRC, TRGM_TRGOCFG_QEI_A, 
                            BOARD_PMSM0_QEI_TRGM_QEI_B_SRC, TRGM_TRGOCFG_QEI_B, BOARD_PMSM0_QEI_IRQ, BOARD_PMSM3_QEI_FOC_PHASE_COUNT_PER_REV);
                      
                            disable_all_pwm_output(BOARD_PMSM0PWM);  
                            
                                                 
                            break;
                        }

                    }

                }
  
          }
         }while(1);
}

void pmsm1_foc_angle_align(void)
{
    uint16_t run_times = 0;
    motor1.adc_trig_event_callback = &motor1_angle_align_loop;    
    do{             
        if(Motor_Control_Global.flag_1ms == 1)
        {/*1ms timer*/
         enable_all_pwm_output(BOARD_PMSM1PWM);
            Motor_Control_Global.flag_1ms = 0;          
            run_times++;      
                if((Motor_Control_Global.commu_IdRef<100)&&(run_times<=5))
                {
                    Motor_Control_Global.commu_IdRef = 20 + Motor_Control_Global.commu_IdRef;
                    Motor_Control_Global.commu_theta = 90;                                             
                }
                else 
                {
                    if(Motor_Control_Global.commu_theta>0)
                    {
                        Motor_Control_Global.commu_theta =  Motor_Control_Global.commu_theta - 0.1 ;   
                   
                    }
                    else
                    {
                        if(Motor_Control_Global.commu_IdRef > 0)
                        {
                            Motor_Control_Global.commu_IdRef = Motor_Control_Global.commu_IdRef - 10;                         
                        }
                        else
                        {
                            qei_cfg_init(BOARD_PMSM1_QEI_TRGM, BOARD_PMSM1_QEI_BASE, BOARD_PMSM1_QEI_TRGM_QEI_A_SRC, TRGM_TRGOCFG_QEI_A, 
                            BOARD_PMSM1_QEI_TRGM_QEI_B_SRC, TRGM_TRGOCFG_QEI_B, BOARD_PMSM1_QEI_IRQ, BOARD_PMSM1_QEI_FOC_PHASE_COUNT_PER_REV);
                            disable_all_pwm_output(BOARD_PMSM1PWM);
                            
                            break;
                        }

                    }

                }
  
          }
         }while(1);
}

void pmsm2_foc_angle_align(void)
{
    uint16_t run_times = 0;
    motor2.adc_trig_event_callback = &motor2_angle_align_loop;
    
     do{             
        if(Motor_Control_Global.flag_1ms == 1)
        {/*1ms timer*/
         enable_all_pwm_output(BOARD_PMSM2PWM);
            Motor_Control_Global.flag_1ms = 0;          
            run_times++;      
                if((Motor_Control_Global.commu_IdRef<100)&&(run_times<=5))
                {
                    Motor_Control_Global.commu_IdRef = 20 + Motor_Control_Global.commu_IdRef;
                    Motor_Control_Global.commu_theta = 90;                                            
                }
                else 
                {
                    if(Motor_Control_Global.commu_theta>0)
                    {
                        Motor_Control_Global.commu_theta =   Motor_Control_Global.commu_theta - 0.1 ;                      
                    }
                    else
                    {
                        if(Motor_Control_Global.commu_IdRef>0)
                        {
                            Motor_Control_Global.commu_IdRef = Motor_Control_Global.commu_IdRef - 10;                         
                        }
                        else
                        {
                            qei_cfg_init(BOARD_PMSM2_QEI_TRGM, BOARD_PMSM2_QEI_BASE, BOARD_PMSM2_QEI_TRGM_QEI_A_SRC, TRGM_TRGOCFG_QEI_A, 
                            BOARD_PMSM2_QEI_TRGM_QEI_B_SRC, TRGM_TRGOCFG_QEI_B, BOARD_PMSM2_QEI_IRQ, BOARD_PMSM2_QEI_FOC_PHASE_COUNT_PER_REV);
                            disable_all_pwm_output(BOARD_PMSM2PWM);
                            motor2.adc_trig_event_callback = &motor2_highspeed_loop;
                            break;
                        }

                    }

                }
  
          }
         }while(1);
}

void pmsm3_foc_angle_align(void)
{
    uint16_t run_times = 0;
    motor3.adc_trig_event_callback = &motor3_angle_align_loop;
    do{             
        if(Motor_Control_Global.flag_1ms == 1)
        {/*1ms timer*/
         enable_all_pwm_output(BOARD_PMSM3PWM);
            Motor_Control_Global.flag_1ms = 0;          
            run_times++;      
                if((Motor_Control_Global.commu_IdRef < 100)&&(run_times <= 5))
                {
                    Motor_Control_Global.commu_IdRef = 20 + Motor_Control_Global.commu_IdRef;
                    Motor_Control_Global.commu_theta = 90;                                            
                }
                else 
                {
                    if(Motor_Control_Global.commu_theta > 0)
                    {
                        Motor_Control_Global.commu_theta =   Motor_Control_Global.commu_theta - 0.1 ;   
                   
                    }
                    else
                    {
                        if(Motor_Control_Global.commu_IdRef > 0)
                        {
                            Motor_Control_Global.commu_IdRef = Motor_Control_Global.commu_IdRef - 10;                         
                        }
                        else
                        {
                            qei_cfg_init(BOARD_PMSM3_QEI_TRGM, BOARD_PMSM3_QEI_BASE, BOARD_PMSM3_QEI_TRGM_QEI_A_SRC, TRGM_TRGOCFG_QEI_A, 
                            BOARD_PMSM3_QEI_TRGM_QEI_B_SRC, TRGM_TRGOCFG_QEI_B, BOARD_PMSM3_QEI_IRQ, BOARD_PMSM3_QEI_FOC_PHASE_COUNT_PER_REV);
                            disable_all_pwm_output(BOARD_PMSM3PWM);
                            motor3.adc_trig_event_callback = &motor3_highspeed_loop;
                            
                            break;
                        }

                    }

                }
  
          }
         }while(1);
}
void pmsm_foc_angle_align(void)
{
    intc_m_enable_irq_with_priority(BOARD_PMSM0APP_PWM_IRQ, 1);
    pwm_enable_irq(BOARD_PMSM0PWM, PWM_IRQ_CMP(BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP));
    pmsm0_foc_angle_align();
    motor0.adc_trig_event_callback = &motor0_highspeed_loop;  
    

    intc_m_enable_irq_with_priority(BOARD_PMSM1APP_PWM_IRQ, 1);
    pwm_enable_irq(BOARD_PMSM1PWM, PWM_IRQ_CMP(BOARD_PMSM1_PWM_TRIG_CMP_INDEX_CURRENTLOOP));
    pmsm1_foc_angle_align();
    motor1.adc_trig_event_callback = &motor1_highspeed_loop;
    

    intc_m_enable_irq_with_priority(BOARD_PMSM2APP_PWM_IRQ, 1);
    pwm_enable_irq(BOARD_PMSM2PWM, PWM_IRQ_CMP(BOARD_PMSM2_PWM_TRIG_CMP_INDEX_CURRENTLOOP));
    pmsm2_foc_angle_align();
    
    
    intc_m_enable_irq_with_priority(BOARD_PMSM3APP_PWM_IRQ, 1);
    pwm_enable_irq(BOARD_PMSM3PWM, PWM_IRQ_CMP(BOARD_PMSM3_PWM_TRIG_CMP_INDEX_CURRENTLOOP));   
    pmsm3_foc_angle_align();
    pwm_sync();
}
void qei_pins_init(PWM_Type *ptr)
{
    if (ptr == HPM_PWM2)
    {
        HPM_IOC->PAD[IOC_PAD_PB18].FUNC_CTL = IOC_PB18_FUNC_CTL_TRGM2_P_6;
        HPM_IOC->PAD[IOC_PAD_PB19].FUNC_CTL = IOC_PB19_FUNC_CTL_TRGM2_P_7;
    }
    else if (ptr == HPM_PWM0)
    {
        HPM_IOC->PAD[IOC_PAD_PB20].FUNC_CTL = IOC_PB20_FUNC_CTL_TRGM0_P_00;
        HPM_IOC->PAD[IOC_PAD_PB21].FUNC_CTL = IOC_PB21_FUNC_CTL_TRGM0_P_01;
       
    }
    else if (ptr == HPM_PWM1)
    {
        HPM_IOC->PAD[IOC_PAD_PA26].FUNC_CTL = IOC_PA26_FUNC_CTL_TRGM1_P_06;
        HPM_IOC->PAD[IOC_PAD_PA27].FUNC_CTL = IOC_PA27_FUNC_CTL_TRGM1_P_07;
    }
    else if (ptr == HPM_PWM3)
    {
        HPM_IOC->PAD[IOC_PAD_PA29].FUNC_CTL = IOC_PA29_FUNC_CTL_TRGM3_P_1;
        HPM_IOC->PAD[IOC_PAD_PA30].FUNC_CTL = IOC_PA30_FUNC_CTL_TRGM3_P_2;
    }
}


void pmsm_motor0_init(void)
{
    adc_pins_init(BOARD_PMSM0PWM);
    pwm_pins_init(BOARD_PMSM0PWM);
    qei_pins_init(BOARD_PMSM0PWM);

    qei_cfg_init(BOARD_PMSM0_QEI_TRGM, BOARD_PMSM0_QEI_BASE, BOARD_PMSM0_QEI_TRGM_QEI_A_SRC, TRGM_TRGOCFG_QEI_A, 
                  BOARD_PMSM0_QEI_TRGM_QEI_B_SRC, TRGM_TRGOCFG_QEI_B, BOARD_PMSM0_QEI_IRQ,BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV);

    adc_cfg_init(BOARD_PMSM0_ADC_U_BASE, BOARD_PMSM0_ADC_CH_U, 15, BOARD_PMSM0_ADC_MODULE);
    adc_cfg_init(BOARD_PMSM0_ADC_V_BASE, BOARD_PMSM0_ADC_CH_V, 15, BOARD_PMSM0_ADC_MODULE);
    adc_cfg_init(BOARD_PMSM0_ADC_W_BASE, BOARD_PMSM0_ADC_CH_W, 15, BOARD_PMSM0_ADC_MODULE);
    init_trigger_mux(BOARD_PMSM0PWM_TRGM, BOARD_PMSM0_TRIGMUX_IN_NUM, BOARD_PMSM0_TRG_NUM);
    init_trigger_cfg(BOARD_PMSM0_ADC_U_BASE, BOARD_PMSM0_ADC_TRG, BOARD_PMSM0_ADC_CH_U, true, BOARD_PMSM0_ADC_MODULE, BOARD_PMSM0_ADC_PREEMPT_TRIG_LEN);//phase A adc trig init
    init_trigger_cfg(BOARD_PMSM0_ADC_V_BASE, BOARD_PMSM0_ADC_TRG, BOARD_PMSM0_ADC_CH_V, true, BOARD_PMSM0_ADC_MODULE, BOARD_PMSM0_ADC_PREEMPT_TRIG_LEN);//phase B adc trig init
    init_trigger_cfg(BOARD_PMSM0_ADC_W_BASE, BOARD_PMSM0_ADC_TRG, BOARD_PMSM0_ADC_CH_W, true, BOARD_PMSM0_ADC_MODULE, BOARD_PMSM0_ADC_PREEMPT_TRIG_LEN);//phase C adc trig init

#if BOARD_PMSM0_ADC_MODULE == ADCX_MODULE_ADC16
    adc16_set_pmt_queue_enable(BOARD_PMSM0_ADC_U_BASE, BOARD_PMSM0_ADC_TRG, true);
    adc16_set_pmt_queue_enable(BOARD_PMSM0_ADC_V_BASE, BOARD_PMSM0_ADC_TRG, true);
    adc16_set_pmt_queue_enable(BOARD_PMSM0_ADC_W_BASE, BOARD_PMSM0_ADC_TRG, true);
#endif

    hpm_adc_init_pmt_dma(&hpm_adc_motor_a, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[0]));
    hpm_adc_init_pmt_dma(&hpm_adc_motor_b, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[1]));
    hpm_adc_init_pmt_dma(&hpm_adc_motor_c, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[2]));
    
    motor0_clock_hz = clock_get_frequency(BOARD_PMSM_MOTOR_CLOCK_SOURCE);
    pwm_duty_init(BOARD_PMSM0PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare);
   
    pwm_trigfor_adc_init(BOARD_PMSM0PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare, BOARD_PMSM0_PWM_TRIG_CMP_INDEX);//pwm trig for adc init
    pwm_trigfor_currentctrl_init(BOARD_PMSM0PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare, BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP);//pwm trig for Current control init

    
    pmsm_param_init(&motor0.foc_para, BLDC_MOTOR0_INDEX);
    pi_para_init(&motor0.speedloop_para, SPEEDLOOP_KP, SPEEDLOOP_KI, SPEEDLOOP_OUTMAX);
    pi_para_init(&motor0.position_para, POSITIONLOOP_KP, POSITIONLOOP_KI, POSITIONLOOP_OUTMAX*BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV);
    lv_set_adval_middle(&motor0.foc_para, BOARD_PMSM0_ADC_TRG);
}

void pmsm_motor1_init(void)
{
    adc_pins_init(BOARD_PMSM1PWM);
    pwm_pins_init(BOARD_PMSM1PWM);
    qei_pins_init(BOARD_PMSM1PWM);

    qei_cfg_init(BOARD_PMSM1_QEI_TRGM, BOARD_PMSM1_QEI_BASE, BOARD_PMSM1_QEI_TRGM_QEI_A_SRC, TRGM_TRGOCFG_QEI_A, 
                 BOARD_PMSM1_QEI_TRGM_QEI_B_SRC, TRGM_TRGOCFG_QEI_B, BOARD_PMSM1_QEI_IRQ, BOARD_PMSM1_QEI_FOC_PHASE_COUNT_PER_REV);

    adc_cfg_init(BOARD_PMSM1_ADC_U_BASE, BOARD_PMSM1_ADC_CH_U, 15, BOARD_PMSM1_ADC_MODULE);
    adc_cfg_init(BOARD_PMSM1_ADC_V_BASE, BOARD_PMSM1_ADC_CH_V, 15, BOARD_PMSM1_ADC_MODULE);
    adc_cfg_init(BOARD_PMSM1_ADC_W_BASE, BOARD_PMSM1_ADC_CH_W, 15, BOARD_PMSM1_ADC_MODULE);
    init_trigger_mux(BOARD_PMSM1PWM_TRGM, BOARD_PMSM1_TRIGMUX_IN_NUM, BOARD_PMSM1_TRG_NUM);
    init_trigger_cfg(BOARD_PMSM1_ADC_U_BASE, BOARD_PMSM1_ADC_TRG, BOARD_PMSM1_ADC_CH_U, true, BOARD_PMSM1_ADC_MODULE, BOARD_PMSM1_ADC_PREEMPT_TRIG_LEN);//phase A adc trig init
    init_trigger_cfg(BOARD_PMSM1_ADC_V_BASE, BOARD_PMSM1_ADC_TRG, BOARD_PMSM1_ADC_CH_V, true, BOARD_PMSM1_ADC_MODULE, BOARD_PMSM1_ADC_PREEMPT_TRIG_LEN);//phase B adc trig init
    init_trigger_cfg(BOARD_PMSM1_ADC_W_BASE, BOARD_PMSM1_ADC_TRG, BOARD_PMSM1_ADC_CH_W, true, BOARD_PMSM1_ADC_MODULE, BOARD_PMSM1_ADC_PREEMPT_TRIG_LEN);//phase C adc trig init


    pwm_duty_init(BOARD_PMSM1PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare);
    
    pwm_trigfor_adc_init(BOARD_PMSM1PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare, BOARD_PMSM1_PWM_TRIG_CMP_INDEX);//pwm trig for adc init
    pwm_trigfor_currentctrl_init(BOARD_PMSM1PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare, BOARD_PMSM1_PWM_TRIG_CMP_INDEX_CURRENTLOOP);//pwm trig for urrent control init
    
    pmsm_param_init(&motor1.foc_para, BLDC_MOTOR1_INDEX);
    pi_para_init(&motor1.speedloop_para, SPEEDLOOP_KP, SPEEDLOOP_KI, SPEEDLOOP_OUTMAX);
    pi_para_init(&motor1.position_para, POSITIONLOOP_KP, POSITIONLOOP_KI, POSITIONLOOP_OUTMAX*BOARD_PMSM1_QEI_FOC_PHASE_COUNT_PER_REV);

    lv_set_adval_middle(&motor1.foc_para, BOARD_PMSM1_ADC_TRG);

}

void pmsm_motor2_init(void)
{
    adc_pins_init(BOARD_PMSM2PWM);
    pwm_pins_init(BOARD_PMSM2PWM);
    qei_pins_init(BOARD_PMSM2PWM);

    qei_cfg_init(BOARD_PMSM2_QEI_TRGM, BOARD_PMSM2_QEI_BASE, BOARD_PMSM2_QEI_TRGM_QEI_A_SRC, TRGM_TRGOCFG_QEI_A, 
                  BOARD_PMSM2_QEI_TRGM_QEI_B_SRC, TRGM_TRGOCFG_QEI_B, BOARD_PMSM2_QEI_IRQ, BOARD_PMSM2_QEI_FOC_PHASE_COUNT_PER_REV);

    adc_cfg_init(BOARD_PMSM2_ADC_U_BASE, BOARD_PMSM2_ADC_CH_U, 15, BOARD_PMSM2_ADC_MODULE);
    adc_cfg_init(BOARD_PMSM2_ADC_V_BASE, BOARD_PMSM2_ADC_CH_V, 15, BOARD_PMSM2_ADC_MODULE);
    adc_cfg_init(BOARD_PMSM2_ADC_W_BASE, BOARD_PMSM2_ADC_CH_W, 15, BOARD_PMSM2_ADC_MODULE);
    init_trigger_mux(BOARD_PMSM2PWM_TRGM, BOARD_PMSM2_TRIGMUX_IN_NUM, BOARD_PMSM2_TRG_NUM);
    init_trigger_cfg(BOARD_PMSM2_ADC_U_BASE, BOARD_PMSM2_ADC_TRG, BOARD_PMSM2_ADC_CH_U, true, BOARD_PMSM2_ADC_MODULE, BOARD_PMSM2_ADC_PREEMPT_TRIG_LEN);//phase A adc trig init
    init_trigger_cfg(BOARD_PMSM2_ADC_V_BASE, BOARD_PMSM2_ADC_TRG, BOARD_PMSM2_ADC_CH_V, true, BOARD_PMSM2_ADC_MODULE, BOARD_PMSM2_ADC_PREEMPT_TRIG_LEN);//phase B adc trig init
    init_trigger_cfg(BOARD_PMSM2_ADC_W_BASE, BOARD_PMSM2_ADC_TRG, BOARD_PMSM2_ADC_CH_W, true, BOARD_PMSM2_ADC_MODULE, BOARD_PMSM2_ADC_PREEMPT_TRIG_LEN);//phase C adc trig init

    pwm_duty_init(BOARD_PMSM2PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare);
    pwm_trigfor_adc_init(BOARD_PMSM2PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare, BOARD_PMSM2_PWM_TRIG_CMP_INDEX);//pwm trig for adc init
    pwm_trigfor_currentctrl_init(BOARD_PMSM2PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare, BOARD_PMSM2_PWM_TRIG_CMP_INDEX_CURRENTLOOP);//pwm trig for urrent control init
    
    pmsm_param_init(&motor2.foc_para, BLDC_MOTOR2_INDEX);
    pi_para_init(&motor2.speedloop_para, SPEEDLOOP_KP, SPEEDLOOP_KI, SPEEDLOOP_OUTMAX);
    pi_para_init(&motor2.position_para, POSITIONLOOP_KP, POSITIONLOOP_KI, POSITIONLOOP_OUTMAX*BOARD_PMSM2_QEI_FOC_PHASE_COUNT_PER_REV);

    lv_set_adval_middle(&motor2.foc_para, BOARD_PMSM2_ADC_TRG);

}

void pmsm_motor3_init(void)
{
    adc_pins_init(BOARD_PMSM3PWM);
    pwm_pins_init(BOARD_PMSM3PWM);
    qei_pins_init(BOARD_PMSM3PWM);

    qei_cfg_init(BOARD_PMSM3_QEI_TRGM, BOARD_PMSM3_QEI_BASE, BOARD_PMSM3_QEI_TRGM_QEI_A_SRC, TRGM_TRGOCFG_QEI_A, 
                  BOARD_PMSM3_QEI_TRGM_QEI_B_SRC, TRGM_TRGOCFG_QEI_B, BOARD_PMSM3_QEI_IRQ, BOARD_PMSM3_QEI_FOC_PHASE_COUNT_PER_REV);

    adc_cfg_init(BOARD_PMSM3_ADC_U_BASE, BOARD_PMSM3_ADC_CH_U, 15, BOARD_PMSM3_ADC_MODULE);
    adc_cfg_init(BOARD_PMSM3_ADC_V_BASE, BOARD_PMSM3_ADC_CH_V, 15, BOARD_PMSM3_ADC_MODULE);
    adc_cfg_init(BOARD_PMSM3_ADC_W_BASE, BOARD_PMSM3_ADC_CH_W, 15, BOARD_PMSM3_ADC_MODULE);
    init_trigger_mux(BOARD_PMSM3PWM_TRGM, BOARD_PMSM3_TRIGMUX_IN_NUM, BOARD_PMSM3_TRG_NUM);
    init_trigger_cfg(BOARD_PMSM3_ADC_U_BASE, BOARD_PMSM3_ADC_TRG, BOARD_PMSM3_ADC_CH_U, true, BOARD_PMSM3_ADC_MODULE, BOARD_PMSM3_ADC_PREEMPT_TRIG_LEN);//phase A adc trig init
    init_trigger_cfg(BOARD_PMSM3_ADC_V_BASE, BOARD_PMSM3_ADC_TRG, BOARD_PMSM3_ADC_CH_V, true, BOARD_PMSM3_ADC_MODULE, BOARD_PMSM3_ADC_PREEMPT_TRIG_LEN);//phase B adc trig init
    init_trigger_cfg(BOARD_PMSM3_ADC_W_BASE, BOARD_PMSM3_ADC_TRG, BOARD_PMSM3_ADC_CH_W, true, BOARD_PMSM3_ADC_MODULE, BOARD_PMSM3_ADC_PREEMPT_TRIG_LEN);//phase C adc trig init


    pwm_duty_init(BOARD_PMSM3PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare);
    pwm_trigfor_adc_init(BOARD_PMSM3PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare, BOARD_PMSM3_PWM_TRIG_CMP_INDEX);//pwm trig for adc init
    pwm_trigfor_currentctrl_init(BOARD_PMSM3PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare, BOARD_PMSM3_PWM_TRIG_CMP_INDEX_CURRENTLOOP);//pwm trig for urrent control init
    
    pmsm_param_init(&motor3.foc_para, BLDC_MOTOR3_INDEX);
    pi_para_init(&motor3.speedloop_para, SPEEDLOOP_KP, SPEEDLOOP_KI, SPEEDLOOP_OUTMAX);
    pi_para_init(&motor3.position_para,  POSITIONLOOP_KP, POSITIONLOOP_KI, POSITIONLOOP_OUTMAX*BOARD_PMSM3_QEI_FOC_PHASE_COUNT_PER_REV);

    lv_set_adval_middle(&motor3.foc_para, BOARD_PMSM3_ADC_TRG);
}
void pmsm_motor_init(void)
{
    board_init_adc_clock(BOARD_PMSM0_ADC_U_BASE, true);
    board_init_adc_clock(BOARD_PMSM0_ADC_V_BASE, true);
    board_init_adc_clock(BOARD_PMSM0_ADC_W_BASE, true);
    pmsm_motor0_init();
    pmsm_motor1_init();
    pmsm_motor2_init();
    pmsm_motor3_init();
}

