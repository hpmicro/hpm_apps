/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "foe.h"
#include "applInterface.h"
#include "hpm_ecat_hw.h"
#include "hpm_ecat_foe.h"
#include "hpm_flash.h"
#include "ota_port.h"

extern bool foe_reset_request;

int hpm_ota_init(void)
{
    hpm_stat_t stat;
    int ret = hpm_flash_init();
    if (ret != 0)
    {
        printf("BAD! flash init fail:%d\r\n", ret);
        while (1)
        {
        }
    }

    board_init_ethercat(HPM_ESC); /* init ESC function pins */
    board_init_switch_led();      /* init switch and led for ECAT display */
    printf("ECAT FOE Funcation\n");

    /* Config ESC with FOE function to download app */
    stat = ecat_hardware_init(HPM_ESC);
    if (stat != status_success) {
        printf("Init ESC peripheral and related devices(EEPROM/PHY) failed!\n");
        return -1;
    }

    MainInit(); /* SSC Initialize the stack */

    stat = foe_support_init();
    if (stat != status_success) {
        printf("FOE support init failed!!\n");
        return -1;
    }

    pAPPL_FoeRead = foe_read;
    pAPPL_FoeReadData = foe_read_data;
    pAPPL_FoeWrite = foe_write;
    pAPPL_FoeWriteData = foe_write_data;

#if defined(ESC_EEPROM_EMULATION) && ESC_EEPROM_EMULATION
    pAPPL_EEPROM_Read  = ecat_eeprom_emulation_read;
    pAPPL_EEPROM_Write = ecat_eeprom_emulation_write;
    pAPPL_EEPROM_Reload = ecat_eeprom_emulation_reload;
    pAPPL_EEPROM_Store  = ecat_eeprom_emulation_store;
#endif

    /* Create basic mapping */
    APPL_GenerateMapping(&nPdInputSize, &nPdOutputSize);

    /* Set stack run flag */
    bRunApplication = TRUE;

    return 0;
}

void hpm_ota_polling_handle(void)
{
    if(bRunApplication == TRUE) {
        if (foe_reset_request) {
            printf("system reset...\n");
            foe_support_soc_reset();
        }
        MainLoop();
    }
}