/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef TCPECHO_H
#define TCPECHO_H

/**
 * @brief LWIP_ADC 接口
 * @addtogroup LWIP_ADC 接口
 * @{
 *
 */


#define TCP_ECHO_PORT (5001U)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief TCP initialization, binding IP address and port number.
 *        ;TCP初始化，绑定IP地址和端口号。
 */
void tcp_echo_init(void);
/**
 * @brief Sending data using polling.Firstly, determine the values of adc_done[0] and adc_done[0]. 
 * If both are 1, it indicates that the data was not sent in a timely manner.
 * If adc_done[0] is 1, set adc_done[0] to 0 and send the first half of the buff data;
 * If adc_done[1] is 1, set adc_done[1] to 0 and send the second half of the buff data;
 *        ;LWIP将ADC的采样结果通过polling的方式发送出去，首先判断
 * adc_done[0]和adc_done[1]的值，如果同为1，表示数据未能及时发送。
 * 若adc_done[0]为1，将adc_done[0]置0并发送adc_buff的前半部分数据；
 * 若adc_done[1]为1，将adc_done[1]置0并发送adc_buff的前后半部分数据
 */
void tcp_poll_handle(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/** @} */

#endif /* TCP_ECHO_H */

