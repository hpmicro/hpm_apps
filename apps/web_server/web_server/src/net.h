/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#pragma once

#include "mongoose.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(HTTP_URL)
#define HTTP_URL "http://0.0.0.0:8000"
#endif

#if !defined(HTTPS_URL)
#define HTTPS_URL "https://0.0.0.0:8443"
#endif

void web_init(struct mg_mgr *mgr); // Initialise Web UI

#ifdef __cplusplus
}
#endif
