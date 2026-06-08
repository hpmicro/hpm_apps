/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_USB_THREAD_H_
#define APPLICATIONS_USB_THREAD_H_
// #include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "shell.h"
#include "usbd_core.h"
#include "drv_log.h"


void app_usb_thread(void *argument);

#endif /* APPLICATIONS_USB_THREAD_H_ */
