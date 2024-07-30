
#include "rdc_init.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_rdc_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_motor_math.h"
#include "hpm_adc16_drv.h"





#ifndef BOARD_APP_PWM_BASE
#define BOARD_APP_PWM_BASE BOARD_BLDCPWM
#endif
#ifndef BOARD_APP_PWM_IRQ 
#define BOARD_APP_PWM_IRQ  IRQn_PWM0
#endif
#ifndef BOARD_APP_MOTOR_CLK
#define BOARD_APP_MOTOR_CLK BOARD_BLDC_QEI_CLOCK_SOURCE
#endif


#define PWM_FREQUENCY               (20000) /*20k pwm freq*/
#define motor_clock_hz              clock_get_frequency(BOARD_APP_MOTOR_CLK)
#define PWM_RELOAD                  (motor_clock_hz/PWM_FREQUENCY) 
#define PWM_TRIG_CMP_INDEX          (9U)

SPEED_CAL g_speed_cal;

/**
 * @brief           init ADC16 trigger configuration struct for the preemption mode.
 * @param[in]       trig_ch    trig channel.
 * @param[in]       inten      interrupt enable.
 */
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
/**
 * @brief           init ADC16 configuration .
 */
void adc_init(void)
{
    adc16_config_t cfg;
    adc16_channel_config_t ch_cfg;
    adc16_prd_config_t prd_cfg;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);
    cfg.res            = adc16_res_16_bits;
    cfg.conv_mode      = adc16_conv_mode_preemption;
    cfg.adc_clk_div    = adc16_clock_divider_4;
    cfg.sel_sync_ahb   = true;
    cfg.adc_ahb_en = true;
    adc16_init(BOARD_RDC_ADC_I_BASE, &cfg);
    adc16_init(BOARD_RDC_ADC_Q_BASE, &cfg);

    adc16_get_channel_default_config(&ch_cfg);
    ch_cfg.sample_cycle  = 20;
    ch_cfg.ch            = BOARD_RDC_ADC_I_CHN;
    adc16_init_channel(BOARD_RDC_ADC_I_BASE, &ch_cfg);
    ch_cfg.sample_cycle  = 20;
    ch_cfg.ch            = BOARD_RDC_ADC_Q_CHN;
    adc16_init_channel(BOARD_RDC_ADC_Q_BASE, &ch_cfg);

    adc16_enable_motor(BOARD_RDC_ADC_I_BASE);
    adc16_enable_motor(BOARD_RDC_ADC_Q_BASE);
}
/**
 * @brief           config TRGM input and output .
 */
void init_pwm_pla_trgm(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;

/*The GPIO is designed for debug */
#ifdef RDC_SAMPLE_TEST_GPIO_OUTPUT
    /* trig0 p0 */
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_falling_edge;
    trgm_output_cfg.input  = BOARD_RDC_TRGIGMUX_IN_NUM;
    trgm_output_config(ptr, BOARD_RDC_TRG_NUM, &trgm_output_cfg);
    trgm_enable_io_output(ptr, 1);
#endif

    /* trig0 adc */
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_falling_edge;
    trgm_output_cfg.input  = BOARD_RDC_TRGIGMUX_IN_NUM;
    trgm_output_config(ptr, BOARD_RDC_TRG_ADC_NUM, &trgm_output_cfg);
}
/**
 * @brief           config Rdc output  .
 */
void rdc_cfg(RDC_Type *rdc)
{
    rdc_output_cfg_t cfg;
#ifndef DAC_MODE
    cfg.mode = rdc_output_pwm;
#else
    cfg.mode = rdc_output_dac;
#endif
/***************User changes ecited signal frequency by modifying rate********************************************************************/
    cfg.excitation_period_cycle = 35840;   //this parameter / RDC_clk(160.000.000Hz) = excited signal cycle(s), and its frequency(Hz) = RDC_clk / (rate * 2 ^ (excitation_precision) * 4).
    cfg.excitation_precision = rdc_output_precision_64_point;
    cfg.pwm_period = rdc_output_pwm_period_1_sample;    //(this parameter + 1) * rate / RDC_clk(160.000.000Hz) (s). 
    cfg.output_swap = true;
    cfg.amp_offset = 0;
    cfg.amp_man = 2;   //The max amplitude of sin(theta) = 4095 x amp_man / 2 ^ amp_exp. 0~4095 to 0~3.3V.
    cfg.amp_exp = 3;
    cfg.pwm_dither_enable = false;
    cfg.pwm_exc_n_low_active = false;
    cfg.pwm_exc_p_low_active = false;
    rdc_output_config(rdc, &cfg);
    rdc_output_trig_offset_config(rdc, trigger_out_0, 150);
    rdc_output_trig_enable(rdc, trigger_out_0);
    rdc_output_trig_disable(rdc, trigger_out_1);
    rdc_exc_enable(rdc);
/*****************************************************************************************************************************************/    
    rdc_input_cfg_t inputcfg;
    inputcfg.rectify_signal_sel = rdc_rectify_signal_exc_0_ph;
    inputcfg.acc_cycle_len = 4;    //position refreshed time(n sin period).if spped is high, its refreshed time should be low. 
    inputcfg.acc_input_chn_i = BOARD_RDC_ADC_I_CHN;    //differ excited signal input pin channels.
    inputcfg.acc_input_chn_q = BOARD_RDC_ADC_Q_CHN;
    inputcfg.acc_input_port_i = 0;    //RDC feedbak sin signal input IP port.
    inputcfg.acc_input_port_q = 1;    //RDC feedbak cos signal input IP port.
    inputcfg.acc_stamp = rdc_acc_stamp_end_of_acc;
    rdc_input_config(rdc, &inputcfg);

    rdc_acc_cfg_t acc_cfg;
    acc_cfg.continue_edge_num = 4;    //zero crossing point filter.
    acc_cfg.edge_distance = 1;    //the minimun sample cycle.
    acc_cfg.exc_carrier_period = 35840;    //this parameter / RDC_clk(160.000.000Hz) = excited signal cycle(s), and its frequency(Hz) = RDC_clk / (rate * 2 ^ (excitation_precision) * 4).
    acc_cfg.right_shift_without_sign = 8;
    rdc_set_acc_config(rdc, &acc_cfg);
    

    rdc_acc_enable(rdc);    //RDC enable cumulative.
}


#if DAC_MODE
#define TEST_DAC HPM_DAC0
void init_dac(void)
{
    init_dac_pins(TEST_DAC);
    HPM_SYNT->GCR = (1 << 4);

    dac_config_t config;
    dac_get_default_config(&config);
    config.dac_mode = 0; /* dac_mode_direct */
    dac_init(TEST_DAC, &config);

    dac_enable_conversion(TEST_DAC, true);

    TEST_DAC->CFG0_BAK &= ~DAC_CFG0_DAC_MODE_MASK;
    TEST_DAC->CFG0_BAK |= DAC_CFG0_DAC_MODE_SET(3); /* trig mode */

    TEST_DAC->CFG0 = TEST_DAC->CFG0_BAK;
}
#endif

/**
 * @brief           init Rdc   .
 */
void rdc_init(void)
{
    uint32_t freq;
    int32_t  val_max_i, val_min_i, adc_val_i = 0;
    uint32_t val_delay_i, val_delay_i_r, val_delay__i_f;
    int32_t  val_max_q, val_min_q, adc_val_q = 0;
    uint32_t val_delay_q, val_delay_q_r, val_delay__q_f;
    int32_t val_middle_i, val_middle_q;
    uint8_t num;
#if DAC_MODE           //outputting the excitation signal in DAC mode, this scheme is not used. 
    init_dac();
    trgm_dac_matrix_config(HPM_TRGM0, trgm_dac_matrix_output_to_dac0, trgm_dac_matrix_in_from_rdc_dac0, false);
#endif
    init_rdc_pin();    //rdc pins init.
    adc_init();        //rdc adc include exc, sin and cos signals.
    init_pwm_pla_trgm(BOARD_RDC_TRGM);      //pwm trig for adc.
    init_trigger_cfg(BOARD_RDC_ADC_TRG, true);    //adc init with preemption mode.
    adc16_set_pmt_queue_enable(BOARD_RDC_ADC_I_BASE, BOARD_RDC_ADC_TRG, true);    //enable adc init with preemption mode.
    adc16_set_pmt_queue_enable(BOARD_RDC_ADC_Q_BASE, BOARD_RDC_ADC_TRG, true);


    freq = clock_get_frequency(BOARD_PLB_PWM_CLOCK_NAME);
    printf("freq:%d.\r\n", freq);
    rdc_cfg(HPM_RDC);    //RDC init. include detailed introduction.
   
    rdc_output_trig_sw(HPM_RDC);    //enable excited signal output.
    num = 0;
    rdc_clear_i_maxval(HPM_RDC);   //init RDC input max and min value.
    rdc_clear_i_minval(HPM_RDC);
    rdc_clear_q_maxval(HPM_RDC);
    rdc_clear_q_minval(HPM_RDC);
/**************************cal excited signal zero offset***************************/
    do {
        num++;
        for (uint32_t j = 0; j < 100000; j++) {
            NOP();
        }
        val_max_i = rdc_get_i_maxval(HPM_RDC);
        val_min_i = rdc_get_i_minval(HPM_RDC);

        val_max_q = rdc_get_q_maxval(HPM_RDC);
        val_min_q = rdc_get_q_minval(HPM_RDC);
    } while (num < 20);      
    val_middle_i =  -((-val_max_i + val_min_i) / 2 - val_min_i + 0x800000);
    rdc_set_edge_detection_offset(HPM_RDC, rdc_acc_chn_i, val_middle_i);
    val_middle_q =  -((-val_max_q + val_min_q) / 2 - val_min_q + 0x800000);
    rdc_set_edge_detection_offset(HPM_RDC, rdc_acc_chn_q, val_middle_q);
/******************************end************************************************/
/*cal feedback excited signal zero crossing point time. cal delay for the flip count*/
    num = 0;
    val_delay_i = 1;
    do {
        num++;
        val_delay_i += rdc_get_rise_delay_i(HPM_RDC);
        for (uint32_t j = 0; j < 100000; j++) {
            NOP();
        }
    } while (num < 100);
    val_delay_i /= num;
    rdc_set_acc_sync_delay(HPM_RDC, rdc_acc_chn_i, val_delay_i >> 1);

    num = 0;
    val_delay_q = 1;
    do {
        num++;
        val_delay_q += rdc_get_rise_delay_q(HPM_RDC);
        for (uint32_t j = 0; j < 100000; j++) {
            ;
        }
    } while (num < 100);
    val_delay_q /= num;
    rdc_set_acc_sync_delay(HPM_RDC, rdc_acc_chn_q, val_delay_q >> 1);
/******************************end**********************************************/
}
/**
 * @brief           config  PWM.
 */
void Pwm_Duty_Init(void)
{
    pwm_cmp_config_t cmp_config[4] = {0};
    pwm_pair_config_t pwm_pair_config = {0};
    pwm_output_channel_t pwm_output_ch_cfg;
    pwm_stop_counter(BOARD_APP_PWM_BASE);
    pwm_set_reload(BOARD_APP_PWM_BASE, 0, PWM_RELOAD);
    pwm_set_start_count(BOARD_APP_PWM_BASE, 0, 0);

    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = PWM_RELOAD + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_shlk;

}

/**
 * @brief           Config the PWM channel trig for speed calculation interrupt.
 */
void Pwm_TrigFor_interrupt_Init(void)
{
    
    pwm_cmp_config_t pwm_trig_currentloop = {0};
    pwm_output_channel_t pwm_output_ch_cfg;
 
    memset(&pwm_trig_currentloop, 0x00, sizeof(pwm_cmp_config_t));
    pwm_trig_currentloop.enable_ex_cmp  = false;
    pwm_trig_currentloop.mode = pwm_cmp_mode_output_compare;
    pwm_trig_currentloop.cmp = 200;

    pwm_trig_currentloop.update_trigger = pwm_shadow_register_update_on_shlk;
    pwm_config_cmp(BOARD_APP_PWM_BASE, PWM_TRIG_CMP_INDEX, &pwm_trig_currentloop);

    pwm_output_ch_cfg.cmp_start_index = PWM_TRIG_CMP_INDEX; 
    pwm_output_ch_cfg.cmp_end_index   = PWM_TRIG_CMP_INDEX;  
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(BOARD_APP_PWM_BASE, PWM_TRIG_CMP_INDEX ,&pwm_output_ch_cfg);
    
    pwm_start_counter(BOARD_APP_PWM_BASE);
    pwm_issue_shadow_register_lock_event(BOARD_APP_PWM_BASE);

    intc_m_enable_irq_with_priority(BOARD_APP_PWM_IRQ, 1);
    pwm_enable_irq(BOARD_APP_PWM_BASE, PWM_IRQ_CMP(PWM_TRIG_CMP_INDEX));
   

}

/**
 * @brief           speed calculation function.
 * @param[in,out]   speed_CalHdl        speed calculation struct pointer.
 */
void speed_cal(SPEED_CAL* speed_CalHdl)
{
    int32_t val_acc_i = 0;
    int32_t val_acc_q = 0;
    float delta = 0;
    val_acc_i = rdc_get_acc_avl(HPM_RDC, rdc_acc_chn_i);    //feedback sin signal value.
    val_acc_q = rdc_get_acc_avl(HPM_RDC, rdc_acc_chn_q);    //feedback cos signal value.
    speed_CalHdl->speedtheta = (atanf(((float)val_acc_i)/val_acc_q) * 180 / 3.141592) + 90;    //cal theta
            
    delta = speed_CalHdl->speedtheta - speed_CalHdl->speedlasttheta; 
    if (delta > HPM_MOTOR_MATH_FL_MDF(90)) {/*-speed*/
        delta = HPM_MOTOR_MATH_FL_MDF(180) -speed_CalHdl->speedtheta +speed_CalHdl->speedlasttheta;
    } else if (delta < HPM_MOTOR_MATH_FL_MDF(-90)) {/*+speed*/
        delta = -(HPM_MOTOR_MATH_FL_MDF(180) - speed_CalHdl->speedlasttheta + speed_CalHdl->speedtheta );
    }
    else{
    delta = -delta;
    }
    speed_CalHdl->speedthetalastn += delta;
    speed_CalHdl->speedlasttheta = speed_CalHdl->speedtheta;
    speed_CalHdl->num++;
//cal spped and speed filter value
    if (20 == speed_CalHdl->num) {
        speed_CalHdl->num = 0;
        speed_CalHdl->o_speedout = HPM_MOTOR_MATH_DIV(speed_CalHdl->speedthetalastn,
            HPM_MOTOR_MATH_MUL(HPM_MOTOR_MATH_MUL(0.001, HPM_MOTOR_MATH_FL_MDF(4)), HPM_MOTOR_MATH_FL_MDF(360)));
         
         speed_CalHdl->o_speedout_filter = speed_CalHdl->o_speedout_filter + HPM_MOTOR_MATH_MUL(0.02,
            (speed_CalHdl->o_speedout - speed_CalHdl->o_speedout_filter));
         speed_CalHdl->speedthetalastn = 0;

    }
}

 /**
 * @brief           interrupt entry function of speed calculation.
 * @details         20k interrupt, get a theta every 50us, and the speed is calculated after getting 20 points .
 */
void isr_speed_cal(void)
{
    uint32_t status; 

    status = pwm_get_status(BOARD_APP_PWM_BASE);
    if (PWM_IRQ_CMP(PWM_TRIG_CMP_INDEX) == (status & PWM_IRQ_CMP(PWM_TRIG_CMP_INDEX))) 
    {
        pwm_clear_status(BOARD_APP_PWM_BASE, status);
        speed_cal(&g_speed_cal);
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_PWM_IRQ, isr_speed_cal)






    




