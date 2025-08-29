/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rdc_cfg.h"
#include "hpm_rdc_drv.h"
#include "hpm_trgm_drv.h"
#include "math.h"
#include "hpm_adc16_drv.h"
#include "hpm_gpio_drv.h"
#include "rdc.h"
#include "qeo_init.h"
//#include "SEGGER_RTT.h"

rdc_obj  rdcObj;
pll_para_t pll;
speed_Cal_Obj speed_CalObj[2];
qeo_abz_Obj qeo_abzObj;
sei_angle_CalObj sei_angleCalObj;
pll_ii_t pll_ii;


void init_trigger_cfg(uint8_t trig_ch, bool inten)
{
    adc16_pmt_config_t pmt_cfg;

    pmt_cfg.trig_ch   = trig_ch;
    pmt_cfg.trig_len  = BOARD_BLDC_ADC_PREEMPT_TRIG_LEN;
    pmt_cfg.adc_ch[0] = BOARD_RDC_ADC_I_CHN;
    pmt_cfg.inten[0] = inten;
    adc16_set_pmt_config(BOARD_RDC_ADC_I_BASE, &pmt_cfg);
    pmt_cfg.adc_ch[0] = BOARD_RDC_ADC_Q_CHN;
    adc16_set_pmt_config(BOARD_RDC_ADC_Q_BASE, &pmt_cfg);
}
void adc_init(void)
{
    adc16_config_t cfg;
    adc16_channel_config_t ch_cfg;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);
    cfg.res            = adc16_res_16_bits;
    cfg.conv_mode      = adc16_conv_mode_oneshot;
    cfg.adc_clk_div    = adc16_clock_divider_4;
    cfg.sel_sync_ahb   = true;
    cfg.adc_ahb_en = true;
    adc16_init(BOARD_RDC_ADC_I_BASE, &cfg);
    adc16_init(BOARD_RDC_ADC_Q_BASE, &cfg);

    adc16_get_channel_default_config(&ch_cfg);
    ch_cfg.sample_cycle  = 20;
    ch_cfg.ch            = BOARD_RDC_ADC_I_CHANNEL;
    adc16_init_channel(BOARD_RDC_ADC_I_BASE, &ch_cfg);
    ch_cfg.sample_cycle  = 20;
    ch_cfg.ch            = BOARD_RDC_ADC_Q_CHANNEL;
    adc16_init_channel(BOARD_RDC_ADC_Q_BASE, &ch_cfg);

    adc16_enable_motor(BOARD_RDC_ADC_I_BASE);
    adc16_enable_motor(BOARD_RDC_ADC_Q_BASE);
}

void init_pwm_pla_trgm(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;

/*The GPIO is designed for debug */
 #ifdef RDC_SAMPLE_TEST_GPIO_OUTPUT
    /* trig0 p0 */
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_falling_edge;
    trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_RDC_TRGO_1;
    trgm_output_config(ptr, BOARD_RDC_TRG_NUM, &trgm_output_cfg);
    trgm_enable_io_output(ptr, 1);
 #endif

    /* trig0 adc */
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_falling_edge;
    trgm_output_cfg.input  = BOARD_RDC_TRGIGMUX_IN_NUM;
    trgm_output_config(ptr, BOARD_RDC_TRG_ADC_NUM, &trgm_output_cfg);
}

///////延时测量验证/////////////////////////////////////

void exc_sgn_cfg_out(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;
    HPM_IOC->PAD[IOC_PAD_PA20].FUNC_CTL = IOC_PA20_FUNC_CTL_TRGM0_P_00;//IOC_PB04_FUNC_CTL_TRGM0_P_00;
    /* trig0 p0 */
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_RDC_TRGO_1;
    trgm_output_config(ptr, BOARD_RDC_TRG_NUM, &trgm_output_cfg);
    trgm_enable_io_output(ptr, 1);
}

void rdc_cfg(RDC_Type *rdc)
{
    rdc_output_cfg_t cfg;
#ifndef DAC_MODE
    cfg.mode = rdc_output_pwm;
#else
    cfg.mode = rdc_output_dac;
#endif
    cfg.excitation_period_cycle = 16000;
    cfg.excitation_precision = rdc_output_precision_32_point;///pwm频率320k
    cfg.pwm_period = rdc_output_pwm_period_1_sample;
    cfg.output_swap = true;
    cfg.amp_offset = 0;
    cfg.amp_man = 7;
    cfg.amp_exp = 4;
    cfg.pwm_dither_enable = false;
    cfg.pwm_exc_n_low_active = false;
    cfg.pwm_exc_p_low_active = false;
    rdc_output_config(rdc, &cfg);
#if defined(HPM_IP_FEATURE_RDC_IIR) && (HPM_IP_FEATURE_RDC_IIR)
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_APP_RDC_ADC_MATRIX_TO_ADC0, BOARD_APP_RDC_ADC_MATRIX_FROM_ADC_I, false);
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_APP_RDC_ADC_MATRIX_TO_ADC1, BOARD_APP_RDC_ADC_MATRIX_FROM_ADC_Q, false);
#endif
    rdc_output_trig_offset_config(rdc, trigger_out_0, 0);
    rdc_output_trig_enable(rdc, trigger_out_0);
    /////延时测量验证//////////////////////////////////////////////
    //rdc_output_trig_offset_config(rdc, trigger_out_1, 0);
    //rdc_output_trig_enable(rdc, trigger_out_1);
    //////////////////////////////////////////////////////////////
    rdc_exc_enable(rdc);

    rdc_input_cfg_t inputcfg;
    inputcfg.rectify_signal_sel = rdc_rectify_signal_exc_0_ph;//延时测量超过半个sin函数周期，配置为2
    inputcfg.acc_cycle_len = 0;
    inputcfg.acc_input_chn_i = BOARD_RDC_ADC_I_CHN;
    inputcfg.acc_input_chn_q = BOARD_RDC_ADC_Q_CHN;
    inputcfg.acc_input_port_i = 0;
    inputcfg.acc_input_port_q = 1;
    inputcfg.acc_stamp = rdc_acc_stamp_end_of_acc;
    rdc_input_config(rdc, &inputcfg);

    rdc_acc_cfg_t acc_cfg;
    acc_cfg.continue_edge_num = 0;
    acc_cfg.edge_distance = 10;
    acc_cfg.exc_carrier_period = 16000;
    acc_cfg.right_shift_without_sign = 8;
    rdc_set_acc_config(rdc, &acc_cfg);

    rdc_acc_enable(rdc);
}

void init_common_config(dac_mode_t mode)
{
    dac_config_t config,config1;

    dac_get_default_config(&config);

    config.dac_mode = mode;
    config.sync_mode = (clk_dac_src_ahb0 == clock_get_source(BOARD_APP_DAC_CLOCK_NAME)) ? true : false;

    dac_init(BOARD_RDC_DAC_I_BASE, &config);

    dac_get_default_config(&config1);
    config1.dac_mode = mode;
    config1.sync_mode = (clk_dac_src_ahb0 == clock_get_source(BOARD_APP_DAC_CLOCK_NAME1)) ? true : false;

    dac_init(BOARD_RDC_DAC_Q_BASE, &config1);
}

void rdc_init(void)
{
    uint32_t freq;
    int32_t  val_max_i, val_min_i; 
    int32_t  val_max_q, val_min_q; 
    int32_t val_middle_i, val_middle_q;
    uint8_t num;
    uint32_t val_delay_i,val_delay_q = 0;
    rdcObj.flag_once = 1;
    /*Open INH */
    HPM_IOC->PAD[GPIO_DO_GPIOB].FUNC_CTL = IOC_PB10_FUNC_CTL_GPIO_B_10;
    gpio_set_pin_output(HPM_GPIO0, GPIO_DO_GPIOB,10);
    gpio_write_pin(HPM_GPIO0, GPIO_DO_GPIOB, 10, 1); 

    /* Initialize a ADC clock */
    board_init_adc_clock(BOARD_RDC_ADC_I_BASE, true);
    board_init_adc_clock(BOARD_RDC_ADC_Q_BASE, true);

    /* Initialize a DAC clock */
    board_init_dac_clock(BOARD_RDC_DAC_I_BASE, false);
    board_init_dac_clock(BOARD_RDC_DAC_Q_BASE, false);

    /* Initialize a DAC pin */
    board_init_dac_pins(BOARD_RDC_DAC_I_BASE);
    board_init_dac_pins(BOARD_RDC_DAC_Q_BASE);
    
    /* Config DAC mode */
    init_common_config(dac_mode_direct);
    dac_enable_conversion(BOARD_RDC_DAC_I_BASE, true);
    dac_set_direct_config(BOARD_RDC_DAC_I_BASE, 2048);
    dac_enable_conversion(BOARD_RDC_DAC_Q_BASE, true);
    dac_set_direct_config(BOARD_RDC_DAC_Q_BASE, 2048);

#ifdef DAC_MODE
    init_dac();
    trgm_dac_matrix_config(HPM_TRGM0, trgm_dac_matrix_output_to_dac0, trgm_dac_matrix_in_from_rdc_dac0, false);
#endif
    init_rdc_pin();
    adc_init();
    init_pwm_pla_trgm(BOARD_RDC_TRGM);
    //exc_sgn_cfg_out(BOARD_RDC_TRGM);
    init_trigger_cfg(BOARD_RDC_ADC_TRG, true);
    adc16_set_pmt_queue_enable(BOARD_RDC_ADC_I_BASE, BOARD_RDC_ADC_TRG, true);
    adc16_set_pmt_queue_enable(BOARD_RDC_ADC_Q_BASE, BOARD_RDC_ADC_TRG, true);   
    freq = clock_get_frequency(BOARD_PLB_PWM_CLOCK_NAME);
    rdc_cfg(BOARD_RDC_BASE);
    rdc_output_trig_sw(BOARD_RDC_BASE);
   
    num = 0;
    board_delay_ms(1000);
    uint16_t dac_value0,dac_value1;
    dac_value0 = 2048;
    dac_value1 = 2048;
    /*DAC modefy offset value dynamically*/
    do {
        rdc_clear_i_maxval(BOARD_RDC_BASE);
        rdc_clear_i_minval(BOARD_RDC_BASE);
        rdc_clear_q_maxval(BOARD_RDC_BASE);
        rdc_clear_q_minval(BOARD_RDC_BASE);
        do {
            num++;
            for (uint32_t j = 0; j < 100000; j++) {
                NOP();
            }
            val_max_i = rdc_get_i_maxval(BOARD_RDC_BASE);
            val_min_i = rdc_get_i_minval(BOARD_RDC_BASE);

            val_max_q = rdc_get_q_maxval(BOARD_RDC_BASE);
            val_min_q = rdc_get_q_minval(BOARD_RDC_BASE);
        } while (num < 20);
        val_middle_i =  ((val_max_i - val_min_i) / 2 + val_min_i - 0x800000);
        val_middle_q =  ((val_max_q - val_min_q) / 2 + val_min_q - 0x800000);

        if ((val_middle_i ) < -1*256)
         {
            dac_set_direct_config(BOARD_RDC_DAC_I_BASE, dac_value0++);
        } 
        else if ((val_middle_i ) > 1*256)
         {
            dac_set_direct_config(BOARD_RDC_DAC_I_BASE, dac_value0--);
        } 
        else 
        {
            break;
        }
        if ((dac_value0 >= 4095) || (dac_value0 <= 0)) {
            break;
        }
        if ((val_middle_q ) < -1*256)
         {
            dac_set_direct_config(BOARD_RDC_DAC_Q_BASE, dac_value1++);
        } 
        else if ((val_middle_q ) > 1*256)
         {
            dac_set_direct_config(BOARD_RDC_DAC_Q_BASE, dac_value1--);
        } 
        else 
        {
            break;
        }
        if ((dac_value1 >= 4095) || (dac_value1 <= 0)) {
            break;
        }

        board_delay_ms(1);
    } while (1);
   /*dealy measure*/
    num = 0;
    val_delay_i = 1;
    do {
        num++;
        val_delay_i += rdc_get_rise_delay_i(BOARD_RDC_BASE);
        for (uint32_t j = 0; j < 100000; j++) {
            NOP();
        }
    } while (num < 100);
    val_delay_i /= num;
    if(abs(val_delay_i-I_DELAY_CONSTANT)>500)
    {
        val_delay_i = I_DELAY_CONSTANT;
    }
    rdc_set_acc_sync_delay(BOARD_RDC_BASE, rdc_acc_chn_i, val_delay_i);
    
    num = 0;
    val_delay_q = 1;
    do {
        num++;
        val_delay_q += rdc_get_rise_delay_q(BOARD_RDC_BASE);
        for (uint32_t j = 0; j < 100000; j++) {
            ;
        }
    } while (num < 100);
    val_delay_q /= num;
    if(abs(val_delay_q-Q_DELAY_CONSTANT)>500)
    {
      val_delay_q = Q_DELAY_CONSTANT;
    }
    rdc_set_acc_sync_delay(BOARD_RDC_BASE, rdc_acc_chn_q, val_delay_q);

    board_delay_ms(100);
    rdcObj.sin_i = rdc_get_acc_avl(BOARD_RDC_BASE, rdc_acc_chn_i);
    rdcObj.cos_q = rdc_get_acc_avl(BOARD_RDC_BASE, rdc_acc_chn_q);
    rdcObj.rdc_theta = (atan2f((float)(rdcObj.sin_i),(float)(rdcObj.cos_q))*180/PI);
    if(rdcObj.rdc_theta < 0)
    {
        rdcObj.rdc_theta = 360+ rdcObj.rdc_theta;
    }
    if(rdcObj.flag_once==1)
    {      
        pll.user.theta_last = rdcObj.rdc_theta/ANGLE_TO_RADIAN;
        pll_ii.user.x3 = rdcObj.rdc_theta/ANGLE_TO_RADIAN;
        pll_ii.user.x2 = pll_ii.user.x3*10000;
        sei_angleCalObj.theta0 = rdcObj.rdc_theta; 
        speed_CalObj[0].speedlasttheta = rdcObj.rdc_theta; 
        speed_CalObj[1].speedlasttheta = rdcObj.rdc_theta;   
                                     
    }
    pll_ii.cfg.sin = (float)rdcObj.sin_i;
    pll_ii.cfg.cos = (float)rdcObj.cos_q;
    rdcObj.flag_once = 2; 
    rdc_interrupt_config(BOARD_RDC_BASE, RDC_INT_EN_ACC_VLD_I_EN_MASK | RDC_INT_EN_ACC_VLD_Q_EN_MASK);
    rdc_interrupt_enable(BOARD_RDC_BASE);
    intc_m_enable_irq_with_priority(IRQn_RDC, 1);

    rdcObj.rdc_config_ok = 1;

}

void fault_diagnosis(int32_t sin_i,int32_t cos_q)
{
    float sin,cos,constant_value=0;
    if(sin_i<0)
    {
        sin = -sin_i>>8;
    }
    else
    {
        sin = sin_i>>8;
    }
    if(cos_q<0)
    {
        cos = -cos_q>>8;
    }
    else
    {
        cos = cos_q>>8;
    }
    constant_value = sin*sin+cos*cos;
    if(sin<5&&cos<5)
    {
       rdcObj.rdc_satus = exc_break;
       return;
    }
    if(sin<5)
    {
       rdcObj.rdc_satus = sin_break;
       return;
    }
    if(cos<5)
    {
       rdcObj.rdc_satus = cos_break;
       return;
    }   
}

void board_init_rtt_test(void)
{
    init_py_pins_as_pgpio();
    board_init_usb_dp_dm_pins();

    board_init_clock();
    board_init_console();
    board_init_pmp();
}
/*10k rdc isr*/
void isr_acc_i_q_sample(void)
{
    uint32_t status = get_interrupt_status(BOARD_RDC_BASE);
    if(rdcObj.rdc_config_ok!=1)
    {
      return; 
    }
    if(RDC_INT_EN_ACC_VLD_I_EN_GET(status))
    {
        rdc_interrupt_clear_flag_bits(BOARD_RDC_BASE,RDC_INT_EN_ACC_VLD_I_EN_MASK);
        rdcObj.acc_cnt_i ++;
    }
    if(RDC_INT_EN_ACC_VLD_Q_EN_GET(status))
    {
       rdc_interrupt_clear_flag_bits(BOARD_RDC_BASE,RDC_INT_EN_ACC_VLD_Q_EN_MASK);
       rdcObj.acc_cnt_q ++;
    }
    if ((rdcObj.acc_cnt_i >= 1) && (rdcObj.acc_cnt_q >= 1)) 
    {
        rdcObj.sin_i = rdc_get_acc_avl(BOARD_RDC_BASE, rdc_acc_chn_i);
        rdcObj.cos_q = rdc_get_acc_avl(BOARD_RDC_BASE, rdc_acc_chn_q);
         
        fault_diagnosis(rdcObj.sin_i,rdcObj.cos_q);
        rdcObj.rdc_theta = (atan2f((float)(rdcObj.sin_i),(float)(rdcObj.cos_q))*180/PI);
        if(rdcObj.rdc_theta < 0)
        {
           rdcObj.rdc_theta = 360+ rdcObj.rdc_theta;
        }

        pll_ii.cfg.sin = (float)rdcObj.sin_i;
        pll_ii.cfg.cos = (float)rdcObj.cos_q;

        //////theta_rdc///////////////////////////////////////////////////
        speed_CalObj[0].speedtheta = rdcObj.rdc_theta;
        speed_cal(&speed_CalObj[0]);
        int32_t rdc_vel = speed_CalObj[0].o_speedout*100;
        int32_t rdc_theta = rdcObj.rdc_theta*100;
        ///////////////////////////////////////////////////////////////   
             
        

        //////qeiv2///////////////////////////////////////////////////////////

         //qeiv2_theta_cal(&qeiv2Obj);
         //int32_t qei_theta = qeiv2Obj.ele_angle*1000;
         //speed_CalObj[1].speedtheta = qeiv2Obj.ele_angle;
         //speed_cal(&speed_CalObj[1]);
         //int32_t qei_speed = speed_CalObj[1].o_speedout_filter*1000;

        ///////////////////////////////////////////////////////////////////////
        

        //////////pll////////////////////////////////////////////////////////
         pll.cfg.sin_val = (float)rdcObj.sin_i;
         pll.cfg.cos_val = (float)rdcObj.cos_q;
         pll_pro(&pll);
        int32_t pll_vel = pll.out.speedout*100/6.28;
        int32_t pll_theta = pll.out.theta*100;
         
        ////////////////////////////////////////////////////////////////////////

        /////////pll_type_ii////////////////////////////////////////////////////
        pll_type_ii_pro(&pll_ii);        
        int32_t pll_ii_vel = pll_ii.out.speed_filter*100/6.28;
        int32_t pll_ii_theta = pll_ii.out.theta*100;
        ///////////////////////////////////////////////////////////////////////



        /////QEO//////////////////////////////////////////////////////////////////

#if ABZ_OUTPUT
         qeo_abzObj.vel = rdcObj.pll_vel;
        //qeo_abz_output(&qeo_abzObj);
#endif

        //////////////////////////////////////////////////////////////////
        

        //////sei_theta////////////////////////////////////////////////////
#if ABS_ENCODER_23BIT
        sei_angle(&sei_angleCalObj);
        rdcObj.sei_theta = sei_angleCalObj.elec_angle;
        int32_t sei_theta = sei_angleCalObj.elec_angle*100;
        speed_CalObj[1].speedtheta = sei_angleCalObj.elec_angle;
        speed_cal(&speed_CalObj[1]);
        int32_t sei_vel = speed_CalObj[1].o_speedout*100;
#endif

#if SEGGER_RTT_DEBUG
        int32_t rdc_error = sei_theta - rdc_theta;
        int32_t pll_error = sei_theta - pll_theta;
        int32_t pll_ii_error = sei_theta-pll_ii_theta;
        SEGGER_RTT_printf(0, "%d,%d,%d,%d,%d\r\n",rdc_error,pll_error,pll_ii_error,pll_vel,pll_ii_vel);
       //SEGGER_RTT_printf(0, "%d,%d,%d\r\n",rdc_vel,pll_vel,pll_ii_vel);
        
#endif
        rdcObj.acc_cnt_q--;
        rdcObj.acc_cnt_i--;
    }
    if ((rdcObj.acc_cnt_q >= 2) || (rdcObj.acc_cnt_i >= 2)) 
    {
#if LED_STATUS_ENABLE
        led_red_error();
#endif
        while (1);
    }

}
SDK_DECLARE_EXT_ISR_M(IRQn_RDC, isr_acc_i_q_sample)



