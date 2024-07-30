#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_trgmmux_src.h"
#include "hpm_synt_drv.h"
#include "hpm_adc16_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_gpio_drv.h"



uint32_t data_buffer[BOARD_ADC_DATA_BUFFER_LEN];
 __attribute__((section(".fast"))) uint32_t seq_buff[BOARD_ADC_DMA_BUFFER_LEN];

uint8_t seq_channel[] = { BOARD_APP_ADC16_CH_1 };

/**
 * @brief Initialize ADC trigger source；初始化ADC采样触发源
 *
 * @param [in] ptr TRGM base address;TRGM基地址
 */

void init_trigger_mux(TRGM_Type * ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type = trgm_output_same_as_input;

    trgm_output_cfg.input  = BOARD_TRGM_SRC_PWM_CH;
    trgm_output_config(ptr, BOARD_TRGM_TRGOCFG_ADC_STRGI, &trgm_output_cfg);
}

/**
 * @brief Setting ADC sampling trigger by PWM hardware;ADC采样触发设置,硬件PWM触发
 *
 * @param [in] ptr PWM base address;触发PWM基地址
 * @param [in] sample_freq PWM trigger frequency for sampling in KHz;PWM触发采样频率，以KHz为单位
 */

void trigger_init(PWM_Type * ptr,uint32_t sample_freq)
{
    pwm_cmp_config_t pwm_cmp_cfg;
    pwm_output_channel_t pwm_output_ch_cfg;

    /* TODO: Set PWM Clock Source and divider */

    pwm_set_reload(ptr, 0, (BOARD_ADC_TRIGGER_PWM_FRE_IN_KHZ/sample_freq)-1);

    /* Set comparator */
    memset(&pwm_cmp_cfg, 0x00, sizeof(pwm_cmp_config_t));
    pwm_cmp_cfg.enable_ex_cmp  = false;
    pwm_cmp_cfg.mode           = pwm_cmp_mode_output_compare;
    pwm_cmp_cfg.update_trigger = pwm_shadow_register_update_on_shlk;

    /* Select comp8 and trigger at the middle of a pwm cycle */
    pwm_cmp_cfg.cmp = (BOARD_ADC_TRIGGER_PWM_FRE_IN_KHZ/sample_freq)/2-1;
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

/**
 * @brief Startup ADC sampling trigger source;启动ADC触发源
 *
 * @param [in] ptr PWM base address;触发PWM基地址
 */

void trigger_start(PWM_Type * ptr)
{
  pwm_start_counter(ptr);
}

/**
 * @brief Initialize ADC;初始化ADC
 */

void adc_init(void)
{
    adc16_config_t cfg;
    adc16_seq_config_t seq_cfg;
    adc16_dma_config_t dma_cfg;
    adc16_channel_config_t ch_cfg;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);
    cfg.res            = adc16_res_16_bits;
    cfg.conv_mode      = adc16_conv_mode_sequence;
    cfg.adc_clk_div    = BOARD_ADC_CLK_DIV;
    cfg.sel_sync_ahb   = true;

    if (cfg.conv_mode == adc16_conv_mode_sequence ||
        cfg.conv_mode == adc16_conv_mode_preemption) {
        cfg.adc_ahb_en = true;
    }

    adc16_init(BOARD_APP_ADC16_BASE, &cfg);

    /* enable irq */
    intc_m_enable_irq_with_priority(BOARD_APP_ADC16_IRQn, 1);

    /* get a default channel config */
    adc16_get_channel_default_config(&ch_cfg);

    /* initialize an ADC channel */
    ch_cfg.sample_cycle = BOARD_ADC_SAMPLE_CYCLE;
    for (uint8_t i = 0; i < sizeof(seq_channel); i++) {
        ch_cfg.ch           = seq_channel[i];
        adc16_init_channel(BOARD_APP_ADC16_BASE, &ch_cfg);
    }

    /* Set a sequence config */ //ADC采样序列的配置
    seq_cfg.seq_len    = sizeof(seq_channel);
    seq_cfg.restart_en = false;
    seq_cfg.cont_en    = false;
    seq_cfg.sw_trig_en = false;
    seq_cfg.hw_trig_en = true;

    for (int i = BOARD_ADC_SEQ_START_POS; i < seq_cfg.seq_len; i++) {
        seq_cfg.queue[i].ch = seq_channel[i];
    }

    /* Initialize a sequence */
    adc16_set_seq_config(BOARD_APP_ADC16_BASE, &seq_cfg);

    /* Set a DMA config */
    dma_cfg.start_addr         = (uint32_t *)core_local_mem_to_sys_address(BOARD_APP_CORE, (uint32_t)seq_buff);
    dma_cfg.buff_len_in_4bytes = BOARD_ADC_DMA_BUFFER_LEN;
    dma_cfg.stop_en            = false;
    dma_cfg.stop_pos           = 0;

    /* Initialize DMA for the sequence mode */
    adc16_init_seq_dma(BOARD_APP_ADC16_BASE, &dma_cfg);
}

int main(void)
{ 
  uint32_t cycle;
  uint32_t seq_rdptr = 0;

  adc16_seq_dma_data_t *dma_data = (adc16_seq_dma_data_t *)data_buffer;

  board_init();
  
  /*DCDC_section*/
  board_DCDC_power_config();

  printf("This is an ADC16 demo for sinad test:\n");

  /* ADC pin initialization */
  board_init_adc16_pins();
  
  /* ADC clock initialization */
  board_init_adc16_clock(BOARD_APP_ADC16_BASE,true);

  /* ADC initialization */
  adc_init();

  /* Trigger source initialization */
  trigger_init(BOARD_APP_ADC_TRIG_PWM,BOARD_ADC_SAMPLE_FRE_IN_KHZ);//2000KHz sample fre

  for(uint32_t i = 0;i < BOARD_ADC_DATA_BUFFER_LEN; i++)
  {
    data_buffer[i]=0;
  }

  for(uint32_t i = 0;i < BOARD_ADC_DMA_BUFFER_LEN; i++)
  {
    seq_buff[i]=0;
  }

  cycle = 1;
  
  board_delay_ms(150);
  /* Start pwm to trigger adc convert */
  trigger_start(BOARD_APP_ADC_TRIG_PWM);

  while(1)
  {
     while(seq_rdptr < BOARD_ADC_DATA_BUFFER_LEN)
     {
        if(cycle) 
        {
          while((seq_buff[BOARD_ADC_DMA_BUFFER_LEN >> 1 ] & 0x80000000) == 0);
        }
        else
        {
          while((seq_buff[BOARD_ADC_DMA_BUFFER_LEN >> 1 ] & 0x80000000) != 0);
        }
        
        for(uint32_t i = 0;i < (BOARD_ADC_DMA_BUFFER_LEN >> 1); i++)
        {
          data_buffer[i+seq_rdptr] = seq_buff[i];
        }
      
        if(cycle) 
        {
          while((seq_buff[0] & 0x80000000) != 0);
        }
        else      
        {
          while((seq_buff[0] & 0x80000000) == 0);
        }
        
        for(uint32_t i = 0;i < (BOARD_ADC_DMA_BUFFER_LEN >> 1); i++)
        {
           data_buffer[i + seq_rdptr + (BOARD_ADC_DMA_BUFFER_LEN >> 1)] = seq_buff[i + (BOARD_ADC_DMA_BUFFER_LEN >> 1)];
        }
    
        seq_rdptr += BOARD_ADC_DMA_BUFFER_LEN;
        
        cycle = 1-cycle;
     }

     printf("adc data buff is full, buffer start addr:%x. end addr:%x\n",&data_buffer[0],&data_buffer[BOARD_ADC_DATA_BUFFER_LEN-1]);
      
     for(uint32_t i = 0; i < BOARD_ADC_DATA_BUFFER_LEN; i++)
     {
        printf("Ch: %d val: %d %d%%\n ",dma_data[i].adc_ch,dma_data[i].result,100*(i+1)/BOARD_ADC_DATA_BUFFER_LEN);
     }
    
     while(1);
  }
}