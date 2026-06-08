/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "drv_oled.h"
#include "drv_oled_font.h"
#if USE_U8G2
u8g2_t u8g2;
#endif
void drv_oled_write_byte(uint8_t byte, bool is_data)
{
    hpm_stat_t stat;
    spi_control_config_t control_config = {0};

    if (is_data) {
        DRV_OLED_DC_SET();
    } else {
        DRV_OLED_DC_RESET();
    }
    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;  /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode = spi_trans_write_only;

    stat = spi_transfer(BOARD_OLED_SPI,
                &control_config,
                NULL, NULL,
                &byte, 1, NULL, 0);

    if (stat != status_success) {
        LOG_E("SPI transfer error[%d]!\n", stat);
    }

    DRV_OLED_DC_SET();
}

void drv_oled_write_bytes(uint8_t *buf, uint16_t len)
{
    hpm_stat_t stat;
    spi_control_config_t control_config = {0};

    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;  /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode = spi_trans_write_only;

    stat = spi_transfer(BOARD_OLED_SPI,
                &control_config,
                NULL, NULL,
                buf, len, NULL, 0);

    if (stat != status_success) {
        LOG_E("SPI transfer error[%d]!\n", stat);
    }

}

void drv_oled_write_data(uint8_t* data, uint32_t size)
{
    hpm_stat_t stat;
    spi_control_config_t control_config = {0};
    dma_handshake_config_t config;

    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.common_config.tx_dma_enable = true;
    control_config.common_config.trans_mode = spi_trans_write_only;

    stat = spi_setup_dma_transfer(BOARD_OLED_SPI,
                        &control_config,
                        NULL, NULL,
                        size, 0);
    if (stat != status_success) {
        LOG_E("spi setup dma transfer failed\n");
        while (1) {
        }
    }

    dmamux_config(BOARD_OLED_DMAMUX, BOARD_OLED_DMAMUX_CH, BOARD_OLED_SPI_DMA, true);

    dma_default_handshake_config(BOARD_OLED_HDMA, &config);
    config.ch_index = BOARD_OLED_SPI_DMA_CH;
    config.dst = (uint32_t)&BOARD_OLED_SPI->DATA;
    config.dst_fixed = true;
    config.src = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)data);
    config.src_fixed = false;
    config.data_width = DMA_TRANSFER_WIDTH_BYTE;
    config.size_in_byte = size;
    stat = dma_setup_handshake(BOARD_OLED_HDMA, &config, true);

    if (stat != status_success) {
        LOG_E("spi tx trigger dma failed\n");
        while (1) {
        }
    }
}

/* -------------------------- 显示控制 -------------------------- */
// 反显控制（i=0:正常，i=1:反色）
void drv_oled_color_turn(uint8_t i)
{
    if(i == 0)
        drv_oled_write_byte(0xA6, DRV_OLED_CMD); // 正常显示
    else if(i == 1)
        drv_oled_write_byte(0xA7, DRV_OLED_CMD); // 反色显示
}

// 屏幕旋转180度（i=0:正常，i=1:旋转）
void drv_oled_display_turn(uint8_t i)
{
    if(i == 0)
    {
        drv_oled_write_byte(0xC8, DRV_OLED_CMD); // 正常扫描
        drv_oled_write_byte(0xA1, DRV_OLED_CMD); // 正常段映射
    }
    else if(i == 1)
    {
        drv_oled_write_byte(0xC0, DRV_OLED_CMD); // 反转扫描
        drv_oled_write_byte(0xA0, DRV_OLED_CMD); // 反转段映射
    }
}

// 开启OLED显示
void drv_oled_display_on(void)
{
    drv_oled_write_byte(0x8D, DRV_OLED_CMD); // 电荷泵使能
    drv_oled_write_byte(0x14, DRV_OLED_CMD); // 开启电荷泵
    drv_oled_write_byte(0xAF, DRV_OLED_CMD); // 点亮屏幕
}

// 关闭OLED显示
void drv_oled_display_off(void)
{
    drv_oled_write_byte(0x8D, DRV_OLED_CMD); // 电荷泵使能
    drv_oled_write_byte(0x10, DRV_OLED_CMD); // 关闭电荷泵
    drv_oled_write_byte(0xAE, DRV_OLED_CMD); // 关闭屏幕
}

// 清屏函数
void drv_oled_clear(void)
{
    uint8_t i, n;
    for(i = 0; i < 8; i++)
    {
        drv_oled_write_byte(0xb0 + i, DRV_OLED_CMD); // 设置页地址
        drv_oled_write_byte(0x10, DRV_OLED_CMD);     // 列地址高4位
        drv_oled_write_byte(0x00, DRV_OLED_CMD);     // 列地址低4位
        for(n = 0; n < 128; n++)
        {
            drv_oled_write_byte(0x00, DRV_OLED_DATA); // 写入空数据
        }
    }
}

// 设置显示起始地址（x:列，y:页）
void drv_oled_set_address(uint8_t x, uint8_t y)
{
    drv_oled_write_byte(0xb0 + y, DRV_OLED_CMD);              // 页地址
    drv_oled_write_byte(((x&0xf0)>>4)|0x10, DRV_OLED_CMD);    // 列地址高4位
    drv_oled_write_byte((x&0x0f), DRV_OLED_CMD);              // 列地址低4位
}

/* -------------------------- 点阵显示 -------------------------- */
// 显示128x64点阵图像
void drv_oled_display_128x64(uint8_t *dp)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        drv_oled_set_address(0, i);
        drv_oled_write_data(dp, 128);
        dp += 128;
        board_delay_us(128);
    }
}

// 显示8x16点阵（ASCII/自定义字符）
void drv_oled_display_8x16(uint8_t x, uint8_t y, uint8_t *dp)
{
    drv_oled_set_address(x, y);
    drv_oled_write_data(dp, 8);
    board_delay_us(4);
    drv_oled_set_address(x, y+1);
    drv_oled_write_data(dp+8, 8);
    board_delay_us(4);
}


void drv_oled_show_count(uint32_t count)
{
    uint8_t ge,shi,bai,qian,wan,shiwan,baiwan;
    
    if(count >= 1000000) {
        ge = count % 10;
        shi = count / 10 % 10;
        bai = count / 100 % 10;
        qian = count / 1000 % 10;
        wan = count / 10000 % 10;
        shiwan = count / 100000 % 10;
        baiwan = count / 1000000 % 10;
        drv_oled_display_8x16(112, 3, f8x16[ge+16]);
        drv_oled_display_8x16(96, 3, f8x16[shi+16]);
        drv_oled_display_8x16(80, 3, f8x16[bai+16]);
        drv_oled_display_8x16(64, 3, f8x16[qian+16]);
        drv_oled_display_8x16(48, 3, f8x16[wan+16]);
        drv_oled_display_8x16(32, 3, f8x16[shiwan+16]);
        drv_oled_display_8x16(16, 3, f8x16[baiwan+16]);
    } else if(count >= 100000) {
        ge = count % 10;
        shi = count / 10 % 10;
        bai = count / 100 % 10;
        qian = count / 1000 % 10;
        wan = count / 10000 % 10;
        shiwan = count / 100000 % 10;
        drv_oled_display_8x16(112, 3, f8x16[ge+16]);
        drv_oled_display_8x16(96, 3, f8x16[shi+16]);
        drv_oled_display_8x16(80, 3, f8x16[bai+16]);
        drv_oled_display_8x16(64, 3, f8x16[qian+16]);
        drv_oled_display_8x16(48, 3, f8x16[wan+16]);
        drv_oled_display_8x16(32, 3, f8x16[shiwan+16]);
    } else if(count >= 10000) {
        ge = count % 10;
        shi = count / 10 % 10;
        bai = count / 100 % 10;
        qian = count / 1000 % 10;
        wan = count / 10000 % 10;
        drv_oled_display_8x16(112, 3, f8x16[ge+16]);
        drv_oled_display_8x16(96, 3, f8x16[shi+16]);
        drv_oled_display_8x16(80, 3, f8x16[bai+16]);
        drv_oled_display_8x16(64, 3, f8x16[qian+16]);
        drv_oled_display_8x16(48, 3, f8x16[wan+16]);
    } else if(count >= 1000) {
        ge = count % 10;
        shi = count / 10 % 10;
        bai = count / 100 % 10;
        qian = count / 1000 % 10;
        drv_oled_display_8x16(112, 3, f8x16[ge+16]);
        drv_oled_display_8x16(96, 3, f8x16[shi+16]);
        drv_oled_display_8x16(80, 3, f8x16[bai+16]);
        drv_oled_display_8x16(64, 3, f8x16[qian+16]);
    } else if(count >= 100) {
        ge = count % 10;
        shi = count / 10 % 10;
        bai = count / 100 % 10;
        drv_oled_display_8x16(112, 3, f8x16[bai+16]);
        drv_oled_display_8x16(96, 3, f8x16[shi+16]);
        drv_oled_display_8x16(80, 3, f8x16[ge+16]);
    } else if(count >= 10) {
        ge = count % 10;
        shi = count / 10 % 10;
        drv_oled_display_8x16(112, 3, f8x16[ge+16]);
        drv_oled_display_8x16(96, 3, f8x16[shi+16]);
    } else {
        ge = count % 10;
        drv_oled_display_8x16(112, 3, f8x16[ge+16]);
    }
}
#if USE_U8G2
uint8_t drv_u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    (void)u8x8;
    (void)arg_ptr;
  
    switch(msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;

        case U8X8_MSG_DELAY_MILLI:
        board_delay_ms(arg_int);
        break;

        case U8X8_MSG_GPIO_DC:
        gpio_write_pin(BOARD_OLED_D_C_CTRL, BOARD_OLED_D_C_INDEX, BOARD_OLED_D_C_PIN, arg_int);
        break;

        case U8X8_MSG_GPIO_RESET:
        gpio_write_pin(BOARD_OLED_RESET_CTRL, BOARD_OLED_RESET_INDEX, BOARD_OLED_RESET_PIN, arg_int);
        break;

        default:
        break;
    }
    return 1;
}

// 4线SPI：字节发送（硬件SPI，必须实现！）
uint8_t drv_u8x8_byte_4wire_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    (void)u8x8;
    uint8_t *data;
    switch(msg)
    {
       case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;

        // if(arg_int < 32)
            drv_oled_write_bytes(data, arg_int);
        // else {
        //     drv_oled_write_data(data, arg_int);
        //     board_delay_us(arg_int*4);
        // }
        break;

       case U8X8_MSG_BYTE_SET_DC:
        gpio_write_pin(BOARD_OLED_D_C_CTRL, BOARD_OLED_D_C_INDEX, BOARD_OLED_D_C_PIN, arg_int);
        break;

       default:
       break;
    }
    return 1;
}
#endif

void drv_oled_init(void)
{
    spi_timing_config_t timing_config = {0};
    spi_format_config_t format_config = {0};
    
    board_init_oled();
    DRV_OLED_RES_RESET();
    board_delay_ms(100);
    DRV_OLED_RES_SET();
    board_delay_ms(10);

    uint32_t spi_clock = clock_get_frequency(BOARD_OLED_SPI_CLK_NAME);

    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.clk_src_freq_in_hz = spi_clock;
    timing_config.master_config.sclk_freq_in_hz = BOARD_OLED_SPI_CLK_FREQ;
    if (status_success != spi_master_timing_init(BOARD_OLED_SPI, &timing_config)) {
        LOG_E("SPI master timing init failed\n");
        while (1) {
        }
    }

    /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = 8U;
    format_config.common_config.lsb = false;
    format_config.common_config.mode = spi_master_mode;
    format_config.common_config.cpol = spi_sclk_low_idle;
    format_config.common_config.cpha = spi_sclk_sampling_odd_clk_edges;
    spi_format_init(BOARD_OLED_SPI, &format_config);

    drv_oled_write_byte(0xAE, DRV_OLED_CMD); // 关闭显示
    drv_oled_write_byte(0xAE, DRV_OLED_CMD); // 关闭显示
    drv_oled_write_byte(0xAE, DRV_OLED_CMD); // 关闭显示
    drv_oled_write_byte(0xD5, DRV_OLED_CMD); // 设置时钟分频因子
    drv_oled_write_byte(0x80, DRV_OLED_CMD); // 分频因子=1
    drv_oled_write_byte(0xA8, DRV_OLED_CMD); // 设置多路复用率
    drv_oled_write_byte(0x3F, DRV_OLED_CMD); // 64行显示
    drv_oled_write_byte(0xD3, DRV_OLED_CMD); // 设置显示偏移
    drv_oled_write_byte(0x00, DRV_OLED_CMD); // 偏移0
    drv_oled_write_byte(0x40, DRV_OLED_CMD); // 设置显示起始行
    drv_oled_write_byte(0x8D, DRV_OLED_CMD); // 电荷泵设置
    drv_oled_write_byte(0x14, DRV_OLED_CMD); // 开启电荷泵
    drv_oled_write_byte(0x20, DRV_OLED_CMD); // 设置内存地址模式
    drv_oled_write_byte(0x00, DRV_OLED_CMD); // 水平寻址模式
    drv_oled_write_byte(0xA1, DRV_OLED_CMD); // 段重映射（0->127）
    drv_oled_write_byte(0xC8, DRV_OLED_CMD); // COM扫描方向（反向）
    drv_oled_write_byte(0xDA, DRV_OLED_CMD); // 设置COM引脚硬件配置
    drv_oled_write_byte(0x12, DRV_OLED_CMD); 
    drv_oled_write_byte(0x81, DRV_OLED_CMD); // 对比度设置
    drv_oled_write_byte(0xCF, DRV_OLED_CMD); // 对比度值（0-255）
    drv_oled_write_byte(0xD9, DRV_OLED_CMD); // 设置预充电周期
    drv_oled_write_byte(0xF1, DRV_OLED_CMD); 
    drv_oled_write_byte(0xDB, DRV_OLED_CMD); // VCOMH电压倍率
    drv_oled_write_byte(0x40, DRV_OLED_CMD); 
    drv_oled_write_byte(0xA4, DRV_OLED_CMD); // 全局显示开启（显示RAM内容）
    drv_oled_write_byte(0xA6, DRV_OLED_CMD); // 正常显示（0=黑，1=白）
    drv_oled_write_byte(0xAF, DRV_OLED_CMD); // 开启显示
    drv_oled_clear();
    drv_oled_display_128x64(hpm_logo);
#if USE_U8G2
    // SPI 4线 SSD1306 128x64 驱动
    u8g2_Setup_ssd1306_128x64_noname_f(
    &u8g2,
    U8G2_R0,
    drv_u8x8_byte_4wire_spi,   // SPI 收发函数
    drv_u8x8_gpio_and_delay // GPIO+延时函数
    );
    // u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
    // u8g2_DrawStrX2(&u8g2, 0, 16, "HPMicro");
    // u8g2_SendBuffer(&u8g2);
#endif
}
