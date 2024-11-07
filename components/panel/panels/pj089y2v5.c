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
    uint8_t cmd[2];
} mipi_cmd_list_t;

static const mipi_cmd_list_t mipi_panel_cmd[] = {
    {2, {0xB0, 0x00}},
    {2, {0xB2, 0x02}},
    {2, {0xB3, 0x03}},
    {2, {0xB4, 0x2C}},
    {2, {0xB6, 0x80}},
    {2, {0xBA, 0x43}},
    {2, {0xB8, 0xC0}},
    {2, {0xBB, 0xB7}},
    {2, {0xBE, 0x23}},
    {2, {0xBF, 0x1E}},
    {2, {0xCB, 0x1F}},
    {2, {0xCC, 0x08}},
    {2, {0xCD, 0x1F}},
    {2, {0xCE, 0x08}},
    {2, {0xB0, 0x01}},
    {2, {0xC0, 0x0E}},
    {2, {0xC1, 0x22}},
    {2, {0xC2, 0x20}},
    {2, {0xC3, 0x1E}},
    {2, {0xC4, 0x1C}},
    {2, {0xC5, 0x0C}},
    {2, {0xC6, 0x0A}},
    {2, {0xC7, 0x08}},
    {2, {0xC8, 0x06}},
    {2, {0xC9, 0x02}},
    {2, {0xCA, 0x00}},
    {2, {0xCB, 0x0F}},
    {2, {0xCC, 0x00}},
    {2, {0xCD, 0x25}},
    {2, {0xCE, 0x11}},
    {2, {0xCF, 0x12}},
    {2, {0xD0, 0x00}},
    {2, {0xD1, 0x00}},
    {2, {0xD2, 0x00}},
    {2, {0xD3, 0x00}},
    {2, {0xD4, 0x00}},
    {2, {0xD5, 0x00}},
    {2, {0xD6, 0x0D}},
    {2, {0xD7, 0x21}},
    {2, {0xD8, 0x1F}},
    {2, {0xD9, 0x1D}},
    {2, {0xDA, 0x1B}},
    {2, {0xDB, 0x0B}},
    {2, {0xDC, 0x09}},
    {2, {0xDD, 0x07}},
    {2, {0xDE, 0x05}},
    {2, {0xDF, 0x01}},
    {2, {0xE0, 0x00}},
    {2, {0xE1, 0x10}},
    {2, {0xE2, 0x00}},
    {2, {0xE3, 0x25}},
    {2, {0xE4, 0x11}},
    {2, {0xE5, 0x12}},
    {2, {0xE6, 0x00}},
    {2, {0xE7, 0x00}},
    {2, {0xE8, 0x00}},
    {2, {0xE9, 0x00}},
    {2, {0xEA, 0x00}},
    {2, {0xEB, 0x00}},
    {2, {0xEC, 0xC0}},
    {2, {0xB0, 0x02}},
    {2, {0xC0, 0x00}},
    {2, {0xC1, 0x00}},
    {2, {0xC2, 0x0F}},
    {2, {0xC3, 0x17}},
    {2, {0xC4, 0x29}},
    {2, {0xC5, 0x27}},
    {2, {0xC6, 0x28}},
    {2, {0xC7, 0x28}},
    {2, {0xC8, 0x26}},
    {2, {0xC9, 0x23}},
    {2, {0xCA, 0x28}},
    {2, {0xCB, 0x27}},
    {2, {0xCC, 0x28}},
    {2, {0xCD, 0x25}},
    {2, {0xCE, 0x2F}},
    {2, {0xCF, 0x2D}},
    {2, {0xD0, 0x32}},
    {2, {0xD1, 0x32}},
    {2, {0xD2, 0x34}},
    {2, {0xD3, 0x32}},
    {2, {0xD4, 0x3A}},
    {2, {0xD5, 0x1F}},
    {2, {0xD6, 0x00}},
    {2, {0xD7, 0x00}},
    {2, {0xD8, 0x0F}},
    {2, {0xD9, 0x17}},
    {2, {0xDA, 0x29}},
    {2, {0xDB, 0x27}},
    {2, {0xDC, 0x28}},
    {2, {0xDD, 0x28}},
    {2, {0xDE, 0x26}},
    {2, {0xDF, 0x23}},
    {2, {0xE0, 0x28}},
    {2, {0xE1, 0x27}},
    {2, {0xE2, 0x28}},
    {2, {0xE3, 0x25}},
    {2, {0xE4, 0x2F}},
    {2, {0xE5, 0x2D}},
    {2, {0xE6, 0x32}},
    {2, {0xE7, 0x32}},
    {2, {0xE8, 0x34}},
    {2, {0xE9, 0x32}},
    {2, {0xEA, 0x3A}},
    {2, {0xEB, 0x1F}},
    {2, {0xB0, 0x03}},
    {2, {0xC0, 0x04}},
    {2, {0xC1, 0x12}},
    {2, {0xC8, 0x10}},
    {2, {0xC9, 0x10}},
    {2, {0xCC, 0xB7}},
    {2, {0xC2, 0x1A}},
    {2, {0xC3, 0x1A}},
    {2, {0xC4, 0x1A}},
    {2, {0xC5, 0x1A}},
    {2, {0xDD, 0x1A}},
    {2, {0xDE, 0x1A}},
    {2, {0xE6, 0x1A}},
    {2, {0xE7, 0x1A}},
    {2, {0xD4, 0x04}},
    {2, {0xDB, 0x00}},
    {2, {0xD2, 0x07}},
    {2, {0xD6, 0x0C}},
    {2, {0xD5, 0x05}},
    {2, {0xD3, 0x07}},
    {2, {0xD7, 0x0C}},
    {2, {0xCF, 0x00}},
    {2, {0xCB, 0x00}},
    {2, {0xCE, 0x0E}},
    {2, {0xCD, 0x01}},
    {2, {0xCA, 0x41}},
    {2, {0xB0, 0x06}},
    {2, {0xB8, 0xA5}},
    {2, {0xC0, 0xA5}},
    {2, {0xD5, 0x5A}},
    {2, {0x36, 0x00}},
    {2, {0x3A, 0x70}},
    {1, {0x11}},
    {1, {0x29}}
};

static void mipi_panel_init_cmd_send(hpm_panel_t *panel)
{
    int ret;
    MIPI_DSI_Type *mipi_host = panel->hw_if.video.mipi.mipi_host_base;
    uint8_t rdata;

    ret = mipi_dsi_dcs_read(mipi_host, 0, 0xB2, &rdata, 1);
    if (ret <= 0) {
        printf("read [0x%02X]: 0x%02X -- failed\n", 0xB2, rdata);
        while (1) {
        }
    } else {
        printf("read [0x%02X]: 0x%02X -- ok\n", 0xB2, rdata);
    }

    int mipi_cmd_num = sizeof(mipi_panel_cmd) / sizeof(mipi_panel_cmd[0]);
    for (int i = 0; i < mipi_cmd_num; i++) {
        if(i == mipi_cmd_num - 1)
        {
            printf("last!\r\n");
            hpm_panel_delay_ms(120);
        }
        ret = mipi_dsi_dcs_write_buffer(mipi_host, 0, mipi_panel_cmd[i].cmd, mipi_panel_cmd[i].len);
        if (ret <= 0)
            printf("mipi_cmd[%d].cmd: 0x%02X -- failed\n", i, mipi_panel_cmd[i].cmd[0]);
    }

    hpm_panel_delay_ms(20);
    ret = mipi_dsi_dcs_read(mipi_host, 0, 0xB2, &rdata, 1);
    if (ret <= 0) {
        printf("read [0x%02X]: 0x%02X -- failed\n", 0xB2, rdata);
        while (1) {
        }
    } else {
        printf("read [0x%02X]: 0x%02X -- ok\n", 0xB2, rdata);
    }


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
    hpm_panel_delay_ms(10);

    panel->hw_if.set_reset_pin_level(0);
    hpm_panel_delay_ms(10);

    panel->hw_if.set_reset_pin_level(1);
    hpm_panel_delay_ms(10);
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

hpm_panel_t panel_pj089y2v5_1 = {
    .name = "pj089y2v5_1",
    .if_type = HPM_PANEL_IF_TYPE_MIPI,
    .timing = {
        .pixel_clock_khz = 81000,
        .hactive = 640,
        .hfront_porch = 80,
        .hback_porch = 80,
        .hsync_len = 24,

        .vactive = 2400,
        .vfront_porch = 16,
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

hpm_panel_t panel_pj089y2v5_2 = {
    .name = "pj089y2v5_2",
    .if_type = HPM_PANEL_IF_TYPE_MIPI,
    .timing = {
        .pixel_clock_khz = 81000,
        .hactive = 640,
        .hfront_porch = 80,
        .hback_porch = 80,
        .hsync_len = 24,

        .vactive = 2400,
        .vfront_porch = 16,
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

