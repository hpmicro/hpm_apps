/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------*/
#include "hpm_common.h"
#include "tusb.h"
#include "diskio.h"
#include "ff.h"
#include "file_op.h"
#include "hpm_adc16_drv.h"
#include "hpm_synt_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_trgmmux_src.h"
#include "board.h"
#include "adc.h"


uint8_t seq_channel[] = {BOARD_APP_ADC16_CH_1};
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(4) uint32_t adc_dma_buff[BOARD_ADC_DMA_BUFFER_LEN];

static void init_trigger_mux(TRGM_Type * ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type = trgm_output_pulse_at_input_falling_edge;

    trgm_output_cfg.input  = BOARD_TRGM_SRC_PWM_CH;
    trgm_output_config(ptr, BOARD_TRGM_TRGOCFG_ADC_STRGI, &trgm_output_cfg);
}

void trigger_init(PWM_Type * ptr,uint32_t sample_freq)
{
    pwm_cmp_config_t pwm_cmp_cfg;
    pwm_output_channel_t pwm_output_ch_cfg;

    /* TODO: Set PWM Clock Source and divider */

    pwm_set_reload(ptr, 0, (200000/sample_freq)-1);

    /* Set comparator */
    memset(&pwm_cmp_cfg, 0x00, sizeof(pwm_cmp_config_t));
    pwm_cmp_cfg.enable_ex_cmp  = false;
    pwm_cmp_cfg.mode           = pwm_cmp_mode_output_compare;
    pwm_cmp_cfg.update_trigger = pwm_shadow_register_update_on_shlk;

    /* Select comp8 and trigger at the middle of a pwm cycle */
    pwm_cmp_cfg.cmp = (200000/sample_freq)/2-1;
    pwm_config_cmp(ptr, BOARD_APP_ADC_PMT_PWM_REFCH_A, &pwm_cmp_cfg);

    /* Issue a shadow lock */
    pwm_issue_shadow_register_lock_event(BOARD_APP_ADC_TRIG_PWM);

    /* Set comparator channel for trigger a */
    pwm_output_ch_cfg.cmp_start_index = BOARD_APP_ADC_PMT_PWM_REFCH_A;   /* start channel */
    pwm_output_ch_cfg.cmp_end_index   = BOARD_APP_ADC_PMT_PWM_REFCH_A;   /* end channel */
    pwm_output_ch_cfg.invert_output   = false;

    pwm_config_output_channel(ptr, BOARD_APP_ADC_PMT_PWM_REFCH_A, &pwm_output_ch_cfg);

    /* Trigger mux initialization */
    init_trigger_mux(BOARD_APP_ADC_TRIG_TRGM);
}

void trigger_start(PWM_Type * ptr)
{
  pwm_start_counter(ptr);
}

static void adc_calibration(ADC16_Type *ptr)
{
    uint32_t i, j;
    uint32_t clk_div_temp;
    int32_t adc16_params[ADC16_SOC_PARAMS_LEN];
    int32_t  param01;
    uint32_t param02;
    uint64_t param64;
    uint32_t param32;
    uint32_t temp;

    /* Get input clock divider */
    clk_div_temp = ADC16_CONV_CFG1_CLOCK_DIVIDER_GET(ptr->CONV_CFG1);

    /* Set input clock divider temporarily */
    ptr->CONV_CFG1 = (ptr->CONV_CFG1 & ~ADC16_CONV_CFG1_CLOCK_DIVIDER_MASK)
                   | ADC16_CONV_CFG1_CLOCK_DIVIDER_SET(1);

    /* Enable ADC config clock */
    ptr->ANA_CTRL0 |= ADC16_ANA_CTRL0_ADC_CLK_ON_MASK;

    for (i = 0; i < ADC16_SOC_PARAMS_LEN; i++) {
        adc16_params[i] = 0;
    }

    /* Enable reg_en */
    /* Enable bandgap_en */
    ptr->ADC16_CONFIG0 |= ADC16_ADC16_CONFIG0_REG_EN_MASK
                       |  ADC16_ADC16_CONFIG0_BANDGAP_EN_MASK;

    /* Set cal_avg_cfg for 5 loops */
    ptr->ADC16_CONFIG0 = (ptr->ADC16_CONFIG0 & ~ADC16_ADC16_CONFIG0_CAL_AVG_CFG_MASK)
                       | ADC16_ADC16_CONFIG0_CAL_AVG_CFG_SET(5);

    /* Enable ahb_en */
    ptr->ADC_CFG0 |= ADC16_ADC_CFG0_ADC_AHB_EN_MASK;

    /* Disable ADC config clock */
    ptr->ANA_CTRL0 &= ~ADC16_ANA_CTRL0_ADC_CLK_ON_MASK;

    /* Recover input clock divider */
    ptr->CONV_CFG1 = (ptr->CONV_CFG1 & ~ADC16_CONV_CFG1_CLOCK_DIVIDER_MASK)
                   | ADC16_CONV_CFG1_CLOCK_DIVIDER_SET(clk_div_temp);

    for (j = 0; j < 64; j++) {
        /* Set startcal */
        ptr->ANA_CTRL0 |= ADC16_ANA_CTRL0_STARTCAL_MASK;

        /* Clear startcal */
        ptr->ANA_CTRL0 &= ~ADC16_ANA_CTRL0_STARTCAL_MASK;

        /* Polling calibration status */
        while (ADC16_ANA_STATUS_CALON_GET(ptr->ANA_STATUS)) {
        }

        /* Read parameters */
        for (i = 0; i < ADC16_SOC_PARAMS_LEN; i++) {
            adc16_params[i] += ADC16_ADC16_PARAMS_PARAM_VAL_GET(ptr->ADC16_PARAMS[i]);
        }
        
        #ifdef PRINT_CALIBRATION_PARAM
        for(i = 0;i < ADC16_SOC_PARAMS_LEN; i++) 
          printf("%x ",(ADC16_ADC16_PARAMS_PARAM_VAL_GET(ptr->ADC16_PARAMS[i]))>>5);
        printf("%2d   \n",j);
        #endif
    }

    adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA33] -= 0x9000;

    param01 = adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA32] - adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA33];

    adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA32] = adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA00] -
                                                    adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA33]+0x800;
   
    adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA16] +=  0x4000;
    adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA08] +=  0x1000;
    adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA24] +=  0x2000;
    adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA28] +=  0x800;
    adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA04] +=  0x800;
    adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA00] = 0;

    for (i = 1; i < ADC16_SOC_PARAMS_LEN - 2; i++) {
        adc16_params[i] = adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA32] + adc16_params[i] -
                          adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA33] + adc16_params[i - 1];
    }
    
    param02 = (param01 + adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA31] + adc16_params[ADC16_ADC16_PARAMS_ADC16_PARA32]) >> 10;
    //printf("err_total = %x \n",param02);
    param64 = 0x10000ll * param02;
    param64 = param64 / (0x20000 - param02 / 2);
    param32 = (uint32_t)param64;

    for (i = 0; i < ADC16_SOC_PARAMS_LEN; i++) {
        adc16_params[i] >>= 10;
    }

    /* Enable ADC config clock */
    ptr->ANA_CTRL0 |= ADC16_ANA_CTRL0_ADC_CLK_ON_MASK;

    ptr->CONV_CFG1 = (ptr->CONV_CFG1 & ~ADC16_CONV_CFG1_CLOCK_DIVIDER_MASK)
                   | ADC16_CONV_CFG1_CLOCK_DIVIDER_SET(1);

    /* Write parameters */
    for (i = 0; i < ADC16_SOC_PARAMS_LEN ; i++) {
        ptr->ADC16_PARAMS[i] = (uint32_t)adc16_params[i];
    }
    
    #ifdef PRINT_CALIBRATION_PARAM
    printf("\n");
    for(i=0;i<34;i++) printf("%x ",adc16_params[i]);

    printf("slope = %x \n",param32);

    #endif
    

    /* Set ADC16 Config0 */
    temp = ptr->ADC16_CONFIG0;

    temp &= ~(ADC16_ADC16_CONFIG0_CAL_AVG_CFG_MASK | ADC16_ADC16_CONFIG0_CONV_PARAM_MASK);

    temp |= ADC16_ADC16_CONFIG0_REG_EN_MASK
         |  ADC16_ADC16_CONFIG0_BANDGAP_EN_MASK
         |  ADC16_ADC16_CONFIG0_CAL_AVG_CFG_MASK
         |  ADC16_ADC16_CONFIG0_CONV_PARAM_SET(param32);

    ptr->ADC16_CONFIG0 = temp;

    /* Recover input clock divider */
    ptr->CONV_CFG1 = (ptr->CONV_CFG1 & ~ADC16_CONV_CFG1_CLOCK_DIVIDER_MASK)
                   | ADC16_CONV_CFG1_CLOCK_DIVIDER_SET(clk_div_temp);

    /* Disable ADC config clock */
    ptr->ANA_CTRL0 &= ~ADC16_ANA_CTRL0_ADC_CLK_ON_MASK;

}

uint32_t* adc_dma_buffer_get(void)
{
  return adc_dma_buff;
}

void adc_init(void)
{
    adc16_config_t cfg;
    adc16_seq_config_t seq_cfg;
    adc16_dma_config_t dma_cfg;
    adc16_channel_config_t ch_cfg;


   /* ADC pin initialization */
    board_init_adc16_pins();
  
    /* ADC clock initialization */
    board_init_adc16_clock(BOARD_APP_ADC16_BASE,true);

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);
    cfg.res            = adc16_res_16_bits;
    cfg.conv_mode      = adc16_conv_mode_sequence;
    cfg.adc_clk_div    = 4;
    cfg.sel_sync_ahb   = false;

    if (cfg.conv_mode == adc16_conv_mode_sequence ||
        cfg.conv_mode == adc16_conv_mode_preemption) {
        cfg.adc_ahb_en = true;
    }

    adc16_init(BOARD_APP_ADC16_BASE, &cfg);
    
    adc_calibration(BOARD_APP_ADC16_BASE);

    /* enable irq */
    intc_m_enable_irq_with_priority(BOARD_APP_ADC16_IRQn, 1);

    /* get a default channel config */
    adc16_get_channel_default_config(&ch_cfg);

    /* initialize an ADC channel */
    ch_cfg.sample_cycle = 15;

    for (uint32_t i = 0; i < sizeof(seq_channel); i++) {
        ch_cfg.ch           = seq_channel[i];
        adc16_init_channel(BOARD_APP_ADC16_BASE, &ch_cfg);
    }

    /* Set a sequence config */
    seq_cfg.seq_len    = sizeof(seq_channel);
    seq_cfg.restart_en = false;
    seq_cfg.cont_en    = true;
    seq_cfg.sw_trig_en = true;
    seq_cfg.hw_trig_en = true;

    for (int i = BOARD_ADC_SEQ_START_POS; i < seq_cfg.seq_len; i++) {
        seq_cfg.queue[i].ch = seq_channel[i];
    }

    /* Initialize a sequence */
    adc16_set_seq_config(BOARD_APP_ADC16_BASE, &seq_cfg);

    /* Set a DMA config */
    dma_cfg.start_addr         = (uint32_t *)core_local_mem_to_sys_address(BOARD_APP_CORE, (uint32_t)adc_dma_buff);
    dma_cfg.buff_len_in_4bytes = BOARD_ADC_DMA_BUFFER_LEN;
    dma_cfg.stop_en            = false;
    dma_cfg.stop_pos           = 0;

    /* Initialize DMA for the sequence mode */
    adc16_init_seq_dma(BOARD_APP_ADC16_BASE, &dma_cfg);
}
