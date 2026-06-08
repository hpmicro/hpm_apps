/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_MISC_GW_MISC_THREAD_H_
#define APPLICATIONS_MISC_GW_MISC_THREAD_H_
#include "drv_log.h"
#include "drv_led.h"
#include "drv_mcan.h"
#include "gw_mq.h"
#include "gw_mq_msg.h"
#include "board.h"
#include "gw_can.h"

typedef enum
{
    GW_MISC_LED_DEV_STATUS_R_BLINKY,
    GW_MISC_LED_DEV_STATUS_G_BLINKY,
    GW_MISC_LED_DEV_STATUS_B_BLINKY,
    GW_MISC_LED_DEV_STATUS_NULL,
}gw_misc_led_dev_status_e;

void gw_misc_thread(void *argument);

#endif /* APPLICATIONS_MISC_GW_MISC_THREAD_H_ */
