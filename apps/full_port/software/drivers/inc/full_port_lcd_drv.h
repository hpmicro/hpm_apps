/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __FULL_PORT_LCD_DRV_H_
#define __FULL_PORT_LCD_DRV_H_

#define USE_HORIZONTAL 2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 172	
#define LCD_H 320

#else
#define LCD_W 320
#define LCD_H 172
#endif

/**
 * @brief lcd init; 液晶屏初始化
 */
void full_port_lcd_init(void);

/**
 * @brief set start addr and end addr; 设置开始地址和结束地址
 * @param[in] x1 start addr of the column; 列的起始地址
 * @param[in] y1 start addr of the row; 行的起始地址
 * @param[in] x2 end addr of the column; 列的结束地址
 * @param[in] y2 end addr of the row; 行的结束地址
 */
void full_port_lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**
 * @brief lcd test; 液晶屏测试
 */
void full_port_lcd_test(void);

/**
 * @brief Fill the specified area with colorr; 在指定区域填充颜色
 * @param[in] xsta Starting coordinates; 起始坐标
 * @param[in] ysta Starting coordinates; 起始坐标
 * @param[in] xend End coordinates; 终止坐标
 * @param[in] yend End coordinates; 终止坐标
 * @param[in] color Color to fill; 要填充的颜色
 */
void full_port_lcd_full(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);//指定区域填充颜色

/**
 * @brief Draw a point at the specified location; 在指定位置画点
 * @param[in] x Draw point coordinates; 画点坐标
 * @param[in] y Draw point coordinates; 画点坐标
 * @param[in] color Point color; 点的颜色
 */
void hpm_lcd_draw_point(uint16_t x,uint16_t y,uint16_t color);//在指定位置画一个点

/**
 * @brief Draw a line at the specified position; 在指定位置画一条线
 * @param[in] x1 Starting coordinates; 起始坐标
 * @param[in] y1 Starting coordinates; 起始坐标
 * @param[in] x2 End coordinates; 终止坐标
 * @param[in] y2 End coordinates; 终止坐标
 * @param[in] color Line color; 线的颜色
 */
void full_port_lcd_draw_line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);//在指定位置画一条线

/**
 * @brief Draw a line at the specified position; 在指定位置画一个矩形
 * @param[in] x1 Starting coordinates; 起始坐标
 * @param[in] y1 Starting coordinates; 起始坐标
 * @param[in] x2 End coordinates; 终止坐标
 * @param[in] y2 End coordinates; 终止坐标
 * @param[in] color The color of the rectangle; 矩形的颜色
 */
void full_port_lcd_draw_rect_angle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);//在指定位置画一个矩形

/**
 * @brief Draw a circle at the specified location; 在指定位置画一个圆
 * @param[in] x0 Center coordinates; 圆心坐标
 * @param[in] y0 Center coordinates; 圆心坐标
 * @param[in] r Radius; 半径
 * @param[in] color Circle color; 圆的颜色
 */
void full_port_lcd_draw_circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color);//在指定位置画一个圆

/**
 * @brief Display Chinese character string; 显示汉字串
 * @param[in] x Display Coordinates; 显示坐标
 * @param[in] y Display Coordinates; 显示坐标
 * @param[in] *s The Chinese character string to be displayed; 要显示的汉字串
 * @param[in] fc Word Color; 字的颜色
 * @param[in] bc Background color of text; 字的背景色
 * @param[in] sizey Font size optional 16 24 32; 字号 可选 16 24 32
 * @param[in] mode 0 Non-overlapping mode  1 Overlay mode;  0非叠加模式  1叠加模式
 */
void full_port_lcd_show_chinese(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);//显示汉字串

/**
 * @brief Display single 12x12 Chinese character; 显示单个12x12汉字
 * @param[in] x Display Coordinates; 显示坐标
 * @param[in] y Display Coordinates; 显示坐标
 * @param[in] *s The Chinese character string to be displayed; 要显示的汉字串
 * @param[in] fc Word Color; 字的颜色
 * @param[in] bc Background color of text; 字的背景色
 * @param[in] sizey Font size; 字号 
 * @param[in] mode 0 Non-overlapping mode  1 Overlay mode;  0非叠加模式  1叠加模式
 */
void full_port_lcd_show_chinese_12x12(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);//显示单个12x12汉字

/**
 * @brief Display single 16x16 Chinese character; 显示单个16x16汉字
 * @param[in] x Display Coordinates; 显示坐标
 * @param[in] y Display Coordinates; 显示坐标
 * @param[in] *s The Chinese character string to be displayed; 要显示的汉字串
 * @param[in] fc Word Color; 字的颜色
 * @param[in] bc Background color of text; 字的背景色
 * @param[in] sizey Font size; 字号 
 * @param[in] mode 0 Non-overlapping mode  1 Overlay mode;  0非叠加模式  1叠加模式
 */
void full_port_lcd_show_chinese_16x16(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);//显示单个16x16汉字

/**
 * @brief Display single 24x24 Chinese character; 显示单个24x24汉字
 * @param[in] x Display Coordinates; 显示坐标
 * @param[in] y Display Coordinates; 显示坐标
 * @param[in] *s The Chinese character string to be displayed; 要显示的汉字串
 * @param[in] fc Word Color; 字的颜色
 * @param[in] bc Background color of text; 字的背景色
 * @param[in] sizey Font size; 字号
 * @param[in] mode 0 Non-overlapping mode  1 Overlay mode;  0非叠加模式  1叠加模式
 */
void full_port_lcd_show_chinese_24x24(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);//显示单个24x24汉字
/**
 * @brief Display single 32x32 Chinese character; 显示单个32x32汉字
 * @param[in] x Display Coordinates; 显示坐标
 * @param[in] y Display Coordinates; 显示坐标
 * @param[in] *s The Chinese character string to be displayed; 要显示的汉字串
 * @param[in] fc Word Color; 字的颜色
 * @param[in] bc Background color of text; 字的背景色
 * @param[in] sizey Font size; 字号
 * @param[in] mode 0 Non-overlapping mode  1 Overlay mode;  0非叠加模式  1叠加模式
 */
void full_port_lcd_show_chinese_32x32(uint16_t x,uint16_t y,uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);//显示单个32x32汉字

/**
 * @brief Display a single character; 显示单个字符
 * @param[in] x Display Coordinates; 显示坐标
 * @param[in] y Display Coordinates; 显示坐标
 * @param[in] num Characters to display; 要显示的字符
 * @param[in] fc Word Color; 字的颜色
 * @param[in] bc Background color of text; 字的背景色
 * @param[in] sizey Font size; 字号
 * @param[in] mode 0 Non-overlapping mode  1 Overlay mode;  0非叠加模式  1叠加模式
 */
void full_port_lcd_show_char(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);//显示单个字符

/**
 * @brief Display String; 显示字符串
 * @param[in] x Display Coordinates; 显示坐标
 * @param[in] y Display Coordinates; 显示坐标
 * @param[in] *p The string to display; 要显示的字符串
 * @param[in] fc Word Color; 字的颜色
 * @param[in] bc Background color of text; 字的背景色
 * @param[in] sizey Font size; 字号
 * @param[in] mode 0 Non-overlapping mode  1 Overlay mode;  0非叠加模式  1叠加模式
 */
void full_port_lcd_show_string(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode);//显示字符串

/**
 * @brief Display integer variables; 显示整数变量
 * @param[in] x Display Coordinates; 显示坐标
 * @param[in] y Display Coordinates; 显示坐标
 * @param[in] inum The integer variable to display; 要显示的整数变量
 * @param[in] len Number of digits to display; 要显示的位数
 * @param[in] fc Word Color; 字的颜色
 * @param[in] bc Background color of text; 字的背景色
 * @param[in] sizey Font size; 字号
 */
void full_port_lcd_show_int(uint16_t x,uint16_t y,uint16_t inum,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey);//显示整数变量

/**
 * @brief Display variables with two decimal places; 显示两位小数变量
 * @param[in] x Display Coordinates; 显示坐标
 * @param[in] y Display Coordinates; 显示坐标
 * @param[in] fnum The decimal variable to display; 要显示的小数变量
 * @param[in] len Number of digits to display; 要显示的位数
 * @param[in] fc Word Color; 字的颜色
 * @param[in] bc Background color of text; 字的背景色
 * @param[in] sizey Font size; 字号
 */
void full_port_lcd_show_float(uint16_t x,uint16_t y,float fnum,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey);//显示两位小数变量

/**
 * @brief Show image; 显示图片
 * @param[in] x Starting point coordinates; 起点坐标
 * @param[in] y Starting point coordinates; 起点坐标
 * @param[in] length Image length; 图片长度
 * @param[in] width Image width; 图片宽度
 * @param[in] pic[]  Array of images; 图片数组
 */
void full_port_lcd_show_picture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[]);//显示图片


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

#endif // __FULL_PORT_LCD_H_