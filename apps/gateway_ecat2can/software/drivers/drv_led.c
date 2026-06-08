/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "drv_led.h"

#define BOARD_LED0_R_GPIO_CTRL BOARD_R_GPIO_CTRL
#define BOARD_LED0_R_GPIO_INDEX BOARD_R_GPIO_INDEX
#define BOARD_LED0_R_GPIO_PIN BOARD_R_GPIO_PIN
#define BOARD_LED0_G_GPIO_CTRL BOARD_G_GPIO_CTRL
#define BOARD_LED0_G_GPIO_INDEX BOARD_G_GPIO_INDEX
#define BOARD_LED0_G_GPIO_PIN BOARD_G_GPIO_PIN
#define BOARD_LED0_B_GPIO_CTRL BOARD_B_GPIO_CTRL
#define BOARD_LED0_B_GPIO_INDEX BOARD_B_GPIO_INDEX
#define BOARD_LED0_B_GPIO_PIN BOARD_B_GPIO_PIN


#define BOARD_LED1_R_GPIO_CTRL BOARD_R_GPIO_CTRL
#define BOARD_LED1_R_GPIO_INDEX BOARD_R_GPIO_INDEX
#define BOARD_LED1_R_GPIO_PIN BOARD_R_GPIO_PIN
#define BOARD_LED1_G_GPIO_CTRL BOARD_G_GPIO_CTRL
#define BOARD_LED1_G_GPIO_INDEX BOARD_G_GPIO_INDEX
#define BOARD_LED1_G_GPIO_PIN BOARD_G_GPIO_PIN
#define BOARD_LED1_B_GPIO_CTRL BOARD_B_GPIO_CTRL
#define BOARD_LED1_B_GPIO_INDEX BOARD_B_GPIO_INDEX
#define BOARD_LED1_B_GPIO_PIN BOARD_B_GPIO_PIN


void drv_led_init(void)
{
   board_init_led_pins();
   drv_led_ctrl(DRV_LED0_R_ID, DRV_LED_CTRL_OFF);
   drv_led_ctrl(DRV_LED0_G_ID, DRV_LED_CTRL_OFF);
   drv_led_ctrl(DRV_LED0_B_ID, DRV_LED_CTRL_OFF);
}

void drv_led_ctrl(drv_led_id_e id, drv_led_ctrl_e ctrl)
{
   switch (id)
   {
      case DRV_LED0_R_ID:
         if(ctrl == DRV_LED_CTRL_ON) {
            gpio_write_pin(BOARD_LED0_R_GPIO_CTRL, BOARD_LED0_R_GPIO_INDEX, BOARD_LED0_R_GPIO_PIN, 1);
         } else if(ctrl == DRV_LED_CTRL_OFF) {
            gpio_write_pin(BOARD_LED0_R_GPIO_CTRL, BOARD_LED0_R_GPIO_INDEX, BOARD_LED0_R_GPIO_PIN, 0);
         } else if(ctrl == DRV_LED_CTRL_TOGGLE) {
            gpio_toggle_pin(BOARD_LED0_R_GPIO_CTRL, BOARD_LED0_R_GPIO_INDEX, BOARD_LED0_R_GPIO_PIN);
         }
         break;
      case DRV_LED0_G_ID:
         if(ctrl == DRV_LED_CTRL_ON) {
            gpio_write_pin(BOARD_LED0_G_GPIO_CTRL, BOARD_LED0_G_GPIO_INDEX, BOARD_LED0_G_GPIO_PIN, 1);
         } else if(ctrl == DRV_LED_CTRL_OFF) {
            gpio_write_pin(BOARD_LED0_G_GPIO_CTRL, BOARD_LED0_G_GPIO_INDEX, BOARD_LED0_G_GPIO_PIN, 0);
         } else if(ctrl == DRV_LED_CTRL_TOGGLE) {
            gpio_toggle_pin(BOARD_LED0_G_GPIO_CTRL, BOARD_LED0_G_GPIO_INDEX, BOARD_LED0_G_GPIO_PIN);
         }
         break;
      case DRV_LED0_B_ID:
         if(ctrl == DRV_LED_CTRL_ON) {
            gpio_write_pin(BOARD_LED0_B_GPIO_CTRL, BOARD_LED0_B_GPIO_INDEX, BOARD_LED0_B_GPIO_PIN, 1);
         } else if(ctrl == DRV_LED_CTRL_OFF) {
            gpio_write_pin(BOARD_LED0_B_GPIO_CTRL, BOARD_LED0_B_GPIO_INDEX, BOARD_LED0_B_GPIO_PIN, 0);
         } else if(ctrl == DRV_LED_CTRL_TOGGLE) {
            gpio_toggle_pin(BOARD_LED0_B_GPIO_CTRL, BOARD_LED0_B_GPIO_INDEX, BOARD_LED0_B_GPIO_PIN);
         }
         break;
      case DRV_LED1_R_ID:
         if(ctrl == DRV_LED_CTRL_ON) {
            gpio_write_pin(BOARD_LED1_R_GPIO_CTRL, BOARD_LED1_R_GPIO_INDEX, BOARD_LED1_R_GPIO_PIN, 1);
         } else if(ctrl == DRV_LED_CTRL_OFF) {
            gpio_write_pin(BOARD_LED1_R_GPIO_CTRL, BOARD_LED1_R_GPIO_INDEX, BOARD_LED1_R_GPIO_PIN, 0);
         } else if(ctrl == DRV_LED_CTRL_TOGGLE) {
            gpio_toggle_pin(BOARD_LED1_R_GPIO_CTRL, BOARD_LED1_R_GPIO_INDEX, BOARD_LED1_R_GPIO_PIN);
         }
         break;
      case DRV_LED1_G_ID:
         if(ctrl == DRV_LED_CTRL_ON) {
            gpio_write_pin(BOARD_LED1_G_GPIO_CTRL, BOARD_LED1_G_GPIO_INDEX, BOARD_LED1_G_GPIO_PIN, 1);
         } else if(ctrl == DRV_LED_CTRL_OFF) {
            gpio_write_pin(BOARD_LED1_G_GPIO_CTRL, BOARD_LED1_G_GPIO_INDEX, BOARD_LED1_G_GPIO_PIN, 0);
         } else if(ctrl == DRV_LED_CTRL_TOGGLE) {
            gpio_toggle_pin(BOARD_LED1_G_GPIO_CTRL, BOARD_LED1_G_GPIO_INDEX, BOARD_LED1_G_GPIO_PIN);
         }
         break;
      case DRV_LED1_B_ID:
         if(ctrl == DRV_LED_CTRL_ON) {
            gpio_write_pin(BOARD_LED1_B_GPIO_CTRL, BOARD_LED1_B_GPIO_INDEX, BOARD_LED1_B_GPIO_PIN, 1);
         } else if(ctrl == DRV_LED_CTRL_OFF) {
            gpio_write_pin(BOARD_LED1_B_GPIO_CTRL, BOARD_LED1_B_GPIO_INDEX, BOARD_LED1_B_GPIO_PIN, 0);
         } else if(ctrl == DRV_LED_CTRL_TOGGLE) {
            gpio_toggle_pin(BOARD_LED1_B_GPIO_CTRL, BOARD_LED1_B_GPIO_INDEX, BOARD_LED1_B_GPIO_PIN);
         }
         break;
      default:
         break;
   }
}