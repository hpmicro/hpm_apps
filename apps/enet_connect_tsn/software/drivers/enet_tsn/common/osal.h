/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef OSAL_H
#define OSAL_H

#if defined(__ENABLE_FREERTOS) && (__ENABLE_FREERTOS)
typedef SemaphoreHandle_t osSemaphoreId_t;
typedef TimerHandle_t     osTimerId_t;

#endif


#endif
