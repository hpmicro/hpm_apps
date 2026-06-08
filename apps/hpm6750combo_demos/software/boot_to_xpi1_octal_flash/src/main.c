/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_romapi.h"
#include "rom_image.h"

#include <hpm_bootheader.h>
#include <sys/stat.h>

typedef void (*app_entry_t)(void);

xpi_nor_config_option_t config_option;
xpi_nor_config_t config_block;

uint32_t image_buf[8192/sizeof(uint32_t)];

ATTR_RAMFUNC void boot_from_xpi1_octal_flash(void);

int main(void)
{
    board_init();
    clock_add_to_group(clock_xpi1, 0);
    printf("Boot from XPI1 NOR FLASH...\n");
    boot_from_xpi1_octal_flash();
}


bool handle_boot_image(const boot_image_hdr_t *image_hdr);


void fallback_to_isp(void);

void boot_from_xpi1_octal_flash(void)
{
    config_option.header.U = 0xfcf90002;
#if 1
    config_option.option0.freq_opt = 7;
    config_option.option0.probe_type = xpi_nor_xccela_ddr;
    config_option.option0.cmd_pads_after_init = XPI_8PADS;
    config_option.option0.cmd_pads_after_por = XPI_1PAD;
#else
    config_option.option0.freq_opt = 1;
#endif
    config_option.option1.io_voltage = 1;
    config_option.option1.pin_group_sel = 1;

    hpm_stat_t status = rom_xpi_nor_auto_config(HPM_XPI1, &config_block, &config_option);
    if (status != status_success) {
        config_option.option0.cmd_pads_after_por = XPI_8PADS;
        status = rom_xpi_nor_auto_config(HPM_XPI1, &config_block, &config_option);
        if (status != status_success) {
            printf("FLASH initialization failed, fall back to ISP mode...\n");
            fallback_to_isp();
        }
    }
    HPM_IOC->PAD[IOC_PAD_PC20].PAD_CTL = IOC_PAD_PAD_CTL_DS_SET(7) | IOC_PAD_PAD_CTL_MS_SET(1);
    printf("Boot from XPI1 NOR FLASH...\n");


    memcpy(image_buf, (void*)0x90001000, sizeof(image_buf));
    const boot_image_hdr_t *boot_hdr = (boot_image_hdr_t *)image_buf;
    handle_boot_image(boot_hdr);
    fallback_to_isp();
}

bool handle_boot_image(const boot_image_hdr_t *image_hdr)
{
    do {
        HPM_BREAK_IF ((image_hdr->tag != IMAGE_HEADER_TAG) || (image_hdr->version != IMAGE_HEADER_VERSION) || (image_hdr->number_of_fw == 0) || (image_hdr->number_of_fw > 2));
        const fw_info_table_t *fw_info_table = (const fw_info_table_t *)(((uint32_t)image_hdr) + sizeof(boot_image_hdr_t));
        app_entry_t app_entry = NULL;
        if ((fw_info_table->load_addr >= 0x90000000) && (fw_info_table->load_addr < 0x9fffffff)) {
            /* XiP Image */
        } else {
            const uint8_t *copy_start = (const uint8_t *)(((uint32_t)fw_info_table) + fw_info_table->offset);
            memcpy((void*)fw_info_table->load_addr, copy_start, fw_info_table->size);
        }
        app_entry = (app_entry_t)fw_info_table->entry_point;

        printf("Jump to application entry point 0x%08x\n", (uint32_t)app_entry);
        app_entry();

    }while (false);

    return false;
}

void fallback_to_isp(void)
{
    api_boot_arg_t boot_arg = { 0 };
    boot_arg.tag = 0xEB;
    boot_arg.src = API_BOOT_SRC_ISP;
    boot_arg.peripheral = API_BOOT_PERIPH_AUTO;
    ROM_API_TABLE_ROOT->run_bootloader(&boot_arg);
    while (1) {
    }
}