/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <string.h>
#include "tcp_echo.h"
#include "lwip/tcp.h"
#include "adc_16_pmt.h"

static struct tcp_pcb *send_newpcb = NULL;

/** 
 * @brief TCP echo receive:This function is called by tcpecho_accept, where it processes 
 * the received data.
 *        ;TCP回环接收函数：该函数被tcpecho_accept调用，在该函数中处理接收到的数据
 * 
 * @param [in] arg The parameters passed in. ;传入的参数。
 * @param [out] tpcb the tcp_pcb for which data is read. ;接收数据的tcp_pcb结构体。
 * @param [out] p main packet buffer struct for recive.  ;接收区的主数据包结构体。
 * @param [in] err error type. ;错误类型。
 * - ERR_OK: No error, everything OK. ;没有错误，一切正常。 
 * - ERR_MEM: Out of memory error.  ;内存溢出错误。 
 * - ERR_BUF: Buffer error. ;缓存区错误。 
 * - ERR_TIMEOUT: Timeout. ;超时。  
 * - ERR_RTE: Routing problem. ;路由问题。   
 * - ERR_INPROGRESS: Operation in progress. ;操作正在进行。 
 * - ERR_VAL: Illegal value. ;非法值。
 * - ERR_WOULDBLOCK: Operation would block. ;操作将阻塞。
 * - ERR_USE: Address in use. ;地址正在使用。  
 * - ERR_ALREADY: Already connecting. ;已连接。
 * - ERR_ISCONN: Conn already established. ;已建立连接。 
 * - ERR_CONN: Not connected. ;未连接。  
 * - ERR_IF: Low-level netif error. ;网络接口低电平错误。 
 * - ERR_ABRT: Connection aborted. ;连接失败。
 * - ERR_RST: Connection reset.  ;连接重置。
 * - ERR_CLSD: Connection closed. ;连接已关闭。 
 * - ERR_ARG: Illegal argument. ;非法参数。
 * @return ERR_OK
 */
static err_t tcpecho_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  if (p != NULL)
  {
    tcp_recved(tpcb, p->tot_len);
    printf("recv len:%d\r\n", p->tot_len);
    pbuf_free(p);
  }
  else if (err != ERR_OK)
  {
    tcp_close(tpcb);
    tpcb = NULL;
  }
  return ERR_OK;
}
/**
 * @brief TCP receive function: This function is called when new data is received.
 *        ;TCP接收函数:当收到新的数据时会调用该函数。
 * 
 * @param [in] arg The parameters passed in. ;传入的参数
 * @param [out] newpcb tcp_pcb to set the receive callback. ;接收回调的tcp_pcb结构体
 * @param [in] err error type. ;错误类型。
 * - ERR_OK: No error, everything OK. ;没有错误，一切正常。 
 * - ERR_MEM: Out of memory error.  ;内存溢出错误。 
 * - ERR_BUF: Buffer error. ;缓存区错误。 
 * - ERR_TIMEOUT: Timeout. ;超时。  
 * - ERR_RTE: Routing problem. ;路由问题。   
 * - ERR_INPROGRESS: Operation in progress. ;操作正在进行。 
 * - ERR_VAL: Illegal value. ;非法值。
 * - ERR_WOULDBLOCK: Operation would block. ;操作将阻塞。
 * - ERR_USE: Address in use. ;地址正在使用。  
 * - ERR_ALREADY: Already connecting. ;已连接。
 * - ERR_ISCONN: Conn already established. ;已建立连接。 
 * - ERR_CONN: Not connected. ;未连接。  
 * - ERR_IF: Low-level netif error. ;网络接口低电平错误。 
 * - ERR_ABRT: Connection aborted. ;连接失败。
 * - ERR_RST: Connection reset.  ;连接重置。
 * - ERR_CLSD: Connection closed. ;连接已关闭。 
 * - ERR_ARG: Illegal argument. ;非法参数。
 * @return ERR_OK
 */
static err_t tcpecho_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  tcp_recv(newpcb, tcpecho_recv);
  send_newpcb = newpcb;
  return ERR_OK;
}
/**
 * @brief Write data for sending.Send data through TCP based on the address 
 * and length of the data being sent.
 *        ;写入需要发送的数据，根据发送数据的地址和长度，将数据通过TCP发送出去。
 * 
 * @param [in] buff Buff for sending data.  ;发送数据的buff.
 * @param [in] len Data length in bytes.  ;发送数据的长度，单位为字节。
 */
static void tcp_block_write(uint8_t *buff, uint32_t len)
{
  int ret;
  if (send_newpcb == NULL)
    return;
  do
  {
    ret = tcp_write(send_newpcb, buff, len, 0);//TCP_WRITE_FLAG_COPY
    if (ret >= ERR_OK)
    {
      tcp_output(send_newpcb);
      break;
    }
    else if (ret == ERR_MEM)
    {
      //printf("send err1:%d\r\n", ret);
      continue;
    }
    else
    {
      tcp_close(send_newpcb);
      send_newpcb = NULL;
      break;
    }
  } while (0);
}

void tcp_poll_handle(void)
{
 int ret;
 uint8_t *addr = NULL;
 uint32_t len = 0;
 if (adc_get_done(0) && adc_get_done(1))
 {
   printf("adc dual buff full up!\r\n");
 }
 if (send_newpcb != NULL)
 {
   if (adc_get_done(0))
   {
     adc_clear_done(0);
     tcp_block_write(adc_get_buf_addr_point(0), APP_ADC16_TCP_SEND_SIZE);
   }
   else if (adc_get_done(1))
   {
     adc_clear_done(1);
     tcp_block_write(adc_get_buf_addr_point(1), APP_ADC16_TCP_SEND_SIZE);
   }
 }
 else
 {
   adc_clear_done(0);
   adc_clear_done(1);
 }
}

void tcp_echo_init(void)
{
  struct tcp_pcb *pcb = NULL;

  pcb = tcp_new();
  pcb->flags |= TF_NODELAY;
  tcp_bind(pcb, IP_ADDR_ANY, TCP_ECHO_PORT);
  pcb = tcp_listen(pcb);
  tcp_accept(pcb, tcpecho_accept);
}