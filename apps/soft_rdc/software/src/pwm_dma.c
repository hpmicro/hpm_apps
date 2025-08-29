/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include "hpm_dmav2_drv.h"
#include "hpm_dmamux_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_trgmmux_src.h"
#include "pwm_dma.h"
#include "hpm_dac_drv.h"

#define BOARD_APP_DAC_CLOCK_NAME1 clock_dac1

const uint32_t sin_wave_dma[SIN_DATA_LEN] ={960,1020,1080,1139,1198,1256,1313,1368,1422,1474,1524,1571,1617,1659,1699,1736,1770,1801,1828,1852,1873,1889,1902,1912,1918,1920,1918,1912,1902,1889,1873,1852,1828,1801,1770,1736,1699
,1659,1617,1571,1524,1474,1422,1368,1313,1256,1198,1139,1080,1020,960,899,839,780,721,663,606,551,497,445,395,348,302,260,220,183,149,118,91,67,46,30,17,7,1,0,1,7,17,30,46,67,91,118,149
,183,220,260,302,348,395,445,497,551,606,663,721,780,839,899,960};///CMP值*16
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) dma_linked_descriptor_t descriptors_cmp[2];
void init_trigger_source_cmp_dma(PWM_Type *ptr,uint8_t dma_src)
{
    pwm_cmp_config_t pwm_cmp_dma_cfg;
    int mot_clock_freq,reload_t;

    mot_clock_freq = clock_get_frequency(clock_mot0);
    reload_t = mot_clock_freq / PWM_FREQ - 1;
        /* Set a comparator for dma request */
    memset(&pwm_cmp_dma_cfg, 0x00, sizeof(pwm_cmp_config_t));
    pwm_cmp_dma_cfg.enable_ex_cmp = false;
    pwm_cmp_dma_cfg.mode = pwm_cmp_mode_output_compare;
    pwm_cmp_dma_cfg.update_trigger = pwm_shadow_register_update_on_shlk;

    /* Select comp8 and trigger at the middle of a pwm cycle */
    pwm_cmp_dma_cfg.cmp = reload_t - 2;
    pwm_config_cmp(ptr, dma_src, &pwm_cmp_dma_cfg);
    pwm_enable_dma_request(ptr, PWM_IRQ_CMP(dma_src));
  
}
void init_trigger_mux_cmp_dma(TRGM_Type *ptr,uint8_t trgm_dma,uint8_t dma_src)
{
   trgm_dma_request_config(ptr, trgm_dma, dma_src);
}

void dma_chain_transfer_config_pwm(PWM_Type *ptr,uint8_t dma_ch)
{
    dma_channel_config_t ch_config = {0};
    dma_default_channel_config(HPM_HDMA, &ch_config);

    /* read tdm data to the first half of rx_buff. */
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&sin_wave_dma[0]);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&ptr->CMP[PWM_RDC_CMP_INDEX]);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = 101 * sizeof(uint32_t);
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&descriptors_cmp[1]);
    dma_config_linked_descriptor(HPM_HDMA, &descriptors_cmp[0], dma_ch, &ch_config);

    
     /*read tdm data to the second half of rx_buff*/
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&sin_wave_dma[0]);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&ptr->CMP[PWM_RDC_CMP_INDEX]);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = 101 * sizeof(uint32_t);
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&descriptors_cmp[0]);
    dma_config_linked_descriptor(HPM_HDMA, &descriptors_cmp[1], dma_ch, &ch_config);
}
void dma_auto_config_pwm_rdc(PWM_Type *ptr,uint8_t dma_ch)
{

    dma_channel_config_t ch_config = {0};
    static uint32_t dummy_data1 = 0xff, dummy_data2 = 0xff;
    dma_chain_transfer_config_pwm(ptr,dma_ch);
    dma_default_channel_config(HPM_HDMA, &ch_config);

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
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&descriptors_cmp[0]);

    /* warning! fixed addr no support burst!*/
    if (status_success != dma_setup_channel(HPM_HDMA, dma_ch, &ch_config, true)) {
        printf("pwm dma setup channel failed\n");
        return;
    }
    dmamux_config(HPM_DMAMUX, DMA_SOC_CHN_TO_DMAMUX_CHN(HPM_HDMA, dma_ch), PWM_RDC_DMA_SRC_TRIG, true); 
    dma_enable_channel(HPM_HDMA, dma_ch);
}
void pwm_cmp_dma_config(void)
{
  init_trigger_source_cmp_dma(PWM_RDC,PWM_RDC_DMA_SRC);
  init_trigger_mux_cmp_dma(PWM_RDC_TRGM,PWM_RDC_TRGM_DMA,PWM_RDC_DMA_SRC);
  dma_auto_config_pwm_rdc(PWM_RDC,PWM_RDC_DMA_CHNNEL);

}
void reset_pwm_counter(void)
{
    pwm_enable_reload_at_synci(PWM_RDC);
    trgm_output_update_source(PWM_RDC_TRGM, TRGM_PWM_OUTPUT, 1);
    trgm_output_update_source(PWM_RDC_TRGM, TRGM_PWM_OUTPUT, 0);
}
void init_pwm_rdc_pins(PWM_Type *ptr)
{
    if (ptr == HPM_PWM0) {

        HPM_IOC->PAD[IOC_PAD_PA28].FUNC_CTL = IOC_PA28_FUNC_CTL_PWM0_P_4;
        HPM_IOC->PAD[IOC_PAD_PA29].FUNC_CTL = IOC_PA29_FUNC_CTL_PWM0_P_5;
    }
}
void generate_central_aligned_waveform_in_pair(void)
{
    uint8_t cmp_index = PWM_RDC_CMP_INDEX;
    pwm_cmp_config_t cmp_config[3] = {0};
    pwm_pair_config_t pwm_pair_config = {0};
    uint32_t freq;
    uint32_t reload;
    freq = clock_get_frequency(PWM_CLOCK_NAME);
    reload = freq / PWM_FREQ;
    pwm_stop_counter(PWM_RDC);
    reset_pwm_counter();

    /*
     * config cmp1 and cmp2
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = reload + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = reload + 1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[2].mode = pwm_cmp_mode_output_compare;
    cmp_config[2].cmp = reload;
    cmp_config[2].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_get_default_pwm_pair_config(PWM_RDC, &pwm_pair_config);
    pwm_pair_config.pwm[0].enable_output = true;
    pwm_pair_config.pwm[0].dead_zone_in_half_cycle = 0;
    pwm_pair_config.pwm[0].invert_output = false;

    pwm_pair_config.pwm[1].enable_output = true;
    pwm_pair_config.pwm[1].dead_zone_in_half_cycle = 0;
    pwm_pair_config.pwm[1].invert_output = false;

    /*
     * config pwm
     */
    if (status_success != pwm_setup_waveform_in_pair(PWM_RDC, PWM_OUTPUT_PIN1, &pwm_pair_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    pwm_load_cmp_shadow_on_match(PWM_RDC, cmp_index + 2,  &cmp_config[2]);


}
void disable_all_pwm_output(void)
{
    pwm_disable_output(PWM_RDC, PWM_OUTPUT_PIN1);
    pwm_disable_output(PWM_RDC, PWM_OUTPUT_PIN2);
}
void init_pwm_out_rdc(void)
{
    init_pwm_rdc_pins(PWM_RDC);
    pwm_cmp_dma_config();
    generate_central_aligned_waveform_in_pair();

}
void start_pwm_rdc(void)
{
    pwm_start_counter(PWM_RDC);
    pwm_issue_shadow_register_lock_event(PWM_RDC);
}

void common_config_init(dac_mode_t mode)
{
    dac_config_t config,config1;

    dac_get_default_config(&config);

    config.dac_mode = mode;
    config.sync_mode = (clk_dac_src_ahb0 == clock_get_source(BOARD_APP_DAC_CLOCK_NAME)) ? true : false;

    dac_init(HPM_DAC0, &config);

    dac_get_default_config(&config1);
    config1.dac_mode = mode;
    config1.sync_mode = (clk_dac_src_ahb0 == clock_get_source(BOARD_APP_DAC_CLOCK_NAME1)) ? true : false;

    dac_init(HPM_DAC1, &config1);
}
void DAC_cfg(void)
{
   /* Initialize a DAC clock */
    board_init_dac_clock(HPM_DAC0, false);
    board_init_dac_clock(HPM_DAC1, false);

    /* Initialize a DAC pin */
    board_init_dac_pins(HPM_DAC0);
    board_init_dac_pins(HPM_DAC1);
    
    /* Config DAC mode */
    common_config_init(dac_mode_direct);
    dac_enable_conversion(HPM_DAC0, true);
    dac_set_direct_config(HPM_DAC0, 2048);
    dac_enable_conversion(HPM_DAC1, true);
    dac_set_direct_config(HPM_DAC1, 2048);
}


void board_init_rtt_test(void)
{
    init_py_pins_as_pgpio();
    board_init_usb_dp_dm_pins();
    board_init_clock();
    board_init_console();
    board_init_pmp();
}
