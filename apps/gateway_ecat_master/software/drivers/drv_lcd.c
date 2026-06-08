/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "drv_lcd.h"
#include "drv_lcd_font.h"
static int drv_lcd_spi_init(void)
{
    spi_timing_config_t timing_config = {0};
    spi_format_config_t format_config = {0};

    uint32_t spi_clcok;

    board_init_lcd();

    clock_add_to_group(BOARD_LCD_SPI_CLK_NAME, 0);
    spi_clcok = clock_get_frequency(BOARD_LCD_SPI_CLK_NAME);

    /* set SPI sclk frequency for master */
    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.clk_src_freq_in_hz = spi_clcok;
    timing_config.master_config.sclk_freq_in_hz = BOARD_LCD_SPI_CLK_FREQ;
    if (status_success != spi_master_timing_init(BOARD_LCD_SPI, &timing_config))
    {
        LOG_D("SPI master timming init failed\r\n");
        while (1)
        {
        }
    }
    LOG_D("SPI-Master transfer source clock frequency: %dHz\r\n", timing_config.master_config.clk_src_freq_in_hz);
    LOG_D("SPI-Master transfer sclk frequency: %dHz\r\n", timing_config.master_config.sclk_freq_in_hz);

    /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = (8U);
    format_config.common_config.mode = spi_master_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(BOARD_LCD_SPI, &format_config);

    return 0;
}

void drv_lcd_reset(void)
{
    board_lcd_reset(30);
}

void drv_lcd_d_c_set(void)
{
    board_lcd_d_c(true);
}

void drv_lcd_d_c_clear(void)
{
    board_lcd_d_c(false);
}

void drv_lcd_bl_open(void)
{
    board_lcd_bl(true);
}

void drv_lcd_bl_close(void)
{
    board_lcd_bl(false);
}

static int drv_lcd_spi_writebytes(uint8_t *data, uint16_t len)
{
    hpm_stat_t stat;
    spi_control_config_t control_config = {0};
    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;  /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode = spi_trans_write_only;

    stat = spi_transfer(BOARD_LCD_SPI,
                        &control_config,
                        NULL, NULL,
                        data, len, NULL, 0);

    if (stat == status_success)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

static int drv_lcd_spi_writebyte(uint8_t data)
{
    hpm_stat_t stat;
    spi_control_config_t control_config = {0};
    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;  /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode = spi_trans_write_only;

    stat = spi_transfer(BOARD_LCD_SPI,
                        &control_config,
                        NULL, NULL,
                        (uint8_t *)&data, 1, NULL, 0);

    if (stat == status_success)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
static void drv_lcd_wr_data8(uint8_t dat)
{
    drv_lcd_spi_writebyte(dat);
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
static void drv_lcd_wr_data(uint16_t dat)
{
    drv_lcd_spi_writebyte(dat >> 8);
    drv_lcd_spi_writebyte(dat);
}

/******************************************************************************
      函数说明：在指定区域填充颜色
      入口数据：xsta,ysta   起始坐标
                xend,yend   终止坐标
                                color       要填充的颜色
      返回值：  无
******************************************************************************/
void drv_lcd_full(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint16_t i, j;
    drv_lcd_address_set(xsta, ysta, xend - 1, yend - 1); // 设置显示范围
    for (i = ysta; i < yend; i++)
    {
        for (j = xsta; j < xend; j++)
        {
            drv_lcd_wr_data(color);
        }
    }
}

/******************************************************************************
      函数说明：在指定位置画点
      入口数据：x,y 画点坐标
                color 点的颜色
      返回值：  无
******************************************************************************/
void drv_lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    drv_lcd_address_set(x, y, x, y); // 设置光标位置
    drv_lcd_wr_data(color);
}

/******************************************************************************
      函数说明：画线
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   线的颜色
      返回值：  无
******************************************************************************/
void drv_lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; // 计算坐标增量
    delta_y = y2 - y1;
    uRow = x1; // 画线起点坐标
    uCol = y1;
    if (delta_x > 0)
        incx = 1; // 设置单步方向
    else if (delta_x == 0)
        incx = 0; // 垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0; // 水平线
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
        distance = delta_x; // 选取基本增量坐标轴
    else
        distance = delta_y;
    for (t = 0; t < distance + 1; t++)
    {
        drv_lcd_draw_point(uRow, uCol, color); // 画点
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/******************************************************************************
      函数说明：画矩形
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   矩形的颜色
      返回值：  无
******************************************************************************/
void drv_lcd_draw_rect_angle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    drv_lcd_draw_line(x1, y1, x2, y1, color);
    drv_lcd_draw_line(x1, y1, x1, y2, color);
    drv_lcd_draw_line(x1, y2, x2, y2, color);
    drv_lcd_draw_line(x2, y1, x2, y2, color);
}

/******************************************************************************
      函数说明：画圆
      入口数据：x0,y0   圆心坐标
                r       半径
                color   圆的颜色
      返回值：  无
******************************************************************************/
void drv_lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    a = 0;
    b = r;
    while (a <= b)
    {
        drv_lcd_draw_point(x0 - b, y0 - a, color); // 3
        drv_lcd_draw_point(x0 + b, y0 - a, color); // 0
        drv_lcd_draw_point(x0 - a, y0 + b, color); // 1
        drv_lcd_draw_point(x0 - a, y0 - b, color); // 2
        drv_lcd_draw_point(x0 + b, y0 + a, color); // 4
        drv_lcd_draw_point(x0 + a, y0 - b, color); // 5
        drv_lcd_draw_point(x0 + a, y0 + b, color); // 6
        drv_lcd_draw_point(x0 - b, y0 + a, color); // 7
        a++;
        if ((a * a + b * b) > (r * r)) // 判断要画的点是否过远
        {
            b--;
        }
    }
}

/******************************************************************************
      函数说明：显示汉字串
      入口数据：x,y显示坐标
                *s 要显示的汉字串
                fc 字的颜色
                bc 字的背景色
                sizey 字号 可选 16 24 32
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void drv_lcd_show_chinese(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    while (*s != 0)
    {
        if (sizey == 12)
            drv_lcd_show_chinese_12x12(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 16)
            drv_lcd_show_chinese_16x16(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 24)
            drv_lcd_show_chinese_24x24(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 32)
            drv_lcd_show_chinese_32x32(x, y, s, fc, bc, sizey, mode);
        else
            return;
        s += 1;
        x += sizey/2;
    }
}
#if 1

/******************************************************************************
      函数说明：显示单个12x12汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void drv_lcd_show_chinese_12x12(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    uint8_t i, j, m = 0;
    uint16_t k;
    uint16_t HZnum;       // 汉字数目
    uint16_t TypefaceNum; // 一个字符所占字节大小
    uint16_t x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;

    HZnum = sizeof(tfont12) / sizeof(typFNT_GB12); // 统计汉字数目
    for (k = 0; k < HZnum; k++)
    {
        if ((tfont12[k].Index[0] == *(s)) && (tfont12[k].Index[1] == *(s + 1)))
        {
            drv_lcd_address_set(x, y, x + sizey - 1, y + sizey - 1);
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // 非叠加方式
                    {
                        if (tfont12[k].Msk[i] & (0x01 << j))
                            drv_lcd_wr_data(fc);
                        else
                            drv_lcd_wr_data(bc);
                        m++;
                        if (m % sizey == 0)
                        {
                            m = 0;
                            break;
                        }
                    }
                    else // 叠加方式
                    {
                        if (tfont12[k].Msk[i] & (0x01 << j))
                            drv_lcd_draw_point(x, y, fc); // 画一个点
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
}

/******************************************************************************
      函数说明：显示单个16x16汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void drv_lcd_show_chinese_16x16(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    uint8_t i, j, m = 0;
    uint16_t k;
    uint16_t HZnum;       // 汉字数目
    uint16_t TypefaceNum; // 一个字符所占字节大小
    uint16_t x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont16) / sizeof(typFNT_GB16); // 统计汉字数目
    for (k = 0; k < HZnum; k++)
    {
        if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)))
        {
            drv_lcd_address_set(x, y, x + sizey - 1, y + sizey - 1);
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // 非叠加方式
                    {
                        if (tfont16[k].Msk[i] & (0x01 << j))
                            drv_lcd_wr_data(fc);
                        else
                            drv_lcd_wr_data(bc);
                        m++;
                        if (m % sizey == 0)
                        {
                            m = 0;
                            break;
                        }
                    }
                    else // 叠加方式
                    {
                        if (tfont16[k].Msk[i] & (0x01 << j))
                            drv_lcd_draw_point(x, y, fc); // 画一个点
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
}

/******************************************************************************
      函数说明：显示单个24x24汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void drv_lcd_show_chinese_24x24(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    uint8_t i, j, m = 0;
    uint16_t k;
    uint16_t HZnum;       // 汉字数目
    uint16_t TypefaceNum; // 一个字符所占字节大小
    uint16_t x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont24) / sizeof(typFNT_GB24); // 统计汉字数目
    for (k = 0; k < HZnum; k++)
    {
        if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1)))
        {
            drv_lcd_address_set(x, y, x + sizey - 1, y + sizey - 1);
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // 非叠加方式
                    {
                        if (tfont24[k].Msk[i] & (0x01 << j))
                            drv_lcd_wr_data(fc);
                        else
                            drv_lcd_wr_data(bc);
                        m++;
                        if (m % sizey == 0)
                        {
                            m = 0;
                            break;
                        }
                    }
                    else // 叠加方式
                    {
                        if (tfont24[k].Msk[i] & (0x01 << j))
                            drv_lcd_draw_point(x, y, fc); // 画一个点
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
}

/******************************************************************************
      函数说明：显示单个32x32汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void drv_lcd_show_chinese_32x32(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    uint8_t i, j, m = 0;
    uint16_t k;
    uint16_t HZnum;       // 汉字数目
    uint16_t TypefaceNum; // 一个字符所占字节大小
    uint16_t x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont32) / sizeof(typFNT_GB32); // 统计汉字数目
    for (k = 0; k < HZnum; k++)
    {
        if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1)))
        {
            drv_lcd_address_set(x, y, x + sizey - 1, y + sizey - 1);
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // 非叠加方式
                    {
                        if (tfont32[k].Msk[i] & (0x01 << j))
                            drv_lcd_wr_data(fc);
                        else
                            drv_lcd_wr_data(bc);
                        m++;
                        if (m % sizey == 0)
                        {
                            m = 0;
                            break;
                        }
                    }
                    else // 叠加方式
                    {
                        if (tfont32[k].Msk[i] & (0x01 << j))
                            drv_lcd_draw_point(x, y, fc); // 画一个点
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
}


/******************************************************************************
      函数说明：显示单个字符
      入口数据：x,y显示坐标
                num 要显示的字符
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void drv_lcd_show_char(uint16_t x, uint16_t y, uint8_t num, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    uint8_t temp, sizex, t, m = 0;
    uint16_t i, TypefaceNum; // 一个字符所占字节大小
    uint16_t x0 = x;
    sizex = sizey / 2;
    TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
    num = num - ' ';                                           // 得到偏移后的值
    drv_lcd_address_set(x, y, x + sizex - 1, y + sizey - 1); // 设置光标位置
    for (i = 0; i < TypefaceNum; i++)
    {
        if (sizey == 12)
            temp = ascii_1206[num][i]; // 调用6x12字体
        else if (sizey == 16)
            temp = ascii_1608[num][i]; // 调用8x16字体
        else if (sizey == 24)
            temp = ascii_2412[num][i]; // 调用12x24字体
        else if (sizey == 32)
            temp = ascii_3216[num][i]; // 调用16x32字体
        else
            return;
        for (t = 0; t < 8; t++)
        {
            if (!mode) // 非叠加模式
            {
                if (temp & (0x01 << t))
                    drv_lcd_wr_data(fc);
                else
                    drv_lcd_wr_data(bc);
                m++;
                if (m % sizex == 0)
                {
                    m = 0;
                    break;
                }
            }
            else // 叠加模式
            {
                if (temp & (0x01 << t))
                    drv_lcd_draw_point(x, y, fc); // 画一个点
                x++;
                if ((x - x0) == sizex)
                {
                    x = x0;
                    y++;
                    break;
                }
            }
        }
    }
}
#endif
/******************************************************************************
      函数说明：显示字符串
      入口数据：x,y显示坐标
                *p 要显示的字符串
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void drv_lcd_show_string(uint16_t x, uint16_t y, const uint8_t *p, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    while (*p != '\0')
    {
        drv_lcd_show_char(x, y, *p, fc, bc, sizey, mode);
        x += sizey / 2;
        p++;
    }
}

/******************************************************************************
      函数说明：显示数字
      入口数据：m底数，n指数
      返回值：  无
******************************************************************************/
uint32_t mypow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}

/******************************************************************************
      函数说明：显示整数变量
      入口数据：x,y显示坐标
                num 要显示整数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void drv_lcd_show_int(uint16_t x, uint16_t y, uint16_t inum, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    uint8_t sizex = sizey / 2;
    for (t = 0; t < len; t++)
    {
        temp = (inum / mypow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                drv_lcd_show_char(x + t * sizex, y, ' ', fc, bc, sizey, 0);
                continue;
            }
            else
                enshow = 1;
        }
        drv_lcd_show_char(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
    }
}

/******************************************************************************
      函数说明：显示两位小数变量
      入口数据：x,y显示坐标
                num 要显示小数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void drv_lcd_show_float(uint16_t x, uint16_t y, float fnum, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey)
{
    uint8_t t, temp, sizex;
    uint16_t num1;
    sizex = sizey / 2;
    num1 = fnum * 100;
    for (t = 0; t < len; t++)
    {
        temp = (num1 / mypow(10, len - t - 1)) % 10;
        if (t == (len - 2))
        {
            drv_lcd_show_char(x + (len - 2) * sizex, y, '.', fc, bc, sizey, 0);
            t++;
            len += 1;
        }
        drv_lcd_show_char(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
    }
}

/******************************************************************************
      函数说明：显示图片
      入口数据：x,y起点坐标
                length 图片长度
                width  图片宽度
                pic[]  图片数组
      返回值：  无
******************************************************************************/
void drv_lcd_show_picture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[])
{
    uint16_t i, j;
    uint32_t k = 0;
    drv_lcd_address_set(x, y, x + length - 1, y + width - 1);
    for (i = 0; i < length; i++)
    {
        for (j = 0; j < width; j++)
        {
            drv_lcd_wr_data8(pic[k * 2]);
            drv_lcd_wr_data8(pic[k * 2 + 1]);
            k++;
        }
    }
}

/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void drv_lcd_wr_reg(uint8_t dat)
{
    drv_lcd_d_c_clear(); // 写命令
    drv_lcd_spi_writebyte(dat);
    drv_lcd_d_c_set(); // 写数据
}

/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void drv_lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (USE_HORIZONTAL == 0)
    {
        drv_lcd_wr_reg(0x2a); // 列地址设置
        drv_lcd_wr_data(x1 + 34);
        drv_lcd_wr_data(x2 + 34);
        drv_lcd_wr_reg(0x2b); // 行地址设置
        drv_lcd_wr_data(y1);
        drv_lcd_wr_data(y2);
        drv_lcd_wr_reg(0x2c); // 储存器写
    }
    else if (USE_HORIZONTAL == 1)
    {
        drv_lcd_wr_reg(0x2a); // 列地址设置
        drv_lcd_wr_data(x1 + 34);
        drv_lcd_wr_data(x2 + 34);
        drv_lcd_wr_reg(0x2b); // 行地址设置
        drv_lcd_wr_data(y1);
        drv_lcd_wr_data(y2);
        drv_lcd_wr_reg(0x2c); // 储存器写
    }
    else if (USE_HORIZONTAL == 2)
    {
        drv_lcd_wr_reg(0x2a); // 列地址设置
        drv_lcd_wr_data(x1);
        drv_lcd_wr_data(x2);
        drv_lcd_wr_reg(0x2b); // 行地址设置
        drv_lcd_wr_data(y1 + 34);
        drv_lcd_wr_data(y2 + 34);
        drv_lcd_wr_reg(0x2c); // 储存器写
    }
    else
    {
        drv_lcd_wr_reg(0x2a); // 列地址设置
        drv_lcd_wr_data(x1);
        drv_lcd_wr_data(x2);
        drv_lcd_wr_reg(0x2b); // 行地址设置
        drv_lcd_wr_data(y1 + 34);
        drv_lcd_wr_data(y2 + 34);
        drv_lcd_wr_reg(0x2c); // 储存器写
    }
}

void drv_lcd_init_hw(void)
{
    drv_lcd_spi_init();

    drv_lcd_reset();
    board_delay_ms(50);

    drv_lcd_wr_reg(0x11);

    drv_lcd_wr_reg(0x36);
    if (USE_HORIZONTAL == 0)
        drv_lcd_wr_data8(0x00);
    else if (USE_HORIZONTAL == 1)
        drv_lcd_wr_data8(0xC0);
    else if (USE_HORIZONTAL == 2)
        drv_lcd_wr_data8(0x70);
    else
        drv_lcd_wr_data8(0xA0);

    drv_lcd_wr_reg(0x3A);
    drv_lcd_wr_data8(0x05);

    drv_lcd_wr_reg(0xB2);
    drv_lcd_wr_data8(0x0C);
    drv_lcd_wr_data8(0x0C);
    drv_lcd_wr_data8(0x00);
    drv_lcd_wr_data8(0x33);
    drv_lcd_wr_data8(0x33);

    drv_lcd_wr_reg(0xB7);
    drv_lcd_wr_data8(0x35);

    drv_lcd_wr_reg(0xBB);
    drv_lcd_wr_data8(0x35);

    drv_lcd_wr_reg(0xC0);
    drv_lcd_wr_data8(0x2C);

    drv_lcd_wr_reg(0xC2);
    drv_lcd_wr_data8(0x01);

    drv_lcd_wr_reg(0xC3);
    drv_lcd_wr_data8(0x13);

    drv_lcd_wr_reg(0xC4);
    drv_lcd_wr_data8(0x20);

    drv_lcd_wr_reg(0xC6);
    drv_lcd_wr_data8(0x0F);

    drv_lcd_wr_reg(0xD0);
    drv_lcd_wr_data8(0xA4);
    drv_lcd_wr_data8(0xA1);

    drv_lcd_wr_reg(0xD6);
    drv_lcd_wr_data8(0xA1);

    drv_lcd_wr_reg(0xE0);
    drv_lcd_wr_data8(0xF0);
    drv_lcd_wr_data8(0x00);
    drv_lcd_wr_data8(0x04);
    drv_lcd_wr_data8(0x04);
    drv_lcd_wr_data8(0x04);
    drv_lcd_wr_data8(0x05);
    drv_lcd_wr_data8(0x29);
    drv_lcd_wr_data8(0x33);
    drv_lcd_wr_data8(0x3E);
    drv_lcd_wr_data8(0x38);
    drv_lcd_wr_data8(0x12);
    drv_lcd_wr_data8(0x12);
    drv_lcd_wr_data8(0x28);
    drv_lcd_wr_data8(0x30);

    drv_lcd_wr_reg(0xE1);
    drv_lcd_wr_data8(0xF0);
    drv_lcd_wr_data8(0x07);
    drv_lcd_wr_data8(0x0A);
    drv_lcd_wr_data8(0x0D);
    drv_lcd_wr_data8(0x0B);
    drv_lcd_wr_data8(0x07);
    drv_lcd_wr_data8(0x28);
    drv_lcd_wr_data8(0x33);
    drv_lcd_wr_data8(0x3E);
    drv_lcd_wr_data8(0x36);
    drv_lcd_wr_data8(0x14);
    drv_lcd_wr_data8(0x14);
    drv_lcd_wr_data8(0x29);
    drv_lcd_wr_data8(0x32);

    drv_lcd_wr_reg(0x21);

    drv_lcd_wr_reg(0x11);
    board_delay_ms(120);
    drv_lcd_wr_reg(0x29);

    drv_lcd_full(0, 0, LCD_W, LCD_H, WHITE);
    board_delay_ms(10);
    drv_lcd_show_picture(75,10,170,140, gImage_p);
    drv_lcd_bl_open();
    // drv_lcd_full(0, 0, LCD_W, LCD_H, WHITE);
    // drv_lcd_show_chinese(40, 10, "先楫半导体", RED, WHITE, 32, 0);
}

void drv_lcd_test(void)
{
    static float t = 0;
    drv_lcd_full(0, 0, LCD_W, LCD_H, WHITE);
    drv_lcd_full(0, 0, LCD_W, LCD_H, RED);
    drv_lcd_full(0, 0, LCD_W, LCD_H, GREEN);
    drv_lcd_full(0, 0, LCD_W, LCD_H, BLUE);
    drv_lcd_full(0, 0, LCD_W, LCD_H, WHITE);
    drv_lcd_show_string(10, 33, (const uint8_t*)"LCD_W:", RED, WHITE, 32, 0);
    drv_lcd_show_int(106, 33, LCD_W, 3, RED, WHITE, 32);
    drv_lcd_show_string(10, 66, (const uint8_t*)"LCD_H:", RED, WHITE, 32, 0);
    drv_lcd_show_int(106, 66, LCD_H, 3, RED, WHITE, 32);
    drv_lcd_show_float(10, 99, t, 4, RED, WHITE, 32);
    t += 0.11;
}



ATTR_RAMFUNC uint32_t lvgl_tick_get_cb(void)
{
    return osKernelGetTickCount();
}

ATTR_RAMFUNC void lvgl_delay_cb(uint32_t ms)
{
    osDelay(ms);
}

void lvgl_tick_init(void)
{
    lv_tick_set_cb(lvgl_tick_get_cb);
    lv_delay_set_cb(lvgl_delay_cb);
}

// 刷新显示回调函数
static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    // 将LVGL的缓冲区数据发送到ST7789
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    drv_lcd_address_set(area->x1, area->y1, area->x2, area->y2);
    
    // 发送像素数据
    drv_lcd_spi_writebytes(px_map, w*h*2);
    // for(int i = 0; i < w*h; i++)
    // {
    //     drv_lcd_spi_writebyte(px_map[i*2]);
    //     drv_lcd_spi_writebyte(px_map[i*2+1]);
    // }
    // printf("flush area: %d, %d, %d, %d\n", area->x1, area->y1, area->x2, area->y2);
    // drv_lcd_show_chinese(40, 100, "先楫半导体", RED, WHITE, 32, 0);
    // if(area->x1 == 287 && area->y1 == 4 && area->x2 == 315 && area->y2 == 30)
    // {
    //     for(int i = 0; i < h; i++)
    //     {   
    //         for(int j = 0; j < w; j++)
    //         {
    //             printf("%02x ", ((uint16_t*)px_map)[i*w+j]);
    //         }
    //         printf("\r\n");
    //     }
    // }

        // for(int i = 0; i < 27; i++)
        // {   
        //     for(int j = 0; j < 29; j++)
        //     {
        //         printf("%02x ", ((uint16_t*)px_map)[(i+4)*320+j+287]);
        //     }
        //     printf("\r\n");
        // }
    // printf("\r\n");
    // 通知LVGL刷新完成
    lv_display_flush_ready(disp);
}
// 分配绘制缓冲区
// 建议使用双缓冲提高性能
ATTR_PLACE_AT_NONCACHEABLE_INIT_WITH_ALIGNMENT(4) static lv_color16_t buf1[LCD_W*LCD_H/10]; // 10行高的缓冲区
ATTR_PLACE_AT_NONCACHEABLE_INIT_WITH_ALIGNMENT(4) static lv_color16_t buf2[LCD_W*LCD_H/10];
// LVGL显示接口初始化
void lv_port_disp_init(void) {
    // 创建显示设备
    lv_display_t *disp = lv_display_create(LCD_W, LCD_H);
    
    // 设置刷新回调函数
    lv_display_set_flush_cb(disp, disp_flush);
    
    // 设置缓冲区
    lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);//LV_DISPLAY_RENDER_MODE_FULL
    
    // 设置颜色深度（16位色）
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
}

void drv_lcd_init(void)
{
    drv_lcd_init_hw();
    osDelay(1000);
    lvgl_tick_init();
    lv_port_disp_init();
}