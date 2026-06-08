/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "gw_thread.h"

static gw_thread gw_thread_list[] =
{
    {NULL, gw_misc_thread, NULL, {.name="gw_thread_misc", .stack_size = GW_THREAD_MISC_STACK_SIZE, .priority = GW_THREAD_MISC_PRIORITY}},
    {NULL, gw_usb_thread,  NULL, {.name="gw_thread_usb",  .stack_size = GW_THREAD_USB_STACK_SIZE,  .priority = GW_THREAD_USB_PRIORITY}},
    {NULL, gw_ecat_thread, NULL, {.name="gw_thread_ecat", .stack_size = GW_THREAD_ECAT_STACK_SIZE, .priority = GW_THREAD_ECAT_PRIORITY}},
    {NULL, gw_can_thread,  NULL, {.name="gw_thread_can",  .stack_size = GW_THREAD_CAN_STACK_SIZE,  .priority = GW_THREAD_CAN_PRIORITY}},
    // {NULL, gw_net_thread,  NULL, {.name="gw_thread_net",  .stack_size = GW_THREAD_NET_STACK_SIZE,  .priority = GW_THREAD_NET_PRIORITY}},
};

osThreadId_t gw_thread_get_handle(gw_thread_id_e id)
{
    return gw_thread_list[id].thread_handle;
}

void gw_thread_init(void)
{
    drv_log_init();
    gw_mq_init();

    for(uint8_t i=0; i<sizeof(gw_thread_list)/sizeof(gw_thread); i++)
    {
        gw_thread_list[i].thread_handle = osThreadNew(gw_thread_list[i].thread_func,
                                                  gw_thread_list[i].argument,
                                                 &gw_thread_list[i].attr);
        if(gw_thread_list[i].thread_handle) {
            LOG_D("%s create success\r\n", gw_thread_list[i].attr.name);
        } else{
            LOG_E("%s create fail\r\n", gw_thread_list[i].attr.name);
        }
    }
}
