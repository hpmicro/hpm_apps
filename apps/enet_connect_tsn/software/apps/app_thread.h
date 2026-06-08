/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_THREAD_H_
#define APPLICATIONS_THREAD_H_
// #include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "shell.h"
#include "usbd_core.h"
#include "drv_log.h"
#include "app_mq.h"
#include "app_mq_msg.h"
#include "app_misc.h"
#include "app_usb.h"
#include "app_enet_tsn.h"

#define APP_THREAD_USB_STACK_SIZE  1024*4
#define APP_THREAD_USB_PRIORITY    (configMAX_PRIORITIES - 5U)

#define APP_THREAD_ENET_TSN_STACK_SIZE 1024*4
#define APP_THREAD_ENET_TSN_PRIORITY   (configMAX_PRIORITIES - 1U)

#define APP_THREAD_MISC_STACK_SIZE 1024*8
#define APP_THREAD_MISC_PRIORITY   (configMAX_PRIORITIES - 12U)
typedef struct
{
    TaskHandle_t       thread_handle;
    const char* const  thread_name;
    TaskFunction_t     thread_func;
    const uint16_t     stack_depth;
    uint16_t           priority;
    void*              argument;
} app_thread;

typedef app_thread* app_thread_t;

typedef enum
{
    APP_THREAD_USB_ID = 0,
    APP_THREAD_NET_TSN_ID,
    APP_THREAD_MISC_ID,
}app_thread_id_e;

extern struct netif gnetif;

void app_thread_init(void);

TaskHandle_t app_thread_get_handle(app_thread_id_e id);

#endif /* APPLICATIONS_THREAD_H_ */
