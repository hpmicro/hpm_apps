/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_panel.h"
#include "hpm_pixelmux_drv.h"
#include "hpm_mipi_dsi_drv.h"
#include "hpm_mipi_dsi_phy_drv.h"
#include <stdio.h>

typedef struct mipi_cmd_list {
    uint16_t len;
    uint8_t cmd[4];
} mipi_cmd_list_t;

static const mipi_cmd_list_t mipi_panel_cmd[] = {
    {2, {0xff,0x25}},               /*-------------------- CMD2 Page 5 --------------------*/
    {2, {0xfb,0x01}},               /*RELOAD_CMD2P5 = 1 */
    {2, {0x38,0x02}},               /*OSC tracking function*/
    {2, {0xff,0xe0}},               /*-------------------- CMD3 --------------------*/
    {2, {0xfb,0x01}},               /*RELOAD_CMD3PA = 1*/
    {2, {0x50,0x0f}},               /*Power Off setting CABC STILL PWM*/
    {2, {0x80,0x20}},               /*CABC CMB Dimming Control*/
    {2, {0xff,0x10}},               /*-------------------- CMD1 --------------------*/
    {2, {0xfb,0x01}},               /*RELOAD_CMD1 = 1*/
    {2, {0x2f,0x01}},               /*FCON[1:0] = 1*/
    {3, {0x51,0x0f,0xff}},          /*DBV[11:0] = FFFh*/ 
    {2, {0x53,0x24}},               /*BCTRL = 1,DD = 1*/
    {2, {0xbb,0x03}},               /*RM = 0,DM[1:0] = 3 (Video bypass RAM)*/
    {2, {0xc0,0x00}},               /*scaling_mode[2:0] = 0,Compression_Method[2:0] = 0 (RAW)*/
    {1, {0x11}},                    /**/
    {1, {0x29}},                    /**/
};
//#define HPM_ZE099DA_BIST_MODE

static const mipi_cmd_list_t mipi_sleep_cmd[] = {
    {2, {0xff, 0x10}},
    {1, {0x28}},
    //delay 150
    {1, {0x10}},
    //delay 120
    //bist
    {2, {0xff, 0x25}},
    {2, {0x58, 0x2f}},
    {2, {0x5e, 0x01}},
};

static void mipi_panel_init_cmd_send(hpm_panel_t *panel)
{
    int ret;
    MIPI_DSI_Type *mipi_host = panel->hw_if.video.mipi.mipi_host_base;
    uint8_t rdata;

    hpm_panel_delay_ms(120);

#ifdef HPM_ZE099DA_BIST_MODE
    int mipi_cmd_num = sizeof(mipi_sleep_cmd) / sizeof(mipi_sleep_cmd[0]);
    for (int i = 0; i < mipi_cmd_num; i++) {
        if(i == 2)
        {
            hpm_panel_delay_ms(150);
        }
        else if(i == 3)
        {
            hpm_panel_delay_ms(120);
        }
        
        ret = mipi_dsi_dcs_write_buffer(mipi_host, 0, mipi_sleep_cmd[i].cmd, mipi_sleep_cmd[i].len);
        if (ret <= 0)
            printf("mipi_cmd[%d].cmd: 0x%02X -- failed\n", i, mipi_sleep_cmd[i].cmd[0]);
    }
#else

    int mipi_cmd_num = sizeof(mipi_panel_cmd) / sizeof(mipi_panel_cmd[0]);
    for (int i = 0; i < mipi_cmd_num; i++) {
        if(i == mipi_cmd_num - 1)
        {
            printf("last!\r\n");
            hpm_panel_delay_ms(150);
        }
        ret = mipi_dsi_dcs_write_buffer(mipi_host, 0, mipi_panel_cmd[i].cmd, mipi_panel_cmd[i].len);
        if (ret <= 0)
            printf("mipi_cmd[%d].cmd: 0x%02X -- failed\n", i, mipi_panel_cmd[i].cmd[0]);
    }

    hpm_panel_delay_ms(20);
    ret = mipi_dsi_dcs_read(mipi_host, 0, 0x38, &rdata, 1);
    if (ret <= 0) {
        printf("read [0x%02X]: 0x%02X -- failed\n", 0x38, rdata);
        //while (1) {
        //}
    } else {
        printf("read [0x%02X]: 0x%02X -- ok\n", 0x38, rdata);
    }
#endif

}

static void mipi_panel_host_init(hpm_panel_t *panel)
{
    MIPI_DSI_Type *mipi_host = panel->hw_if.video.mipi.mipi_host_base;
    const hpm_panel_timing_t *timing = &panel->timing;
    mipi_dsi_config_t mipi_cfg;
    mipi_dsi_get_defconfig_on_video(&mipi_cfg);

    mipi_cfg.channel = 0;
    mipi_cfg.lanes = 4;
    mipi_cfg.video_mode = MIPI_DSI_VIDEO_MODE_BURST;
    mipi_cfg.lane_mbps = 800;
    mipi_cfg.video_para.pixel_clock_khz = panel->hw_if.lcdc_pixel_clk_khz;
    mipi_cfg.video_para.hactive = timing->hactive;
    mipi_cfg.video_para.hsync_len = timing->hsync_len;
    mipi_cfg.video_para.hback_porch = timing->hback_porch;
    mipi_cfg.video_para.hfront_porch = timing->hfront_porch;
    mipi_cfg.video_para.vsync_len = timing->vsync_len;
    mipi_cfg.video_para.vactive = timing->vactive;
    mipi_cfg.video_para.vback_porch = timing->vback_porch;
    mipi_cfg.video_para.vfront_porch = timing->vfront_porch;

    mipi_dsi_init(mipi_host, &mipi_cfg);
}

static void mipi_panel_phy_init(hpm_panel_t *panel)
{
    MIPI_DSI_Type *mipi_host = panel->hw_if.video.mipi.mipi_host_base;
    MIPI_DSI_PHY_Type *mipi_phy = panel->hw_if.video.mipi.mipi_phy_base;

    mipi_dsi_phy_config_t mipi_phy_cfg = {
        .lanes = 4,
        .lane_mbps = 800
    };
    mipi_dsi_phy_powerdown(mipi_host);
    mipi_dsi_phy_init(mipi_phy, &mipi_phy_cfg);
    mipi_dsi_phy_poweron(mipi_host);
}

static void reset(hpm_panel_t *panel)
{
    if (!panel->hw_if.set_reset_pin_level)
        return;
    panel->hw_if.set_reset_pin_level(1);
    hpm_panel_delay_ms(20);

    panel->hw_if.set_reset_pin_level(0);
    hpm_panel_delay_ms(20);

    panel->hw_if.set_reset_pin_level(1);
    hpm_panel_delay_ms(20);
}

static void init(hpm_panel_t *panel)
{
    if (panel->hw_if.set_video_router)
        panel->hw_if.set_video_router();

    mipi_panel_host_init(panel);
    mipi_panel_phy_init(panel);
    mipi_panel_init_cmd_send(panel);
}

static void power_on(hpm_panel_t *panel)
{
    MIPI_DSI_Type *mipi_host = panel->hw_if.video.mipi.mipi_host_base;

    if (panel->state.power_state != HPM_PANEL_STATE_POWER_ON) {
        mipi_dsi_video_mode_hs_transfer_enable(mipi_host);

        if (panel->hw_if.set_backlight) {
            if (panel->state.backlight_percent == 0)
                panel->state.backlight_percent = 100;
            panel->hw_if.set_backlight(panel->state.backlight_percent);
        }
        panel->state.power_state = HPM_PANEL_STATE_POWER_ON;
    }
}

static void power_off(hpm_panel_t *panel)
{
    MIPI_DSI_Type *mipi_host = panel->hw_if.video.mipi.mipi_host_base;

    if (panel->state.power_state != HPM_PANEL_STATE_POWER_OFF) {
        if (panel->hw_if.set_backlight)
            panel->hw_if.set_backlight(0);
        mipi_dsi_video_mode_hs_transfer_disable(mipi_host);
        panel->state.power_state = HPM_PANEL_STATE_POWER_OFF;
    }
}

hpm_panel_t panel_ze099da_1 = {
    .name = "ze099da_1",
    .if_type = HPM_PANEL_IF_TYPE_MIPI,
    .timing = {
        .pixel_clock_khz = 400000,
        .hactive = 3840,
        .hfront_porch = 172,
        .hback_porch = 56,
        .hsync_len = 16,

        .vactive = 5120,
        .vfront_porch = 18,
        .vback_porch = 20,
        .vsync_len = 2,
    },
    .funcs = {
        .reset = reset,
        .init = init,
        .power_on = power_on,
        .power_off = power_off,
    },
};

hpm_panel_t panel_ze099da_2 = {
    .name = "ze099da_2",
    .if_type = HPM_PANEL_IF_TYPE_MIPI,
    .timing = {
        .pixel_clock_khz = 400000,
        .hactive = 3840,
        .hfront_porch = 172,
        .hback_porch = 56,
        .hsync_len = 16,

        .vactive = 5120,
        .vfront_porch = 18,
        .vback_porch = 20,
        .vsync_len = 2,
    },
    .funcs = {
        .reset = reset,
        .init = init,
        .power_on = power_on,
        .power_off = power_off,
    },
};

