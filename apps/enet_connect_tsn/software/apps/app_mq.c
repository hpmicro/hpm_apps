/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "app_mq.h"

static app_mq app_mq_list[] =
{
    {NULL, "app_mq_misc", APP_MQ_MAX_MSGS_MISC, sizeof(app_mq_msg)},
};

void app_mq_init(void)
{
    for(uint8_t i=0; i<sizeof(app_mq_list)/sizeof(app_mq); i++)
    {
        app_mq_list[i].mq_handle = xQueueCreate (app_mq_list[i].msg_count, app_mq_list[i].msg_size);
        if(app_mq_list[i].mq_handle) {
            LOG_I("%s create success\r\n", app_mq_list[i].mq_name);
        } else{
            LOG_E("%s create fail\r\n", app_mq_list[i].mq_name);
        }
    }
}

BaseType_t app_mq_send(app_mq_id_e mq_id, app_mq_msg_t pmq_msg, uint32_t timeout)
{
    BaseType_t ret = pdFAIL;

    if (mq_id < APP_MQ_MAX_ID) {
        ret = xQueueSend(app_mq_list[mq_id].mq_handle, pmq_msg, timeout);
        if(!ret) {
            LOG_E("send mq msg fail %s, %d, %d\r\n", app_mq_list[mq_id].mq_name, pmq_msg->submsg_id, ret);
        }
    }

    return ret;
}

BaseType_t app_mq_send_isr(app_mq_id_e mq_id, app_mq_msg_t pmq_msg)
{
    BaseType_t ret = pdFAIL;
    BaseType_t higher_priority_task_woken = pdFALSE;

    if (mq_id < APP_MQ_MAX_ID) {
        ret = xQueueSendFromISR(app_mq_list[mq_id].mq_handle, pmq_msg, &higher_priority_task_woken);
        if(!ret) {
            LOG_E("send mq msg fail %s, %d, %d\r\n", app_mq_list[mq_id].mq_name, pmq_msg->submsg_id, ret);
        }
        portYIELD_FROM_ISR(higher_priority_task_woken);
    }

    return ret;
}

BaseType_t app_mq_send_over(app_mq_id_e mq_id, app_mq_msg_t pmq_msg)
{
    BaseType_t ret = pdFAIL;


    if (mq_id < APP_MQ_MAX_ID) {
        ret = xQueueOverwrite(app_mq_list[mq_id].mq_handle, pmq_msg);
        if(!ret) {
            LOG_E("send mq msg fail %s, %d, %d\r\n", app_mq_list[mq_id].mq_name, pmq_msg->submsg_id, ret);
        }
    }

    return ret;
}

BaseType_t app_mq_send_over_isr(app_mq_id_e mq_id, app_mq_msg_t pmq_msg)
{
    BaseType_t ret = pdFAIL;
    BaseType_t higher_priority_task_woken = pdFALSE;

    if (mq_id < APP_MQ_MAX_ID) {
        ret = xQueueOverwriteFromISR(app_mq_list[mq_id].mq_handle, pmq_msg, &higher_priority_task_woken);
        if(!ret) {
            LOG_E("send mq msg fail %s, %d, %d\r\n", app_mq_list[mq_id].mq_name, pmq_msg->submsg_id, ret);
        }
        portYIELD_FROM_ISR(higher_priority_task_woken);
    }

    return ret;
}

BaseType_t app_mq_send_urgent(app_mq_id_e mq_id, app_mq_msg_t pmq_msg, uint32_t timeout)
{
    BaseType_t ret = pdFAIL;

    if (mq_id < APP_MQ_MAX_ID) {
        ret = xQueueSendToFront( app_mq_list[mq_id].mq_handle, pmq_msg, timeout);
        if(!ret) {
            LOG_E("send mq msg fail %s, %d, %d\r\n", app_mq_list[mq_id].mq_name, pmq_msg->submsg_id, ret);
        }
    }

    return ret;
}

BaseType_t app_mq_send_urgent_isr(app_mq_id_e mq_id, app_mq_msg_t pmq_msg)
{
    BaseType_t ret = pdFAIL;
    BaseType_t higher_priority_task_woken = pdFALSE;

    if (mq_id < APP_MQ_MAX_ID) {
        ret = xQueueSendToFrontFromISR( app_mq_list[mq_id].mq_handle, pmq_msg, &higher_priority_task_woken);
        if(!ret) {
            LOG_E("send mq msg fail %s, %d, %d\r\n", app_mq_list[mq_id].mq_name, pmq_msg->submsg_id, ret);
        }
        portYIELD_FROM_ISR(higher_priority_task_woken);
    }

    return ret;
}

BaseType_t app_mq_recv(app_mq_id_e mq_id, app_mq_msg_t pmq_msg, uint32_t timeout)
{
    BaseType_t ret = pdFAIL;

    if((mq_id < APP_MQ_MAX_ID) && (NULL != pmq_msg)) {
        ret = xQueueReceive(app_mq_list[mq_id].mq_handle, pmq_msg, timeout);
        if(!ret) {
            LOG_E("recv mq msg fail %s, %d, %d\r\n", app_mq_list[mq_id].mq_name, pmq_msg->submsg_id, ret);
        }
    }

    return ret;
}