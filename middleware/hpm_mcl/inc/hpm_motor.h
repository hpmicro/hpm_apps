/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MOTOR_H
#define HPM_MOTOR_H

#include "hpm_stepper_motor.h"



/**
 * @brief stepper motor foc control
 * 
 * @param par @ref STEPPER_CONTROL_FOC_PARA
 */
void hpm_stepper_loop(STEPPER_CONTROL_FOC_PARA * par);

#endif