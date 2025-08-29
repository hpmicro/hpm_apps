/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_spi_drv.h"
#include "hpm_clock_drv.h"
#include "spi_init.h"
#include "uart_init.h"
#include "rdc.h"

#define TEST_SPI           HPM_SPI2
#define TEST_SPI_IRQ       IRQn_SPI2

volatile bool spi_transfer_done;

uint8_t *sent_buff;
uint8_t *receive_buff;
uint32_t sent_count;
uint32_t receive_count;
int8_t pt_send[5] ;

void  spi_data_package(void)
{
    memset(&pt_send,0,sizeof(pt_send)) ;
    uint16_t len =  0;
    uint16_t theta_rdc = rdcObj.rdc_theta*100;
    int16_t pll_vel = rdcObj.pll_vel*100;
    
    pt_send[0]= (int8_t)(pll_vel & 0x00ff);
    pt_send[1]= (int8_t)((pll_vel & 0xff00) >> 8);
    pt_send[2]= (uint8_t)(theta_rdc & 0x00ff);
    pt_send[3]= (uint8_t)((theta_rdc & 0xff00) >> 8);
    pt_send[4]= pt_send[0]^pt_send[1]^pt_send[2]^pt_send[3];
}
void spi_isr(void)
{
    volatile uint32_t irq_status;
    volatile uint8_t data_len_in_bytes;
    hpm_stat_t stat;

    data_len_in_bytes = spi_get_data_length_in_bytes(TEST_SPI);
    irq_status = spi_get_interrupt_status(TEST_SPI); /* get interrupt stat */

    if (irq_status & spi_slave_cmd_int) {
        spi_data_package();
        spi_disable_interrupt(TEST_SPI, spi_slave_cmd_int);
        spi_enable_interrupt(TEST_SPI, spi_tx_fifo_threshold_int | spi_rx_fifo_threshold_int | spi_end_int);
    }
    if (irq_status & spi_end_int) {
        spi_transfer_done = true;

        spi_disable_interrupt(TEST_SPI, spi_end_int);
        spi_clear_interrupt_status(TEST_SPI, spi_end_int);
    }
    if (irq_status & spi_rx_fifo_threshold_int) {
        stat = spi_read_data(TEST_SPI, data_len_in_bytes, receive_buff, 1);
        if (stat != status_success) {
            printf("spi read data filed, error: %d\n", stat);
        }
        spi_clear_interrupt_status(TEST_SPI, spi_rx_fifo_threshold_int);

        receive_buff += data_len_in_bytes;
        receive_count--;
        if (receive_count == 0) {
            spi_disable_interrupt(TEST_SPI, spi_rx_fifo_threshold_int);
        }
    }
    if (irq_status & spi_tx_fifo_threshold_int)
     {
        stat = spi_write_data(TEST_SPI, data_len_in_bytes, sent_buff, 1);
        if (stat != status_success) {
            printf("spi write data filed, error: %d\n", stat);
        }
        spi_clear_interrupt_status(TEST_SPI, spi_tx_fifo_threshold_int);

        sent_buff += data_len_in_bytes;
        sent_count--;
        if (sent_count == 0) {
            spi_disable_interrupt(TEST_SPI, spi_tx_fifo_threshold_int);
        }
    }
}
SDK_DECLARE_EXT_ISR_M(TEST_SPI_IRQ, spi_isr)
void spi_pins_init(void)
{
   
      HPM_IOC->PAD[IOC_PAD_PA23].FUNC_CTL = IOC_PA23_FUNC_CTL_SPI2_MOSI;
      HPM_IOC->PAD[IOC_PAD_PA20].FUNC_CTL = IOC_PA20_FUNC_CTL_SPI2_CS_0;
      HPM_IOC->PAD[IOC_PAD_PA21].FUNC_CTL = IOC_PA21_FUNC_CTL_SPI2_SCLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
      HPM_IOC->PAD[IOC_PAD_PA22].FUNC_CTL = IOC_PA22_FUNC_CTL_SPI2_MISO;
     
    
}
uint32_t init_spi_clock(SPI_Type *ptr)
{
    if (ptr == HPM_SPI2) {
        clock_add_to_group(clock_spi2, 0);
        return clock_get_frequency(clock_spi2);
    }
    return 0;
}

uint8_t wbuff[5] = {0xb0, 0xb1, 0xb2, 0xb3, 0xb4};
uint8_t rbuff[5] = {0};
void spi_trans_data(void)
{
        sent_buff = (uint8_t *)pt_send;
        receive_buff = (uint8_t *)rbuff;
        sent_count = ARRAY_SIZE(pt_send);
        receive_count = ARRAY_SIZE(rbuff);
        
        spi_enable_interrupt(TEST_SPI, spi_slave_cmd_int);
        while (!spi_transfer_done) {
            __asm("nop");
        }
        spi_transfer_done = 0;
        memset(&rbuff,0,sizeof(rbuff));
}
int spi_salve_interrupt_config(void)
{
    uint8_t cmd = 0;

    spi_format_config_t format_config = {0};
    spi_control_config_t control_config = {0};
    hpm_stat_t stat;

    spi_pins_init();
    init_spi_clock(TEST_SPI);
    intc_m_enable_irq_with_priority(TEST_SPI_IRQ, 1);

    spi_slave_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = 8U;
    format_config.common_config.mode = spi_slave_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(TEST_SPI, &format_config);

    /* set SPI control config for slave */
    spi_slave_get_default_control_config(&control_config);
    control_config.slave_config.slave_data_only = false;
    control_config.common_config.dummy_cnt = spi_dummy_count_1;
    control_config.common_config.trans_mode = spi_trans_read_dummy_write;

    stat = spi_control_init(TEST_SPI, &control_config, ARRAY_SIZE(wbuff), ARRAY_SIZE(rbuff));
    if (stat != status_success) {
        printf("SPI control init failed.\n");
        return stat;
    }
    spi_set_tx_fifo_threshold(TEST_SPI, SPI_SOC_FIFO_DEPTH - 1U);
    spi_set_rx_fifo_threshold(TEST_SPI, 1U);

    return 0;
}
