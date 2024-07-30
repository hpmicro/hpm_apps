/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_appheader.h"

__attribute__ ((section(".app_fw_header"))) const hpm_app_header_t app_head = {
    HPM_APP_FILE_FLAG_MAGIC,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    {0},
};
