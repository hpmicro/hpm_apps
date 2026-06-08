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
#include "gw_ecat_master.h"
#include "gw_lcd.h"

#define GW_THREAD_MISC_STACK_SIZE 1024*10
#define GW_THREAD_MISC_PRIORITY   (configMAX_PRIORITIES - 10U)

#define GW_THREAD_USB_STACK_SIZE  1024*4
#define GW_THREAD_USB_PRIORITY    (configMAX_PRIORITIES - 5U)

#define GW_THREAD_ECAT_MASTER_STACK_SIZE 1024*20
#define GW_THREAD_ECAT_MASTER_PRIORITY   (configMAX_PRIORITIES - 1U)

#define GW_THREAD_LCD_STACK_SIZE  1024*20
#define GW_THREAD_LCD_PRIORITY    (configMAX_PRIORITIES - 12U)

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
    GW_THREAD_USB_ID,
}gw_thread_id_e;

void gw_thread_init(void);

#endif /* APPLICATIONS_GW_THREAD_H_ */
