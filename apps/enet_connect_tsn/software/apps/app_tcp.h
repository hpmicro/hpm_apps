/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef APPLICATIONS_TCP_H_
#define APPLICATIONS_TCP_H_
#include "lwip/inet.h"
#include "lwip/sys.h"
#include <stdio.h>
#include <string.h>
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "drv_log.h"


#define APP_TCP_LOCAL_PORT0 (5001U)
#define APP_TCP_LOCAL_PORT1 (5002U)


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void app_tcp_echo_init(struct netif *netif);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* APPLICATIONS_TCP_H_ */
