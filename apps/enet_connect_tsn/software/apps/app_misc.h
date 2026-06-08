/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_MISC_THREAD_H_
#define APPLICATIONS_MISC_THREAD_H_
#include "board.h"
#include "drv_log.h"
#include "drv_led.h"
#include "drv_key.h"
#include "drv_oled.h"
#include "app_mq.h"
#include "app_mq_msg.h"


void app_misc_thread(void *argument);

#endif /* APPLICATIONS_MISC_THREAD_H_ */
