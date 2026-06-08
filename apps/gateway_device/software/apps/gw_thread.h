/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_GW_THREAD_H_
#define APPLICATIONS_GW_THREAD_H_
#include "cmsis_os2.h"
#include "board.h"
#include "shell.h"
#include "usbd_core.h"
#include "drv_log.h"
#include "gw_mq.h"
#include "gw_mq_msg.h"
#include "gw_misc.h"
#include "gw_usb.h"
#include "gw_can.h"

#define GW_THREAD_MISC_STACK_SIZE 1024*10
#define GW_THREAD_MISC_PRIORITY   18

#define GW_THREAD_CAN_STACK_SIZE  1024*10
#define GW_THREAD_CAN_PRIORITY    (configMAX_PRIORITIES - 4U)

#define GW_THREAD_USB_STACK_SIZE  1024*4
#define GW_THREAD_USB_PRIORITY    (configMAX_PRIORITIES - 5U)

#define GW_THREAD_ECAT_STACK_SIZE 1024*20
#define GW_THREAD_ECAT_PRIORITY   (configMAX_PRIORITIES - 1U)


typedef struct
{
    osThreadId_t thread_handle;
    osThreadFunc_t thread_func; 
    void *argument;
    const osThreadAttr_t attr;
}gw_thread;

typedef gw_thread* gw_thread_t;

typedef enum
{
    GW_THREAD_MISC_ID = 0,
    GW_THREAD_UART_ID,
    GW_THREAD_USB_ID,
    GW_THREAD_ECAT_ID,
    GW_THREAD_NET_ID,
}gw_thread_id_e;

void gw_thread_init(void);

#endif /* APPLICATIONS_GW_THREAD_H_ */
