/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_GW_MQ_H_
#define APPLICATIONS_GW_MQ_H_
#include "FreeRTOS.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "drv_log.h"
#include "gw_mq_msg.h"

#define GW_MQ_MAX_MSGS_MISC (32)
#define GW_MQ_MAX_MSGS_LCD (32)
#define GW_MQ_MAX_MSGS_ECAT (32)

typedef struct
{
    osMessageQueueId_t mq_handle;
    uint32_t msg_count;
    uint32_t msg_size;
    osMessageQueueAttr_t attr;
}gw_mq;

typedef gw_mq* gw_mq_t;

typedef enum
{
    GW_MQ_MISC_ID = 0,
    GW_MQ_LCD_ID,
    GW_MQ_ECAT_ID,
    GW_MQ_MAX_ID
}gw_mq_id_e;

void gw_mq_init(void);
osStatus_t gw_mq_send(gw_mq_id_e mq_id, gw_mq_msg_t pmq_msg, uint32_t timeout);
osStatus_t gw_mq_send_urgent(gw_mq_id_e mq_id, gw_mq_msg_t pmq_msg, uint32_t timeout);
osStatus_t gw_mq_recv(gw_mq_id_e mq_id, gw_mq_msg_t pmq_msg, uint32_t timeout);

#endif /* APPLICATIONS_GW_MQ_H_ */
