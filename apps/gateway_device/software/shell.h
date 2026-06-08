/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef SHELL_H
#define SHELL_H

#include "csh.h"

extern int shell_init(uint8_t busid, uint32_t regbase, bool need_login);
extern void shell_lock(void);
extern void shell_unlock(void);

#endif
