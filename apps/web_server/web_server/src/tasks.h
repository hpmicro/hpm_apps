/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef APP_TASKS_H
#define APP_TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create all system tasks (USB, Mongoose, GPIO)
 */
void create_system_tasks(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_TASKS_H */
