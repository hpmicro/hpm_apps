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
#include "drv_segment.h"


void gw_misc_thread(void *argument);

#endif /* APPLICATIONS_MISC_GW_MISC_THREAD_H_ */
