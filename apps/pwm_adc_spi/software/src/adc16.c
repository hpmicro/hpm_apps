/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_adc16_drv.h"
#include "hpm_pwm_drv.h"
#include "adc16.h"

#ifndef ADC_SOC_PMT_NO_TRIGSOURCE
#include "hpm_trgm_drv.h"
#include "hpm_trgmmux_src.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_dma_drv.h"
#include "hpm_spi_drv.h"
#include "hpm_misc.h"

/* spi section */
#define APP_SPI_BASE              HPM_SPI3
#define APP_SPI_CLK_NAME          clock_spi3
#define APP_SPI_IRQ               IRQn_SPI3
#define APP_SPI_SCLK_FREQ         (20000000UL)
#define BOARD_APP_SPI_ADDR_LEN_IN_BYTES (1U)
#define BOARD_APP_SPI_DATA_LEN_IN_BITS  (8U)
#define APP_SPI_RX_DMA HPM_DMA_SRC_SPI3_RX
#define APP_SPI_TX_DMA HPM_DMA_SRC_SPI3_TX
#define SPI_CS_GPIO_CTRL           HPM_GPIO0
#define SPI_CS_PIN                 IOC_PAD_PA00
#define SPI_CS_ACTIVE_LEVEL        (0U)


#define APP_ADC16_CORE HPM_CORE0

#define APP_ADC16_SEQ_START_POS (0U)
#define APP_ADC16_SEQ_IRQ_EVENT adc16_event_seq_single_complete
#define APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES (1024U)

#ifndef ADC_SOC_PMT_NO_TRIGSOURCE
#define BOARD_APP_ADC16_PMT_PWM HPM_PWM0
#define BOARD_APP_ADC16_PMT_TRGM HPM_TRGM0
#define BOARD_APP_ADC16_PMT_TRGM_IN HPM_TRGM0_INPUT_SRC_PWM0_CH8REF
#define BOARD_APP_ADC16_PMT_TRGM_OUT TRGM_TRGOCFG_ADCX_PTRGI0A
#endif

#ifndef ADC_SOC_PMT_NO_TRIGSOURCE
#define APP_ADC16_PMT_PWM_REFCH_A (8U)
#define APP_ADC16_PMT_PWM BOARD_APP_ADC16_PMT_PWM
#define APP_ADC16_PMT_TRGM BOARD_APP_ADC16_PMT_TRGM
#define APP_ADC16_PMT_TRGM_IN BOARD_APP_ADC16_PMT_TRGM_IN
#define APP_ADC16_PMT_TRGM_OUT BOARD_APP_ADC16_PMT_TRGM_OUT
#endif

#define APP_ADC16_PMT_TRIG_CH BOARD_APP_ADC16_PMT_TRIG_CH
#define APP_ADC16_PMT_IRQ_EVENT adc16_event_trig_complete
#define APP_ADC16_PMT_DMA_BUFF_LEN_IN_4BYTES ADC_SOC_PMT_MAX_DMA_BUFF_LEN_IN_4BYTES

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT)
uint32_t seq_buff[APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT)
uint32_t pmt_buff[APP_ADC16_PMT_DMA_BUFF_LEN_IN_4BYTES];

uint8_t seq_adc_channel[] = {BOARD_APP_ADC16_CH_1};
uint8_t trig_adc_channel[] = {BOARD_APP_ADC16_CH_1};
__IO uint8_t seq_full_complete_flag;
__IO uint8_t trig_complete_flag;

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT)
uint8_t sendbuff[2];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT)
uint8_t spi_buf[4];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT)
uint8_t test_buff3[2];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT)
uint8_t test_buff4[2];

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8)
dma_linked_descriptor_t adc_descriptors1[6];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8)
static uint32_t spi_transctrl[5];


void board_init_spi_pins_test(SPI_Type *ptr)
{
    if (ptr == HPM_SPI1) {
        HPM_IOC->PAD[IOC_PAD_PB02].FUNC_CTL = IOC_PB02_FUNC_CTL_SPI1_CSN;
        HPM_IOC->PAD[IOC_PAD_PB05].FUNC_CTL = IOC_PB05_FUNC_CTL_SPI1_MOSI;
        HPM_IOC->PAD[IOC_PAD_PB03].FUNC_CTL = IOC_PB03_FUNC_CTL_SPI1_MISO;
        HPM_IOC->PAD[IOC_PAD_PB04].FUNC_CTL = IOC_PB04_FUNC_CTL_SPI1_SCLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
    }

        if (ptr == HPM_SPI3) {
        HPM_IOC->PAD[IOC_PAD_PA00].FUNC_CTL = IOC_PA00_FUNC_CTL_SPI3_CSN;
        HPM_IOC->PAD[IOC_PAD_PA03].FUNC_CTL = IOC_PA03_FUNC_CTL_SPI3_MOSI;
        HPM_IOC->PAD[IOC_PAD_PA01].FUNC_CTL = IOC_PA01_FUNC_CTL_SPI3_MISO;
        HPM_IOC->PAD[IOC_PAD_PA02].FUNC_CTL = IOC_PA02_FUNC_CTL_SPI3_SCLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
    }
}


static uint8_t get_adc_conv_mode(void)
{
    uint8_t ch;

    while (1)
    {
        printf("1. Oneshot    mode\n");
        printf("2. Period     mode\n");
        printf("3. Sequence   mode\n");
        printf("4. Preemption mode\n");

        printf("Please enter one of ADC conversion modes above (e.g. 1 or 2 ...): ");
        printf("%c\n", ch = getchar());
        ch -= '0' + 1;
        if (ch > adc16_conv_mode_preemption)
        {
            printf("The ADC mode is not supported!\n");
        }
        else
        {
            return ch;
        }
    }
}

void isr_adc16(void)
{
    uint32_t status;

    status = adc16_get_status_flags(BOARD_APP_ADC16_BASE);

    if (ADC16_INT_STS_TRIG_CMPT_GET(status))
    {
        /* Clear trig_cmpt status */
        adc16_clear_status_flags(BOARD_APP_ADC16_BASE, adc16_event_trig_complete);
        /* Set flag to read memory data */
        trig_complete_flag = 1;

        // use spi buf todo.
        // spi_buf[x]
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_ADC16_IRQn, isr_adc16)

hpm_stat_t process_pmt_data(uint32_t *buff, int32_t start_pos, uint32_t len)
{
    adc16_pmt_dma_data_t *dma_data = (adc16_pmt_dma_data_t *)buff;

    if (ADC16_IS_PMT_DMA_BUFF_LEN_INVLAID(len))
    {
        return status_invalid_argument;
    }

    for (int i = start_pos; i < start_pos + len; i++)
    {
        if (dma_data[i].cycle_bit)
        {
            printf("Preemption Mode - %s - ", BOARD_APP_ADC16_NAME);
            printf("Trigger Channel: %02d - ", dma_data[i].trig_ch);
            printf("Cycle Bit: %02d - ", dma_data[i].cycle_bit);
            printf("Sequence Number: %02d - ", dma_data[i].seq_num);
            printf("ADC Channel: %02d - ", dma_data[i].adc_ch);
            printf("Result: 0x%04x\n", dma_data[i].result);
            dma_data[i].cycle_bit = 0;
        }
        else
        {
            printf("invalid data\n");
        }
    }

    return status_success;
}

#define PWM_FREQUENCY (20000)
#define PWM_RELOAD (clock_get_frequency(clock_mot0) / PWM_FREQUENCY - 1) /*20K hz  = 200 000 000/PWM_RELOAD */

#ifndef ADC_SOC_PMT_NO_TRIGSOURCE
void init_trigger_source(PWM_Type *ptr)
{
    pwm_cmp_config_t pwm_cmp_cfg;
    pwm_cmp_config_t pwm_cmp_cfg_spi[2];
    pwm_output_channel_t pwm_output_ch_cfg;

    /* TODO: Set PWM Clock Source and divider */

    pwm_set_reload(ptr, 0, PWM_RELOAD);

    /* Set a comparator */
    memset(&pwm_cmp_cfg, 0x00, sizeof(pwm_cmp_config_t));
    pwm_cmp_cfg.enable_ex_cmp = false;
    pwm_cmp_cfg.mode = pwm_cmp_mode_output_compare;
    pwm_cmp_cfg.update_trigger = pwm_shadow_register_update_on_shlk;

    /* Select comp8 and trigger at the middle of a pwm cycle */
    pwm_cmp_cfg.cmp = 2999;
    pwm_config_cmp(ptr, APP_ADC16_PMT_PWM_REFCH_A, &pwm_cmp_cfg);

    pwm_cmp_cfg_spi[0].enable_ex_cmp = false;
    pwm_cmp_cfg_spi[0].mode = pwm_cmp_mode_output_compare;
    pwm_cmp_cfg_spi[0].update_trigger = pwm_shadow_register_update_on_shlk;
    pwm_cmp_cfg_spi[0].cmp = 4999;
    pwm_config_cmp(ptr, APP_ADC16_PMT_PWM_REFCH_A + 1, &pwm_cmp_cfg_spi[0]);

    pwm_enable_dma_request(HPM_PWM0, PWM_IRQ_CMP(9));

    /* Issue a shadow lock */
    pwm_issue_shadow_register_lock_event(APP_ADC16_PMT_PWM);

    /* Set comparator channel to generate a trigger signal */
    pwm_output_ch_cfg.cmp_start_index = APP_ADC16_PMT_PWM_REFCH_A; /* start channel */
    pwm_output_ch_cfg.cmp_end_index = APP_ADC16_PMT_PWM_REFCH_A;   /* end channel */
    pwm_output_ch_cfg.invert_output = false;
    pwm_config_output_channel(ptr, APP_ADC16_PMT_PWM_REFCH_A, &pwm_output_ch_cfg);

    /* Start the comparator counter */
    pwm_start_counter(ptr);
}

void init_trigger_mux(TRGM_Type *ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type = trgm_output_same_as_input;

    trgm_output_cfg.input = APP_ADC16_PMT_TRGM_IN;
    trgm_output_config(ptr, APP_ADC16_PMT_TRGM_OUT, &trgm_output_cfg);

    trgm_dma_request_config(ptr, TRGM_DMACFG_0, HPM_TRGM0_DMA_SRC_PWM0_CMP9);
}
#endif

void hdma_spi_tx_chain_config(void)
{
    dma_channel_config_t dma_ch_config;
    static uint8_t dummy_cmd = 0xff;
    static uint8_t dummy_buff1 = 0xff, dummy_buff2 = 0xff;

    dma_default_channel_config(HPM_HDMA, &dma_ch_config);

    /* SPI CTRL */
    dma_ch_config.size_in_byte = 4;
    dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&spi_transctrl[0]);
    dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&APP_SPI_BASE->TRANSCTRL);
    dma_ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[1]);
    dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[0], 0, &dma_ch_config);

    /* SPI CMD */
    dma_ch_config.size_in_byte = 1;
    dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dummy_cmd);
    dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&APP_SPI_BASE->CMD);
    dma_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[2]);
    dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[1], 0, &dma_ch_config);

    dma_ch_config.size_in_byte = sizeof(sendbuff);
    dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&sendbuff);
    dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&APP_SPI_BASE->DATA);
    dma_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[3]);
    dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[2], 0, &dma_ch_config);

    // dummy
    dma_ch_config.size_in_byte = 64;
    dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dummy_buff1);
    dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dummy_buff2);
    dma_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[4]);
    dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[3], 0, &dma_ch_config);

    // read
    dma_ch_config.size_in_byte = sizeof(spi_buf);
    dma_ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&APP_SPI_BASE->DATA);
    dma_ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&spi_buf);
    dma_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[0]);
    dma_config_linked_descriptor(HPM_HDMA, &adc_descriptors1[4], 0, &dma_ch_config);
}

void hdma_spi_config(void)
{
    int i;
    hpm_stat_t stat;
    dma_channel_config_t ch_config = {0};
    static uint32_t dummy_data1 = 0xff, dummy_data2 = 0xff;

    hdma_spi_tx_chain_config();

    dma_reset(HPM_HDMA);

    dma_default_channel_config(HPM_HDMA, &ch_config);

    // TX
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dummy_data1);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dummy_data2);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.size_in_byte = 4;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&adc_descriptors1[0]);

    // warning! fixed addr no support burst!
    if (status_success != dma_setup_channel(HPM_HDMA, 0, &ch_config, true))
    {
        printf(" dma setup channel failed 0\n");
        return;
    }

    dmamux_config(HPM_DMAMUX, DMAMUX_MUXCFG_HDMA_MUX0, HPM_DMA_SRC_MOT0_0, true);
}

void prepare_spi_sent_data(void)
{
    sendbuff[0] = 0xA0;
    sendbuff[1] = 0x03;
}

spi_control_config_t control_config = {0};

void spi_init(void)
{
    spi_timing_config_t timing_config = {0};
    spi_format_config_t format_config = {0};
    // spi_control_config_t control_config = {0};

    hpm_stat_t stat;
    uint32_t spi_clock;
    uint32_t spi_tx_trans_count, spi_rx_trans_count;

    prepare_spi_sent_data();

    spi_clock = board_init_spi_clock(APP_SPI_BASE);
    board_init_spi_pins_test(APP_SPI_BASE);
    printf("SPI Master DMA Transfer Example\n");

    /* set SPI sclk frequency for master */
    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.cs2sclk = spi_cs2sclk_half_sclk_4;
    timing_config.master_config.clk_src_freq_in_hz = spi_clock;
    timing_config.master_config.sclk_freq_in_hz = BOARD_APP_SPI_SCLK_FREQ;
    if (status_success != spi_master_timing_init(APP_SPI_BASE, &timing_config))
    {
        printf("SPI master timming init failed\n");
    }

    /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.master_config.addr_len_in_bytes = 2U;
    format_config.common_config.data_len_in_bits = 8;
    format_config.common_config.data_merge = false;
    format_config.common_config.mosi_bidir = false;
    format_config.common_config.lsb = false;
    format_config.common_config.mode = spi_master_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(APP_SPI_BASE, &format_config);

    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;
    control_config.master_config.addr_enable = false;
    control_config.master_config.addr_phase_fmt = spi_address_phase_format_single_io_mode;
    control_config.common_config.trans_mode = spi_trans_write_dummy_read;
    control_config.common_config.data_phase_fmt = spi_single_io_mode;
    control_config.common_config.dummy_cnt = spi_dummy_count_1;

    spi_transfer(APP_SPI_BASE,
                 &control_config,
                 NULL, NULL,
                 sendbuff, ARRAY_SIZE(sendbuff), (uint8_t *)spi_buf, ARRAY_SIZE(spi_buf));
    printf("recv:");
    for (int i = 0; i < ARRAY_SIZE(spi_buf); i++)
    {
        printf(" %02x", spi_buf[i]);
    }
    printf("\r\n");
    spi_transctrl[0] = APP_SPI_BASE->TRANSCTRL;
}

void init_trigger_target(ADC16_Type *ptr, uint8_t trig_ch, bool inten)
{
    adc16_pmt_config_t pmt_cfg;

    pmt_cfg.trig_len = sizeof(trig_adc_channel);
    pmt_cfg.trig_ch = trig_ch;

    for (int i = 0; i < pmt_cfg.trig_len; i++)
    {
        pmt_cfg.adc_ch[i] = trig_adc_channel[i];
        pmt_cfg.inten[i] = false;
    }

    pmt_cfg.inten[pmt_cfg.trig_len - 1] = inten;

    adc16_set_pmt_config(ptr, &pmt_cfg);
    adc16_set_pmt_queue_enable(ptr, trig_ch, true);
}

hpm_stat_t init_common_config(adc16_conversion_mode_t conv_mode)
{
    adc16_config_t cfg;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);
    cfg.res = adc16_res_16_bits;
    cfg.conv_mode = conv_mode;
    cfg.adc_clk_div = adc16_clock_divider_4;
    cfg.sel_sync_ahb = (clk_adc_src_ahb0 == clock_get_source(BOARD_APP_ADC16_CLK_NAME)) ? true : false;

    if (cfg.conv_mode == adc16_conv_mode_sequence ||
        cfg.conv_mode == adc16_conv_mode_preemption)
    {
        cfg.adc_ahb_en = true;
    }

    /* adc16 initialization */
    if (adc16_init(BOARD_APP_ADC16_BASE, &cfg) == status_success)
    {
        /* enable irq */
        intc_m_enable_irq_with_priority(BOARD_APP_ADC16_IRQn, 1);
        return status_success;
    }
    else
    {
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
    ch_cfg.sample_cycle = 20;

    for (int i = 0; i < sizeof(trig_adc_channel); i++)
    {
        ch_cfg.ch = trig_adc_channel[i];
        adc16_init_channel(BOARD_APP_ADC16_BASE, &ch_cfg);
    }

#ifndef ADC_SOC_PMT_NO_TRIGSOURCE
    /* Trigger source initialization */
    init_trigger_source(APP_ADC16_PMT_PWM);

    /* Trigger mux initialization */
    init_trigger_mux(APP_ADC16_PMT_TRGM);
#endif

    /* Trigger target initialization */
    init_trigger_target(BOARD_APP_ADC16_BASE, APP_ADC16_PMT_TRIG_CH, true);

    /* Set DMA start address for preemption mode */
    adc16_init_pmt_dma(BOARD_APP_ADC16_BASE, core_local_mem_to_sys_address(APP_ADC16_CORE, (uint32_t)pmt_buff));

    /* Enable trigger complete interrupt */
    adc16_enable_interrupts(BOARD_APP_ADC16_BASE, APP_ADC16_PMT_IRQ_EVENT);
}

void preemption_handler(void)
{
#if defined(ADC_SOC_PMT_NO_TRIGSOURCE) && ADC_SOC_PMT_NO_TRIGSOURCE
    /* SW trigger */
    adc16_trigger_pmt_by_sw(BOARD_APP_ADC16_BASE, APP_ADC16_PMT_TRIG_CH);
#endif

    /* Wait for a complete of conversion */
    while (trig_complete_flag == 0)
    {
    }

    /* Process data */
    process_pmt_data(pmt_buff, APP_ADC16_PMT_TRIG_CH * sizeof(adc16_pmt_dma_data_t), sizeof(trig_adc_channel));

    /* Clear the flag */
    trig_complete_flag = 0;
}

int main(void)
{
    uint8_t conv_mode;

    /* Bsp initialization */
    board_init();

    /* ADC pin initialization */
    board_init_adc16_pins();

    /* ADC clock initialization */
    board_init_adc_clock(BOARD_APP_ADC16_BASE, true);

    printf("This is an ADC16 demo:\n");

    /* ADC16 common initialization */
    init_common_config(adc16_conv_mode_preemption);

    /* ADC16 read patter and DMA initialization */
    init_preemption_config();

    spi_init();
    hdma_spi_config();

    /* Main loop */
    while (1)
    {
        board_delay_ms(200);

        preemption_handler();

        printf("recv:");
        for (int i = 0; i < ARRAY_SIZE(spi_buf); i++)
        {
            printf(" %02x", spi_buf[i]);
        }
        printf("\r\n");
    }
}
