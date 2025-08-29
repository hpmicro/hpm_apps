#include "board.h"
#include "hpm_qeiv2_drv.h"
#include  "qei_init.h"
#include "hpm_trgm_soc_drv.h"

#ifndef APP_QEI_BASE
#define APP_QEI_BASE BOARD_APP_QEIV2_BASE
#endif
#ifndef APP_QEI_IRQ
#define APP_QEI_IRQ BOARD_APP_QEIV2_IRQ
#endif
#ifndef APP_MOTOR_CLK
#define APP_MOTOR_CLK BOARD_APP_QEI_CLOCK_SOURCE
#endif


 void qeiv2_init(void)
{
    qeiv2_adc_config_t adc_config;
    qeiv2_pos_cmp_match_config_t pos_cmp_config = { 0 };
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_APP_QEI_ADC_MATRIX_TO_ADC0, trgm_adc_matrix_in_from_rdc_adc1, false);
    trgm_adc_matrix_config(HPM_TRGM0, BOARD_APP_QEI_ADC_MATRIX_TO_ADC1, trgm_adc_matrix_in_from_rdc_adc0, false);

    adc_config.adc_select = 0;
    adc_config.adc_channel = 0;
    adc_config.offset = 0;  
    adc_config.param0 = 0x4000;
    adc_config.param1 = 0;
    qeiv2_config_adcx(APP_QEI_BASE, &adc_config, true);
    adc_config.adc_select = 1;
    adc_config.adc_channel = 0;
    adc_config.offset = 0;
    adc_config.param0 = 0;
    adc_config.param1 = 0x4000;
    qeiv2_config_adcy(APP_QEI_BASE, &adc_config, true);
    qeiv2_set_adc_xy_delay(APP_QEI_BASE, 0xFFFFFF);

    qeiv2_reset_counter(APP_QEI_BASE);

    qeiv2_set_work_mode(APP_QEI_BASE, qeiv2_work_mode_sincos);
    qeiv2_select_spd_tmr_register_content(APP_QEI_BASE, qeiv2_spd_tmr_as_pos_angle);
    qeiv2_config_z_phase_counter_mode(APP_QEI_BASE, qeiv2_z_count_inc_on_phase_count_max);
    qeiv2_config_phmax_phparam(APP_QEI_BASE, 1);
    qeiv2_pause_pos_counter_on_fault(APP_QEI_BASE, true);
    qeiv2_phcnt_cmp_match_config_t phcnt_cmp_config = {0};
    phcnt_cmp_config.phcnt_cmp_value = 1000;
    phcnt_cmp_config.ignore_rotate_dir = true;
    phcnt_cmp_config.ignore_zcmp = true;
    qeiv2_config_phcnt_cmp_match_condition(APP_QEI_BASE, &phcnt_cmp_config);
    qeiv2_release_counter(APP_QEI_BASE);
}


void qeiv2_theta_cal(qeiv2_Obj* qeiv2Hdl)
{

        qeiv2Hdl->ele_angle = qeiv2_get_postion(APP_QEI_BASE)*360.0/0x100000000;

        if(qeiv2Hdl->ele_angle >360)
        {
           qeiv2Hdl->ele_angle = qeiv2Hdl->ele_angle-360;
        }
        else if(qeiv2Hdl->ele_angle<0)
        {
           qeiv2Hdl->ele_angle = qeiv2Hdl->ele_angle+360;
        }

}

