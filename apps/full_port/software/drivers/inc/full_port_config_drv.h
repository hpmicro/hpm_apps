/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __FULL_PORT_CONFIG_DRV_H
#define __FULL_PORT_CONFIG_DRV_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "pinmux.h"
#include "board.h"
#include "hpm_common.h"
#include "hpm_uart_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_i2c_drv.h"
#include "hpm_gpio_drv.h"
#include "pinmux.h"
#include "hpm_pmp_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_sysctl_drv.h"
#include "hpm_pllctlv2_drv.h"
#include "hpm_pcfg_drv.h"
#include "hpm_femc_drv.h"
#include "hpm_spi_drv.h"
#include "hpm_ppor_drv.h"
#include "hpm_dmamux_drv.h"
#include "full_port_log_drv.h"
#include "hpm_tsw_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_romapi.h"

/* irq config */
typedef void (*full_port_irq_cb_func)(void);

#include "full_port_err_drv.h"
#include "full_port_key_drv.h"
#include "full_port_led_drv.h"
#include "full_port_tsn_drv.h"
#include "full_port_lcd_drv.h"

#endif 
