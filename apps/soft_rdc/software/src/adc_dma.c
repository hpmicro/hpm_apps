/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#if defined(HPMSOC_HAS_HPMSDK_DMAV2)
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
//#include "hpm_dmav2_drv.h"
#include "hpm_dmamux_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "adc_dma.h"


ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t pmt_buff[ADC_PMT_DMA_BUFF_LEN_IN_4BYTES];
uint8_t trig_adc_channel[] = {ADC_CHANNEL_MOTOR1_U,ADC_CHANNEL_MOTOR1_V,ADC_CHANNEL_MOTOR1_W};

ATTR_RAMFUNC_WITH_ALIGNMENT(8) dma_linked_descriptor_t adc_descriptors1[2 * ADC_DMA_BUFF_LEN_IN_BYTES * MOTOR_PHASE_CNT + 2];

ATTR_RAMFUNC_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint16_t adc_buff_u[ADC_DMA_BUFF_LEN_IN_BYTES * 2];
ATTR_RAMFUNC_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint16_t adc_buff_v[ADC_DMA_BUFF_LEN_IN_BYTES * 2];
ATTR_RAMFUNC_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint16_t adc_buff_w[ADC_DMA_BUFF_LEN_IN_BYTES * 2];
ATTR_RAMFUNC_WITH_ALIGNMENT(8) uint8_t adc_done[2];
uint32_t reload;
void init_trigger_source(PWM_Type *ptr,uint8_t pwm_ch)
{
    pwm_cmp_config_t pwm_cmp_cfg;
    
    pwm_output_channel_t pwm_output_ch_cfg;

    int mot_clock_freq;

    mot_clock_freq = clock_get_frequency(clock_mot0);
    reload = mot_clock_freq / ADC_TRIG_SRC_FREQUENCY;

    /* Set a comparator */
    memset(&pwm_cmp_cfg, 0x00, sizeof(pwm_cmp_config_t));
    pwm_cmp_cfg.enable_ex_cmp  = false;
    pwm_cmp_cfg.mode           = pwm_cmp_mode_output_compare;
    pwm_cmp_cfg.update_trigger = pwm_shadow_register_update_on_shlk;

    /* Select comp8 and trigger at the middle of a pwm cycle */
    pwm_cmp_cfg.cmp = reload - 1;
    pwm_config_cmp(ptr, pwm_ch, &pwm_cmp_cfg);

    /* Set comparator channel to generate a trigger signal */
    pwm_output_ch_cfg.cmp_start_index = pwm_ch;   /* start channel */
    pwm_output_ch_cfg.cmp_end_index   = pwm_ch;   /* end channel */
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(ptr, pwm_ch, &pwm_output_ch_cfg);

}
void init_trigger_source_dma(PWM_Type *ptr,uint8_t dma_src)
{
    pwm_cmp_config_t pwm_cmp_dma_cfg;
    int mot_clock_freq,reload;

    mot_clock_freq = clock_get_frequency(clock_mot0);
    reload = mot_clock_freq / ADC_TRIG_SRC_FREQUENCY - 1;
        /* Set a comparator for dma request */
    memset(&pwm_cmp_dma_cfg, 0x00, sizeof(pwm_cmp_config_t));
    pwm_cmp_dma_cfg.enable_ex_cmp = false;
    pwm_cmp_dma_cfg.mode = pwm_cmp_mode_output_compare;
    pwm_cmp_dma_cfg.update_trigger = pwm_shadow_register_update_on_shlk;

    /* Select comp8 and trigger at the middle of a pwm cycle */
    pwm_cmp_dma_cfg.cmp = reload - 2;
    pwm_config_cmp(ptr, dma_src, &pwm_cmp_dma_cfg);
    pwm_enable_dma_request(ptr, PWM_IRQ_CMP(dma_src));
  
}
void init_trigger_mux_adc(TRGM_Type *ptr, uint8_t input, uint8_t output)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type = trgm_output_same_as_input;

    trgm_output_cfg.input  = input;
    trgm_output_config(ptr, output, &trgm_output_cfg);
}
void init_trigger_mux_dma(TRGM_Type *ptr,uint8_t trgm_dma,uint8_t dma_src)
{
   trgm_dma_request_config(ptr, trgm_dma, dma_src);
}
void init_adc_dma_chian(void)
{
  init_trigger_mux_dma(ADC_HW_TRGM_MOTOR1,ADC_TRGM_DMACFG_MOTOR1, ADC_DMA_SRC_MOTOR1);
  init_trigger_source_dma(ADC_HW_TRIG_SRC_MOTOR1,ADC_DMA_SRC_MOTOR1);
  hdma_auto_config(ADC_DMA_CH_MOTOR1,(uint32_t)&pmt_buff,(uint32_t)&adc_buff_u);
}


void init_trigger_target(ADC_TYPE *ptr, uint8_t trig_ch,uint8_t trig_adc_channel[],uint8_t len)
{
    adc16_pmt_config_t pmt_cfg;

    pmt_cfg.trig_len = len;
    pmt_cfg.trig_ch = trig_ch;

    for (int i = 0; i < pmt_cfg.trig_len; i++) {
        pmt_cfg.adc_ch[i] = trig_adc_channel[i];
        pmt_cfg.inten[i] = false;
    }

    pmt_cfg.inten[pmt_cfg.trig_len - 1] = true;

    adc16_set_pmt_config(ptr, &pmt_cfg);
    adc16_set_pmt_queue_enable(ptr, trig_ch, true);
}

hpm_stat_t init_common_config(ADC_TYPE *ptr,ADC_CONV_MODE_T conv_mode)
{
    adc16_config_t cfg;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);

    cfg.res            = adc16_res_16_bits;
    cfg.conv_mode      = conv_mode;
    cfg.adc_clk_div    = adc16_clock_divider_4;
    cfg.sel_sync_ahb   = (clk_adc_src_ahb0 == clock_get_source(ADC_CLK_NAME)) ? true : false;

    if (cfg.conv_mode == adc16_conv_mode_sequence ||
        cfg.conv_mode == adc16_conv_mode_preemption) {
        cfg.adc_ahb_en = true;
    }

    /* adc16 initialization */
    if (adc16_init(ptr, &cfg) == status_success) {
        return status_success;
    } else {
        printf("%s initialization failed!\n", BOARD_APP_ADC16_NAME);
        return status_fail;
    }
}
void init_preemption_config(void)
{
    adc16_channel_config_t ch_cfg;

    /* get a default channel config */
    adc16_get_channel_default_config(&ch_cfg);

    /* initialize an ADC channel */
    ch_cfg.sample_cycle = ADC_CH_SAMPLE_CYCLE;

    for (uint32_t i = 0; i < sizeof(trig_adc_channel); i++) {
        ch_cfg.ch = trig_adc_channel[i];
        adc16_init_channel(ADC_MOTOR1, &ch_cfg);
    }

    /* Trigger target initialization */
    init_trigger_target(ADC_MOTOR1, ADC_PMT_TRIG_CH_MOTOR1,trig_adc_channel,MOTOR_PHASE_CNT);

    /* Set DMA start address for preemption mode */
    adc16_init_pmt_dma(ADC_MOTOR1, core_local_mem_to_sys_address(ADC_CORE, (uint32_t)pmt_buff));


#if !defined(ADC_SOC_NO_HW_TRIG_SRC) && !defined(__ADC16_USE_SW_TRIG)
    /* Trigger mux initialization */
    init_trigger_mux_adc(ADC_HW_TRGM_MOTOR1, ADC_HW_TRGM_IN_MOTOR1, ADC_HW_TRGM_OUT_MOTOR1);

    /* Trigger source initialization */
    init_trigger_source(ADC_HW_TRIG_SRC_MOTOR1,ADC_HW_TRIG_SRC_PWM_MOTOR1);
#endif
}

void init_adc_pins_motor(void)
{
    HPM_IOC->PAD[IOC_PAD_PB07].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK; //oexc
    HPM_IOC->PAD[IOC_PAD_PB13].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK; //osin
    HPM_IOC->PAD[IOC_PAD_PB14].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK; //ocos
}

void init_adc_motor1(void)
{
    /* ADC pin initialization */
    init_adc_pins_motor();
    /* ADC clock initialization */
    board_init_adc_clock(ADC_MOTOR1, true);
    init_common_config(ADC_MOTOR1,adc16_conv_mode_preemption);
    init_preemption_config();
    init_adc_dma_chian();
 
    pwm_set_reload(ADC_HW_TRIG_SRC_MOTOR1, 0, reload);
    pwm_issue_shadow_register_lock_event(ADC_HW_TRIG_SRC_MOTOR1);
    pwm_start_counter(ADC_HW_TRIG_SRC_MOTOR1);
    
}


void hdma_dma_chain_config(uint8_t dma_ch,uint32_t src_addr,uint32_t dst_addr)
{
    dma_channel_config_t dma_ch_config;
    static uint8_t dummy_cmd = 1;

    dma_default_channel_config(HPM_HDMA, &dma_ch_config);

    /* dma trans */
    for (int i = 0;i < ADC_DMA_BUFF_LEN_IN_BYTES;i++)
    {
      dma_ch_config.size_in_byte =  sizeof(uint16_t);
      dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, src_addr);
      dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, dst_addr + i * 2);
      dma_ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
      dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
      dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
      dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[3* i + 1]);
      dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[3* i], dma_ch, &dma_ch_config);

      dma_ch_config.size_in_byte = sizeof(uint16_t);
      dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, src_addr + 4);
      dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff_v + i * 2);
      dma_ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
      dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
      dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
      dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[3* i + 2]);
      dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[3* i + 1], dma_ch, &dma_ch_config);

      dma_ch_config.size_in_byte = sizeof(uint16_t);
      dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, src_addr + 8);
      dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff_w + i * 2);
      dma_ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
      dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
      dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
      dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[3* i + 3]);
      dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[3* i + 2], dma_ch, &dma_ch_config);
    }
    /*set done*/
    dma_ch_config.size_in_byte = 1;
    dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dummy_cmd);
    dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_done[0]);
    dma_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[3 * ADC_DMA_BUFF_LEN_IN_BYTES + 1]);
    dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[3 * ADC_DMA_BUFF_LEN_IN_BYTES], dma_ch, &dma_ch_config);

    /* dma trans */
    for (int i = 0;i < ADC_DMA_BUFF_LEN_IN_BYTES;i++)
    {
      dma_ch_config.size_in_byte = sizeof(uint16_t);
      dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, src_addr);
      dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (dst_addr + (ADC_DMA_BUFF_LEN_IN_BYTES + i) * sizeof(uint16_t)));
      dma_ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
      dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
      dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
      dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[3 * (ADC_DMA_BUFF_LEN_IN_BYTES + i) + 2]);
      dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[3 * (ADC_DMA_BUFF_LEN_IN_BYTES + i) + 1], dma_ch, &dma_ch_config);

      dma_ch_config.size_in_byte = sizeof(uint16_t);
      dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, src_addr + 4);
      dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, ((uint32_t)&adc_buff_v + (ADC_DMA_BUFF_LEN_IN_BYTES + i) * sizeof(uint16_t)));
      dma_ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
      dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
      dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
      dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[3 * (ADC_DMA_BUFF_LEN_IN_BYTES + i) + 3]);
      dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[3 * (ADC_DMA_BUFF_LEN_IN_BYTES + i) + 2], dma_ch, &dma_ch_config);

      dma_ch_config.size_in_byte = sizeof(uint16_t);
      dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, src_addr + 8);
      dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, ((uint32_t)&adc_buff_w + (ADC_DMA_BUFF_LEN_IN_BYTES + i) * sizeof(uint16_t)));
      dma_ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
      dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
      dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
      dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
      dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
      dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[3 * (ADC_DMA_BUFF_LEN_IN_BYTES + i) + 4]);
      dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[3 * (ADC_DMA_BUFF_LEN_IN_BYTES + i) + 3], dma_ch, &dma_ch_config);
    }
    /*set done*/
    dma_ch_config.size_in_byte = 1;
    dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dummy_cmd);
    dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_done[1]);
    dma_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[0]);
    dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[6 * ADC_DMA_BUFF_LEN_IN_BYTES + 1], dma_ch, &dma_ch_config);
}

void hdma_auto_config(uint8_t dma_ch,uint32_t src_addr,uint32_t dst_addr)
{
    dma_channel_config_t ch_config = {0};
    static uint32_t dummy_data1 = 0xff, dummy_data2 = 0xff;
  
    hdma_dma_chain_config(dma_ch,src_addr,dst_addr);

    dma_default_channel_config(HPM_HDMA, &ch_config);

    // TX
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dummy_data1);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dummy_data2);
    ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.size_in_byte = 1;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[0]);

    // warning! fixed addr no support burst!
    if (status_success != dma_setup_channel(HPM_HDMA, dma_ch, &ch_config, true))
    {
        printf(" dma setup channel failed 0\n");
        return;
    }
    //**********************************
    dmamux_config(HPM_DMAMUX, DMA_SOC_CHN_TO_DMAMUX_CHN(HPM_HDMA, dma_ch), ADC_DMA_SRC_ADC_MOTOR1, true);
}
void adc_data_printf(void)
{
  if (adc_done[0] == 1)
  {
     adc_done[0] = 0;
     printf("adc0:%x,%x,%x\n",adc_buff_u[0],adc_buff_v[0],adc_buff_w[0]);
  }
    if (adc_done[1] == 1)
  {
     adc_done[1] = 0;
     printf("adc1:%x,%x,%x\n",adc_buff_u[ADC_DMA_BUFF_LEN_IN_BYTES],adc_buff_v[ADC_DMA_BUFF_LEN_IN_BYTES],adc_buff_w[ADC_DMA_BUFF_LEN_IN_BYTES]);
  }
}

