/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "app_thread.h"

static app_thread app_thread_list[] =
{
    {NULL, "app_thread_usb",      app_usb_thread,      APP_THREAD_USB_STACK_SIZE,      APP_THREAD_USB_PRIORITY,      NULL},
    {NULL, "app_thread_enet_tsn", app_enet_tsn_thread, APP_THREAD_ENET_TSN_STACK_SIZE, APP_THREAD_ENET_TSN_PRIORITY, NULL},
    {NULL, "app_thread_misc",     app_misc_thread,     APP_THREAD_MISC_STACK_SIZE,     APP_THREAD_MISC_PRIORITY,     NULL},
};

TaskHandle_t app_thread_get_handle(app_thread_id_e id)
{
    return app_thread_list[id].thread_handle;
}


void app_thread_init(void)
{
    int ret;

    drv_log_init();
    app_mq_init();

    for(uint8_t i=0; i<sizeof(app_thread_list)/sizeof(app_thread); i++)
    {
        ret = xTaskCreate(app_thread_list[i].thread_func, 
                        app_thread_list[i].thread_name, 
                        app_thread_list[i].stack_depth, 
                        app_thread_list[i].argument,
                        app_thread_list[i].priority,
                        &app_thread_list[i].thread_handle);
        if(ret == pdPASS) {
            LOG_I("thread create success Handle = %p stacksize = %d name = %s\r\n", app_thread_list[i].thread_handle, app_thread_list[i].stack_depth, app_thread_list[i].thread_name);
        } else {
            LOG_E("thread create fail ret = %d stacksize = %d name = %s\r\n", ret, app_thread_list[i].stack_depth, app_thread_list[i].thread_name);
        }
    }

    vTaskStartScheduler();
}
