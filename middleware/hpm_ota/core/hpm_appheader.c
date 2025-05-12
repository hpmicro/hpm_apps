/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_ota.h"
#include "hpm_appheader.h"

__attribute__ ((section(".app_fw_header"))) const hpm_app_header_t app_head = {
    .magic = HPM_APP_FILE_FLAG_MAGIC,
    .touchid = 0,
    .device = 0,
    .len = 0,
    .version = HEADER_INIT_VERSION,
#if defined(CONFIG_DIFF_IMAGE_ENABLE) && CONFIG_DIFF_IMAGE_ENABLE
    .type = OTA_TYPE_DIFF_FIRMWARE,
#else
    .type = OTA_TYPE_NORMAL_FIRMWARE,
#endif
    .hash_enable = 0,
    .pwr_hash = 0,
    .hash_type = 0,
    .start_addr = 0,
    .hash_data = {0},
    .diff_header = {0},
};
