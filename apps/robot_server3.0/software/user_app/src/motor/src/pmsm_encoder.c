/*
 * Copyright (c) 2025 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "pmsm_encoder.h"

#if USE_MOTOR_DM
uint32_t rbuff[1] = {0};
uint32_t encoder_data = 0;
spi_control_config_t control_config = {0};

void mt6701_encoder_spi_cfg(void)
{
    spi_timing_config_t timing_config = {0};
    spi_format_config_t format_config = {0};
    //spi_control_config_t control_config = {0};
    
    uint32_t spi_clcok;
    spi_clcok = board_init_spi_clock(MT6701_ENCODER_SPI_BASE);
    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.clk_src_freq_in_hz = spi_clcok;
    timing_config.master_config.sclk_freq_in_hz = MT6701_ENCODER_SPI_SCLK_FREQ;
    if (status_success != spi_master_timing_init(MT6701_ENCODER_SPI_BASE, &timing_config)) {
        printf("SPI master timming init failed\n");
        while (1) {
        }
    }

    /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = MT6701_ENCODER_SPI_DATA_LEN_IN_BITS;
    format_config.common_config.mode = spi_master_mode;
    format_config.common_config.cpol = spi_sclk_low_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(MT6701_ENCODER_SPI_BASE, &format_config);
    //printf("SPI-Master transfer format is configured.\n");

    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;  /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode = spi_trans_read_only;//spi_trans_read_only  spi_trans_write_read_together
}
void init_mt6701_encoder_spi_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PC06].FUNC_CTL = IOC_PC06_FUNC_CTL_SPI0_MISO;

    HPM_IOC->PAD[IOC_PAD_PC07].FUNC_CTL = IOC_PC07_FUNC_CTL_SPI0_MOSI;

    HPM_IOC->PAD[IOC_PAD_PC04].FUNC_CTL = IOC_PC04_FUNC_CTL_SPI0_SCLK | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;

    HPM_IOC->PAD[IOC_PAD_PC05].FUNC_CTL = IOC_PC05_FUNC_CTL_SPI0_CS_0;
}

void init_encoder_mt6701(void)
{
    init_mt6701_encoder_spi_pins();
    mt6701_encoder_spi_cfg();
}

uint32_t encoder_read_mt6701(void)
{
    hpm_stat_t stat;
    uint8_t wbuff[1] = {0x12};
    stat = spi_transfer(MT6701_ENCODER_SPI_BASE,
                &control_config,
                NULL, NULL,
                (uint8_t *)wbuff, ARRAY_SIZE(wbuff), (uint8_t *)rbuff, ARRAY_SIZE(rbuff));
      if (stat == status_success)
      {
          encoder_data = rbuff[0] >> 10;
      }
      return encoder_data;
}


#endif

/**********************************************
 * *void motor_encoder_spi_elecinit()
 * * spi encoder init angle
 * *@Input: motor control global structure
 * *#Output：None
 * ***************************************/
void motor_encoder_spi_elecinit(MOTOR_CONTROL_Global* global)
{

#if defined (USE_MOTOR_DM) && (USE_MOTOR_DM)
 
    uint16_t m_pos = 0;
    uint32_t read_pos = encoder_read_mt6701();
    m_pos = read_pos & 0x3FFF;
    global->Motor_ElecInit = (float)(m_pos);

#else
    spi_control_config_t control_config = {0};

    uint16_t wbuff[1] = {0x3FFF};
    uint16_t rbuff[1] = {0};
  
    uint16_t m_pos = 0;

    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;  /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode = spi_trans_write_read_together;//spi_trans_write_read_together;
    spi_transfer(BOARD_APP_SPI_BASE, &control_config, NULL, NULL, (uint16_t *)wbuff, ARRAY_SIZE(wbuff), (uint16_t *)rbuff, ARRAY_SIZE(rbuff));
 
    m_pos = rbuff[0] & 0x3FFF;
    global->Motor_ElecInit = (float)(m_pos);
    uint16_t test;
    test = global->Motor_ElecInit;
#endif
}
/**********************************************
 * *void motor_encoder_abz()
 * * abz encoder: read position, cal speed and electrical angle
 * *@Input: QEI structure, motor control global structure
 * *#Output：None
 * ***************************************/
void motor_encoder_abz(qei_CalObj* qei_CalHdl, MOTOR_CONTROL_Global* global)
{
      

     qei_CalHdl->z =qeiv2_get_current_count(BOARD_PMSM0_QEI_BASE, qeiv2_counter_type_z)&0x1fffff;
     qei_CalHdl->ph = qeiv2_get_current_count(BOARD_PMSM0_QEI_BASE, qeiv2_counter_type_phase)&0x1fffff;
     if(qei_CalHdl->z >= (0x200000 >> 1))
     {
        qei_CalHdl->pos = -(((qei_CalHdl->z - 0x200000)*qei_CalHdl->maxph)+qei_CalHdl->ph);
     }
     else
     {
        qei_CalHdl->pos =  -((qei_CalHdl->z*qei_CalHdl->maxph)+qei_CalHdl->ph);
     }
     uint32_t enc_cnt = qei_CalHdl->maxph/qei_CalHdl->motor_pole;
     
     qei_CalHdl->elec_angle = 360- (qei_CalHdl->ph%enc_cnt)*360.0/enc_cnt;

     motor.foc_para.speedcalpar.speedtheta = qei_CalHdl->pos;
     motor.foc_para.electric_angle = qei_CalHdl->elec_angle;
}
/**********************************************
 * *void motor_encoder_spi()
 * * spi encoder: read postion, cal speed and electrical angle
 * *@Input: motor control global structure
 * *#Output：None
 * ***************************************/
void motor_encoder_spi(MOTOR_CONTROL_Global* global)
{

#if defined (USE_MOTOR_DM) && (USE_MOTOR_DM)
    static uint16_t m_pos = 0;
    static float m_pos_cal = 0; 
    static float m_pos_to_ctrl = 0;
    uint32_t read_pos = encoder_read_mt6701();
    m_pos = read_pos & 0x3FFF;
    m_pos_cal = (float)m_pos;
#else
    spi_timing_config_t timing_config = {0};
    spi_format_config_t format_config = {0};
    spi_control_config_t control_config = {0};

    uint16_t wbuff[1] = {0x3FFF};
    uint16_t rbuff[1] = {0};
    static uint16_t m_pos = 0;
    static uint16_t m_poslast = 0; 
    static float m_pos_cal = 0;
    static float m_pos_callast = 0; 
    static float m_pos_to_ctrl = 0;
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;  /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode = spi_trans_write_read_together;//spi_trans_write_read_together;
    spi_transfer(BOARD_APP_SPI_BASE, &control_config, NULL, NULL, (uint16_t *)wbuff, ARRAY_SIZE(wbuff), (uint16_t *)rbuff, ARRAY_SIZE(rbuff));

    m_pos = rbuff[0] & 0x3FFF;
    m_pos_cal = (float)m_pos;
#endif
    uint32_t enc_cnt = BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV/motor.foc_para.motorpar.i_poles_n;
    float m_cnt = enc_cnt;
    if((m_pos_cal - global->Motor_ElecInit) < -m_cnt)
    {
        m_pos_to_ctrl = m_pos_to_ctrl + (m_pos_cal - global->Motor_ElecInit) + BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV;
    }
    else if ((m_pos_cal - global->Motor_ElecInit) > m_cnt)
    {
        m_pos_to_ctrl = m_pos_to_ctrl + BOARD_PMSM0_SPI_FOC_PHASE_COUNT_PER_REV - (m_pos_cal - global->Motor_ElecInit);
    }
    else
    {
        m_pos_to_ctrl = m_pos_to_ctrl + (m_pos_cal - global->Motor_ElecInit);
    }

    motor.foc_para.speedcalpar.speedtheta = m_pos_to_ctrl;//motor.foc_para.speedcalpar.speedtheta - global->Motor_ElecInit;
#if USE_MOTOR_DM
    motor.foc_para.electric_angle =  (float)(((m_pos % enc_cnt)) * 360.0 / enc_cnt);
#else
    motor.foc_para.electric_angle =  (float)(((m_pos % enc_cnt)) * 360.0 / enc_cnt - 200.6);// 
#endif
    if (motor.foc_para.electric_angle > 360)
    {
        motor.foc_para.electric_angle = fmodf(motor.foc_para.electric_angle, 360);
    }
    else if(motor.foc_para.electric_angle < 0)
    {
        motor.foc_para.electric_angle = fmodf(motor.foc_para.electric_angle, 360) + 360;
    }
    global->Motor_ElecInit = m_pos;
}
/**********************************************
 * *void electric_angle_cal()
 * * choose abz or spi for encoder, and reflesh position
 * *@Input: QEI structure, motor control global structure
 * *#Output：None
 * ***************************************/
void electric_angle_cal(qei_CalObj* qei_CalHdl, MOTOR_CONTROL_Global* global)
{
     if (Motor_Control_Global.Motor_Encoder_Type == BOARD_PMSM0_ENCODER_TYPE)
     {
          motor_encoder_spi(global);
     }
     else
     {
          motor_encoder_abz(qei_CalHdl, global);
     }
}