/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "gw_ecat.h"

extern bool foe_reset_request;

void gw_ecat_thread(void *argument)
{
    (void)argument;
    hpm_stat_t stat;
    uint8_t al_status_last;

    board_init_ethercat(HPM_ESC); /* init ESC function pins */
    board_init_switch_led();      /* init switch and led for ECAT display */

    /* Config ESC with FOE function to download app */
    stat = ecat_hardware_init(HPM_ESC);
    if (stat != status_success) {
        LOG_E("Init ESC peripheral and related devices(EEPROM/PHY) failed!\r\n");
        return;
    }

    MainInit(); /* SSC Initialize the stack */

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
    /* Execute the stack */
    while (bRunApplication == TRUE) {
        MainLoop();

        gw_mq_msg mq_msg;
        mq_msg.msg_id = GW_MQ_MSG_MISC_LED_ID;
        if(al_status_last != nAlStatus && nAlStatus == STATE_INIT) {
            mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_LED_DEV_STATE_INIT_ID;
            gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 10);
        } else if(al_status_last != nAlStatus && nAlStatus == STATE_PREOP) {
            mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_LED_DEV_STATE_PRE_OP_ID;
            gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 10);
        } else if(al_status_last != nAlStatus && nAlStatus == STATE_BOOT) {
            mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_LED_DEV_STATE_BOOTSTRAP_ID;
            gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 10);
        } else if(al_status_last != nAlStatus && nAlStatus == STATE_SAFEOP) {
            mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_LED_DEV_STATE_SAFE_OP_ID;
            gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 10);
        } else if(al_status_last != nAlStatus && nAlStatus == STATE_OP) {
            mq_msg.submsg_id = GW_MQ_SUBMSG_MISC_LED_DEV_STATE_OP_ID;
            gw_mq_send(GW_MQ_MISC_ID, &mq_msg, 10);
        }
        
        al_status_last = nAlStatus;

        osDelay(1);
    }
}
