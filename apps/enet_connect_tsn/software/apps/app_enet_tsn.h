/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_ENET_TSN_THREAD_H_
#define APPLICATIONS_ENET_TSN_THREAD_H_
#include "drv_log.h"
#include "drv_led.h"
#include "drv_key.h"
#include "drv_enet_tsn.h"
#include "app_mq.h"
#include "app_mq_msg.h"
#include "timers.h"
#include "board.h"
#include <stdio.h>
#include "common.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "netconf.h"
#include "app_tcp.h"
#include "app_udp.h"

void app_enet_tsn_thread(void *argument);

#endif /* APPLICATIONS_ENET_TSN_THREAD_H_ */
