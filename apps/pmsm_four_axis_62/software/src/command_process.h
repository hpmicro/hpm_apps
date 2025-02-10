/*
 * Copyright (c) 2024 hpmicro
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __COMMAND_PROCESS_H__
#define __COMMAND_PROCESS_H__

/**
 * @brief     number of motors controlled
 *            ;控制的电机个数
 */
#define MOTOR_COUNT 4
/**
 * @brief     types of motor operation modes: speed mode and speed mode
 *            ;电机运行模式的种类，速度模式和速度模式两种
 */
#define OP_MODE_COUNT 2
/**
 * @brief     The states of motor control: running and stopping
 *            ;电机控制的状态，分为运行和停止两种状态
 */
#define CONTROL_MODE_COUNT 2

typedef enum{
  MOTOR_DISABLE,
  MOTOR_ENABLE,
}motor_op_mode;
typedef enum{
  MOTOR_CONTROL_INDEPEND,
  MOTOR_CONTROL_ALL,
}motor_control_mode;
/**
 * @brief     Enable all motors
 *            ;使能所有电机
 */
void enable_all_motor(void);
/**
 * @brief     Disable all motors
 *            ;禁用所有电机
 */
void disable_all_motor(void);
/**
 * @brief     Command processing when all motors are enabled
 *            ;所有电机使能时的命令处理
 */
void cmd_all(void);
/**
 * @brief     Command processing when independently controlling motors
 *            ;独立控制电机时的命令处理
 */
void cmd_indivdual(void);
/**
 * @brief     Command processing：handling commands sent by the upper computer
 *            ;命令处理:处理上位机发送的命令
 */
void process_cmd(void);

#endif
