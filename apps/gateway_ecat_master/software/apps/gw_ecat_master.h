/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_ECAT_GW_ECAT_MASTER_THREAD_H_
#define APPLICATIONS_ECAT_GW_ECAT_MASTER_THREAD_H_
#include "drv_log.h"
#include "drv_led.h"
#include "gw_mq.h"
#include "gw_mq_msg.h"
#include "board.h"
#include "ec_master.h"
#include "gw_can.h"

void gw_ecat_master_thread(void *argument);

#endif /* APPLICATIONS_ECAT_GW_ECAT_MASTER_THREAD_H_ */
