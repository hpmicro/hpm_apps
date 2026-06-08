/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef APPLICATIONS_LCD_GW_LCD_THREAD_H_
#define APPLICATIONS_LCD_GW_LCD_THREAD_H_

#include "drv_log.h"
#include "drv_lcd.h"
#include "gw_mq.h"
#include "gw_mq_msg.h"
#include "board.h"
#include "lvgl.h"
#include "gw_can.h"

// 屏幕参数定义
#define LCD_W   320
#define LCD_H   172
#define STATUS_BAR_H  34
#define BOTTOM_BAR_H  34
#define CONTENT_H     (LCD_H - STATUS_BAR_H - BOTTOM_BAR_H)

void gw_lcd_thread(void *argument);

#endif /* APPLICATIONS_LCD_GW_LCD_THREAD_H_ */
