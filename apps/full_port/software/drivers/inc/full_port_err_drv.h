/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#ifndef __FULL_PORT_ERR_DRV_H_
#define __FULL_PORT_ERR_DRV_H_


enum __tag_FULL_PORT_ErrorNo__
{
    FULL_PORT_EER_OK = 0,
    FULL_PORT_EER_ERROR       = -1,
    FULL_PORT_EER_INVALID_ID  = -2,
    FULL_PORT_EER_MAX = -10000
};

#endif // __FULL_PORT_ERR_H_

