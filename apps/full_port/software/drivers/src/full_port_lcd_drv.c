/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "full_port_config_drv.h"
#include "full_port_lcd_font_drv.h"

static int full_port_lcdspi_init(void)
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
        FULL_PORT_LOG_INFO("SPI master timming init failed\n");
        while (1)
        {
        }
    }
    FULL_PORT_LOG_INFO("SPI-Master transfer source clock frequency: %dHz\n", timing_config.master_config.clk_src_freq_in_hz);
    FULL_PORT_LOG_INFO("SPI-Master transfer sclk frequency: %dHz\n", timing_config.master_config.sclk_freq_in_hz);

    /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = (8U);
    format_config.common_config.mode = spi_master_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(BOARD_LCD_SPI, &format_config);

    return 0;
}

void full_port_lcd_reset(void)
{
    board_lcd_reset(30);
}

void full_port_lcd_d_c_set(void)
{
    board_lcd_d_c(true);
}

void full_port_lcd_d_c_clear(void)
{
    board_lcd_d_c(false);
}

void full_port_lcd_bl_open(void)
{
    board_lcd_bl(true);
}

void full_port_lcd_bl_close(void)
{
    board_lcd_bl(false);
}

static int full_port_lcd_spi_writebyte(uint8_t data)
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
static void full_port_wr_data8(uint8_t dat)
{
    full_port_lcd_spi_writebyte(dat);
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
static void full_port_wr_data(uint16_t dat)
{
    full_port_lcd_spi_writebyte(dat >> 8);
    full_port_lcd_spi_writebyte(dat);
}

/******************************************************************************
      函数说明：在指定区域填充颜色
      入口数据：xsta,ysta   起始坐标
                xend,yend   终止坐标
                                color       要填充的颜色
      返回值：  无
******************************************************************************/
void full_port_lcd_full(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint16_t i, j;
    full_port_lcd_address_set(xsta, ysta, xend - 1, yend - 1); // 设置显示范围
    for (i = ysta; i < yend; i++)
    {
        for (j = xsta; j < xend; j++)
        {
            full_port_wr_data(color);
        }
    }
}

/******************************************************************************
      函数说明：在指定位置画点
      入口数据：x,y 画点坐标
                color 点的颜色
      返回值：  无
******************************************************************************/
void hpm_lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    full_port_lcd_address_set(x, y, x, y); // 设置光标位置
    full_port_wr_data(color);
}

/******************************************************************************
      函数说明：画线
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   线的颜色
      返回值：  无
******************************************************************************/
void full_port_lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
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
        hpm_lcd_draw_point(uRow, uCol, color); // 画点
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
void full_port_lcd_draw_rect_angle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    full_port_lcd_draw_line(x1, y1, x2, y1, color);
    full_port_lcd_draw_line(x1, y1, x1, y2, color);
    full_port_lcd_draw_line(x1, y2, x2, y2, color);
    full_port_lcd_draw_line(x2, y1, x2, y2, color);
}

/******************************************************************************
      函数说明：画圆
      入口数据：x0,y0   圆心坐标
                r       半径
                color   圆的颜色
      返回值：  无
******************************************************************************/
void full_port_lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    a = 0;
    b = r;
    while (a <= b)
    {
        hpm_lcd_draw_point(x0 - b, y0 - a, color); // 3
        hpm_lcd_draw_point(x0 + b, y0 - a, color); // 0
        hpm_lcd_draw_point(x0 - a, y0 + b, color); // 1
        hpm_lcd_draw_point(x0 - a, y0 - b, color); // 2
        hpm_lcd_draw_point(x0 + b, y0 + a, color); // 4
        hpm_lcd_draw_point(x0 + a, y0 - b, color); // 5
        hpm_lcd_draw_point(x0 + a, y0 + b, color); // 6
        hpm_lcd_draw_point(x0 - b, y0 + a, color); // 7
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
void full_port_lcd_show_chinese(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    while (*s != 0)
    {
        if (sizey == 12)
            full_port_lcd_show_chinese_12x12(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 16)
            full_port_lcd_show_chinese_16x16(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 24)
            full_port_lcd_show_chinese_24x24(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 32)
            full_port_lcd_show_chinese_32x32(x, y, s, fc, bc, sizey, mode);
        else
            return;
        s += 1;
        x += sizey/2;
    }
}

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
void full_port_lcd_show_chinese_12x12(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
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
            full_port_lcd_address_set(x, y, x + sizey - 1, y + sizey - 1);
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // 非叠加方式
                    {
                        if (tfont12[k].Msk[i] & (0x01 << j))
                            full_port_wr_data(fc);
                        else
                            full_port_wr_data(bc);
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
                            hpm_lcd_draw_point(x, y, fc); // 画一个点
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
void full_port_lcd_show_chinese_16x16(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
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
            full_port_lcd_address_set(x, y, x + sizey - 1, y + sizey - 1);
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // 非叠加方式
                    {
                        if (tfont16[k].Msk[i] & (0x01 << j))
                            full_port_wr_data(fc);
                        else
                            full_port_wr_data(bc);
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
                            hpm_lcd_draw_point(x, y, fc); // 画一个点
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
void full_port_lcd_show_chinese_24x24(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
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
            full_port_lcd_address_set(x, y, x + sizey - 1, y + sizey - 1);
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // 非叠加方式
                    {
                        if (tfont24[k].Msk[i] & (0x01 << j))
                            full_port_wr_data(fc);
                        else
                            full_port_wr_data(bc);
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
                            hpm_lcd_draw_point(x, y, fc); // 画一个点
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
void full_port_lcd_show_chinese_32x32(uint16_t x, uint16_t y, uint8_t *s, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
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
            full_port_lcd_address_set(x, y, x + sizey - 1, y + sizey - 1);
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // 非叠加方式
                    {
                        if (tfont32[k].Msk[i] & (0x01 << j))
                            full_port_wr_data(fc);
                        else
                            full_port_wr_data(bc);
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
                            hpm_lcd_draw_point(x, y, fc); // 画一个点
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
void full_port_lcd_show_char(uint16_t x, uint16_t y, uint8_t num, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    uint8_t temp, sizex, t, m = 0;
    uint16_t i, TypefaceNum; // 一个字符所占字节大小
    uint16_t x0 = x;
    sizex = sizey / 2;
    TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
    num = num - ' ';                                           // 得到偏移后的值
    full_port_lcd_address_set(x, y, x + sizex - 1, y + sizey - 1); // 设置光标位置
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
                    full_port_wr_data(fc);
                else
                    full_port_wr_data(bc);
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
                    hpm_lcd_draw_point(x, y, fc); // 画一个点
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
void full_port_lcd_show_string(uint16_t x, uint16_t y, const uint8_t *p, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
    while (*p != '\0')
    {
        full_port_lcd_show_char(x, y, *p, fc, bc, sizey, mode);
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
void full_port_lcd_show_int(uint16_t x, uint16_t y, uint16_t inum, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey)
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
                full_port_lcd_show_char(x + t * sizex, y, ' ', fc, bc, sizey, 0);
                continue;
            }
            else
                enshow = 1;
        }
        full_port_lcd_show_char(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
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
void full_port_lcd_show_float(uint16_t x, uint16_t y, float fnum, uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey)
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
            full_port_lcd_show_char(x + (len - 2) * sizex, y, '.', fc, bc, sizey, 0);
            t++;
            len += 1;
        }
        full_port_lcd_show_char(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
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
void full_port_lcd_show_picture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[])
{
    uint16_t i, j;
    uint32_t k = 0;
    full_port_lcd_address_set(x, y, x + length - 1, y + width - 1);
    for (i = 0; i < length; i++)
    {
        for (j = 0; j < width; j++)
        {
            full_port_wr_data8(pic[k * 2]);
            full_port_wr_data8(pic[k * 2 + 1]);
            k++;
        }
    }
}

/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void full_port_lcd_wr_reg(uint8_t dat)
{
    full_port_lcd_d_c_clear(); // 写命令
    full_port_lcd_spi_writebyte(dat);
    full_port_lcd_d_c_set(); // 写数据
}

/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void full_port_lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (USE_HORIZONTAL == 0)
    {
        full_port_lcd_wr_reg(0x2a); // 列地址设置
        full_port_wr_data(x1 + 34);
        full_port_wr_data(x2 + 34);
        full_port_lcd_wr_reg(0x2b); // 行地址设置
        full_port_wr_data(y1);
        full_port_wr_data(y2);
        full_port_lcd_wr_reg(0x2c); // 储存器写
    }
    else if (USE_HORIZONTAL == 1)
    {
        full_port_lcd_wr_reg(0x2a); // 列地址设置
        full_port_wr_data(x1 + 34);
        full_port_wr_data(x2 + 34);
        full_port_lcd_wr_reg(0x2b); // 行地址设置
        full_port_wr_data(y1);
        full_port_wr_data(y2);
        full_port_lcd_wr_reg(0x2c); // 储存器写
    }
    else if (USE_HORIZONTAL == 2)
    {
        full_port_lcd_wr_reg(0x2a); // 列地址设置
        full_port_wr_data(x1);
        full_port_wr_data(x2);
        full_port_lcd_wr_reg(0x2b); // 行地址设置
        full_port_wr_data(y1 + 34);
        full_port_wr_data(y2 + 34);
        full_port_lcd_wr_reg(0x2c); // 储存器写
    }
    else
    {
        full_port_lcd_wr_reg(0x2a); // 列地址设置
        full_port_wr_data(x1);
        full_port_wr_data(x2);
        full_port_lcd_wr_reg(0x2b); // 行地址设置
        full_port_wr_data(y1 + 34);
        full_port_wr_data(y2 + 34);
        full_port_lcd_wr_reg(0x2c); // 储存器写
    }
}

void full_port_lcd_init(void)
{
    full_port_lcdspi_init();

    full_port_lcd_reset();
    board_delay_ms(50);

    full_port_lcd_wr_reg(0x11);

    full_port_lcd_wr_reg(0x36);
    if (USE_HORIZONTAL == 0)
        full_port_wr_data8(0x00);
    else if (USE_HORIZONTAL == 1)
        full_port_wr_data8(0xC0);
    else if (USE_HORIZONTAL == 2)
        full_port_wr_data8(0x70);
    else
        full_port_wr_data8(0xA0);

    full_port_lcd_wr_reg(0x3A);
    full_port_wr_data8(0x05);

    full_port_lcd_wr_reg(0xB2);
    full_port_wr_data8(0x0C);
    full_port_wr_data8(0x0C);
    full_port_wr_data8(0x00);
    full_port_wr_data8(0x33);
    full_port_wr_data8(0x33);

    full_port_lcd_wr_reg(0xB7);
    full_port_wr_data8(0x35);

    full_port_lcd_wr_reg(0xBB);
    full_port_wr_data8(0x35);

    full_port_lcd_wr_reg(0xC0);
    full_port_wr_data8(0x2C);

    full_port_lcd_wr_reg(0xC2);
    full_port_wr_data8(0x01);

    full_port_lcd_wr_reg(0xC3);
    full_port_wr_data8(0x13);

    full_port_lcd_wr_reg(0xC4);
    full_port_wr_data8(0x20);

    full_port_lcd_wr_reg(0xC6);
    full_port_wr_data8(0x0F);

    full_port_lcd_wr_reg(0xD0);
    full_port_wr_data8(0xA4);
    full_port_wr_data8(0xA1);

    full_port_lcd_wr_reg(0xD6);
    full_port_wr_data8(0xA1);

    full_port_lcd_wr_reg(0xE0);
    full_port_wr_data8(0xF0);
    full_port_wr_data8(0x00);
    full_port_wr_data8(0x04);
    full_port_wr_data8(0x04);
    full_port_wr_data8(0x04);
    full_port_wr_data8(0x05);
    full_port_wr_data8(0x29);
    full_port_wr_data8(0x33);
    full_port_wr_data8(0x3E);
    full_port_wr_data8(0x38);
    full_port_wr_data8(0x12);
    full_port_wr_data8(0x12);
    full_port_wr_data8(0x28);
    full_port_wr_data8(0x30);

    full_port_lcd_wr_reg(0xE1);
    full_port_wr_data8(0xF0);
    full_port_wr_data8(0x07);
    full_port_wr_data8(0x0A);
    full_port_wr_data8(0x0D);
    full_port_wr_data8(0x0B);
    full_port_wr_data8(0x07);
    full_port_wr_data8(0x28);
    full_port_wr_data8(0x33);
    full_port_wr_data8(0x3E);
    full_port_wr_data8(0x36);
    full_port_wr_data8(0x14);
    full_port_wr_data8(0x14);
    full_port_wr_data8(0x29);
    full_port_wr_data8(0x32);

    full_port_lcd_wr_reg(0x21);

    full_port_lcd_wr_reg(0x11);
    board_delay_ms(120);
    full_port_lcd_wr_reg(0x29);

    full_port_lcd_full(0, 0, LCD_W, LCD_H, WHITE);
    board_delay_ms(10);
    full_port_lcd_show_picture(75,10,170,140,gImage_p);
    full_port_lcd_bl_open();
    // full_port_lcd_full(0, 0, LCD_W, LCD_H, WHITE);
    // full_port_lcd_show_chinese(40, 10, "先楫半导体", RED, WHITE, 32, 0);
}

void full_port_lcd_test(void)
{
    static float t = 0;
    full_port_lcd_full(0, 0, LCD_W, LCD_H, WHITE);
    full_port_lcd_full(0, 0, LCD_W, LCD_H, RED);
    full_port_lcd_full(0, 0, LCD_W, LCD_H, GREEN);
    full_port_lcd_full(0, 0, LCD_W, LCD_H, BLUE);
    full_port_lcd_full(0, 0, LCD_W, LCD_H, WHITE);
    full_port_lcd_show_string(10, 33, (const uint8_t*)"LCD_W:", RED, WHITE, 32, 0);
    full_port_lcd_show_int(106, 33, LCD_W, 3, RED, WHITE, 32);
    full_port_lcd_show_string(10, 66, (const uint8_t*)"LCD_H:", RED, WHITE, 32, 0);
    full_port_lcd_show_int(106, 66, LCD_H, 3, RED, WHITE, 32);
    full_port_lcd_show_float(10, 99, t, 4, RED, WHITE, 32);
    t += 0.11;
}