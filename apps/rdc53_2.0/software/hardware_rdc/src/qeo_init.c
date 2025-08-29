#include "hpm_qeo_drv.h"
#include "board.h"
#include "qeo_init.h"
#include "hpm_pwm_drv.h"
#include "rdc_cfg.h"
#include "math.h"

#define TEST_QEO BOARD_QEO
#define QEO_TRGM_POS BOARD_QEO_TRGM_POS

#define TEST_QEO_ABZ_LINES    (1024U)
#define TEST_QEO_ABZ_MAX_FREQ1  (20000000U)
#define QEO_POSITION_MAX_VAL  (0x100000000UL)

#ifndef BOARD_APP_MOTOR_CLK
#define BOARD_APP_MOTOR_CLK BOARD_BLDC_QEI_CLOCK_SOURCE
#endif
#define PWM_FREQUENCY               (100000) 
#define motor_clock_hz              clock_get_frequency(BOARD_APP_MOTOR_CLK)
#define PWM_RELOAD                  (motor_clock_hz/PWM_FREQUENCY) 
#define PWM_TRIG_CMP_INDEX          (9U)

#ifndef BOARD_APP_PWM_BASE
#define BOARD_APP_PWM_BASE BOARD_BLDCPWM
#endif

/* QEO generates ABZ signals based on the hardware provide position */
void qeo_gen_abz_signal_hardware(void)
{
    qeo_abz_mode_t config;
    qeo_abz_get_default_mode_config(TEST_QEO, &config);
    qeo_abz_config_mode(TEST_QEO, &config);
    /* TEST_QEO_ABZ_LINES * 1s / TEST_QEO_ABZ_MAX_FREQ = 4000us, speed should less than 1r / 4000us = 250 r/s */
    qeo_abz_set_resolution_lines(TEST_QEO, TEST_QEO_ABZ_LINES);
    if (status_success != qeo_abz_set_max_frequency(TEST_QEO, clock_get_frequency(BOARD_MOTOR_CLK_NAME), TEST_QEO_ABZ_MAX_FREQ1)) {
        return;
    }
    qeo_enable_software_position_inject(TEST_QEO);
    board_delay_ms(100);
}

void qeo_pins_init(void)
{

        HPM_IOC->PAD[IOC_PAD_PA20].FUNC_CTL = IOC_PA20_FUNC_CTL_QEO0_A;
        HPM_IOC->PAD[IOC_PAD_PA21].FUNC_CTL = IOC_PA21_FUNC_CTL_QEO0_B;
        HPM_IOC->PAD[IOC_PAD_PA22].FUNC_CTL = IOC_PA22_FUNC_CTL_QEO0_Z;
}

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

    intc_m_enable_irq_with_priority(BOARD_APP_PWM_IRQ, 2);
    pwm_enable_irq(BOARD_APP_PWM_BASE, PWM_IRQ_CMP(PWM_TRIG_CMP_INDEX));
   

}
int64_t current_pos =0;
int64_t previous_pos =  0;
void qeo_output(qeo_abz_Obj* qeo_abzHdl)
{

    float delata_pos;
    uint32_t line_step_counter;
    uint32_t qeo_line_width;
    float motor_vel = qeo_abzHdl->vel;
    if(fabs(motor_vel)<0.1)
    {
        motor_vel = 0;
    }
    float motor_pos = motor_vel*QEO_POSITION_MAX_VAL*0.00001;//100k

    current_pos = previous_pos + motor_pos;
    if(current_pos == previous_pos)
    {
       current_pos = motor_pos;
    }
     delata_pos = current_pos - previous_pos;

    line_step_counter = (uint32_t)(((uint64_t)delata_pos * TEST_QEO_ABZ_LINES) >> 30); /* Shift left by 30 bits to retain 1/4 line accuracy */
    line_step_counter += 1;  /* round up */
    uint32_t clk_counter = clock_get_frequency(BOARD_MOTOR_CLK_NAME) /100000;//100k

    qeo_line_width = (uint32_t)((uint64_t)clk_counter / line_step_counter);

    BOARD_QEO->ABZ.LINE_WIDTH  = QEO_ABZ_LINE_WIDTH_LINE_SET(qeo_line_width); /* adjust qeo abz line width */

    qeo_software_position_inject(BOARD_QEO, current_pos);

    previous_pos = current_pos; 
}
void isr_position_update(void)
{
    uint32_t status; 
    uint8_t io_status;
    uint8_t pt_rx[5]={0};
    uint8_t pt_send[1] = {0};
    status = pwm_get_status(BOARD_APP_PWM_BASE);
    if (PWM_IRQ_CMP(PWM_TRIG_CMP_INDEX) == (status & PWM_IRQ_CMP(PWM_TRIG_CMP_INDEX))) 
    {    
        pwm_clear_status(BOARD_APP_PWM_BASE, status);
        qeo_output(&qeo_abzObj);
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_PWM_IRQ, isr_position_update)////100k isr