#ifndef _HPM_DEBUG_H
#define _HPM_DEBUG_H

#include "board.h"

/**
 * @brief UART TRANS API ;串口数据传输接口函数
 * @addtogroup UART_TRANS_API
 * @{
 *
 */

/**
 * @brief     UART BASE
 *            ;串口基地址
 */
#define BOARD_UART                HPM_UART5

/**
 * @brief     UART clock
 *            ;串口时钟
 */
#define BOARD_UART_CLOCK          clock_uart5

/**                                            
 * @brief     uart status
 *            ;串口读写状态
 */
typedef enum _tag_DataTrans
{
    UART_READ , 
    UART_WRITE ,
}DataTrans ;


extern int8_t pt_send[5];
/**
 * @brief     uart pins init
 *            ;串口引脚初始化
 */
void uart_pins_init(void);
/**
 * @brief     uart config
 *            ;串口配置
 */
void uart_cfg(void);
/**
 * @brief     uart trans data process
 *            ;串口传输过程
 */
void uart_trans_pro (void);
/**
 * @brief     send data
 *            ;数据包发送
 */
void sendDataToPC(void) ;

/** @} */

#endif//_HPM_DEBUG_H