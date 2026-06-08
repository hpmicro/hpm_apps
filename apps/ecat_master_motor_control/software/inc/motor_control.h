/**
 * @file motor_control.h
 * @brief Motor control and EtherCAT functions
 */

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>

void ec_start(void);

void ec_stop(void);

void motor_control_update(void);

void motor_control_start(void);

void motor_control_stop(void);

void motor_control_forward(void);

void motor_control_backward(void);

void motor_control_reset(void);

uint32_t get_actual_value(void);

uint32_t get_target_value(void);

#endif /* MOTOR_CONTROL_H */
