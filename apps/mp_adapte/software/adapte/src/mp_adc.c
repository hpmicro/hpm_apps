/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "hpm_adc16_drv.h"
#include "hpm_adc.h"
#include "mp_pwm.h"
#include "mp_adc.h"
#include "hpm_dmamux_drv.h"
#ifdef CONFIG_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif

#define MP_ADC_MAP_CTX        mp_adc_map
#define MP_ADC_MAP_COUNT      adc_map_count

// #define BOARD_APP_ADC_PMT_DMA_BUFF_LEN_IN_4BYTES (48U)
#define MP_HDMA_MAX_COUNT           (8)


typedef struct
{
    uint8_t adc_irq;
    uint8_t trigger_ch;
    uint8_t trigger_len;
    uint8_t enable_adc_ch;
    uint8_t type;
    void* handle;
} mp_adc_pack_handle_t;

typedef struct
{
    uint8_t count;
    mp_adc_pack_handle_t pack_handle[MP_PWM_PAIR_COUNT];
} mp_adc_group_handle_t;

typedef struct
{
    uint8_t hdma_ch;
    uint8_t mux_ch;
    uint8_t mux_src;
#ifndef HDMA_ADC_CHAIN_DOUBLE_BUF
    volatile bool transfer_done;
    volatile bool transfer_error;
#else
    volatile bool transfer_done[2];
    volatile bool transfer_error[2];
#endif
    ADC16_Type* adc16;
    uint8_t adc_index;
    uint32_t size;
} hpm_hdma_config_t;

typedef struct
{
    uint8_t count;
    uint8_t adc_mask;
    hpm_hdma_config_t hdma_config[MP_HDMA_MAX_COUNT];
} hpm_hdma_t;

static hpm_hdma_t hpm_hdma = { 0 };
static hpm_mp_hdma_over_callback hdma_over_cb = NULL;
static hpm_hdma_adc_t hdma_adc = { 0 };
static hpm_adc_t hdma_pack[MP_HDMA_MAX_COUNT];

static const mp_adc_map_t* mp_adc_map = NULL;
static uint32_t adc_map_count = 0;

uint8_t mp_adcmap_get_adc_index(ADC16_Type* adc16, uint8_t adc_ch);

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) static uint32_t adc_buff[3][USE_ADC_COUNT];
#ifndef HDMA_ADC_DOUBLE_BUF
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) static uint32_t dma_buff[USE_ADC_COUNT][HDMA_ADC_BUF_MAXSIZE];
#else
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) static uint16_t dma_buff[USE_ADC_COUNT][2 * HDMA_ADC_BUF_MAXSIZE];
#ifdef HDMA_ADC_CHAIN_DOUBLE_BUF
/* descriptor should be 8-byte aligned */
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) dma_linked_descriptor_t adc_descriptors1[USE_ADC_COUNT][2];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) dma_linked_descriptor_t adc_descriptors2[USE_ADC_COUNT][2];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) static uint32_t dma_ctrl_buff[USE_ADC_COUNT][2][8];
#endif
#endif

static adc_type hpm_adc_type[3] =
{
    {.adc_base.adc16 = HPM_ADC0, .module = ADCX_MODULE_ADC16},
    {.adc_base.adc16 = HPM_ADC1, .module = ADCX_MODULE_ADC16},
#ifdef HPM_ADC2
    {.adc_base.adc16 = HPM_ADC2, .module = ADCX_MODULE_ADC16},
#endif
};

static mp_adc_pack_handle_t adc_handle[3] = { 0 };

ATTR_RAMFUNC static inline void irq_process(uint8_t index)
{
    if (index > 3)
        return;
    int i, j;
    adc16_pmt_dma_data_t* dma_data = NULL;
    if (adc_handle[index].type == MP_PWM_SINGLE_TYPE)
    {
        hpm_pwm_t* pwm_t = (hpm_pwm_t*)adc_handle[index].handle;

        for (i = 0; i < pwm_t->adc_count; i += adc_handle[pwm_t->adc_pack[i].adc_irq_index].trigger_len)
        {
            dma_data = (adc16_pmt_dma_data_t*)&adc_buff[pwm_t->adc_pack[i].adc_irq_index][pwm_t->adc_pack[i].adc_trig_ch * sizeof(adc16_pmt_dma_data_t)];
            for (j = 0; j < adc_handle[pwm_t->adc_pack[i].adc_irq_index].trigger_len; j++)
            {
#ifdef HPM_MCU_5300
                if (dma_data[j].trig_ch == MP_ADC_MAP_CTX[pwm_t->adc_pack[i + j].adc_map_index].adc_ch)
#else
                if (dma_data[j].adc_ch == MP_ADC_MAP_CTX[pwm_t->adc_pack[i + j].adc_map_index].adc_ch)
#endif
                {
                    pwm_t->adc_pack[i + j].adc_data = dma_data[j].result;
                }
                else
                {
                    printf("error!\n");
                }
            }
        }

        if (pwm_t->adc_over_callback != NULL)
        {
            pwm_t->adc_over_callback(pwm_t);
        }
    }
    else if (adc_handle[index].type == MP_PWM_PAIR_TYPE)
    {
        hpm_pwm_pair_t* pwm_pair_t = (hpm_pwm_pair_t*)adc_handle[index].handle;

        for (i = 0; i < pwm_pair_t->adc_count; i += adc_handle[pwm_pair_t->adc_pack[i].adc_irq_index].trigger_len)
        {
            dma_data = (adc16_pmt_dma_data_t*)&adc_buff[pwm_pair_t->adc_pack[i].adc_irq_index][pwm_pair_t->adc_pack[i].adc_trig_ch * sizeof(adc16_pmt_dma_data_t)];
            for (j = 0; j < adc_handle[pwm_pair_t->adc_pack[i].adc_irq_index].trigger_len; j++)
            {
#ifdef HPM_MCU_5300
                if (dma_data[j].trig_ch == MP_ADC_MAP_CTX[pwm_pair_t->adc_pack[i + j].adc_map_index].adc_ch)
#else
                if (dma_data[j].adc_ch == MP_ADC_MAP_CTX[pwm_pair_t->adc_pack[i + j].adc_map_index].adc_ch)
#endif
                {
                    pwm_pair_t->adc_pack[i + j].adc_data = dma_data[j].result;
                }
                else
                {
                    printf("error!\n");
                }
            }
        }

        if (pwm_pair_t->adc_over_callback != NULL)
        {
            pwm_pair_t->adc_over_callback(pwm_pair_t);
        }
    }
}

static void isr_adc0(void)
{
    uint32_t status;
    status = hpm_adc_get_status_flags(&hpm_adc_type[0]);

    if ((status & adc16_event_trig_complete) != 0)
    {
        hpm_adc_clear_status_flags(&hpm_adc_type[0], adc16_event_trig_complete);

        irq_process(0);
    }
}

static void isr_adc1(void)
{
    uint32_t status;
    status = hpm_adc_get_status_flags(&hpm_adc_type[1]);

    if ((status & adc16_event_trig_complete) != 0)
    {
        hpm_adc_clear_status_flags(&hpm_adc_type[1], adc16_event_trig_complete);

        irq_process(1);
    }
}

#ifdef IRQn_ADC2
static void isr_adc2(void)
{
    uint32_t status;
    status = hpm_adc_get_status_flags(&hpm_adc_type[2]);

    if ((status & adc16_event_trig_complete) != 0)
    {
        hpm_adc_clear_status_flags(&hpm_adc_type[2], adc16_event_trig_complete);

        irq_process(2);
    }
}
#endif

#ifndef HDMA_ADC_CHAIN_DOUBLE_BUF
static void isr_hdma(void)
{
    volatile hpm_stat_t stat;
    adc16_pmt_dma_data_t* dma_data = NULL;
    int i, done_count = 0;
    for (i = 0; i < hpm_hdma.count; i++)
    {
        stat = dma_check_transfer_status(HPM_HDMA, hpm_hdma.hdma_config[i].hdma_ch);
        if (0 != (stat & DMA_CHANNEL_STATUS_TC))
        {
            dmamux_disable_channel(HPM_DMAMUX, hpm_hdma.hdma_config[i].mux_ch);
            hpm_hdma.hdma_config[i].transfer_done = true;
        }
        else if (0 != (stat & DMA_CHANNEL_STATUS_ERROR))
        {
            dmamux_disable_channel(HPM_DMAMUX, hpm_hdma.hdma_config[i].mux_ch);
            hpm_hdma.hdma_config[i].transfer_done = true;
            hpm_hdma.hdma_config[i].transfer_error = true;
            printf("hdma transfer error,ch:%d\n", hpm_hdma.hdma_config[i].hdma_ch);
        }
    }


    // hdma_adc
    for (i = 0; i < hpm_hdma.count; i++)
    {
        if (hpm_hdma.hdma_config[i].transfer_done)
        {
#ifndef HDMA_ADC_DOUBLE_BUF
            dma_data = (adc16_pmt_dma_data_t*)hdma_adc.adc_pack[i].adc_buff_addr;
            hdma_adc.adc_pack[i].adc_index = mp_adcmap_get_adc_index(hpm_hdma.hdma_config[i].adc16, dma_data->adc_ch);
#endif
            done_count++;
        }
    }
    
    if (done_count == hpm_hdma.count && hpm_hdma.count != 0)
    {
        //cb
        if (hdma_over_cb != NULL)
        {
            hdma_over_cb(&hdma_adc);
        }
        for (i = 0; i < hpm_hdma.count; i++)
        {
            hpm_hdma.hdma_config[i].transfer_done = false;
            hpm_hdma.hdma_config[i].transfer_error = false;
        }
    }
}
#else

static void isr_hdma(void)
{
    volatile hpm_stat_t stat;
    volatile uint32_t llpoint;
    int i, j, temp=0;
    uint8_t done_count[2] = {0};
    for (i = 0; i < hpm_hdma.count; i++)
    {
        stat = dma_check_transfer_status(HPM_HDMA, hpm_hdma.hdma_config[i].hdma_ch);
        llpoint = HPM_HDMA->CHCTRL[hpm_hdma.hdma_config[i].hdma_ch].LLPOINTER;
        if (0 != (stat & DMA_CHANNEL_STATUS_TC) && llpoint == 0)
        {
            hpm_hdma.hdma_config[i].transfer_done[0] = true;
        }
        else if (0 != (stat & DMA_CHANNEL_STATUS_ERROR))
        {
            hpm_hdma.hdma_config[i].transfer_done[0] = true;
            hpm_hdma.hdma_config[i].transfer_error[0] = true;
            printf("hdma transfer error,ch:%d\n", hpm_hdma.hdma_config[i].hdma_ch);
        }

        stat = dma_check_transfer_status(HPM_HDMA, hpm_hdma.hdma_config[i].hdma_ch + hpm_hdma.count);
        llpoint = HPM_HDMA->CHCTRL[hpm_hdma.hdma_config[i].hdma_ch + hpm_hdma.count].LLPOINTER;
        if (0 != (stat & DMA_CHANNEL_STATUS_TC) && llpoint == 0)
        {
            hpm_hdma.hdma_config[i].transfer_done[1] = true;
        }
        else if (0 != (stat & DMA_CHANNEL_STATUS_ERROR))
        {
            hpm_hdma.hdma_config[i].transfer_done[1] = true;
            hpm_hdma.hdma_config[i].transfer_error[1] = true;
            printf("hdma transfer error,ch:%d\n", hpm_hdma.hdma_config[i].hdma_ch + hpm_hdma.count);
        }
    }

    // hdma_adc
    for (i = 0; i < hpm_hdma.count; i++)
    {
       if (hpm_hdma.hdma_config[i].transfer_done[0])
       {
           hdma_adc.adc_pack[i].adc_buff_addr = hdma_adc.adc_pack[i].adc_buff_reserve[0];
           done_count[0]++;
       }
       if (hpm_hdma.hdma_config[i].transfer_done[1])
       {
           hdma_adc.adc_pack[i].adc_buff_addr = hdma_adc.adc_pack[i].adc_buff_reserve[1];
           done_count[1]++;
       }
    }

    for (j = 0; j < 2; j++)
    {
        if (done_count[j] == hpm_hdma.count && hpm_hdma.count != 0)
        {
            temp++;
            //cb
            if (hdma_over_cb != NULL)
            {
                hdma_over_cb(&hdma_adc);
            }
            for (i = 0; i < hpm_hdma.count; i++)
            {
                hpm_hdma.hdma_config[i].transfer_done[j] = false;
                hpm_hdma.hdma_config[i].transfer_error[j] = false;
            }
        }
    }
    if(temp > 1)
    {
        printf("Bad. If you run this, it's a tragedy\n");
    }
}
#endif
SDK_DECLARE_EXT_ISR_M(IRQn_ADC0, isr_adc0)
SDK_DECLARE_EXT_ISR_M(IRQn_ADC1, isr_adc1)
#ifdef IRQn_ADC2
SDK_DECLARE_EXT_ISR_M(IRQn_ADC2, isr_adc2)
#endif
SDK_DECLARE_EXT_ISR_M(IRQn_HDMA, isr_hdma)

uint8_t mp_adcmap_get_adc_index(ADC16_Type* adc16, uint8_t adc_ch)
{
    uint8_t i = 0;
    for (i = 0; i < MP_ADC_MAP_COUNT; i++)
    {
        if (MP_ADC_MAP_CTX[i].adc16 == adc16 && MP_ADC_MAP_CTX[i].adc_ch == adc_ch)
        {
            return MP_ADC_MAP_CTX[i].adc_index;
        }
    }
    return -1;
}

uint8_t mp_adc_get_mapindex(uint8_t adc_index)
{
    uint8_t i = 0;
    for (i = 0; i < MP_ADC_MAP_COUNT; i++)
    {
        if (MP_ADC_MAP_CTX[i].adc_index == adc_index)
        {
            return i;
        }
    }
    return -1;
}

void mp_adc_get_default(uint8_t adc_index, hpm_adc_t* adc_t)
{
    adc_t->adc_index = adc_index;
    adc_t->adc_map_index = mp_adc_get_mapindex(adc_index);
    adc_t->adc_irq_index = 0;
    adc_t->adc_trig_ch = 0;
    adc_t->adc_trig_index = 0;
    adc_t->adc_data = 0;
}

static int mp_adc_pwmpair_init(hpm_pwm_pair_t* pwm_pair_t)
{
    adc_config_t cfg;
    adc_channel_config_t ch_cfg;
    int i;
    if (NULL == pwm_pair_t)
        return -1;
    if (NULL == pwm_pair_t->adc_pack || pwm_pair_t->adc_count == 0)
        return 0;
    if (pwm_pair_t->adc_count > USE_ADC_COUNT)
        return -2;
    
    for (i = 0; i < pwm_pair_t->adc_count; i++)
    {
        cfg.module = ADCX_MODULE_ADC16;
        hpm_adc_init_default_config(&cfg);
        cfg.config.adc16.res = adc16_res_16_bits;
        cfg.config.adc16.conv_mode = adc16_conv_mode_preemption;
        cfg.config.adc16.adc_clk_div = 3;
        cfg.config.adc16.sel_sync_ahb = false;
        cfg.config.adc16.adc_ahb_en = true;

        cfg.adc_base.adc16 = MP_ADC_MAP_CTX[pwm_pair_t->adc_pack[i].adc_map_index].adc16;
        hpm_adc_init(&cfg);

    }
    for (i = 0; i < pwm_pair_t->adc_count; i++)
    {
        ch_cfg.module = ADCX_MODULE_ADC16;
        hpm_adc_init_channel_default_config(&ch_cfg);

        ch_cfg.config.adc16_ch.sample_cycle = 16;

        ch_cfg.adc_base.adc16 = MP_ADC_MAP_CTX[pwm_pair_t->adc_pack[i].adc_map_index].adc16;
        ch_cfg.config.adc16_ch.ch = MP_ADC_MAP_CTX[pwm_pair_t->adc_pack[i].adc_map_index].adc_ch;
        hpm_adc_channel_init(&ch_cfg);
    }
    return 0;
}

static int mp_adc_pwm_init(hpm_pwm_t* pwm_t)
{
    adc_config_t cfg;
    adc_channel_config_t ch_cfg;
    int i;
    uint8_t adc_init[3] = {0};
    if (NULL == pwm_t)
        return -1;
    if (NULL == pwm_t->adc_pack || pwm_t->adc_count == 0)
        return 0;
    if (pwm_t->adc_count > USE_ADC_COUNT)
        return -2;

    for (i = 0; i < pwm_t->adc_count; i++)
    {
        cfg.module = ADCX_MODULE_ADC16;
        hpm_adc_init_default_config(&cfg);
        cfg.config.adc16.res = adc16_res_16_bits;
        cfg.config.adc16.conv_mode = adc16_conv_mode_preemption;
        cfg.config.adc16.adc_clk_div = 3;
        cfg.config.adc16.sel_sync_ahb = false;
        cfg.config.adc16.adc_ahb_en = true;

        cfg.adc_base.adc16 = MP_ADC_MAP_CTX[pwm_t->adc_pack[i].adc_map_index].adc16;
        if(cfg.adc_base.adc16 == HPM_ADC0 && adc_init[0] == 0)
        {
            adc_init[0] = 1;
            clock_add_to_group(clock_adc0, 0);
            clock_set_adc_source(clock_adc0, clk_adc_src_ahb0);
        }
        else if(cfg.adc_base.adc16 == HPM_ADC1 && adc_init[1] == 0)
        {
            adc_init[1] = 1;
            clock_add_to_group(clock_adc1, 0);
            clock_set_adc_source(clock_adc1, clk_adc_src_ahb0);
        }
#ifdef HPM_ADC2
        else if(cfg.adc_base.adc16 == HPM_ADC2 && adc_init[2] == 0)
        {
            adc_init[2] = 1;
            clock_add_to_group(clock_adc2, 0);
            clock_set_adc_source(clock_adc2, clk_adc_src_ahb0);
        }
#endif

        hpm_adc_init(&cfg);

    }
    for (i = 0; i < pwm_t->adc_count; i++)
    {
        ch_cfg.module = ADCX_MODULE_ADC16;
        hpm_adc_init_channel_default_config(&ch_cfg);

        ch_cfg.config.adc16_ch.sample_cycle = 16;

        ch_cfg.adc_base.adc16 = MP_ADC_MAP_CTX[pwm_t->adc_pack[i].adc_map_index].adc16;
        ch_cfg.config.adc16_ch.ch = MP_ADC_MAP_CTX[pwm_t->adc_pack[i].adc_map_index].adc_ch;
        hpm_adc_channel_init(&ch_cfg);
    }
    return 0;
}

int mp_adc_init(uint8_t type, void* handle)
{
    int ret = -1;
    static uint8_t init_done = 0;
    if (type == MP_PWM_SINGLE_TYPE)
    {
        ret = mp_adc_pwm_init((hpm_pwm_t*)handle);
    }
    else if(type == MP_PWM_PAIR_TYPE)
    {
        ret = mp_adc_pwmpair_init((hpm_pwm_pair_t*)handle);
    }

    if(init_done == 0)
    {
        init_done = 1;
        /* Set DMA start address for preemption mode */
        hpm_adc_init_pmt_dma(&hpm_adc_type[0], core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[0][0]));
        hpm_adc_init_pmt_dma(&hpm_adc_type[1], core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[1][0]));
#ifdef HPM_ADC2
        hpm_adc_init_pmt_dma(&hpm_adc_type[2], core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[2][0]));
#endif
    }
    return ret;
}

int mp_adc_trigger_config(uint8_t type, void* handle)
{
    int i;
    const mp_pwm_trigger_map_t* trigger_map_t = NULL;
    mp_pwm_trigger_map_t* trigger_map = NULL;
    hpm_pwm_pair_t* pwm_pair_t = NULL;
    hpm_pwm_t* pwm_t = NULL;
    adc_pmt_config_t pmt_cfg[3] = { 0 };
    uint8_t adc_trig_len[3] = { 0 };
    uint8_t trig_ch;
    uint8_t adc_count = 0, index = 0, adc_map_index;
    if (type == MP_PWM_SINGLE_TYPE)
    {
        pwm_t = (hpm_pwm_t*)handle;
        if (NULL == pwm_t)
            return -1;
        if (!(pwm_t->trigger_enable & (1 << MP_PWM_TRIGGER_ADC_BIT)))
            return 0;
        adc_count = pwm_t->adc_count;
        index = pwm_t->pwm_index;
    }
    else if (type == MP_PWM_PAIR_TYPE)
    {
        pwm_pair_t = (hpm_pwm_pair_t*)handle;
        if (NULL == pwm_pair_t)
            return -1;
        if (!(pwm_pair_t->trigger_enable & (1 << MP_PWM_TRIGGER_ADC_BIT)))
            return 0;
        adc_count = pwm_pair_t->adc_count;
        index = pwm_pair_t->pwm_pair_index;
    }
    else
        return -1;

    for (i = 0; i < adc_count; i++)
    {
        adc_map_index = (type == MP_PWM_SINGLE_TYPE) ? pwm_t->adc_pack[i].adc_map_index : pwm_pair_t->adc_pack[i].adc_map_index;
        trigger_map_t = mp_pwm_get_trigger_map(type, index);
        trigger_map = (mp_pwm_trigger_map_t*)(&trigger_map_t[MP_PWM_TRIGGER_ADC_BIT]);
        if (trigger_map->trgm_base == HPM_TRGM0 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0A)
        {
            trig_ch = ADC16_CONFIG_TRG0A;
        }
        else if (trigger_map->trgm_base == HPM_TRGM0 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0B)
        {
            trig_ch = ADC16_CONFIG_TRG0B;
        }
        else if (trigger_map->trgm_base == HPM_TRGM0 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0C)
        {
            trig_ch = ADC16_CONFIG_TRG0C;
        }
#ifdef TRGM_TRGOCFG_ADCX_PTRGI1A
        else if (trigger_map->trgm_base == HPM_TRGM0 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI1A)
        {
            trig_ch = ADC16_CONFIG_TRG1A;
        }
#endif
#ifdef TRGM_TRGOCFG_ADCX_PTRGI1B
        else if (trigger_map->trgm_base == HPM_TRGM0 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI1B)
        {
            trig_ch = ADC16_CONFIG_TRG1B;
        }
#endif
#ifdef TRGM_TRGOCFG_ADCX_PTRGI1C
        else if (trigger_map->trgm_base == HPM_TRGM0 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI1C)
        {
            trig_ch = ADC16_CONFIG_TRG1C;
        }
#endif
#ifdef HPM_TRGM1
        else if (trigger_map->trgm_base == HPM_TRGM1 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0A)
        {
            trig_ch = ADC16_CONFIG_TRG1A;
        }
        else if (trigger_map->trgm_base == HPM_TRGM1 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0B)
        {
            trig_ch = ADC16_CONFIG_TRG1B;
        }
        else if (trigger_map->trgm_base == HPM_TRGM1 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0C)
        {
            trig_ch = ADC16_CONFIG_TRG1C;
        }
#endif
#ifdef HPM_TRGM2
        else if (trigger_map->trgm_base == HPM_TRGM2 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0A)
        {
            trig_ch = ADC16_CONFIG_TRG2A;
        }
        else if (trigger_map->trgm_base == HPM_TRGM2 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0B)
        {
            trig_ch = ADC16_CONFIG_TRG2B;
        }
        else if (trigger_map->trgm_base == HPM_TRGM2 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0C)
        {
            trig_ch = ADC16_CONFIG_TRG2C;
        }
#endif
#ifdef HPM_TRGM3
        else if (trigger_map->trgm_base == HPM_TRGM3 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0A)
        {
            trig_ch = ADC16_CONFIG_TRG3A;
        }
        else if (trigger_map->trgm_base == HPM_TRGM3 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0B)
        {
            trig_ch = ADC16_CONFIG_TRG3B;
        }
        else if (trigger_map->trgm_base == HPM_TRGM3 && trigger_map->trgm_output == TRGM_TRGOCFG_ADCX_PTRGI0C)
        {
            trig_ch = ADC16_CONFIG_TRG3C;
        }
#endif
        else
        {
            printf("trig ch error!\n");
            return -2;
        }

        if (MP_ADC_MAP_CTX[adc_map_index].adc16 == HPM_ADC0)
        {
            pmt_cfg[0].module = ADCX_MODULE_ADC16;
            pmt_cfg[0].config.adc16.trig_ch = trig_ch;
            pmt_cfg[0].config.adc16.adc_ch[adc_trig_len[0]] = MP_ADC_MAP_CTX[adc_map_index].adc_ch;
            if (type == MP_PWM_SINGLE_TYPE)
            {
                if (pwm_t->adc_intr_index == pwm_t->adc_pack[i].adc_index)
                {
                    pmt_cfg[0].config.adc16.inten[adc_trig_len[0]] = true;
                }
                else
                {
                    pmt_cfg[0].config.adc16.inten[adc_trig_len[0]] = false;
                }

                pwm_t->adc_pack[i].adc_irq_index = 0;
                pwm_t->adc_pack[i].adc_trig_ch = trig_ch;
                pwm_t->adc_pack[i].adc_trig_index = adc_trig_len[0];
            }
            else
            {
                if (pwm_pair_t->adc_intr_index == pwm_pair_t->adc_pack[i].adc_index)
                {
                    pmt_cfg[0].config.adc16.inten[adc_trig_len[0]] = true;
                }
                else
                {
                    pmt_cfg[0].config.adc16.inten[adc_trig_len[0]] = false;
                }

                pwm_pair_t->adc_pack[i].adc_irq_index = 0;
                pwm_pair_t->adc_pack[i].adc_trig_ch = trig_ch;
                pwm_pair_t->adc_pack[i].adc_trig_index = adc_trig_len[0];
            }
            
            adc_trig_len[0]++;
        }
        else if (MP_ADC_MAP_CTX[adc_map_index].adc16 == HPM_ADC1)
        {
            pmt_cfg[1].module = ADCX_MODULE_ADC16;
            pmt_cfg[1].config.adc16.trig_ch = trig_ch;
            pmt_cfg[1].config.adc16.adc_ch[adc_trig_len[1]] = MP_ADC_MAP_CTX[adc_map_index].adc_ch;
            if (type == MP_PWM_SINGLE_TYPE)
            {
                if (pwm_t->adc_intr_index == pwm_t->adc_pack[i].adc_index)
                {
                    pmt_cfg[1].config.adc16.inten[adc_trig_len[1]] = true;
                }
                else
                {
                    pmt_cfg[1].config.adc16.inten[adc_trig_len[1]] = false;
                }

                pwm_t->adc_pack[i].adc_irq_index = 1;
                pwm_t->adc_pack[i].adc_trig_ch = trig_ch;
                pwm_t->adc_pack[i].adc_trig_index = adc_trig_len[1];
            }
            else
            {
                if (pwm_pair_t->adc_intr_index == pwm_pair_t->adc_pack[i].adc_index)
                {
                    pmt_cfg[1].config.adc16.inten[adc_trig_len[1]] = true;
                }
                else
                {
                    pmt_cfg[1].config.adc16.inten[adc_trig_len[1]] = false;
                }

                pwm_pair_t->adc_pack[i].adc_irq_index = 1;
                pwm_pair_t->adc_pack[i].adc_trig_ch = trig_ch;
                pwm_pair_t->adc_pack[i].adc_trig_index = adc_trig_len[1];
            }
            adc_trig_len[1]++;
        }
#ifdef HPM_ADC2
        else if (MP_ADC_MAP_CTX[adc_map_index].adc16 == HPM_ADC2)
        {
            pmt_cfg[2].module = ADCX_MODULE_ADC16;
            pmt_cfg[2].config.adc16.trig_ch = trig_ch;
            pmt_cfg[2].config.adc16.adc_ch[adc_trig_len[2]] = MP_ADC_MAP_CTX[adc_map_index].adc_ch;
            if (type == MP_PWM_SINGLE_TYPE)
            {
                if (pwm_t->adc_intr_index == pwm_t->adc_pack[i].adc_index)
                {
                    pmt_cfg[2].config.adc16.inten[adc_trig_len[2]] = true;
                }
                else
                {
                    pmt_cfg[2].config.adc16.inten[adc_trig_len[2]] = false;
                }

                pwm_t->adc_pack[i].adc_irq_index = 2;
                pwm_t->adc_pack[i].adc_trig_ch = trig_ch;
                pwm_t->adc_pack[i].adc_trig_index = adc_trig_len[2];
            }
            else
            {
                if (pwm_pair_t->adc_intr_index == pwm_pair_t->adc_pack[i].adc_index)
                {
                    pmt_cfg[2].config.adc16.inten[adc_trig_len[2]] = true;
                }
                else
                {
                    pmt_cfg[2].config.adc16.inten[adc_trig_len[2]] = false;
                }

                pwm_pair_t->adc_pack[i].adc_irq_index = 2;
                pwm_pair_t->adc_pack[i].adc_trig_ch = trig_ch;
                pwm_pair_t->adc_pack[i].adc_trig_index = adc_trig_len[2];
            }
            adc_trig_len[2]++;
        }
#endif
        else
        {
            return -3;
        }
    }

    if (adc_trig_len[0] > 0)
    {
        // pmt_cfg[0].config.adc16.inten[adc_trig_len[0] - 1] = true;
        pmt_cfg[0].config.adc16.trig_len = adc_trig_len[0];
        pmt_cfg[0].adc_base.adc16 = HPM_ADC0;
        hpm_adc_set_preempt_config(&pmt_cfg[0]);
        adc16_set_pmt_queue_enable(HPM_ADC0, pmt_cfg[0].config.adc16.trig_ch, true);

        adc_handle[0].type = type;
        adc_handle[0].adc_irq = IRQn_ADC0;
        adc_handle[0].trigger_ch = pmt_cfg[0].config.adc16.trig_ch;
        adc_handle[0].trigger_len = pmt_cfg[0].config.adc16.trig_len;
        for (i = 0; i < adc_trig_len[0]; i++)
        {
            if (pmt_cfg[0].config.adc16.inten[i])
            {
                if (adc_handle[0].handle != NULL)
                {
                    printf("error! repeat 1 config handle0!\n");
                    return -3;
                }
                adc_handle[0].enable_adc_ch = pmt_cfg[0].config.adc16.adc_ch[i];
                if (type == MP_PWM_SINGLE_TYPE)
                {
                    adc_handle[0].handle = (void*)pwm_t;
                    if (pwm_t->adc_handle == NULL)
                    {
                        pwm_t->adc_handle = &adc_handle[0];
                    }
                    else
                    {
                        printf("error! repeat config handle0!\n");
                        return -3;
                    }
                }
                else
                {
                    adc_handle[0].handle = (void*)pwm_pair_t;
                    if (pwm_pair_t->adc_handle == NULL)
                    {
                        pwm_pair_t->adc_handle = &adc_handle[0];
                    }
                    else
                    {
                        printf("error! repeat config handle0!\n");
                        return -3;
                    }
                }
            }
        }
    }
    if (adc_trig_len[1] > 0)
    {
        // pmt_cfg[1].config.adc16.inten[adc_trig_len[1] - 1] = true;
        pmt_cfg[1].config.adc16.trig_len = adc_trig_len[1];
        pmt_cfg[1].adc_base.adc16 = HPM_ADC1;
        hpm_adc_set_preempt_config(&pmt_cfg[1]);
        adc16_set_pmt_queue_enable(HPM_ADC1, pmt_cfg[1].config.adc16.trig_ch, true);
        adc_handle[1].type = type;
        adc_handle[1].adc_irq = IRQn_ADC1;
        adc_handle[1].trigger_ch = pmt_cfg[1].config.adc16.trig_ch;
        adc_handle[1].trigger_len = pmt_cfg[1].config.adc16.trig_len;
        for (i = 0; i < adc_trig_len[1]; i++)
        {
            if (pmt_cfg[1].config.adc16.inten[i])
            {
                if (adc_handle[1].handle != NULL)
                {
                    printf("error! repeat 1 config handle1!\n");
                    return -4;
                }
                adc_handle[1].enable_adc_ch = pmt_cfg[1].config.adc16.adc_ch[i];
                if (type == MP_PWM_SINGLE_TYPE)
                {
                    adc_handle[1].handle = (void*)pwm_t;
                    if (pwm_t->adc_handle == NULL)
                    {
                        pwm_t->adc_handle = &adc_handle[1];
                    }
                    else
                    {
                        printf("error! repeat config handle1!\n");
                        return -4;
                    }
                }
                else
                {
                    adc_handle[1].handle = (void*)pwm_pair_t;
                    if (pwm_pair_t->adc_handle == NULL)
                    {
                        pwm_pair_t->adc_handle = &adc_handle[1];
                    }
                    else
                    {
                        printf("error! repeat config handle1!\n");
                        return -4;
                    }
                }
            }
        }
    }
#ifdef HPM_ADC2
    if (adc_trig_len[2] > 0)
    {
        // pmt_cfg[2].config.adc16.inten[adc_trig_len[2] - 1] = true;
        pmt_cfg[2].config.adc16.trig_len = adc_trig_len[2];
        pmt_cfg[2].adc_base.adc16 = HPM_ADC2;
        hpm_adc_set_preempt_config(&pmt_cfg[2]);
        adc16_set_pmt_queue_enable(HPM_ADC2, pmt_cfg[2].config.adc16.trig_ch, true);
        adc_handle[2].type = type;
        adc_handle[2].adc_irq = IRQn_ADC2;
        adc_handle[2].trigger_ch = pmt_cfg[2].config.adc16.trig_ch;
        adc_handle[2].trigger_len = pmt_cfg[2].config.adc16.trig_len;
        for (i = 0; i < adc_trig_len[2]; i++)
        {
            if (pmt_cfg[2].config.adc16.inten[i])
            {
                if (adc_handle[2].handle != NULL)
                {
                    printf("error! repeat 1 config handle2!\n");
                    return -5;
                }
                adc_handle[2].enable_adc_ch = pmt_cfg[2].config.adc16.adc_ch[i];
                if (type == MP_PWM_SINGLE_TYPE)
                {
                    adc_handle[2].handle = (void*)pwm_t;
                    if (pwm_t->adc_handle == NULL)
                    {
                        pwm_t->adc_handle = (void*)&adc_handle[2];
                    }
                    else
                    {
                        printf("error! repeat config handle1!\n");
                        return -5;
                    }
                }
                else
                {
                    adc_handle[2].handle = (void*)pwm_pair_t;
                    if (pwm_pair_t->adc_handle == NULL)
                    {
                        pwm_pair_t->adc_handle = (void*)&adc_handle[2];
                    }
                    else
                    {
                        printf("error! repeat config handle1!\n");
                        return -5;
                    }
                }
            }
        }
    }
#endif
    return 0;
}

int mp_adc_interrupt_enable(uint8_t type, void* handle, bool enable)
{
    mp_adc_pack_handle_t* adc_pack_handle = NULL;
    hpm_pwm_pair_t* pwm_pair_t = NULL;
    hpm_pwm_t* pwm_t = NULL;
    int index = -1;
    if (type == MP_PWM_SINGLE_TYPE)
    {
        pwm_t = (hpm_pwm_t*)handle;
        if (NULL == pwm_t)
            return -1;
        if (!pwm_t->trigger_enable || NULL == pwm_t->adc_handle)
            return 0;
        adc_pack_handle = (mp_adc_pack_handle_t*)pwm_t->adc_handle;
    }
    else if (type == MP_PWM_PAIR_TYPE)
    {
        pwm_pair_t = (hpm_pwm_pair_t*)handle;
        if (NULL == pwm_pair_t)
            return -1;
        if (!pwm_pair_t->trigger_enable || NULL == pwm_pair_t->adc_handle)
            return 0;
        adc_pack_handle = (mp_adc_pack_handle_t*)pwm_pair_t->adc_handle;
    }
    else
        return -1;
    
    intc_m_enable_irq_with_priority(adc_pack_handle->adc_irq, 1);
    if (adc_pack_handle->adc_irq == IRQn_ADC0)
    {
        index = 0;
    }
    else if (adc_pack_handle->adc_irq == IRQn_ADC1)
    {
        index = 1;
    }
#ifdef IRQn_ADC2
    else if (adc_pack_handle->adc_irq == IRQn_ADC2)
    {
        index = 2;
    }
#endif
    if (index < 0)
        return -2;

    if (enable)
    {
        hpm_adc_enable_interrupts(&hpm_adc_type[index], adc16_event_trig_complete);
    }
    else
    {
        hpm_adc_disable_interrupts(&hpm_adc_type[index], adc16_event_trig_complete);
    }
    return 0;
}


static uint8_t mp_hdma_ch_to_mux_ch(uint8_t hdma_ch)
{
    uint8_t mux_ch;
    switch (hdma_ch)
    {
    case 0:
    {
        mux_ch = DMAMUX_MUXCFG_HDMA_MUX0;
    }break;
    case 1:
    {
        mux_ch = DMAMUX_MUXCFG_HDMA_MUX1;
    }break;
    case 2:
    {
        mux_ch = DMAMUX_MUXCFG_HDMA_MUX2;
    }break;
    case 3:
    {
        mux_ch = DMAMUX_MUXCFG_HDMA_MUX3;
    }break;
    case 4:
    {
        mux_ch = DMAMUX_MUXCFG_HDMA_MUX4;
    }break;
    case 5:
    {
        mux_ch = DMAMUX_MUXCFG_HDMA_MUX5;
    }break;
    case 6:
    {
        mux_ch = DMAMUX_MUXCFG_HDMA_MUX6;
    }break;
    case 7:
    {
        mux_ch = DMAMUX_MUXCFG_HDMA_MUX7;
    }break;
    default:
    {
        mux_ch = 255;
    }break;
    }
    return mux_ch;
}

static uint8_t mp_hdma_pwmch_to_muxsrc(uint8_t pwmch)
{
    uint8_t mux_src;
    switch (pwmch)
    {
    case 0:
    {
#ifdef HPM_DMA_SRC_MOT0_0
        mux_src = HPM_DMA_SRC_MOT0_0;
#else
        mux_src = HPM_DMA_SRC_MOT_0;
#endif
    }break;
    case 1:
    {
#ifdef HPM_DMA_SRC_MOT0_1
        mux_src = HPM_DMA_SRC_MOT0_1;
#else
        mux_src = HPM_DMA_SRC_MOT_1;
#endif
    }break;
    case 2:
    {
#ifdef HPM_DMA_SRC_MOT0_2
        mux_src = HPM_DMA_SRC_MOT0_2;
#else
        mux_src = HPM_DMA_SRC_MOT_2;
#endif
    }break;
    case 3:
    {
#ifdef HPM_DMA_SRC_MOT0_3
        mux_src = HPM_DMA_SRC_MOT0_3;
#else
        mux_src = HPM_DMA_SRC_MOT_3;
#endif
    }break;
    case 4:
    {
#ifdef HPM_DMA_SRC_MOT1_0
        mux_src = HPM_DMA_SRC_MOT1_0;
#else
        mux_src = HPM_DMA_SRC_MOT_4;
#endif
    }break;
    case 5:
    {
#ifdef HPM_DMA_SRC_MOT1_1
        mux_src = HPM_DMA_SRC_MOT1_1;
#else
        mux_src = HPM_DMA_SRC_MOT_5;
#endif
    }break;
    case 6:
    {
#ifdef HPM_DMA_SRC_MOT1_2
        mux_src = HPM_DMA_SRC_MOT1_2;
#else
        mux_src = HPM_DMA_SRC_MOT_6;
#endif
    }break;
    case 7:
    {
#ifdef HPM_DMA_SRC_MOT1_3
        mux_src = HPM_DMA_SRC_MOT1_3;
#else
        mux_src = HPM_DMA_SRC_MOT_7;
#endif
    }break;
    default:
    {
        mux_src = 0;
    }break;
    }
    return mux_src;
}

int mp_hdma_init_config(uint8_t type, void* handle, hpm_mp_hdma_over_callback cb)
{
    int i;
    const mp_pwm_trigger_map_t* trigger_map_t = NULL;
    hpm_pwm_pair_t* pwm_pair_t = NULL;
    hpm_pwm_t* pwm_t = NULL;
    uint8_t adc_count = 0, index = 0, adc_map_index;

    hdma_over_cb = cb;

    if (type == MP_PWM_SINGLE_TYPE)
    {
        pwm_t = (hpm_pwm_t*)handle;
        if (NULL == pwm_t)
            return -1;
        if (pwm_t->trigger_enable == 0 || pwm_t->trigger_count == 0)
            return 0;

        adc_count = pwm_t->adc_count;
        index = pwm_t->pwm_index;

        trigger_map_t = mp_pwm_get_trigger_map(type, index);

        for (i = 0; i < pwm_t->trigger_count; i++)
        {
            if ((pwm_t->trigger_enable & (1 << i)) && trigger_map_t[i].mode == MP_PWM_TRIGGER_MODE_DMA)
            {
                hpm_hdma.hdma_config[hpm_hdma.count].hdma_ch = hpm_hdma.count;
                hpm_hdma.hdma_config[hpm_hdma.count].mux_ch = mp_hdma_ch_to_mux_ch(hpm_hdma.hdma_config[hpm_hdma.count].hdma_ch);
                hpm_hdma.hdma_config[hpm_hdma.count].mux_src = mp_hdma_pwmch_to_muxsrc(trigger_map_t[i].trgm_input);
                hpm_hdma.count++;
            }
        }
    }
    else if (type == MP_PWM_PAIR_TYPE)
    {
        pwm_pair_t = (hpm_pwm_pair_t*)handle;
        if (NULL == pwm_pair_t)
            return -1;
        if (pwm_pair_t->trigger_enable == 0 || pwm_pair_t->trigger_count == 0)
            return 0;

        adc_count = pwm_pair_t->adc_count;
        index = pwm_pair_t->pwm_pair_index;

        trigger_map_t = mp_pwm_get_trigger_map(type, index);

        for (i = 0; i < pwm_pair_t->trigger_count; i++)
        {
            if ((pwm_pair_t->trigger_enable & (1 << i)) && trigger_map_t[i].mode == MP_PWM_TRIGGER_MODE_DMA)
            {
                hpm_hdma.hdma_config[hpm_hdma.count].hdma_ch = hpm_hdma.count;
                hpm_hdma.hdma_config[hpm_hdma.count].mux_ch = mp_hdma_ch_to_mux_ch(hpm_hdma.hdma_config[hpm_hdma.count].hdma_ch);
                hpm_hdma.hdma_config[hpm_hdma.count].mux_src = mp_hdma_pwmch_to_muxsrc(trigger_map_t[i].trgm_input);
                hpm_hdma.count++;
            }
        }
    }
    else
        return -1;

    if (hpm_hdma.count > adc_count)
    {
        printf("BAD! hdma count must be less than adc count");
        return -2;
    }

    hdma_adc.adc_count = hpm_hdma.count;
    hdma_adc.adc_pack = hdma_pack;

    for (i = 0; i < hpm_hdma.count; i++)
    {
        adc_map_index = (type == MP_PWM_SINGLE_TYPE) ? pwm_t->adc_pack[i].adc_map_index : pwm_pair_t->adc_pack[i].adc_map_index;
        if (MP_ADC_MAP_CTX[adc_map_index].adc16 == HPM_ADC0)
        {
            hpm_hdma.adc_mask |= (1 << 0);
            hpm_hdma.hdma_config[i].adc16 = HPM_ADC0;
            hpm_hdma.hdma_config[i].adc_index = MP_ADC_MAP_CTX[adc_map_index].adc_index;
        }
        else if (MP_ADC_MAP_CTX[adc_map_index].adc16 == HPM_ADC1)
        {
            hpm_hdma.adc_mask |= (1 << 1);
            hpm_hdma.hdma_config[i].adc16 = HPM_ADC1;
            hpm_hdma.hdma_config[i].adc_index = MP_ADC_MAP_CTX[adc_map_index].adc_index;

        }
#ifdef HPM_ADC2
        else if (MP_ADC_MAP_CTX[adc_map_index].adc16 == HPM_ADC2)
        {
            hpm_hdma.adc_mask |= (1 << 2);
            hpm_hdma.hdma_config[i].adc16 = HPM_ADC2;
            hpm_hdma.hdma_config[i].adc_index = MP_ADC_MAP_CTX[adc_map_index].adc_index;
        }
#endif
#ifdef HDMA_ADC_DOUBLE_BUF
        hdma_adc.adc_pack[i].adc_index = hpm_hdma.hdma_config[i].adc_index;
        printf("adc_index[%d]:%d\n", i, hdma_adc.adc_pack[i].adc_index);
#endif
    }
    for (i = 0; i < hpm_hdma.count; i++)
    {
        dma_enable_channel_interrupt(HPM_HDMA, hpm_hdma.hdma_config[i].hdma_ch, DMA_INTERRUPT_MASK_TERMINAL_COUNT);
        dmamux_config(HPM_DMAMUX, hpm_hdma.hdma_config[i].mux_ch, hpm_hdma.hdma_config[i].mux_src, false);
    }
    intc_m_enable_irq_with_priority(IRQn_HDMA, 1);

    return 0;
}

int mp_hdma_transfer_start(uint32_t size)
{
    int i;
    dma_channel_config_t ch_config = { 0 };
    if (size > HDMA_ADC_BUF_MAXSIZE)
        return -1;
    
    dma_reset(HPM_HDMA);
    // hdma_adc

    for (i = 0; i < hpm_hdma.count; i++)
    {
        dma_default_channel_config(HPM_HDMA, &ch_config);

        if (hpm_hdma.hdma_config[i].adc16 == HPM_ADC0)
        {
            ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[0][i]);
        }
        else if (hpm_hdma.hdma_config[i].adc16 == HPM_ADC1)
        {
            ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[1][i]);
        }
#ifdef HPM_ADC2
        else if (hpm_hdma.hdma_config[i].adc16 == HPM_ADC2)
        {
            ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[2][i]);
        }
#endif
        else
        {
            return -2;
        }
#ifndef HDMA_ADC_DOUBLE_BUF
        hdma_adc.adc_pack[i].adc_buff_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_buff[i][0]);
#else
        if (hdma_adc.adc_pack[i].adc_buff_addr == core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_buff[i][0]))
        {
            hdma_adc.adc_pack[i].adc_buff_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_buff[i][HDMA_ADC_BUF_MAXSIZE]);
        }
        else if (hdma_adc.adc_pack[i].adc_buff_addr == core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_buff[i][HDMA_ADC_BUF_MAXSIZE]))
        {
            hdma_adc.adc_pack[i].adc_buff_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_buff[i][0]);
        }
        else
        {
            hdma_adc.adc_pack[i].adc_buff_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_buff[i][0]);
        }
#endif
        ch_config.dst_addr = hdma_adc.adc_pack[i].adc_buff_addr;

#ifndef HDMA_ADC_DOUBLE_BUF
        ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
        ch_config.size_in_byte = size * sizeof(uint32_t);
        ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
#else
        ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
        ch_config.size_in_byte = size * sizeof(uint16_t);
        ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
#endif
        ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
        ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
        ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;

        hpm_hdma.hdma_config[i].size = size;

        ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;

        //warning! fixed addr no support burst!
        if (status_success != dma_setup_channel(HPM_HDMA, hpm_hdma.hdma_config[i].hdma_ch, &ch_config, true))
        {
            printf(" dma setup channel failed 0\n");
            return -3;
        }
        dmamux_enable_channel(HPM_DMAMUX, hpm_hdma.hdma_config[i].mux_ch);
    }
    return 0;
}


int mp_dma_chained_transfer_config(uint32_t size)
{
#ifndef HDMA_ADC_CHAIN_DOUBLE_BUF
    (void)size;
    return -1;
#else
    int i;
    hpm_stat_t stat;
    dma_channel_config_t ch_config = { 0 };
    if (size > HDMA_ADC_BUF_MAXSIZE)
        return -1;

    if (hpm_hdma.count > 4)
    {
        printf("ERROR: chain mode no support >4 adc!\n");
        return -2;
    }

    dma_reset(HPM_HDMA);
    for (i = 0; i < hpm_hdma.count; i++)
    {
        adc_descriptors1[i][0].trans_size = DMA_CHCTRL_TRANSIZE_TRANSIZE_SET((size*sizeof(uint16_t)) >> DMA_TRANSFER_WIDTH_HALF_WORD);
        if (hpm_hdma.hdma_config[i].adc16 == HPM_ADC0)
        {
            adc_descriptors1[i][0].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[0][i]);
        }
        else if (hpm_hdma.hdma_config[i].adc16 == HPM_ADC1)
        {
            adc_descriptors1[i][0].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[1][i]);
        }
#ifdef HPM_ADC2
        else if (hpm_hdma.hdma_config[i].adc16 == HPM_ADC2)
        {
            adc_descriptors1[i][0].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[2][i]);
        }
#endif
        else
        {
            return -2;
        }

        adc_descriptors1[i][0].dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_buff[i][0]);
        hdma_adc.adc_pack[i].adc_buff_reserve[0] = sys_address_to_core_local_mem(HPM_CORE0, adc_descriptors1[i][0].dst_addr);
        adc_descriptors1[i][0].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_HALF_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_HALF_WORD)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_SRCMODE_SET(DMA_HANDSHAKE_MODE_HANDSHAKE)
            | DMA_CHCTRL_CTRL_DSTMODE_SET(DMA_HANDSHAKE_MODE_NORMAL)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_INCREMENT)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_INTERRUPT_MASK_NONE;
        adc_descriptors1[i][0].linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[i][1]);

        //next dma ctrl
        dma_ctrl_buff[i][0][0] = DMA_CHCTRL_CTRL_SRCBUSINFIDX_SET(0)
            | DMA_CHCTRL_CTRL_DSTBUSINFIDX_SET(0)
            | DMA_CHCTRL_CTRL_PRIORITY_SET(0)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_HALF_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_HALF_WORD)
            | DMA_CHCTRL_CTRL_SRCMODE_SET(DMA_HANDSHAKE_MODE_HANDSHAKE)
            | DMA_CHCTRL_CTRL_DSTMODE_SET(DMA_HANDSHAKE_MODE_NORMAL)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_INCREMENT)
            | DMA_CHCTRL_CTRL_SRCREQSEL_SET(hpm_hdma.hdma_config[i].hdma_ch)
            | DMA_CHCTRL_CTRL_DSTREQSEL_SET(hpm_hdma.hdma_config[i].hdma_ch)
            | DMA_CHCTRL_CTRL_ENABLE_MASK
            | DMA_INTERRUPT_MASK_NONE;

        dma_ctrl_buff[i][0][1] = DMA_CHCTRL_TRANSIZE_TRANSIZE_SET((size*sizeof(uint16_t)) >> DMA_TRANSFER_WIDTH_HALF_WORD);
        dma_ctrl_buff[i][0][2] = DMA_CHCTRL_SRCADDR_SRCADDRL_SET(adc_descriptors1[i][0].src_addr);
        dma_ctrl_buff[i][0][3] = 0;
        dma_ctrl_buff[i][0][4] = DMA_CHCTRL_DSTADDR_DSTADDRL_SET(core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_buff[i][HDMA_ADC_BUF_MAXSIZE]));
        dma_ctrl_buff[i][0][5] = 0;
        dma_ctrl_buff[i][0][6] = DMA_CHCTRL_LLPOINTER_LLPOINTERL_SET((core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors2[i][1])) >> 3);
        dma_ctrl_buff[i][0][7] = 0;

        adc_descriptors1[i][1].trans_size = DMA_CHCTRL_TRANSIZE_TRANSIZE_SET((8*sizeof(uint32_t)) >> DMA_TRANSFER_WIDTH_WORD);
        adc_descriptors1[i][1].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_ctrl_buff[i][0][7]);
        adc_descriptors1[i][1].dst_addr = (uint32_t)&HPM_HDMA->CHCTRL[hpm_hdma.hdma_config[i].hdma_ch + hpm_hdma.count].LLPOINTERH;
        adc_descriptors1[i][1].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCMODE_SET(DMA_HANDSHAKE_MODE_NORMAL)
            | DMA_CHCTRL_CTRL_DSTMODE_SET(DMA_HANDSHAKE_MODE_NORMAL)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_DECREMENT)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_DECREMENT)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_8T);
        adc_descriptors1[i][1].linked_ptr = 0;

        //descriptors2
        adc_descriptors2[i][0].trans_size = DMA_CHCTRL_TRANSIZE_TRANSIZE_SET((size*sizeof(uint16_t)) >> DMA_TRANSFER_WIDTH_HALF_WORD);
        if (hpm_hdma.hdma_config[i].adc16 == HPM_ADC0)
        {
            adc_descriptors2[i][0].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[0][i]);
        }
        else if (hpm_hdma.hdma_config[i].adc16 == HPM_ADC1)
        {
            adc_descriptors2[i][0].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[1][i]);
        }
#ifdef HPM_ADC2
        else if (hpm_hdma.hdma_config[i].adc16 == HPM_ADC2)
        {
            adc_descriptors2[i][0].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_buff[2][i]);
        }
#endif
        else
        {
            return -2;
        }
        adc_descriptors2[i][0].dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_buff[i][HDMA_ADC_BUF_MAXSIZE]);
        hdma_adc.adc_pack[i].adc_buff_reserve[1] = sys_address_to_core_local_mem(HPM_CORE0, adc_descriptors2[i][0].dst_addr);
        adc_descriptors2[i][0].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_HALF_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_HALF_WORD)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_SRCMODE_SET(DMA_HANDSHAKE_MODE_HANDSHAKE)
            | DMA_CHCTRL_CTRL_DSTMODE_SET(DMA_HANDSHAKE_MODE_NORMAL)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_INCREMENT)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_INTERRUPT_MASK_NONE;
        adc_descriptors2[i][0].linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors2[i][1]);

        //next dma ctrl
        dma_ctrl_buff[i][1][0] = DMA_CHCTRL_CTRL_SRCBUSINFIDX_SET(0)
            | DMA_CHCTRL_CTRL_DSTBUSINFIDX_SET(0)
            | DMA_CHCTRL_CTRL_PRIORITY_SET(0)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_1T)
            | DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_HALF_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_HALF_WORD)
            | DMA_CHCTRL_CTRL_SRCMODE_SET(DMA_HANDSHAKE_MODE_HANDSHAKE)
            | DMA_CHCTRL_CTRL_DSTMODE_SET(DMA_HANDSHAKE_MODE_NORMAL)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_FIXED)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_INCREMENT)
            | DMA_CHCTRL_CTRL_SRCREQSEL_SET(hpm_hdma.hdma_config[i].hdma_ch)
            | DMA_CHCTRL_CTRL_DSTREQSEL_SET(hpm_hdma.hdma_config[i].hdma_ch)
            | DMA_CHCTRL_CTRL_ENABLE_MASK
            | DMA_INTERRUPT_MASK_NONE;

        dma_ctrl_buff[i][1][1] = DMA_CHCTRL_TRANSIZE_TRANSIZE_SET((size*sizeof(uint16_t)) >> DMA_TRANSFER_WIDTH_HALF_WORD);
        dma_ctrl_buff[i][1][2] = DMA_CHCTRL_SRCADDR_SRCADDRL_SET(adc_descriptors2[i][0].src_addr);
        dma_ctrl_buff[i][1][3] = 0;
        dma_ctrl_buff[i][1][4] = DMA_CHCTRL_DSTADDR_DSTADDRL_SET(core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_buff[i][0]));
        dma_ctrl_buff[i][1][5] = 0;
        dma_ctrl_buff[i][1][6] = DMA_CHCTRL_LLPOINTER_LLPOINTERL_SET((core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[i][1])) >> 3);
        dma_ctrl_buff[i][1][7] = 0;

        adc_descriptors2[i][1].trans_size = DMA_CHCTRL_TRANSIZE_TRANSIZE_SET((8*sizeof(uint32_t)) >> DMA_TRANSFER_WIDTH_WORD);
        adc_descriptors2[i][1].src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_ctrl_buff[i][1][7]);
        adc_descriptors2[i][1].dst_addr = (uint32_t)&HPM_HDMA->CHCTRL[hpm_hdma.hdma_config[i].hdma_ch].LLPOINTERH;
        adc_descriptors2[i][1].ctrl = DMA_CHCTRL_CTRL_SRCWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_DSTWIDTH_SET(DMA_TRANSFER_WIDTH_WORD)
            | DMA_CHCTRL_CTRL_SRCMODE_SET(DMA_HANDSHAKE_MODE_NORMAL)
            | DMA_CHCTRL_CTRL_DSTMODE_SET(DMA_HANDSHAKE_MODE_NORMAL)
            | DMA_CHCTRL_CTRL_SRCADDRCTRL_SET(DMA_ADDRESS_CONTROL_DECREMENT)
            | DMA_CHCTRL_CTRL_DSTADDRCTRL_SET(DMA_ADDRESS_CONTROL_DECREMENT)
            | DMA_CHCTRL_CTRL_SRCBURSTSIZE_SET(DMA_NUM_TRANSFER_PER_BURST_8T);
        adc_descriptors2[i][1].linked_ptr = 0;
    }

    for (i = 0; i < hpm_hdma.count; i++)
    {
        hpm_hdma.hdma_config[i].size = size;
        
        dma_default_channel_config(HPM_HDMA, &ch_config);
        ch_config.src_addr = adc_descriptors1[i][0].src_addr;
        ch_config.dst_addr = adc_descriptors1[i][0].dst_addr;
        ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
        ch_config.src_width = DMA_TRANSFER_WIDTH_HALF_WORD;
        ch_config.dst_width = DMA_TRANSFER_WIDTH_HALF_WORD;
        ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
        ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
        ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
        ch_config.size_in_byte = size * sizeof(uint16_t);
        ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[i][1]);

        //HPM_XDMA->INTSTATUS = (DMA_INTSTATUS_TC_SET(1) | DMA_INTSTATUS_ABORT_SET(1) | DMA_INTSTATUS_ERROR_SET(1)) << hpm_hdma.hdma_config[i].hdma_ch;
        //warning! fixed addr no support burst!
        if (status_success != dma_setup_channel(HPM_HDMA, hpm_hdma.hdma_config[i].hdma_ch, &ch_config, true))
        {
            printf(" dma setup channel failed 0\n");
            return -3;
        }
        dmamux_enable_channel(HPM_DMAMUX, hpm_hdma.hdma_config[i].mux_ch);
    }
    return 0;
#endif
}
void hpm_mp_api_adc_io_init(uint8_t adc_index)
{
    uint8_t map_index = mp_adc_get_mapindex(adc_index);
    HPM_IOC->PAD[MP_ADC_MAP_CTX[map_index].io].FUNC_CTL = IOC_PAD_FUNC_CTL_ANALOG_MASK;
}

int hpm_mp_api_adc_set_map(const mp_adc_map_t* map_t, int count)
{
    if (map_t == NULL)
        return -1;
    if (MP_ADC_MAP_CTX != NULL)
        return -2;

    MP_ADC_MAP_CTX = map_t;
    MP_ADC_MAP_COUNT = count;

    return 0;
}