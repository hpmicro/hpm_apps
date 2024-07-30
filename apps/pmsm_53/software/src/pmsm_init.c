
#include "pmsm_init.h"


MOTOR_PARA motor0;
MOTOR_CONTROL_Global Motor_Control_Global;
uint32_t motor0_clock_hz;
pos_angle_CalObj pos_angleObj[MAX_AXIS_NUM];


void adc_module_cfg(adc_type* adc_typ,uint8_t adc_module,ADC16_Type* HPM_ADC_BASE)
{
    adc_typ->adc_base.adc16 = HPM_ADC_BASE;
    adc_typ->module = adc_module;
}


void motor_param_int(MOTOR_PARA *motor_par, uint8_t MOTOR_ID)
{

    motor_par->foc_para.motorpar.func_smc_const = (void*)&hpm_mcl_smc_const_cal;
    motor_par->foc_para.motorpar.i_lstator_h = globalAxisParm[MOTOR_ID-1].parmMotor.Mortor_L0x4003_3;
    motor_par->foc_para.motorpar.i_maxspeed_rs = 50;
    motor_par->foc_para.motorpar.i_phasecur_a = globalAxisParm[MOTOR_ID-1].parmMotor.Motor_RatedCurrent0x4000_3;
    motor_par->foc_para.motorpar.i_phasevol_v = globalAxisParm[MOTOR_ID-1].parmMotor.Mortor_RatedVlotage0x4001;
    motor_par->foc_para.motorpar.i_poles_n = 2;
    motor_par->foc_para.motorpar.i_rstator_ohm = globalAxisParm[MOTOR_ID-1].parmMotor.Mortor_R0x4002_3;
    motor_par->foc_para.motorpar.i_samplingper_s = 0.00005;
    motor_par->foc_para.motorpar.func_smc_const(&motor_par->foc_para.motorpar);

    motor_par->foc_para.speedcalpar.i_speedacq = 20;
    motor_par->foc_para.speedcalpar.i_speedfilter = 0.1;
    motor_par->foc_para.speedcalpar.i_speedlooptime_s = HPM_MOTOR_MATH_FL_MDF(0.00005*20);
    motor_par->foc_para.speedcalpar.i_motorpar = &motor_par->foc_para.motorpar;
    motor_par->foc_para.speedcalpar.func_getspd = (void*)hpm_mcl_bldc_foc_al_speed;

    motor_par->foc_para.currentdpipar.i_kp = globalAxisParm[MOTOR_ID-1].parmCurrent.Kp0x5000_3;
    motor_par->foc_para.currentdpipar.i_ki = globalAxisParm[MOTOR_ID-1].parmCurrent.Ki0x5001_3;
    motor_par->foc_para.currentdpipar.i_max = HPM_MOTOR_MATH_FL_MDF(4000);
    motor_par->foc_para.currentdpipar.func_pid = (void*)hpm_mcl_bldc_foc_pi_contrl;

    motor_par->foc_para.currentqpipar.i_kp = globalAxisParm[MOTOR_ID-1].parmCurrent.Kp0x5000_3;
    motor_par->foc_para.currentqpipar.i_ki = globalAxisParm[MOTOR_ID-1].parmCurrent.Ki0x5001_3;
    motor_par->foc_para.currentqpipar.i_max = HPM_MOTOR_MATH_FL_MDF(7000);
    motor_par->foc_para.currentqpipar.func_pid = (void*)hpm_mcl_bldc_foc_pi_contrl;

    motor_par->foc_para.pwmpar.func_spwm = (void*)hpm_mcl_bldc_foc_svpwm;
    motor_par->foc_para.pwmpar.i_pwm_reload_max = PWM_RELOAD*0.95;
    motor_par->foc_para.pwmpar.pwmout.func_set_pwm = (void*)bldc_foc_pwmset;
    motor_par->foc_para.pwmpar.pwmout.i_pwm_reload = PWM_RELOAD;
    motor_par->foc_para.pwmpar.pwmout.i_motor_id = MOTOR_ID;

    motor_par->foc_para.samplcurpar.func_sampl = (void*)hpm_mcl_bldc_foc_current_cal;
    motor_par->foc_para.func_dqsvpwm =  (void*)hpm_mcl_bldc_foc_ctrl_dq_to_pwm;

    motor_par->position_para.func_pid = (void*)hpm_mcl_bldc_foc_pi_contrl;
    motor_par->position_para.i_kp      = HPM_MOTOR_MATH_FL_MDF(globalAxisParm[MOTOR_ID-1].parmPos.Kp0x7000_3);
    motor_par->position_para.i_ki      = 0;
    motor_par->position_para.i_max     = 50*globalAxisParm[MOTOR_ID-1].parmMotor.Motor_PlusCnt0x4008;

    motor_par->speedloop_para.func_pid = (void*)hpm_mcl_bldc_foc_pi_contrl;
    motor_par->speedloop_para.i_kp      = HPM_MOTOR_MATH_FL_MDF(globalAxisParm[MOTOR_ID-1].parmVel.Kp0x6000);
    motor_par->speedloop_para.i_ki      = HPM_MOTOR_MATH_FL_MDF(globalAxisParm[MOTOR_ID-1].parmVel.Ki0x6001_3);
    motor_par->speedloop_para.i_max     = 500;
    
    motor_par->adc_trig_event_callback = &motor0_highspeed_loop;


}

void pwm_duty_init(PWM_Type *ptr, uint32_t PWM_PRD, uint8_t CMP_SHADOW_REGISTER_UPDATE_TYPE, uint8_t CMP_COMPARE)
{
    uint8_t cmp_index = BOARD_PMSM0PWM_CMP_INDEX_0;
    pwm_cmp_config_t cmp_config[4] = {0};
    pwm_pair_config_t pwm_pair_config = {0};
    
    pwm_stop_counter(ptr);

    pwm_set_reload(ptr, 0, PWM_PRD);
    pwm_set_start_count(ptr, 0, 0);
    /*
     * config cmp1 and cmp2
     */
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

    {   
        if (status_success != pwm_setup_waveform_in_pair(ptr, BOARD_PMSM0_UH_PWM_OUTPIN, &pwm_pair_config, cmp_index, &cmp_config[0], 2)) {
            printf("failed to setup waveform\n");
            while(1);
        }
        if (status_success != pwm_setup_waveform_in_pair(ptr, BOARD_PMSM0_VH_PWM_OUTPIN, &pwm_pair_config, cmp_index+4, &cmp_config[0], 2)) {
            printf("failed to setup waveform\n");
            while(1);
        }
        if (status_success != pwm_setup_waveform_in_pair(ptr, BOARD_PMSM0_WH_PWM_OUTPIN, &pwm_pair_config, cmp_index+6, &cmp_config[0], 2)) {
            printf("failed to setup waveform\n");
            while(1);
        }
      
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
    cmp_config[2].cmp = PWM_PRD/2;
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
    pwm_trig_currentloop.cmp = PWM_PRD/2 + 200;

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

    {
       pmt_cfg.config.adc16.trig_ch   = trig_ch;
       pmt_cfg.config.adc16.trig_len  = ADC_PREEMPT_TRIG_LEN;
       pmt_cfg.config.adc16.adc_ch[0] = channel;
       pmt_cfg.config.adc16.inten[0] = inten;
       pmt_cfg.adc_base.adc16 = ptr;
    }
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


void adc_pins_init(PWM_Type *ptr)
{
     if  (ptr == HPM_PWM0)
    {
        
      HPM_IOC->PAD[IOC_PAD_PB10].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;//53 solution
      HPM_IOC->PAD[IOC_PAD_PB11].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
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
     if  (ptr == HPM_PWM0)
    {
    
        HPM_IOC->PAD[IOC_PAD_PA24].FUNC_CTL = IOC_PA24_FUNC_CTL_PWM0_P_0;
        HPM_IOC->PAD[IOC_PAD_PA25].FUNC_CTL = IOC_PA25_FUNC_CTL_PWM0_P_1;
    
        HPM_IOC->PAD[IOC_PAD_PA28].FUNC_CTL = IOC_PA28_FUNC_CTL_PWM0_P_4;
        HPM_IOC->PAD[IOC_PAD_PA29].FUNC_CTL = IOC_PA29_FUNC_CTL_PWM0_P_5;
        HPM_IOC->PAD[IOC_PAD_PA30].FUNC_CTL = IOC_PA30_FUNC_CTL_PWM0_P_6;
        HPM_IOC->PAD[IOC_PAD_PA31].FUNC_CTL = IOC_PA31_FUNC_CTL_PWM0_P_7;
    }
}



void qeiv2_pins_init(PWM_Type *ptr)
{

    if (ptr == HPM_PWM0)
    {

        HPM_IOC->PAD[IOC_PAD_PB12].FUNC_CTL = IOC_PB12_FUNC_CTL_QEI0_A;
        HPM_IOC->PAD[IOC_PAD_PB13].FUNC_CTL = IOC_PB13_FUNC_CTL_QEI0_B;

    }


}


void qeiv2_cfg_init(void)
{
    qeiv2_reset_counter(BOARD_PMSM0_QEI_BASE);

    qeiv2_set_work_mode(BOARD_PMSM0_QEI_BASE, qeiv2_work_mode_abz);

    qeiv2_config_z_phase_counter_mode(BOARD_PMSM0_QEI_BASE, qeiv2_z_count_inc_on_phase_count_max);
    qeiv2_config_phmax_phparam(BOARD_PMSM0_QEI_BASE, BOARD_PMSM0_QEI_FOC_PHASE_COUNT_PER_REV);


    intc_m_enable_irq_with_priority(BOARD_PMSM0_QEI_IRQ, 1);

    qeiv2_set_phcnt_cmp_value(BOARD_PMSM0_QEI_IRQ, 4);

    qeiv2_set_cmp2_match_option(BOARD_PMSM0_QEI_BASE, true, false, true, true, true, true, true);
    qeiv2_enable_load_read_trigger_event(BOARD_PMSM0_QEI_BASE, QEIV2_EVENT_POSITION_COMPARE_FLAG_MASK);


    qeiv2_release_counter(BOARD_PMSM0_QEI_BASE);

}


void sei_pins_int(SEI_Type *ptr, uint8_t sei_ctrl_idx)
{
      if (ptr == HPM_SEI)
       {

          if (sei_ctrl_idx == SEI_CTRL_1) 
          {
              HPM_IOC->PAD[IOC_PAD_PA16].FUNC_CTL = IOC_PA16_FUNC_CTL_SEI1_DE;
              HPM_IOC->PAD[IOC_PAD_PA17].FUNC_CTL = IOC_PA17_FUNC_CTL_SEI1_CK;
              HPM_IOC->PAD[IOC_PAD_PA18].FUNC_CTL = IOC_PA18_FUNC_CTL_SEI1_TX;
              HPM_IOC->PAD[IOC_PAD_PA19].FUNC_CTL = IOC_PA19_FUNC_CTL_SEI1_RX;
          }
          else if(sei_ctrl_idx == SEI_CTRL_0)
          {
              HPM_IOC->PAD[IOC_PAD_PB12].FUNC_CTL = IOC_PB12_FUNC_CTL_SEI0_DE;
              HPM_IOC->PAD[IOC_PAD_PB13].FUNC_CTL = IOC_PB13_FUNC_CTL_SEI0_CK;
              HPM_IOC->PAD[IOC_PAD_PB14].FUNC_CTL = IOC_PB14_FUNC_CTL_SEI0_TX;
              HPM_IOC->PAD[IOC_PAD_PB15].FUNC_CTL = IOC_PB15_FUNC_CTL_SEI0_RX;
           }
        }
}


void sei_cfg_init(void)
{
    sei_tranceiver_config_t tranceiver_config = {0};
    sei_data_format_config_t data_format_config = {0};
    sei_engine_config_t engine_config = {0};
    sei_trigger_input_config_t trigger_input_conifg = {0};
    sei_state_transition_config_t state_transition_config = {0};
    sei_state_transition_latch_config_t state_transition_latch_config = {0};
    sei_sample_config_t sample_config = {0};
    sei_update_config_t update_config = {0};
    uint8_t instr_idx;
    sei_set_engine_enable(HPM_SEI, PMSM_SEI_CTRL, false);

    /* [1] tranceiver config */
    tranceiver_config.mode = sei_asynchronous_mode;
    tranceiver_config.tri_sample = false;
    tranceiver_config.src_clk_freq = clock_get_frequency(BOARD_MOTOR_CLK_NAME);
    tranceiver_config.asynchronous_config.wait_len = 0;
    tranceiver_config.asynchronous_config.data_len = 8;
    tranceiver_config.asynchronous_config.parity_enable = false;
    tranceiver_config.asynchronous_config.data_idle_high_z = false;
    tranceiver_config.asynchronous_config.data_idle_state = sei_idle_high_state;
    tranceiver_config.asynchronous_config.baudrate = 2500000;
    sei_tranceiver_config_init(HPM_SEI, PMSM_SEI_CTRL, &tranceiver_config);

    /* [2] data register config */
    /* data register 2: send CF */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(HPM_SEI, SEI_SELECT_DATA, SEI_DAT_2, &data_format_config);
    sei_set_data_value(HPM_SEI, SEI_DAT_2, 0x1A);    /* Data ID 3 */
    /* data register 3: recv CF */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(HPM_SEI, SEI_SELECT_DATA, SEI_DAT_3, &data_format_config);
    /* data register 4: recv SF  */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(HPM_SEI, SEI_SELECT_DATA, SEI_DAT_4, &data_format_config);
    /* data register 5: recv ABS0 ABS1 ABS2 */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 24;
    data_format_config.last_bit = 23;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 23;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(HPM_SEI, SEI_SELECT_DATA, SEI_DAT_5, &data_format_config);
    /* data register 6: recv ENID */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;//7
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(HPM_SEI, SEI_SELECT_DATA, SEI_DAT_6, &data_format_config);
    /* data register 7: recv ABM0 ABM1 ABM2 */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 24;
    data_format_config.last_bit = 23;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 23;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(HPM_SEI, SEI_SELECT_DATA, SEI_DAT_7, &data_format_config);
    /* data register 8: recv ALMC */
    data_format_config.mode = sei_data_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    sei_cmd_data_format_config_init(HPM_SEI, SEI_SELECT_DATA, SEI_DAT_8, &data_format_config);
    /* data register 9: check crc */
    data_format_config.mode = sei_crc_mode;
    data_format_config.signed_flag = false;
    data_format_config.bit_order = sei_bit_lsb_first;
    data_format_config.word_order = sei_word_nonreverse;
    data_format_config.word_len = 8;
    data_format_config.crc_invert = false;
    data_format_config.crc_shift_mode = false;
    data_format_config.crc_len = 8;
    data_format_config.last_bit = 7;
    data_format_config.first_bit = 0;
    data_format_config.max_bit = 7;
    data_format_config.min_bit = 0;
    data_format_config.crc_init_value = 0;
    data_format_config.crc_poly = 1;
    sei_cmd_data_format_config_init(HPM_SEI, SEI_SELECT_DATA, SEI_DAT_9, &data_format_config);

    /* [3] instructions */
    instr_idx = 0;
    sei_set_instr(HPM_SEI, instr_idx++, SEI_INSTR_OP_SEND, 0, SEI_DAT_0, SEI_DAT_2, 8);  /* Send CF */
    sei_set_instr(HPM_SEI, instr_idx++, SEI_INSTR_OP_RECV, 0, SEI_DAT_9, SEI_DAT_3, 8);  /* CF */
    sei_set_instr(HPM_SEI, instr_idx++, SEI_INSTR_OP_RECV, 0, SEI_DAT_9, SEI_DAT_4, 8);  /* SF */
    sei_set_instr(HPM_SEI, instr_idx++, SEI_INSTR_OP_RECV, 0, SEI_DAT_9, SEI_DAT_5, 24); /* ST(POS) */
    sei_set_instr(HPM_SEI, instr_idx++, SEI_INSTR_OP_RECV, 0, SEI_DAT_9, SEI_DAT_6, 8);  /* ENID:0x17 */
    sei_set_instr(HPM_SEI, instr_idx++, SEI_INSTR_OP_RECV, 0, SEI_DAT_9, SEI_DAT_7, 24); /* MT */
    sei_set_instr(HPM_SEI, instr_idx++, SEI_INSTR_OP_RECV, 0, SEI_DAT_9, SEI_DAT_8, 8);  /* ALMC */
    sei_set_instr(HPM_SEI, instr_idx++, SEI_INSTR_OP_RECV, 0, SEI_DAT_0, SEI_DAT_9, 8);  /* CRC */
    sei_set_instr(HPM_SEI, instr_idx++, SEI_INSTR_OP_HALT, 0, SEI_DAT_0, SEI_DAT_0, 0);

    /* [4] state transition config */
    /* latch0 */
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = 0;
    sei_state_transition_config_init(HPM_SEI, PMSM_SEI_CTRL, SEI_LATCH_0, SEI_CTRL_LATCH_TRAN_0_1, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(HPM_SEI, PMSM_SEI_CTRL, SEI_LATCH_0, SEI_CTRL_LATCH_TRAN_1_2, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(HPM_SEI, PMSM_SEI_CTRL, SEI_LATCH_0, SEI_CTRL_LATCH_TRAN_2_3, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(HPM_SEI, PMSM_SEI_CTRL, SEI_LATCH_0, SEI_CTRL_LATCH_TRAN_3_0, &state_transition_config);

    state_transition_latch_config.enable = true;
    state_transition_latch_config.output_select = SEI_CTRL_LATCH_TRAN_0_1;
    state_transition_latch_config.delay = (48 * (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 1000000)) / 100;    /*  unit: 1us, 0.48us */
    sei_state_transition_latch_config_init(HPM_SEI, PMSM_SEI_CTRL, SEI_LATCH_0, &state_transition_latch_config);

    /* latch1 */
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = false;
    state_transition_config.instr_ptr_cfg = sei_state_tran_condition_fall_leave;
    state_transition_config.instr_ptr_value = (instr_idx - 2);
    sei_state_transition_config_init(HPM_SEI, PMSM_SEI_CTRL, SEI_LATCH_1, SEI_CTRL_LATCH_TRAN_0_1, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(HPM_SEI, PMSM_SEI_CTRL, SEI_LATCH_1, SEI_CTRL_LATCH_TRAN_1_2, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(HPM_SEI, PMSM_SEI_CTRL, SEI_LATCH_1, SEI_CTRL_LATCH_TRAN_2_3, &state_transition_config);
    state_transition_config.disable_clk_check = true;
    state_transition_config.disable_txd_check = true;
    state_transition_config.disable_rxd_check = true;
    state_transition_config.disable_timeout_check = true;
    state_transition_config.disable_instr_ptr_check = true;
    sei_state_transition_config_init(HPM_SEI, PMSM_SEI_CTRL, SEI_LATCH_1, SEI_CTRL_LATCH_TRAN_3_0, &state_transition_config);

    state_transition_latch_config.enable = true;
    state_transition_latch_config.output_select = SEI_CTRL_LATCH_TRAN_0_1;
    state_transition_latch_config.delay = 0;
    sei_state_transition_latch_config_init(HPM_SEI, PMSM_SEI_CTRL, SEI_LATCH_1, &state_transition_latch_config);

    /* [5] sample config*/
    sample_config.latch_select = SEI_LATCH_0;
    sei_sample_config_init(HPM_SEI, PMSM_SEI_CTRL, &sample_config);

     /* [6] update config*/
    update_config.pos_data_idx = SEI_DAT_5;
    update_config.rev_data_idx = SEI_DAT_7;
    update_config.pos_data_use_rx = true;
    update_config.rev_data_use_rx = true;
    update_config.update_on_err = false;
    update_config.latch_select = SEI_LATCH_1;
    update_config.data_register_select = BIT5_MASK | BIT7_MASK;    /* SEI_DAT_5, SEI_DAT_7 */
    sei_update_config_init(HPM_SEI, PMSM_SEI_CTRL, &update_config);

    /* [8] enbale sync timer timestamp */
    synt_enable_timestamp(HPM_SYNT, true);

    /* [9] engine config */
    engine_config.arming_mode = sei_arming_wait_trigger;
    engine_config.data_cdm_idx = 0;
    engine_config.data_base_idx = 0;
    engine_config.init_instr_idx = 0;
    engine_config.wdg_enable = false;
    sei_engine_config_init(HPM_SEI, PMSM_SEI_CTRL, &engine_config);
    sei_set_engine_enable(HPM_SEI, PMSM_SEI_CTRL, true);
   // printf("Started sei engine!\n");

    /* [10] trigger config */
    trigger_input_conifg.trig_period_enable = true;
    trigger_input_conifg.trig_period_arming_mode = sei_arming_direct_exec;
    trigger_input_conifg.trig_period_sync_enable = false;
    trigger_input_conifg.trig_period_time = (0.05* (clock_get_frequency(BOARD_MOTOR_CLK_NAME) / 1000));    /* unit: 1ms, 200ms */
    sei_trigger_input_config_init(HPM_SEI, PMSM_SEI_CTRL, &trigger_input_conifg);

   
}


void encoder_init(MOTOR_PARA *motor_par,pos_angle_CalObj* pos_angleCalHdl)
{
    uint8_t motor_id = motor_par->foc_para.pwmpar.pwmout.i_motor_id-1;
    motor_par->foc_para.motorpar.i_poles_n = globalAxisParm[motor_id].parmMotor.Mortor_PoleNum0x4004;
    pos_angleCalHdl->maxph = globalAxisParm[motor_id].parmMotor.Motor_PlusCnt0x4008 ;
    pos_angleCalHdl->motor_pole = globalAxisParm[motor_id].parmMotor.Mortor_PoleNum0x4004;
    pos_angleCalHdl->encoder_type= globalAxisParm[motor_id].parmMotor.Motor_EncType0x4007;

    if(pos_angleCalHdl->encoder_type == ABS)
    {
        sei_pins_int(HPM_SEI, PMSM_SEI_CTRL);
        sei_cfg_init();
        board_delay_ms(5);
        pos_angleCalHdl->z = sei_get_data_value(HPM_SEI, SEI_DAT_7); 
        pos_angleCalHdl->ph = sei_get_data_value(HPM_SEI, SEI_DAT_5);
        pos_angleCalHdl->pos0 = pos_angleCalHdl->ph + pos_angleCalHdl->z*pos_angleCalHdl->maxph ;
        

    }
    else if(pos_angleCalHdl->encoder_type  == ABZ)
    {
        qeiv2_pins_init(BOARD_PMSM0PWM);
        qeiv2_cfg_init();
    }
    
}

 
void motor_foc_angle_align(uint8_t i_motor_id)
{
     
    uint8_t motor_id = i_motor_id-1;
    int16_t zero_controlword = globalAxisParm[motor_id].parmCmd.zero_ControlWord0x9002 &(~ globalAxisParm[motor_id].parmCmd.status0x9003);
    if(zero_controlword)
    {    
       motor0.adc_trig_event_callback =  &motor0_angle_align_loop;
       enable_all_pwm_output(BOARD_PMSM0PWM);
       Motor_Control_Global.zero_cnt++;   

       if((Motor_Control_Global.commu_IdRef < 100)&&(Motor_Control_Global.zero_cnt<=5))
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
                    if(Motor_Control_Global.commu_IdRef>0)
                    {
                           Motor_Control_Global.commu_IdRef = Motor_Control_Global.commu_IdRef - 10;                         
                    }
                    else
                    { 
                           disable_all_pwm_output(BOARD_PMSM0PWM);     
                           globalAxisParm[motor_id].parmCmd.status0x9003 = 1;   
                           encoder_init(&motor0,&pos_angleObj[0]);
                           motor0.adc_trig_event_callback = motor0_highspeed_loop;                 
                    }

              }

        }

    }

}
    



void pmsm_motor1_init(void)
{
    adc_pins_init(BOARD_PMSM0PWM);

    pwm_pins_init(BOARD_PMSM0PWM);

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
    motor0_clock_hz = clock_get_frequency(BOARD_MOTOR_CLK_NAME);
    pwm_duty_init(BOARD_PMSM0PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare);
    pwm_trigfor_adc_init(BOARD_PMSM0PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare, BOARD_PMSM0_PWM_TRIG_CMP_INDEX);
    pwm_trigfor_currentctrl_init(BOARD_PMSM0PWM, PWM_RELOAD, pwm_shadow_register_update_on_shlk, pwm_cmp_mode_output_compare, BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP);

    timer_init();
    motor_param_int(&motor0, BLDC_MOTOR0_INDEX);
    lv_set_adval_middle(&motor0.foc_para, BOARD_PMSM0_ADC_TRG);
}

