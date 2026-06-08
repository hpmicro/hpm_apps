/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_MQ_H_
#define APPLICATIONS_MQ_H_
#include "FreeRTOS.h"
#include "queue.h"
#include "drv_log.h"
#include "app_mq_msg.h"

#define APP_MQ_MAX_MSGS_MISC (32)

typedef struct 
{
    QueueHandle_t        mq_handle;
    const char *const    mq_name;
    uint32_t             msg_count;
    uint32_t             msg_size;
} app_mq;

typedef app_mq* app_mq_t;

typedef enum
{
    APP_MQ_MISC_ID = 0,
    APP_MQ_MAX_ID
}app_mq_id_e;

void app_mq_init(void);
BaseType_t app_mq_send(app_mq_id_e mq_id, app_mq_msg_t pmq_msg, uint32_t timeout);
BaseType_t app_mq_send_isr(app_mq_id_e mq_id, app_mq_msg_t pmq_msg);
BaseType_t app_mq_send_over(app_mq_id_e mq_id, app_mq_msg_t pmq_msg);
BaseType_t app_mq_send_over_isr(app_mq_id_e mq_id, app_mq_msg_t pmq_msg);
BaseType_t app_mq_send_urgent(app_mq_id_e mq_id, app_mq_msg_t pmq_msg, uint32_t timeout);
BaseType_t app_mq_send_urgent_isr(app_mq_id_e mq_id, app_mq_msg_t pmq_msg);
BaseType_t app_mq_recv(app_mq_id_e mq_id, app_mq_msg_t pmq_msg, uint32_t timeout);

#endif /* APPLICATIONS_MQ_H_ */
