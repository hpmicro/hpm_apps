/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef LCD8080_H
#define LCD8080_H



/**
 * @brief HPM FEMC LCD8080 sample API ;HPM FEMC LCD8080例程接口
 * @addtogroup FEMC_LCD8080 
 * 
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/
#define API_VERSION         "V0.1.0"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Set the LCD window range
 * ;设置LCD窗口范围
 *
 * @param [in] x X Origin coordinate ;X起点坐标
 * @param [in] y  Y Origin coordinate ;Y起点坐标
 * @param [in] width Width of window ;窗口长度
 * @param [in] height Height of window ;窗口宽度
 *
 */
static void lcd_open_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height);


/**
 * @brief Draw a point
 * ;绘画一个点
 *
 * @param [in] x X Origin coordinate ;X起点坐标
 * @param [in] y  Y Origin coordinate ;Y起点坐标
 * @param [in] color Color setting;颜色设置
 *
 */
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);



/**
 * @brief Draw a line
 * ;绘画一条线
 *
 * @param [in] start_x X Origin coordinate ;X起点坐标
 * @param [in] start_y  Y Origin coordinate ;Y起点坐标
 * @param [in] end_x  Y Terminal coordinate ;X终点坐标
 * @param [in] end_y  Y Terminal coordinate ;Y终点坐标
 * @param [in] color Color setting;颜色设置
 *
 */
void lcd_draw_line(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color);



/**
 * @brief Draw rectangle
 * ;绘画矩形
 *
 * @param [in] start_x X Origin coordinate ;X起点坐标
 * @param [in] start_y  Y Origin coordinate ;Y起点坐标
 * @param [in] end_x  Y Terminal coordinate ;X终点坐标
 * @param [in] end_y  Y Terminal coordinate ;Y终点坐标
 * @param [in] color Color setting;颜色设置
 *
 */
void lcd_draw_rectangle(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color);




/**
 * @brief Draw circle
 * ;绘画圆形
 *
 * @param [in] x X Center of a circle ;X圆心坐标
 * @param [in] y  Y Center of a circle ;Y圆心坐标
 * @param [in] radius  Radius ;半径
 * @param [in] color Color setting;颜色设置
 *
 */
void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color);




/**
 * @brief Fill rectangle
 * ;填充矩形
 *
 * @param [in] x0 X Origin coordinate ;X起点坐标
 * @param [in] y0  Y Origin coordinate ;Y起点坐标
 * @param [in] x1  Width of window ;窗口长度
 * @param [in] y1 Height of window ;窗口宽度
 * @param [in] color Color setting;颜色设置
 *
 */
void lcd_fill_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);


/**
 * @brief Fill the entire screen
 * ;填充整个屏幕
 *
 * @param [in] color Color setting;颜色设置
 *
 */
void lcd_fill_screen(uint16_t color);



/**
 * @}
 *
 */

#ifdef __cplusplus
}
#endif

#endif 