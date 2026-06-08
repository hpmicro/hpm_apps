/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __DRV_LCD_H__
#define __DRV_LCD_H__
#include "board.h"
#include "hpm_common.h"
#include "hpm_clock_drv.h"
#include "hpm_spi_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "drv_log.h"
#include "lvgl.h"

#define USE_HORIZONTAL 2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 172	
#define LCD_H 320
#else
#define LCD_W 320
#define LCD_H 172
#endif

//画笔颜色
#define WHITE         	 0xFFFF//白色
#define BLACK         	 0x0000//黑色  
#define BLUE           	 0x001F//蓝色  
#define BRED             0XF81F//蓝红色 
#define GRED 			 0XFFE0//红绿色
#define GBLUE			 0X07FF//蓝绿色 
#define RED           	 0xF800//红色 
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0//绿色 
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0//黄色 
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

void drv_lcd_init(void);
void drv_lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void drv_lcd_show_chinese_12x12(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);
void drv_lcd_show_chinese_16x16(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);
void drv_lcd_show_chinese_24x24(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);
void drv_lcd_show_chinese_32x32(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode);
#endif