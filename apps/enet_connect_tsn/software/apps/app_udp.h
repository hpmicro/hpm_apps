/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef APPLICATIONS_UDP_H_
#define APPLICATIONS_UDP_H_
#include "lwip/inet.h"
#include "lwip/sys.h"
#include <stdio.h>
#include <string.h>
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/api.h"
#include "lwip/netbuf.h"
#include "lwip/sys.h"
#include "drv_log.h"

#define APP_UDP_LOCAL_PORT0 (5001U)
#define APP_UDP_LOCAL_PORT1 (5002U)


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void app_udp_echo_init(struct netif *netif);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* APPLICATIONS_UDP_H_ */
