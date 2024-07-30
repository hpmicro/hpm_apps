#ifndef __UART_XMODEM_H
#define __UART_XMODEM_H

#define USE_UART_ID   HPM_UART0

/**
 *
 * @brief Uart xmodem APIs; Uart xmodem 接口
 * @defgroup Uart xmodem APIs
 * @{
 */

/**
 * @brief Receives data using the XMODEM protocol;使用 xmodem 协议接收数据
 *
 * @param proc Pointer to a processor-specific data structure or function used for communication;指向flash 自动下载的接口
 *
 * @return The number of bytes successfully received and stored in the destination buffer.
 *         Returns a negative value if an error occurs during the reception.
 *        ;返回实际接收的长度，如果小于0 表示数据通信错误
 *
 */
int xmodemReceive(void* proc);

/**
 * @}
 */
#endif// __UART_XMODEM_H