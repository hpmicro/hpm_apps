/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "pmsm_currentctrl_3p3z.h"



/**********************************************
 * *void clc_3p3z_init(void)
 * * preposition
 * *@Input: None
 * *#Output：None
 * ***************************************/
 #if defined  (CONFIG_USE_HPM6E00) &&(CONFIG_USE_FUNCTION)&&(BOARD_PMSM0_CLC_3P3Z) 
void clc_3p3z_init(void)
{
    clc_param_config_t clc_param;
    clc_coeff_config_t clc_coeff0;
    BLDC_CONTRL_PID_PARA pid;

    clc_param.eadc_lowth = 0xA0000000;
    clc_param.eadc_mid_lowth = 0xD0000000;
    clc_param.eadc_mid_highth = 0x30000000;
    clc_param.eadc_highth = 0x60000000;
    clc_param._2p2z_clamp_lowth = 0x80000000;
    clc_param._2p2z_clamp_highth = 0x7FFFFFFF;
    clc_param._3p3z_clamp_lowth = 0x80000000;
    clc_param._3p3z_clamp_highth = 0x7FFFFFFF;
    clc_param.output_forbid_lowth = 0;
    clc_param.output_forbid_mid = 0;
    clc_param.output_forbid_highth = 0;
    clc_config_param(BOARD_CLC, clc_vd_chn, &clc_param);
    clc_config_param(BOARD_CLC, clc_vq_chn, &clc_param);

    /**
     * @brief Value of incremental pid, this value is different from the positional pid
     *
     */
    pid.i_kp = 0.5;
    pid.i_ki = 0.02;
    pid.i_kd = 0;

    hpm_mcl_pid_to_3p3z(&pid, (mcl_clc_coeff_cfg_t *)&clc_coeff0);
    clc_config_coeff(BOARD_CLC, clc_vd_chn, clc_coeff_zone_0, &clc_coeff0);
    clc_config_coeff(BOARD_CLC, clc_vq_chn, clc_coeff_zone_0, &clc_coeff0);

    clc_config_coeff(BOARD_CLC, clc_vd_chn, clc_coeff_zone_1, &clc_coeff0);
    clc_config_coeff(BOARD_CLC, clc_vq_chn, clc_coeff_zone_1, &clc_coeff0);

    clc_config_coeff(BOARD_CLC, clc_vd_chn, clc_coeff_zone_2, &clc_coeff0);
    clc_config_coeff(BOARD_CLC, clc_vq_chn, clc_coeff_zone_2, &clc_coeff0);

    clc_set_adc_chn_offset(BOARD_CLC, clc_vd_chn, 0, 0);
    clc_set_adc_chn_offset(BOARD_CLC, clc_vq_chn, 0, 0);
    clc_set_pwm_period(BOARD_CLC, clc_vd_chn, 0);
    clc_set_pwm_period(BOARD_CLC, clc_vq_chn, 0);

    clc_set_enable(BOARD_CLC, clc_vd_chn, true);
    clc_set_enable(BOARD_CLC, clc_vq_chn, true);

    clc_set_sw_inject_dq_mode_enable(BOARD_CLC, clc_vd_chn, true);
    clc_set_sw_inject_dq_mode_enable(BOARD_CLC, clc_vq_chn, true);
}
#endif
/**********************************************
 * *void hpm_mcl_pid_to_3p3z()
 * * 
 * *@Input: dq PI kp, ki,kd
 * *#Output：3p3z coff
 * ***************************************/
hpm_mcl_stat_t hpm_mcl_pid_to_3p3z(BLDC_CONTRL_PID_PARA *cfg_pid, mcl_clc_coeff_cfg_t *cfg_3p3z)
{
/**
 * @brief The relationship between the 3p3z coefficients and
 * the P, I, and D gains of the basic PID is obtained by the standard discrete transform method,
 * and the final result given here. Principle View Knowledge Base.
 *
 */
    cfg_3p3z->a0 = 1;
    cfg_3p3z->a1 = 0;
    cfg_3p3z->b0 = cfg_pid->i_kp + cfg_pid->i_ki + cfg_pid->i_kd;
    cfg_3p3z->b1 = -cfg_pid->i_kp + cfg_pid->i_ki - 2 * cfg_pid->i_kd;
    cfg_3p3z->b2 = cfg_pid->i_kd;
    cfg_3p3z->b3 = 0;
    cfg_3p3z->a2 = 0;
}
/**********************************************
 * *void int32_t motor0_clc_float_convert_clc
 * * 
 * *@Input: realdata
 * *#Output：data with int32_t type
 * ***************************************/
int32_t motor0_clc_float_convert_clc(float realdata)
{
    int32_t data0;
    double data1 = realdata;
    data0 = data1 * 0x7FFFF;

    return data0;
}

void CLC_control(BLDC_CONTROL_FOC_PARA* par)
{      

    static float m_current_err_daxis = 0;
    static float m_current_err_daxis_Last = 0;
    static float m_current_err_daxis_LastLast = 0;
    static float m_current_err_qaxis = 0;
    static float m_current_err_qaxis_Last = 0;
    static float m_current_err_qaxis_LastLast = 0;
    
    m_current_err_daxis = par->currentdpipar.target - par->currentdpipar.cur;
    par->currentdpipar.outval = par->currentdpipar.outval + (par->currentdpipar.i_kp + par->currentdpipar.i_ki + par->currentdpipar.i_kd) * m_current_err_daxis
                                  + (-par->currentdpipar.i_kp + par->currentdpipar.i_ki - 2 * par->currentdpipar.i_kd) * m_current_err_daxis_Last
                                      + (par->currentdpipar.i_kd) * m_current_err_daxis_LastLast;
    m_current_err_daxis_LastLast = m_current_err_daxis_Last;
    m_current_err_daxis_Last = m_current_err_daxis;

    m_current_err_qaxis = par->currentqpipar.target - par->currentqpipar.cur;
    par->currentqpipar.outval = par->currentqpipar.outval + (par->currentqpipar.i_kp + par->currentqpipar.i_ki + par->currentqpipar.i_kd) * m_current_err_qaxis
                                  + (-par->currentqpipar.i_kp + par->currentqpipar.i_ki - 2 * par->currentqpipar.i_kd) * m_current_err_qaxis_Last
                                      + (par->currentqpipar.i_kd) * m_current_err_qaxis_LastLast;
    m_current_err_qaxis_LastLast = m_current_err_qaxis_Last;
    m_current_err_qaxis_Last = m_current_err_qaxis;
}
