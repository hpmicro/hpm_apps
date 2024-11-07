/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef I2S_DMA_CHAIN_H
#define I2S_DMA_CHAIN_H

#include "board.h"


/**
 * @brief I2S TX DMA configuration
 * ;I2S发送数据的DMA相关配置
 */
#define APP_DMA_TX                BOARD_APP_HDMA
#define DMA_CHANNEL               (1U)
#define DMA_MUX_CHANNEL           DMA_SOC_CHN_TO_DMAMUX_CHN(APP_DMA_TX, DMA_CHANNEL)
#define DMA_MUX_SRC               HPM_DMA_SRC_I2S1_TX
#define TX_DATA_LEN               (32U)

/* I2S Master Transfer Format */
/**
 * @brief I2S TX I2S format
 * ;I2S发送数据格式
 */
#define I2S_SAMPLE_RATE_HZ        (16000U)
#define I2S_SAMPLE_CHANNEL        (2U)
#define I2S_SAMPLE_BITDEPTH       i2s_audio_depth_32_bits
#define I2S_CHANNEL_LENGTH        i2s_channel_length_32_bits

/* DAO */
#define I2S_DAO                   DAO_I2S
#define I2S_DAO_CLK_NAME          clock_i2s1
#define I2S_DAO_DATA_LINE         (0U)
#define I2S_DAO_MCLK_FREQ_IN_HZ   (24576000UL)

/**
 * @brief CODEC used for microphone I2S configuration
 * ;麦克风使用的CODEC的相关配置
 */
#define CODEC_I2C                 BOARD_APP_I2C_BASE
#define CODEC_I2S                 BOARD_APP_I2S_BASE
#define CODEC_I2S_CLK_NAME        BOARD_APP_I2S_CLK_NAME
#define CODEC_I2S_DATA_LINE       BOARD_APP_I2S_DATA_LINE
#define CODEC_BIT_WIDTH           (32U)
#define CODEC_SAMPLE_RATE_HZ      (16000U)
/**
 * @brief I2S RX DMA configuration
 * ;I2S接收数据的DMA相关配置
 */
#define APP_DMA_RX                BOARD_APP_HDMA
#define I2S_MASTER_RX_LINE        CODEC_I2S_DATA_LINE
#define DMA_RX_CHANNEL            (0U)
#define DMA_MUX_RX_CHANNEL        DMA_SOC_CHN_TO_DMAMUX_CHN(APP_DMA_RX, DMA_RX_CHANNEL)
#define DMA_MUX_RX_SRC            HPM_DMA_SRC_I2S0_RX

/**
 * @brief sampling rate of audio，this routine uses 16000Hz
 * ;音频数据的采样率，本例程使用的是16000Hz
 */
#define  SAMPLE_FREQ              CODEC_SAMPLE_RATE_HZ
/**
 * @brief the frequency of the sine wave played, in this routine is 1000Hz
 * ;播放的正弦波频率，本例程使用的是1000Hz
 */
#define  SINWAVE_FREQ             (1000u)
/**
 * @brief shift base 2 logarithm value of the sample number and it can be set from 4 to 14
 * ;以2为底的数据个数的对数值，可以设置为4到14
 */
#define  FFT_SHIFT                7
/**
 * @brief The number of FFT operation points, must be a power of 2 and within the range of 2^4 to 2^14
 * ;FFT运算的点数，必须是2的幂次方，且范围在2的4次方到2的14次方之间
 */
#define  FFT_POINTS               (1 << FFT_SHIFT)
/**
 * @brief The FFT operation result shifted,the FFT calculation results in larger values,reduced the result in size
 * ;FFT运算结果移位，FFT计算后数值较大，对结果进行缩小处理
 */
#define  FFT_RESULT_SHIFT         18
/**
 * @brief  double the number of FFT operation points, FFT operates on monaural data, but the received buff stores binaural data
 * ;FFT运算点数2倍，FFT运算的是单声道数据，接收buff中存放的是双声道数据
 */
#define  FFT_POINTS_DUAL          (FFT_POINTS * 2)
/**
 * @brief  FFT calculation result index, index position corresponding to 1kHz
 * ;FFT计算结果索引，1kHz对应的索引位置
 */
#define  FFT_RESULT_INDEX         (FFT_POINTS * SINWAVE_FREQ / SAMPLE_FREQ)
/**
 * @brief FFT calculation result printing interval, print the result every 20 calculations, for testing purposes only.
 * ;FFT计算结果打印间隔，每计算20次打印一次结果，仅供测试时使用
 */
#define TEST_DELAY_COUNT          20


typedef float rfft_type_t;

/* I2S TX fuction */

/**
 * @brief DAO I2S configuration
 * ;DAO I2S配置
 *
 * @param [in] sample_rate the sample rate of audio data;播放音频的采样率
 * @param [in] audio_depth the sample depth of audio data,the supports sample depth are as follows:
 * ;音频数据的通道宽度采样深度,支持的采样深度如下：
 * - i2s_audio_depth_16_bits: 16bits sample depth;16位采样深度
 * - i2s_audio_depth_24_bits: 24bits sample depth;24位采样深度
 * - i2s_audio_depth_32_bits: 32bits sample depth;32位采样深度
 * @param [in] channel_num the num of channels;通道数量
 *
 */
void i2s_dao_config(uint32_t sample_rate, uint8_t audio_depth, uint8_t channel_num);
/**
 * @brief I2S TX DMA chain configuration
 * ;I2S发送数据的DMA链配置
 */
void dma_chain_transfer_config_tx(void);
/**
 * @brief I2S TX DMA chain start
 * ;I2S发送数据的DMA链启动
 */
void dma_auto_config_tx(void);
/**
 * @brief I2S TX and DMA chain init
 * ;发送数据的I2S和DMA链初始化
 */
void i2s_dma_tx_init(void);


/* I2S RX fuction */

/**
 * @brief FFT calculation:perform FFT calculation on the received audio data
 * ;FFT计算：对接收到音频数据进行FFT运算
 *
 * @param [in] buf data buff that requires FFT calculation;需要进行FFT计算的数据buff
 * @param [in] output the sample depth of audio data,;
 * ;FFT计算结果的输出buff
 * @param [in] points the number of data requiring FFT calculation;需要进行FFT计算的数据个数
 * @param [in] shift base 2 logarithm value of the sample number and it can be set from 4 to 14
 * ;以2为底的数据个数的对数值，可以设置为4到14
 *
 */
void fft_cal(rfft_type_t *buf, rfft_type_t *output, uint16_t points, uint16_t shift);
/**
 * @brief I2S RX DMA chain configuration
 * ;I2S接收数据的DMA链配置
 */
void dma_chain_transfer_config_rx(void);
/**
 * @brief I2S RX DMA chain start
 * ;I2S接收数据的DMA链启动
 */
void dma_auto_config_rx(void);
/**
 * @brief CODEC used for microphone I2S init
 * ;麦克风使用的CODEC I2S初始化
 */
void codec_i2s_init(void);
/**
 * @brief I2S RX and DMA chain init
 * ;接收数据的I2S和DMA链初始化
 */
void i2s_dma_rx_init(void);
/**
 * @brief processing received data
 * ;处理接收的数据
 */
void rx_data_process(void);

#endif /* I2S_DMA_CHAIN_H */