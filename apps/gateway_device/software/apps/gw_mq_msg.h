/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_GW_MQ_MSG_H_
#define APPLICATIONS_GW_MQ_MSG_H_

typedef struct
{
    uint16_t msg_id;
    uint16_t submsg_id;
    uint32_t param;
    void*    user_data;
} gw_mq_msg;

typedef gw_mq_msg* gw_mq_msg_t;

typedef enum
{
    GW_MQ_MSG_CAN_BASE_ID  = 0,
    GW_MQ_MSG_CAN_RX_ID,
    GW_MQ_MSG_CAN_TX_ID,
    GW_MQ_MSG_CAN_CONFIG_ID,
    GW_MQ_MSG_MISC_BASE_ID  = 100,
    GW_MQ_MSG_MISC_LED_ID,
    GW_MQ_MSG_MISC_TIMER_ID,
    GW_MQ_MSG_MISC_SEGMENT_ID,
    GW_MQ_MSG_MAX_ID
}gw_mq_msg_id_e;

typedef enum
{
    GW_MQ_SUBMSG_MISC_LED_BASE_ID  = 0,
    GW_MQ_SUBMSG_MISC_LED_DEV_STATE_CAN_TX_ID,
    GW_MQ_SUBMSG_MISC_LED_DEV_STATE_CAN_RX_ID,
    GW_MQ_SUBMSG_MISC_LED_MAX_ID
}gw_mq_submsg_misc_led_id_e;

typedef enum
{
    GW_MQ_SUBMSG_MISC_TIMER_BASE_ID  = 0,
    GW_MQ_SUBMSG_MISC_TIMER_1MS_ID,
    GW_MQ_SUBMSG_MISC_TIMER_10MS_ID,
    GW_MQ_SUBMSG_MISC_TIMER_100MS_ID,
    GW_MQ_SUBMSG_MISC_TIMER_1000MS_ID,    
    GW_MQ_SUBMSG_MISC_TIMER_MAX_ID
}gw_mq_submsg_misc_timer_id_e;

typedef enum
{
   GW_MQ_SUBMSG_MISC_SEGMENT_BASE_ID  = 0,
   GW_MQ_SUBMSG_MISC_SEGMENT_GE_ID,
   GW_MQ_SUBMSG_MISC_SEGMENT_SHI_ID,
   GW_MQ_SUBMSG_MISC_SEGMENT_BAI_ID,
   GW_MQ_SUBMSG_MISC_SEGMENT_QIAN_ID,    
   GW_MQ_SUBMSG_MISC_SEGMENT_WAN_ID,
   GW_MQ_SUBMSG_MISC_SEGMENT_MAX_ID
}gw_mq_submsg_misc_segment_id_e;

typedef enum
{
    GW_MQ_SUBMSG_CAN_BASE_ID  = 0,
    GW_MQ_SUBMSG_CAN_TX_STD_RXNUM_ID,
    GW_MQ_SUBMSG_CAN_TX_EXT_RXNUM_ID,
    GW_MQ_SUBMSG_CAN_MAX_ID 
}gw_mq_submsg_can_id_e;


#endif /* APPLICATIONS_GW_MQ_MSG_H_ */
