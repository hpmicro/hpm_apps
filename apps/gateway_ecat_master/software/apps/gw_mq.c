/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "gw_mq.h"

static gw_mq gw_mq_list[] =
{
    {NULL, GW_MQ_MAX_MSGS_MISC, sizeof(gw_mq_msg), {.name = "gw_mq_misc"}},
    {NULL, GW_MQ_MAX_MSGS_LCD, sizeof(gw_mq_msg), {.name = "gw_mq_lcd"}},
    {NULL, GW_MQ_MAX_MSGS_ECAT, sizeof(gw_mq_msg), {.name = "gw_mq_ecat"}},
};

void gw_mq_init(void)
{
    for(uint8_t i=0; i<sizeof(gw_mq_list)/sizeof(gw_mq); i++)
    {
        gw_mq_list[i].mq_handle = osMessageQueueNew(gw_mq_list[i].msg_count, 
                                               gw_mq_list[i].msg_size,
                                               &gw_mq_list[i].attr);
        if(gw_mq_list[i].mq_handle) {
            LOG_D("%s create success\r\n", gw_mq_list[i].attr.name);
        } else{
            LOG_E("%s create fail\r\n", gw_mq_list[i].attr.name);
        }
    }
}

osStatus_t gw_mq_send(gw_mq_id_e mq_id, gw_mq_msg_t pmq_msg, uint32_t timeout)
{
    osStatus_t ret = osError;

    if (mq_id < GW_MQ_MAX_ID) {
        ret = osMessageQueuePut(gw_mq_list[mq_id].mq_handle, pmq_msg, 0, timeout);
        if(ret) {
            LOG_E("send mq msg fail %s, %d, %d\r\n", gw_mq_list[mq_id].attr.name, pmq_msg->submsg_id, ret);
        }
    }

    return ret;
}

osStatus_t gw_mq_send_urgent(gw_mq_id_e mq_id, gw_mq_msg_t pmq_msg, uint32_t timeout)
{
    osStatus_t ret = osError;

    if (mq_id < GW_MQ_MAX_ID) {
        #if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
        ret = xQueueSendToFront( gw_mq_list[mq_id].mq_handle, pmq_msg, timeout);
        #endif
        if(ret) {
            LOG_E("send mq msg fail %s, %d, %d\r\n", gw_mq_list[mq_id].attr.name, pmq_msg->submsg_id, ret);
        }
    }

    return ret;
}

osStatus_t gw_mq_recv(gw_mq_id_e mq_id, gw_mq_msg_t pmq_msg, uint32_t timeout)
{
    osStatus_t ret = osError;

    if((mq_id < GW_MQ_MAX_ID) && (NULL != pmq_msg)) {
        ret = osMessageQueueGet(gw_mq_list[mq_id].mq_handle, pmq_msg, NULL, timeout);
        if(ret < 0 && ret != osErrorTimeout) {
            LOG_E("recv mq msg fail %s, %d, %d\r\n", gw_mq_list[mq_id].attr.name, pmq_msg->submsg_id, ret);
        }
    }

    return ret;
}
