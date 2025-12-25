/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __MONITOR_H
#define __MONITOR_H

#include "monitor_kconfig.h"
#include "monitor_report.h"

extern int monitor_init(void);

extern void monitor_handle(void);

#endif //__MONITOR_H