/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * HPM5E00EVK Web Control System
 * Mongoose configuration
 */

#ifndef  MONGOOSE_CONFIG_H
#define  MONGOOSE_CONFIG_H

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lwip/sockets.h"
#include "lwip/errno.h"

#define MG_ARCH                MG_ARCH_FREERTOS
#define MG_ENABLE_LWIP         1
#define MG_ENABLE_PACKED_FS    1
#define MG_TLS                 MG_TLS_NONE	// change to 'MG_TLS_MBED' to enable TLS
#define MG_IO_SIZE             1450 

#endif