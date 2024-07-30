/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_l1c_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_femc_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_pmp_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_pcfg_drv.h"
#include "lcd8080.h"


#define SRAM_BASE_ADDR 0x48000000U
#define SRAM_SIZE 2

#define TFTReadData() (*(uint8_t *)0x48000001U)
#define Bank1_LCD_DATA ((uint32_t)0x48000001U)
#define Bank1_LCD_REG ((uint32_t)0x48000000U)

/* LCD write data and register */
#define LCD_WR_DATA(value) ((*(__IO uint8_t *)(Bank1_LCD_DATA)) = ((uint8_t)(value))) // 写数据寄存器
#define LCD_WR_REG(index) ((*(__IO uint8_t *)(Bank1_LCD_REG)) = ((uint8_t)index))     // 写命令寄存器

// 设置窗口范围
// X,Y 为起点坐标；width，height为窗口长宽
static void lcd_open_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    LCD_WR_REG(0x2A); // 设置X范围坐标

    LCD_WR_DATA(x >> 8);     // 起始点高8位
    LCD_WR_DATA(x & 0x00FF); // 起始点低8位

    LCD_WR_DATA((x + width - 1) >> 8);     // 结束点高8位
    LCD_WR_DATA((x + width - 1) & 0x00FF); // 结束点低8位

    LCD_WR_REG(0X2B); // 设置Y范围坐标

    LCD_WR_DATA(y >> 8);     // 起始点高8位
    LCD_WR_DATA(y & 0x00FF); // 起始点低8位
    /* pate end */
    LCD_WR_DATA((y + height - 1) >> 8);     // 结束点高8位
    LCD_WR_DATA((y + height - 1) & 0x00FF); // 结束点低8位

    LCD_WR_REG(0x2C); // 开始写入GRAM
}

// 画一个点
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_open_window(y, x, 1, 1); // 设置坐标
    LCD_WR_DATA(color >> 8);     // 颜色高8位
    LCD_WR_DATA(color & 0xFF);   // 颜色低8位
}

// 画一条线
void lcd_draw_line(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color)
{
    uint16_t x = 0, y = 0, k = 0;

    if ((start_x == end_x) && (start_y == end_y)) // 如果是一个点
    {
        lcd_draw_point(start_x, start_y, color);
    }
    else if (abs(end_y - start_y) > abs(end_x - start_x))
    {
        if (start_y > end_y)
        {
            k = start_y;
            start_y = end_y;
            end_y = k;

            k = start_x;
            start_x = end_x;
            end_x = k;
        }
        for (y = start_y; y < end_y; ++y)
        {
            x = (uint16_t)(y - start_y) * (end_x - start_x) / (end_y - start_y) + start_x;
            lcd_draw_point(x, y, color);
        }
    }
    else
    {
        if (start_x > end_x)
        {
            k = start_y;
            start_y = end_y;
            end_y = k;

            k = start_x;
            start_x = end_x;
            end_x = k;
        }
        for (x = start_x; x < end_x; ++x)
        {
            y = (uint16_t)(x - start_x) * (end_y - start_y) / (end_x - start_x) + start_y;
            lcd_draw_point(x, y, color);
        }
    }
}

// 画矩形
void lcd_draw_rectangle(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color)
{
    /*
    top-left coordinate(start_x, start_y), bottom-right coordinate(end_x, end_y)
     '----------------'
     '                '
     '                '
     '                '
     '                '
     '----------------'
    */
    lcd_draw_line(start_x, start_y, start_x, end_y, color);
    lcd_draw_line(start_x, start_y, end_x, start_y, color);
    lcd_draw_line(end_x, start_y, end_x, end_y, color);
    lcd_draw_line(start_x, end_y, end_x, end_y, color);
}

// 画圆，圆心，半径，颜色
void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color)
{
    uint16_t a, b;
    int16_t di;
    a = 0;
    b = radius;
    di = 3 - (radius << 1);
    while (a <= b)
    {
        lcd_draw_point(x - b, y - a, color); // 3
        lcd_draw_point(x + b, y - a, color); // 0
        lcd_draw_point(x - a, y + b, color); // 1
        lcd_draw_point(x - a, y - b, color); // 2
        lcd_draw_point(x + b, y + a, color); // 4
        lcd_draw_point(x + a, y - b, color); // 5
        lcd_draw_point(x + a, y + b, color); // 6
        lcd_draw_point(x - b, y + a, color);
        a++;
        /* Bresenham algorithm */
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
        lcd_draw_point(x + a, y + b, color);
    }
}

void lcd_fill_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    lcd_open_window(x0, y0, x1, y1);
    LCD_WR_REG(0x2c);
    for (uint16_t i = 0; i <= x1 - x0; i++)
    {
        for (uint16_t j = 0; j <= y1 - y0; j++)
        {
            LCD_WR_DATA(color >> 8);   // 颜色高8位
            LCD_WR_DATA(color & 0xFF); // 颜色低8位
        }
    }
}

// 填充整个屏幕
void lcd_fill_screen(uint16_t color)
{
    lcd_fill_rectangle(0, 0, 320, 240, color);
}

// 显示图片1
// width，height 图片长宽
void LCD_DispPicture1(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *str)
{
    lcd_open_window(y, x, height, width);

    uint32_t i, j;
    j = (width) * (height);

    for (i = 0; i < j; i++)
    {
        LCD_WR_DATA(str[2 * i + 1]);
        LCD_WR_DATA(str[2 * i]);
    }
}

void femc_lcd8080_config(void)
{
    uint32_t femc_clk_in_hz = board_init_femc_clock();
    femc_config_t config = {0};
    femc_sram_config_t sram_config = {0};

    femc_default_config(HPM_FEMC, &config);
    config.dqs = FEMC_DQS_INTERNAL;
    femc_init(HPM_FEMC, &config);

    femc_get_typical_sram_config(HPM_FEMC, &sram_config);

    sram_config.oeh_in_ns = 100;
    sram_config.oel_in_ns = 70;
    sram_config.weh_in_ns = 35;
    sram_config.wel_in_ns = 35;
    sram_config.ah_in_ns = 50;
    sram_config.as_in_ns = 10;
    sram_config.ceh_in_ns = 10;
    sram_config.ces_in_ns = 15;

    sram_config.base_address = SRAM_BASE_ADDR;
    sram_config.size_in_byte = SRAM_SIZE;
    sram_config.port_size = FEMC_SRAM_PORT_SIZE_8_BITS;
    femc_config_sram(HPM_FEMC, femc_clk_in_hz, &sram_config);
}

void init_femc_lcd8080_pins(void)
{
    /* Non-MUX */                                                                       /* MUX */
    HPM_IOC->PAD[IOC_PAD_PB18].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* A0 */ /* A16 */

    HPM_IOC->PAD[IOC_PAD_PB00].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* D0 */ /* AD0 */
    HPM_IOC->PAD[IOC_PAD_PA31].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* D1 */ /* AD1 */
    HPM_IOC->PAD[IOC_PAD_PA30].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* D2 */ /* AD2 */
    HPM_IOC->PAD[IOC_PAD_PA29].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* D3 */ /* AD3 */
    HPM_IOC->PAD[IOC_PAD_PA28].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* D4 */ /* AD4 */
    HPM_IOC->PAD[IOC_PAD_PA27].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* D5 */ /* AD5 */
    HPM_IOC->PAD[IOC_PAD_PA26].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* D6 */ /* AD6 */
    HPM_IOC->PAD[IOC_PAD_PA25].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* D7 */ /* AD7 */

    HPM_IOC->PAD[IOC_PAD_PA23].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* #CE */
    HPM_IOC->PAD[IOC_PAD_PB24].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* #OE */
    HPM_IOC->PAD[IOC_PAD_PB25].FUNC_CTL = IOC_PAD_FUNC_CTL_ALT_SELECT_SET(12); /* #WE */

    HPM_IOC->PAD[IOC_PAD_PA20].FUNC_CTL = IOC_PA20_FUNC_CTL_GPIO_A_20;
    HPM_IOC->PAD[IOC_PAD_PB20].FUNC_CTL = IOC_PB20_FUNC_CTL_GPIO_B_20;

    gpio_set_pin_output(HPM_GPIO0, GPIO_DO_GPIOA, 20);
    gpio_write_pin(HPM_GPIO0, GPIO_DO_GPIOA, 20, 0);

    gpio_set_pin_output(HPM_GPIO0, GPIO_DO_GPIOB, 20);
    gpio_write_pin(HPM_GPIO0, GPIO_DO_GPIOB, 20, 0);
}

void tft_driver_st7789_init(void)
{
    gpio_write_pin(HPM_GPIO0, GPIO_DO_GPIOA, 20, 1);
    board_delay_ms(1);
    gpio_write_pin(HPM_GPIO0, GPIO_DO_GPIOA, 20, 0);
    board_delay_ms(10);
    gpio_write_pin(HPM_GPIO0, GPIO_DO_GPIOA, 20, 1);
    board_delay_ms(100);

    //***************************************************************//LCD SETING
    LCD_WR_REG(0x11);
    board_delay_ms(50);
    LCD_WR_REG(0x36);
    LCD_WR_DATA(0x60);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x05); // 65k
    LCD_WR_REG(0x21);
    //--------------------------------ST7789V Frame rate setting----------------------------------//
    LCD_WR_REG(0xB0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF8);

    LCD_WR_REG(0xb2);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x33);

    LCD_WR_REG(0xb7);
    LCD_WR_DATA(0x35);

    //---------------------------------ST7789V Power setting--------------------------------------//
    LCD_WR_REG(0xb8); //
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x2B);
    LCD_WR_DATA(0x2F);

    LCD_WR_REG(0xbb);
    LCD_WR_DATA(0x20); // VCOM

    LCD_WR_REG(0xc0);
    LCD_WR_DATA(0x2c);

    LCD_WR_REG(0xc2);
    LCD_WR_DATA(0x01);

    LCD_WR_REG(0xc3);
    LCD_WR_DATA(0x02);

    LCD_WR_REG(0xc4);
    LCD_WR_DATA(0x20);

    LCD_WR_REG(0xc6);
    LCD_WR_DATA(0x11);

    LCD_WR_REG(0xd0);
    LCD_WR_DATA(0xa4);
    LCD_WR_DATA(0xA1);

    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x03);

    LCD_WR_REG(0xE9);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0x00);

    //--------------------------------ST7789V gamma setting---------------------------------------//
    LCD_WR_REG(0xe0);
    LCD_WR_DATA(0xd0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x13);
    LCD_WR_DATA(0x1c);
    LCD_WR_DATA(0x3a);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x48);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x12);
    LCD_WR_DATA(0x0e);
    LCD_WR_DATA(0x19);
    LCD_WR_DATA(0x1e);

    LCD_WR_REG(0xe1);
    LCD_WR_DATA(0xd0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x25);
    LCD_WR_DATA(0x3a);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x50);
    LCD_WR_DATA(0x3d);
    LCD_WR_DATA(0x1c);
    LCD_WR_DATA(0x1d);
    LCD_WR_DATA(0x1d);
    LCD_WR_DATA(0x1e);

    LCD_WR_REG(0x29); // display on
}

void board_init_pmp_sample(void)
{
    extern uint32_t __noncacheable_start__[];
    extern uint32_t __noncacheable_end__[];
    extern uint32_t __8080_start__[];
    extern uint32_t __8080_end__[];

    uint32_t start_addr = (uint32_t)__noncacheable_start__;
    uint32_t end_addr = (uint32_t)__noncacheable_end__;
    uint32_t length = end_addr - start_addr;
    uint32_t start_addr1 = (uint32_t)__8080_start__;
    uint32_t end_addr1 = (uint32_t)__8080_end__;
    uint32_t length1 = end_addr1 - start_addr1;
    if (length == 0)
    {
        return;
    }
    if (length1 == 0)
    {
        return;
    }
    /* Ensure the address and the length are power of 2 aligned */
    assert((length & (length - 1U)) == 0U);
    assert((start_addr & (length - 1U)) == 0U);
    assert((length1 & (length1 - 1U)) == 0U);
    assert((start_addr1 & (length1 - 1U)) == 0U);
    pmp_entry_t pmp_entry[4] = {0};
    pmp_entry[0].pmp_addr = PMP_NAPOT_ADDR(0x0000000, 0x80000000);
    pmp_entry[0].pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);

    pmp_entry[1].pmp_addr = PMP_NAPOT_ADDR(0x80000000, 0x80000000);
    pmp_entry[1].pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);

    pmp_entry[2].pmp_addr = PMP_NAPOT_ADDR(start_addr, length);
    pmp_entry[2].pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);
    pmp_entry[2].pma_addr = PMA_NAPOT_ADDR(start_addr, length);
    pmp_entry[2].pma_cfg.val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_NON_CACHE_BUF, AMO_EN);

    pmp_entry[3].pmp_addr = PMP_NAPOT_ADDR(start_addr1, length1);
    pmp_entry[3].pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);
    pmp_entry[3].pma_addr = PMA_NAPOT_ADDR(start_addr1, length1);
    pmp_entry[3].pma_cfg.val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_NON_CACHE_BUF, AMO_EN);
    pmp_config(&pmp_entry[0], ARRAY_SIZE(pmp_entry));
}

void board2_print_banner(void)
{
    const uint8_t banner[] = {"\n\
----------------------------------------------------------------------\n\
$$\\   $$\\ $$$$$$$\\  $$\\      $$\\ $$\\\n\
$$ |  $$ |$$  __$$\\ $$$\\    $$$ |\\__|\n\
$$ |  $$ |$$ |  $$ |$$$$\\  $$$$ |$$\\  $$$$$$$\\  $$$$$$\\   $$$$$$\\\n\
$$$$$$$$ |$$$$$$$  |$$\\$$\\$$ $$ |$$ |$$  _____|$$  __$$\\ $$  __$$\\\n\
$$  __$$ |$$  ____/ $$ \\$$$  $$ |$$ |$$ /      $$ |  \\__|$$ /  $$ |\n\
$$ |  $$ |$$ |      $$ |\\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |\n\
$$ |  $$ |$$ |      $$ | \\_/ $$ |$$ |\\$$$$$$$\\ $$ |      \\$$$$$$  |\n\
\\__|  \\__|\\__|      \\__|     \\__|\\__| \\_______|\\__|       \\______/\n\
----------------------------------------------------------------------\n"};
#ifdef SDK_VERSION_STRING
    printf("hpm_sdk: %s\n", SDK_VERSION_STRING);
#endif
    printf("%s", banner);
}

void board_init_sample(void)
{
    pcfg_dcdc_set_voltage(HPM_PCFG, 1100);
    board_init_clock();
    board_init_console();
    board_init_pmp_sample();
#if BOARD_SHOW_BANNER
    board2_print_banner();
#endif
}

int main(void)
{
    // l1c_dc_disable();
    board_init_sample();
    init_femc_lcd8080_pins();
    femc_lcd8080_config();
    tft_driver_st7789_init();

    printf("femc lcd8080 example\n");

    // lcd_draw_circle(100, 100, 100, 0xffff);
    lcd_fill_screen(0x0);

    while (1)
    {
        lcd_fill_screen(0x00);
        board_delay_ms(1000);
        lcd_fill_screen(0xF0F0);
        board_delay_ms(1000);
        lcd_fill_screen(0x0F0F);
        board_delay_ms(1000);
        lcd_fill_screen(0xAF00);
        board_delay_ms(1000);
        lcd_fill_screen(0xFFFF);
        board_delay_ms(1000);
    }
    return 0;
}
