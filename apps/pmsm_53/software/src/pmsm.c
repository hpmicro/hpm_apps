/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "pmsm.h"


int main(void)
{
   
    board_init();
    parm_UpdatePowerOn();
    debug_init();


    #if defined(CONFIG_UART_CHANNEL) && CONFIG_UART_CHANNEL
    hpm_uart_channel_init();
    debug_register(hpm_uart_channel_output);
    #elif defined(CONFIG_ENET_CHANNEL) && CONFIG_ENET_CHANNEL
    //todo
    #elif defined(CONFIG_USB_DEVICE_CHANNEL) && CONFIG_USB_DEVICE_CHANNEL
    hpm_usb_device_channel_init();
    debug_register(usbd_channel_output);
    #endif

    pmsm_motor1_init();
    intc_m_enable_irq_with_priority(BOARD_PMSM0APP_PWM_IRQ, 1);
    pwm_enable_irq(BOARD_PMSM0PWM, PWM_IRQ_CMP(BOARD_PMSM0_PWM_TRIG_CMP_INDEX_CURRENTLOOP));

    while(1)
    {
        debug_handle();
        #if defined(CONFIG_UART_CHANNEL) && CONFIG_UART_CHANNEL
        hpm_uart_channel_handle(debug_src_process);
        #elif defined(CONFIG_ENET_CHANNEL) && CONFIG_ENET_CHANNEL
        //todo
        #elif defined(CONFIG_USB_DEVICE_CHANNEL) && CONFIG_USB_DEVICE_CHANNEL
        hpm_usb_device_channel_handle();
        #endif
        parm_GlobalManage();
    }
    return 0;
}

