/*
 * Copyright (c) 2024 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "command_process.h"
#include "pmsm_init.h"

uint8_t op_mode[MOTOR_COUNT] = {0};
uint8_t control_word[MOTOR_COUNT] = {0};
static uint8_t op_mode_old[MOTOR_COUNT] = {0};
static uint8_t control_word_old[MOTOR_COUNT] = {0};
uint8_t control_word_all;
uint8_t op_mode_all;
static uint8_t control_word_all_old;
static uint8_t op_mode_all_old;

void cmd_indivdual(void)
{
    for (int i = 0;i < MOTOR_COUNT;i++)
    {
      if (op_mode[i] < OP_MODE_COUNT && control_word[i] < CONTROL_MODE_COUNT)
      {
          if (op_mode[i] != op_mode_old[i])
          {
             Motor_Control_Global.ControlWord[i] = MOTOR_DISABLE;
             board_delay_ms(500);
             op_mode_old[i] = op_mode[i];
             Motor_Control_Global.OpMode[i] = op_mode[i];
             Motor_Control_Global.ControlWord[i] = control_word[i];
          }
          if (control_word[i] != control_word_old[i])
          {
               control_word_old[i] = control_word[i];
               Motor_Control_Global.ControlWord[i] = control_word[i];
          }

      }else
      {
          if (op_mode[i] >= OP_MODE_COUNT)
          {
              op_mode[i] = op_mode_old[i];
          }else
          {
              control_word[i] = control_word_old[i];
          }
      }
    }
}
void enable_all_motor(void)
{
    for (int i = 0;i < MOTOR_COUNT;i++)
    {
       Motor_Control_Global.ControlWord[i] = MOTOR_ENABLE;
       Motor_Control_Global.OpMode[i] = op_mode_all;
    }
}
void disable_all_motor(void)
{
    for (int i = 0;i < MOTOR_COUNT;i++)
    {
       Motor_Control_Global.ControlWord[i] = MOTOR_DISABLE;
    }
    board_delay_ms(500);
}
void cmd_all(void)
{
  if (op_mode_all >= OP_MODE_COUNT)
  {
     op_mode_all = op_mode_all_old;
     return;
  }
  if (control_word_all_old != control_word_all)
  {
      control_word_all_old = control_word_all;
  }
  if (op_mode_all_old != op_mode_all)
  {

      disable_all_motor();
      op_mode_all_old = op_mode_all;
  }
  enable_all_motor();
}
void process_cmd(void)
{
    if (control_word_all == MOTOR_CONTROL_ALL)
    {
      cmd_all();
    }else 
    {
         if (control_word_all_old != control_word_all)
         {
             disable_all_motor();
             for (int i = 0;i < MOTOR_COUNT;i++)
             {
                Motor_Control_Global.OpMode[i] = op_mode_old[i];
             }
             control_word_all_old = control_word_all;
         }
         cmd_indivdual();
    }

}
