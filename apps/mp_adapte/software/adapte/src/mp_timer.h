/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef MP_TIMER_H
#define MP_TIMER_H

#include "hpm_soc.h"
#include "hpm_clock_drv.h"
#include "mp_common.h"

/**
 * @brief HPM Power Timer interface; HPM电源高精定时器接口
 * @addtogroup POWER_TIMER_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MP timer manager Used
 *        ;MP 使用的定时器管理器
 */
#define MP_CALLBACK_TIMER             (HPM_GPTMR2)

/**
 * @brief  MP timer channel Used
 *        ;MP 使用的定时器通道
 */
#define MP_CALLBACK_TIMER_CH          1

/**
 * @brief MP timer interrupt Used
 *       ;MP 使用的定时器中断号
 */
#define MP_CALLBACK_TIMER_IRQ         IRQn_GPTMR2

/**
 * @brief  MP timer clock Used
 *         ;MP 使用的定时器时钟NAME
 */
#define MP_CALLBACK_TIMER_CLK_NAME    (clock_gptmr2)

#ifdef __cplusplus
}
#endif

/** @} */

#endif //MP_TIMER_H