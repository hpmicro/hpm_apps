/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
//#include "foe.h"
#include "ecat_def.h"
#include "ecatappl.h"
#include "ecatslv.h"
#include "applInterface.h"
#include "hpm_ecat_hw.h"
#include "hpm_ecat_foe.h"

#include "ecat_foe.h"
#include "cia402appl.h"
#if defined(CONFIG_CIA402_USING_ACTUAL_MOTOR) && CONFIG_CIA402_USING_ACTUAL_MOTOR
#include "pmsm_init.h"
#if USE_HPMMONITOR
#include "monitor.h"
#endif
#endif


extern bool foe_reset_request;

int hpm_ecat_foe_init(void)
{
    return 0;
}
int hpm_ecat_foe_cia402_init(void)
{
    hpm_stat_t stat;
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
    CiA402_Init();
    /* Create basic mapping */
    APPL_GenerateMapping(&nPdInputSize, &nPdOutputSize);

    /* Set stack run flag */
    bRunApplication = TRUE; 

    return 0;
}
int hpm_ecat_foe_task(void)
{
    int res;
    res = hpm_ecat_foe_cia402_init();
    
#if defined(CONFIG_CIA402_USING_ACTUAL_MOTOR) && CONFIG_CIA402_USING_ACTUAL_MOTOR
    motor_function_init();
#if USE_HPMMONITOR
    monitor_init();
#endif
    
    printf("ecat cia402 with actual motor!\n");
    //printf("version:%x.%x\n",BUILD_VERSION,SUB_VERSION);
#endif
    
    /* Execute the stack */
    while (bRunApplication == TRUE) {
        if (foe_reset_request) {
            printf("system reset...\n");
            foe_support_soc_reset();
        }
        MainLoop();
#if defined(CONFIG_CIA402_USING_ACTUAL_MOTOR) && CONFIG_CIA402_USING_ACTUAL_MOTOR
#if USE_HPMMONITOR
        monitor_handle();
#endif
    

#endif
    }  
    /* hardware deinit */
    CiA402_DeallocateAxis();
    return res;
}

