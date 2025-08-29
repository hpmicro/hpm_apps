/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "global_config.h"

static uint8_t s_u8ITFlagForKeyA = false;
static uint8_t s_u8ITFlagForKeyB = false;
static uint8_t s_u8ITFlagForKeyC = false;
static uint8_t s_u8ITFlagForKeyD = false;
static uint16_t s_u16TimeOutFor10MsCount = 0;
static uint16_t s_u16TimeOutFor100MsCount = 0;
static uint16_t s_u16TimeOutFor1000MsCount = 0;
/* key filter */
static uint8_t s_u8TimeOutForKeyA = 0;
static uint8_t s_u8TimeOutForKeyB = 0;
static uint8_t s_u8TimeOutForKeyC = 0;
static uint8_t s_u8TimeOutForKeyD = 0;	

static void hpm_a_irq_timer_handler_cb(void)
{
	s_u16TimeOutFor10MsCount++;
	s_u16TimeOutFor100MsCount++;
	s_u16TimeOutFor1000MsCount++;

	if(s_u16TimeOutFor10MsCount>=10) {
		s_u16TimeOutFor10MsCount = 10;
	}
	if(s_u16TimeOutFor100MsCount>=100) {
		s_u16TimeOutFor100MsCount = 100;
	}
	if(s_u16TimeOutFor1000MsCount>=1000) {
		s_u16TimeOutFor1000MsCount = 1000;
	}

		

	if(s_u8ITFlagForKeyA) {
		s_u8TimeOutForKeyA++;
		if(s_u8TimeOutForKeyA >= 50) {
			s_u8TimeOutForKeyA = 50;
			s_u8ITFlagForKeyA = false;
			if(full_port_key_get_status(FULL_PORT_KEY_INDEX_KEYA_ID)) {
				s_u8TimeOutForKeyA = 0;
			}
		}
	}

	if(s_u8ITFlagForKeyB) {
		s_u8TimeOutForKeyB++;
		if(s_u8TimeOutForKeyB >= 50) {
			s_u8TimeOutForKeyB = 50;
			s_u8ITFlagForKeyB = false;
			if(full_port_key_get_status(FULL_PORT_KEY_INDEX_KEYB_ID)) {
				s_u8TimeOutForKeyB = 0;
			}
		}
	}

	if(s_u8ITFlagForKeyC) {
		s_u8TimeOutForKeyC++;
		if(s_u8TimeOutForKeyC >= 50) {
			s_u8TimeOutForKeyC = 50;
			s_u8ITFlagForKeyC = false;
			if(full_port_key_get_status(FULL_PORT_KEY_INDEX_KEYC_ID)) {
				s_u8TimeOutForKeyC = 0;
			}
		}
	}

	if(s_u8ITFlagForKeyD) {
		s_u8TimeOutForKeyD++;
		if(s_u8TimeOutForKeyD >= 50) {
			s_u8TimeOutForKeyD = 50;
			s_u8ITFlagForKeyD = false;
			if(full_port_key_get_status(FULL_PORT_KEY_INDEX_KEYD_ID)) {
				s_u8TimeOutForKeyD = 0;
			}
		}
	}
}

static void hpm_a_irq_keya_handler_cb(void)
{
	s_u8ITFlagForKeyA = true;
}

static void hpm_a_irq_keyb_handler_cb(void)
{
	s_u8ITFlagForKeyB = true;
}

static void hpm_a_irq_keyc_handler_cb(void)
{
	s_u8ITFlagForKeyC = true;
}

static void hpm_a_irq_keyd_handler_cb(void)
{
	s_u8ITFlagForKeyD = true;
}

int main(void)
{
	uint8_t u8Port1Mac[6] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
	uint8_t u8Port2Mac[6] = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22};
	uint8_t u8Port3Mac[6] = {0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
	uint16_t u16Value;
	hpm_stat_t stat;
	
    board_init();
    board_timer_create(1, hpm_a_irq_timer_handler_cb);
    board_init_ethercat(HPM_ESC); /* init ESC function pins */
    board_init_switch_led();      /* init switch and led for ECAT display */

    full_port_lcd_init();
    full_port_led_init();
    full_port_key_init();
	
    /* hardware init */
    stat = ecat_hardware_init(HPM_ESC);
    if (stat != status_success) {
        printf("Init ESC peripheral and related devices(EEPROM/PHY) failed!\n");
        return 0;
    }

    MainInit(); /* SSC Initilize the stack */

#if defined(ESC_EEPROM_EMULATION) && ESC_EEPROM_EMULATION
    pAPPL_EEPROM_Read  = ecat_eeprom_emulation_read;
    pAPPL_EEPROM_Write = ecat_eeprom_emulation_write;
    pAPPL_EEPROM_Reload = ecat_eeprom_emulation_reload;
    pAPPL_EEPROM_Store  = ecat_eeprom_emulation_store;
#endif

    /* Create basic mapping */
    APPL_GenerateMapping(&nPdInputSize, &nPdOutputSize);

	full_port_irq_keya_init(hpm_a_irq_keya_handler_cb);
	full_port_irq_keyb_init(hpm_a_irq_keyb_handler_cb);
	full_port_irq_keyc_init(hpm_a_irq_keyc_handler_cb);
	full_port_irq_keyd_init(hpm_a_irq_keyd_handler_cb);


    /* Set stack run flag */
    bRunApplication = TRUE;
    while (bRunApplication == TRUE)
    {
		MainLoop();
        if(s_u16TimeOutFor1000MsCount>=1000) {
			s_u16TimeOutFor1000MsCount = 0;
            full_port_led_ctrl(FULL_PORT_LED_INDEX_LEDA_ID, FULL_PORT_LED_CTRL_TOGGLE);
        }

        if(s_u16TimeOutFor100MsCount>=100) {
			s_u16TimeOutFor100MsCount = 0;
            full_port_led_ctrl(FULL_PORT_LED_INDEX_LEDB_ID, FULL_PORT_LED_CTRL_TOGGLE);
        }

        if(s_u16TimeOutFor10MsCount>=10) {
			s_u16TimeOutFor10MsCount = 0;

        }

        if(s_u8TimeOutForKeyA>=50) {
			s_u8TimeOutForKeyA = 0;
			FULL_PORT_LOG_INFO("KEYA\r\n");
			FULL_PORT_LOG_INFO("10M\r\n");
        }
        if(s_u8TimeOutForKeyB>=50) {
			s_u8TimeOutForKeyB = 0;
			FULL_PORT_LOG_INFO("KEYB\r\n");
			FULL_PORT_LOG_INFO("100M\r\n");
        }
        if(s_u8TimeOutForKeyC>=50) {
			s_u8TimeOutForKeyC = 0;
			FULL_PORT_LOG_INFO("KEYC\r\n");
			FULL_PORT_LOG_INFO("1000M\r\n");
        }
        if(s_u8TimeOutForKeyD>=50) {
			s_u8TimeOutForKeyD = 0;
            FULL_PORT_LOG_INFO("s_u8TimeOutForKeyD\r\n"); 
        }
    }
    return 0;
}
