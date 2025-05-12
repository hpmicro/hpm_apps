/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __HPM_DIFFCONFIG_H
#define __HPM_DIFFCONFIG_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define bs_printf(...) printf(__VA_ARGS__)
// #define bs_malloc(...) pvPortMalloc(__VA_ARGS__)
// #define bs_free(...) vPortFree(__VA_ARGS__)
#define bs_malloc(...) malloc(__VA_ARGS__)
#define bs_free(...) free(__VA_ARGS__)

#endif