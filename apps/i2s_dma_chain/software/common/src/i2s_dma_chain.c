/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_wm8960.h"
#include "hpm_math.h"
#include "hpm_i2s_drv.h"
#include "hpm_dao_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "i2s_dma_chain.h"

/* DMA transfer tx variable and constant*/
ATTR_RAMFUNC_WITH_ALIGNMENT(8) dma_linked_descriptor_t i2s_descriptor_tx[4];
ATTR_PLACE_AT_NONCACHEABLE_BSS uint8_t dma_tx_done[2];

/*16k sample rate,1kHz sin wave*/
const int sin_16k_1k_dual[TX_DATA_LEN]={
0x0,0x0,
0x187de2a,0x187de2a,
0x2d413cc,0x2d413cc,
0x3b20d79,0x3b20d79,
0x3ffffff,0x3ffffff,
0x3b20d7a,0x3b20d7a,
0x2d413ce,0x2d413ce,
0x187de2d,0x187de2d,
0x3,0x3,
0xfe7821da,0xfe7821da,
0xfd2bec37,0xfd2bec37,
0xfc4df288,0xfc4df288,
0xfc000001,0xfc000001,
0xfc4df284,0xfc4df284,
0xfd2bec2f,0xfd2bec2f,
0xfe7821d0,0xfe7821d0,
};

/* DMA transfer rx variable and struct */
uint32_t result[4];
u32_t complete_count;
int mic_rx_test = 1; 
rfft_type_t rfft_buf_r[FFT_POINTS];
rfft_type_t rfft_buf_l[FFT_POINTS];
rfft_type_t rfft_mag_output[FFT_POINTS];
int test_buff[FFT_POINTS];

ATTR_RAMFUNC_WITH_ALIGNMENT(8) dma_linked_descriptor_t i2s_descriptor[4];
ATTR_PLACE_AT_NONCACHEABLE_BSS int rx_buff[FFT_POINTS * 4];
ATTR_PLACE_AT_NONCACHEABLE_BSS uint8_t rx_done[2];

wm8960_config_t wm8960_config = {
    .route       = wm8960_route_record,
    .left_input  = wm8960_input_closed,
    .right_input = wm8960_input_differential_mic_input2,
    .play_source = wm8960_play_source_dac,
    .bus         = wm8960_bus_left_justified,
    .format = {.mclk_hz = 0U, .sample_rate = CODEC_SAMPLE_RATE_HZ, .bit_width = CODEC_BIT_WIDTH},
};

wm8960_control_t wm8960_control = {
    .ptr = CODEC_I2C,
    .slave_address = WM8960_I2C_ADDR, /* I2C address */
};

/* DMA transfer tx fuction */
void i2s_dao_config(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num)
{
    uint32_t i2s_mclk_hz;
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    dao_config_t dao_config;

    i2s_get_default_config(I2S_DAO, &i2s_config);
    i2s_config.tx_fifo_threshold = 2;
    i2s_init(I2S_DAO, &i2s_config);

    /*
     * config transfer for DAO
     */
    i2s_get_default_transfer_config_for_dao(&transfer);
    transfer.sample_rate = sample_rate;
    transfer.audio_depth = audio_depth;
    transfer.channel_num_per_frame = channel_num;

    i2s_mclk_hz = clock_get_frequency(I2S_DAO_CLK_NAME);

    if (I2S_DAO_MCLK_FREQ_IN_HZ != i2s_mclk_hz) {
        printf("MCLK error for I2S_DAO!\n");
    }

    if (status_success != i2s_config_tx(I2S_DAO, i2s_mclk_hz, &transfer)) {
        printf("I2S config failed for I2S_DAO\n");
        while (1) {

        }
    }
   
    dao_get_default_config(HPM_DAO, &dao_config);
    dao_init(HPM_DAO, &dao_config);
    i2s_enable_tx_dma_request(I2S_DAO);
    i2s_start(I2S_DAO);
    dao_start(HPM_DAO);
}

void dma_chain_transfer_config_tx(void)
{
    dma_channel_config_t ch_config = {0};

    dma_default_channel_config(APP_DMA_TX, &ch_config);

    static uint8_t dma_tx_complete = 1;
    /* send the first half of sin_16k_1k_dual data to DAO */
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)(uint32_t*)sin_16k_1k_dual);
    ch_config.dst_addr = (uint32_t)(uint8_t *)&I2S_DAO->TXD[I2S_DAO_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = TX_DATA_LEN * sizeof(uint32_t) ;
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s_descriptor_tx[1]);
    dma_config_linked_descriptor(APP_DMA_TX, &i2s_descriptor_tx[0], DMA_CHANNEL, &ch_config);

    /* set dma_tx_complete[0] = 1,the first half of sin_16k_1k_dual data transfer completed */
    ch_config.src_addr =  core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_tx_complete);
  
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_tx_done[0]);
    ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = 1;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s_descriptor_tx[2]);
    dma_config_linked_descriptor(APP_DMA_TX, &i2s_descriptor_tx[1], DMA_CHANNEL, &ch_config);
    
    /* send the second half of sin_16k_1k_dual data to DAO */
    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)(uint32_t*)sin_16k_1k_dual);
    ch_config.dst_addr = (uint32_t)(uint8_t *)&I2S_DAO->TXD[I2S_DAO_DATA_LINE];
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = TX_DATA_LEN * sizeof(uint32_t);
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s_descriptor_tx[3]);
    dma_config_linked_descriptor(APP_DMA_TX, &i2s_descriptor_tx[2], DMA_CHANNEL, &ch_config);

    /* set dma_tx_complete[1] = 1,the second half of sin_16k_1k_dual data transfer completed */
    ch_config.src_addr =  core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_tx_complete);
  
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&dma_tx_done[1]);
    ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = 1;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s_descriptor_tx[0]);
    dma_config_linked_descriptor(APP_DMA_TX, &i2s_descriptor_tx[3], DMA_CHANNEL, &ch_config);
}

void dma_auto_config_tx(void)
{

    dma_channel_config_t ch_config = {0};
    static uint32_t dummy_data1 = 0xff, dummy_data2 = 0xff;
    dma_chain_transfer_config_tx();
    dma_default_channel_config(APP_DMA_TX, &ch_config);

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
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s_descriptor_tx[0]);

    /* warning! fixed addr no support burst! */
    if (status_success != dma_setup_channel(APP_DMA_TX, DMA_CHANNEL, &ch_config, true)) {
        printf("tx dma setup channel failed\n");
        return;
    }
    dmamux_config(BOARD_APP_DMAMUX, DMA_MUX_CHANNEL, DMA_MUX_SRC, true);
}
void i2s_dma_tx_init(void)
{
    board_init_dao_clock();
    init_dao_pins();
    i2s_dao_config(I2S_SAMPLE_RATE_HZ, I2S_SAMPLE_BITDEPTH, I2S_SAMPLE_CHANNEL);
    dma_auto_config_tx();
}
void tx_test(void)
{
    if (dma_tx_done[0] == 1){
      dma_tx_done[0] = 0;
    }
    if (dma_tx_done[1] == 1){
      dma_tx_done[1] = 0;
    }
    if (dma_tx_done[0] == 1 && dma_tx_done[1] == 1)
    {
      printf("dma_tx error!");
    }
}

/* DMA transfer rx fuction */
void fft_cal(rfft_type_t *buf, rfft_type_t *output, uint16_t points, uint16_t shift)
{
    hpm_dsp_rfft_f32(buf, shift);  
    hpm_dsp_cmag_f32(buf, output, points);
}

void dma_chain_transfer_config_rx(void)
{
    dma_channel_config_t ch_config = {0};
    dma_default_channel_config(APP_DMA_RX, &ch_config);

    static uint8_t rx_complete = 1;

    /* read tdm data to the first half of rx_buff. */
    ch_config.src_addr = (uint32_t)((uint8_t *)&CODEC_I2S->RXD[I2S_MASTER_RX_LINE]);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&rx_buff[0]);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.size_in_byte = FFT_POINTS_DUAL * sizeof(uint32_t);
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s_descriptor[1]);
    dma_config_linked_descriptor(APP_DMA_RX, &i2s_descriptor[0], DMA_RX_CHANNEL, &ch_config);

    /*set rx_done[0] = 1,the first half of rx_buff if filled with data*/
    ch_config.src_addr =  core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&rx_complete);
  
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&rx_done[0]);
    ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = 1;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s_descriptor[2]);
    dma_config_linked_descriptor(APP_DMA_RX, &i2s_descriptor[1], DMA_RX_CHANNEL, &ch_config);
    
     /*read tdm data to the second half of rx_buff*/
    ch_config.src_addr = (uint32_t)((uint8_t *)&CODEC_I2S->RXD[I2S_MASTER_RX_LINE]);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&rx_buff[FFT_POINTS_DUAL]);
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.size_in_byte = FFT_POINTS_DUAL * sizeof(uint32_t);
    ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s_descriptor[3]);
    dma_config_linked_descriptor(APP_DMA_RX, &i2s_descriptor[2], DMA_RX_CHANNEL, &ch_config);

    /*set rx_done[1] = 1,the second half of rx_buff if filled with data*/
    ch_config.src_addr =  core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&rx_complete);
    ch_config.dst_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&rx_done[1]);
    ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = 1;
    
    ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_2T;
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s_descriptor[0]);
    dma_config_linked_descriptor(APP_DMA_RX, &i2s_descriptor[3], DMA_RX_CHANNEL, &ch_config);
}
void dma_auto_config_rx(void)
{

    dma_channel_config_t ch_config = {0};
    static uint32_t dummy_data1 = 0xff, dummy_data2 = 0xff;
    dma_chain_transfer_config_rx();
    dma_default_channel_config(APP_DMA_RX, &ch_config);

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
    ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&i2s_descriptor[0]);

    /* warning! fixed addr no support burst!*/
    if (status_success != dma_setup_channel(APP_DMA_RX, DMA_RX_CHANNEL, &ch_config, true)) {
        printf("rx dma setup channel failed\n");
        return;
    }
    dmamux_config(BOARD_APP_DMAMUX, DMA_MUX_RX_CHANNEL, DMA_MUX_RX_SRC, true);
}

void codec_i2s_init(void)
{
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    uint32_t i2s_mclk_hz;

    /* Config I2S interface to CODEC */ 
    i2s_get_default_config(CODEC_I2S, &i2s_config);
    i2s_config.enable_mclk_out = true;
    i2s_init(CODEC_I2S, &i2s_config);

    i2s_get_default_transfer_config(&transfer);
    transfer.data_line = CODEC_I2S_DATA_LINE;
    transfer.sample_rate = CODEC_SAMPLE_RATE_HZ;
    transfer.master_mode = true;
    i2s_mclk_hz = clock_get_frequency(CODEC_I2S_CLK_NAME);
    /* configure I2S RX and TX */
    if (status_success != i2s_config_transfer(CODEC_I2S, i2s_mclk_hz, &transfer))
    {
        printf("I2S config failed for CODEC\n");
        while(1);
    }

    wm8960_config.format.mclk_hz = i2s_mclk_hz;
    if (wm8960_init(&wm8960_control, &wm8960_config) != status_success) {
        printf("Init Audio Codec failed\n");
    }
    i2s_enable_rx_dma_request(CODEC_I2S);
    i2s_start(CODEC_I2S);
}

void i2s_dma_rx_init(void)
{
    board_init_i2c(CODEC_I2C);
    init_i2s_pins(CODEC_I2S);
    dma_auto_config_rx();
    codec_i2s_init();
}
void rx_data_process(void)
{

    if (rx_done[0] == 1)
    {
     /* you can process the rx data here */
      complete_count++;
      rx_done[0] = 0;
      for (int i = 0;i < FFT_POINTS;i++)
      {
        /* left channel data */
        rfft_buf_l[i] = rx_buff[2 * i];
        /* right channel data */
        rfft_buf_r[i] = rx_buff[2 * i + 1];
      }
      fft_cal(&rfft_buf_r[0], &rfft_mag_output[0], FFT_POINTS, FFT_SHIFT);
      result[0] = (uint32_t)rfft_mag_output[FFT_RESULT_INDEX] >> FFT_RESULT_SHIFT;
      result[1] = (uint32_t)rfft_mag_output[FFT_RESULT_INDEX + 2] >> FFT_RESULT_SHIFT;
      result[2] = (uint32_t)rfft_mag_output[FFT_RESULT_INDEX + 4] >> FFT_RESULT_SHIFT;
      result[3] = (uint32_t)rfft_mag_output[FFT_RESULT_INDEX + 6] >> FFT_RESULT_SHIFT;
      /*FFT calculation result printing interval, print the result every 20 calculations, for testing purposes only.*/
      if (complete_count > TEST_DELAY_COUNT)
      {
        complete_count = 0;
        printf("fft result:%d,%d,%d,%d\n",result[0],result[1],result[2],result[3]);         
      }
    }
    if (rx_done[1] == 1)
    {
      /* you can process the rx data here */
      rx_done[1] = 0;
    }
    if (rx_done[0] == 1 && rx_done[0] == 1)
    {
      printf("rx_buff full!");
    }
}

