/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __OTA_KCONFIG_H
#define __OTA_KCONFIG_H

#if (__has_include("hpm_flashmap.h"))

#include "hpm_flashmap.h"

#else

#include "hpm_dflashmap.h"

#endif

#if (__has_include("ota_config.h"))

#include "ota_config.h"

#else

#include "hpm_soc.h"

#define HEADER_INIT_VERSION         (0U)

#define ota_malloc(...) malloc(__VA_ARGS__)
#define ota_free(...) free(__VA_ARGS__)

#ifdef HPM_PGPR0
#define HPM_OTA_INFO_RAM_ADDR       (HPM_PGPR0)
#else
#define HPM_OTA_INFO_RAM_ADDR       (HPM_PGPR)
#endif

#define HPM_OTA_USER_EXIP_INDEX     (3)

#define HPM_OTA_RETRY_COUNT         (3)

/*--------HPM log--------*/
#define CONFIG_HPM_PRINTF(...) printf(__VA_ARGS__)

#ifndef CONFIG_HPM_DBG_LEVEL
#define CONFIG_HPM_DBG_LEVEL HPM_DBG_INFO
#endif

/* Enable print with color */
#define CONFIG_HPM_PRINTF_COLOR_ENABLE

#endif


#define OTA_VERSION       0x000200
#define OTA_VERSION_STR   "v0.2.0"

#endif //__OTA_KCONFIG_H