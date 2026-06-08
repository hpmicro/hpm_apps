/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_MQ_MSG_H_
#define APPLICATIONS_MQ_MSG_H_

typedef struct
{
    uint16_t msg_id;
    uint16_t submsg_id;
    uint32_t param;
    void*    user_data;
} app_mq_msg;

typedef app_mq_msg* app_mq_msg_t;

typedef enum
{
    APP_MQ_MSG_MISC_BASE_ID  = 100,
    APP_MQ_MSG_MISC_TIMER_ID,
    APP_MQ_MSG_MISC_LED_ID,
    APP_MQ_MSG_MISC_KEY_ID,
    APP_MQ_MSG_MAX_ID
}app_mq_msg_id_e;

typedef enum
{
    APP_MQ_SUBMSG_MISC_TIMER_BASE_ID  = 0,
    APP_MQ_SUBMSG_MISC_TIMER_1MS_ID,
    APP_MQ_SUBMSG_MISC_TIMER_10MS_ID,
    APP_MQ_SUBMSG_MISC_TIMER_100MS_ID,
    APP_MQ_SUBMSG_MISC_TIMER_1000MS_ID,    
    APP_MQ_SUBMSG_MISC_TIMER_MAX_ID
}app_mq_submsg_misc_timer_id_e;

typedef enum
{
    APP_MQ_SUBMSG_MISC_LED_BASE_ID  = 0,
    APP_MQ_SUBMSG_MISC_LED_MAX_ID
}app_mq_submsg_misc_led_id_e;

typedef enum
{
    APP_MQ_SUBMSG_MISC_KEY_BASE_ID  = 0,
    APP_MQ_SUBMSG_MISC_KEY_A_PRESS_ID, 
    APP_MQ_SUBMSG_MISC_KEY_A_LONG_PRESS_ID, 
    APP_MQ_SUBMSG_MISC_KEY_B_PRESS_ID,    
    APP_MQ_SUBMSG_MISC_KEY_B_LONG_PRESS_ID, 
    APP_MQ_SUBMSG_MISC_KEY_C_PRESS_ID,    
    APP_MQ_SUBMSG_MISC_KEY_C_LONG_PRESS_ID,    
    APP_MQ_SUBMSG_MISC_KEY_D_PRESS_ID,       
    APP_MQ_SUBMSG_MISC_KEY_D_LONG_PRESS_ID,    
    APP_MQ_SUBMSG_MISC_KEY_MAX_ID
}app_mq_submsg_misc_key_id_e;

#endif /* APPLICATIONS_MQ_MSG_H_ */
