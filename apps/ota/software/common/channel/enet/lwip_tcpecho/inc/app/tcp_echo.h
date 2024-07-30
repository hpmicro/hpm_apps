/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef TCPECHO_H
#define TCPECHO_H

#define TCP_LOCAL_PORT (5001U)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void tcp_echo_init(void);

typedef int (*DataProcess)(unsigned char* data, int len);

extern DataProcess data_process;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* TCP_ECHO_H */