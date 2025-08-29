/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef GPIO_LED_H
#define GPIO_LED_H

#include "board.h"
#include "hpm_gpio_drv.h"
#include "math.h"


/**
 * @brief     Status indicator light switch, 1: enabled, 0: disabled
 *            ;状态指示灯开关，1：使能，0：禁用
 */
#define LED_STATUS_ENABLE    1
/**
 * @brief     GPIO group used for status indicator lights
 *            ;状态指示灯使用的GPIO组
 */
#define LED_STATUS_CTRL      HPM_GPIO0
#define LED_STATUS_INDEX     GPIO_DO_GPIOA

/**
 * @brief     GPIO pins used for status indicator lights
 *            ;状态指示灯使用的GPIO引脚
 */
#define LED_RED_ERROR_PIN    31
#define LED_GREEN_OK_PIN     30
#define LED_BLUE_WORK_PIN    29

/**
 * @brief     Status indicator light on and off control, LED_ON： light on, LED_OFF： light off
 *            ;状态指示灯亮灭控制，LED_ON：亮，LED_OFF：灭
 */
#define LED_ON               0
#define LED_OFF              1

/**
 * @brief     LED display tube control, LED_DIG_ENABLE： The display tube is on,
              LED_DIG_DISABLE： Display tube out
 *            ;LED显示管控制，LED_DIG_ENABLE：显示管亮，LED_DIG_DISABLE：显示管灭
 */
#define LED_DIG_ENABLE       1
#define LED_DIG_DISABLE      0


/**
 * @brief     LED display tube control pins, a total of 5 display tubes
 *            ;LED显示管控制引脚，共5个显示管
 */
#define LED_DIG0_PIN  15 
#define LED_DIG1_PIN  12
#define LED_DIG2_PIN  10
#define LED_DIG3_PIN  14
#define LED_DIG4_PIN  13

/**
 * @brief     LED display tube control pins, a total of 5 display tubes
 *            ;LED显示管控制引脚，共5个显示管
 */
#define LED_NUMBER              5
/**
 * @brief     LED displays the number of decimal places, 1 or 2 digits
 *            ;LED显示小数位的个数，1位或2位
 */
#define DECIMAL_NUMBER          2//1 or 2,max 2
/**
 * @brief     Multiplier coefficient
 *            ;乘数系数
 */
#define MULTI_COFFI             pow(10,DECIMAL_NUMBER)
/**
 * @brief     The duration of each LED display
 *            ;每个LED显示的持续时间
 */
#define DISPLAY_DELAY_MS        5
/**
 * @brief     LED displays the number of cycles per display
 *            ;LED每次显示循环次数
 */
#define DISPLAY_COUNT           10

/**
 * @brief     LED display tube control pins, a total of 5 display tubes
 *            ;LED显示管控制引脚，共5个显示管
 */
typedef enum{
    LED_DIG_0 = LED_DIG0_PIN,
    LED_DIG_1 = LED_DIG1_PIN,
    LED_DIG_2 = LED_DIG2_PIN,
    LED_DIG_3 = LED_DIG3_PIN,
    LED_DIG_4 = LED_DIG4_PIN,
}LED_DIG_CTRL;

/**
 * @brief     LED display tube power supply control pin group
 *            ;LED显示管供电控制引脚组
 */
#define LED_POWER_CTRL        HPM_GPIO0
#define LED_POWER_INDEX       GPIO_DO_GPIOA
/**
 * @brief     LED display control pin group
 *            ;LED显示控制引脚组
 */
#define LED_DISPLAY_CTRL      HPM_GPIO0
#define LED_DISPLAY_INDEX     GPIO_DO_GPIOY
/**
 * @brief     LED display control pins, 8 pins control the display of each number
 *            ;LED显示控制引脚，8个引脚控制每一个数字的显示
 */
#define LED_DISPLAY_A_PIN     0
#define LED_DISPLAY_F_PIN     1
#define LED_DISPLAY_B_PIN     2
#define LED_DISPLAY_E_PIN     3
#define LED_DISPLAY_D_PIN     4
#define LED_DISPLAY_DP_PIN    5
#define LED_DISPLAY_C_PIN     6
#define LED_DISPLAY_G_PIN     7

/**
 * @brief     Initialize the control pins of the LED light
 *            ;初始化LED灯的控制引脚
 */
void init_led_pins(void);
/**
 * @brief     The red LED light is on, indicating that the current status is error
 *            ;红色LED灯亮，表示当前状态为error
 */
void led_red_error(void);
/**
 * @brief     The green LED light is on, indicating that the current status is OK
 *            ;绿色LED灯亮，表示当前状态为ok
 */
void led_green_ok(void);
/**
 * @brief     The blue LED light is on, indicating that the current status is work
 *            ;蓝色LED灯亮，表示当前状态为work
 */
void led_blue_work(void);

/**
 * @brief     Initialize LED display control pins
 *            ;初始化LED显示控制引脚
 */
void init_led_diplay_pins(void);
/**
 * @brief     Initialize LED display tube power supply control pins
 *            ;初始化LED显示管供电控制引脚
 */
void init_led_dig_pins(void);
/**
 * @brief      LED display tube enable, 5 LED display tubes in total from 0 to 4
 *            ;LED显示管使能，0~4共5个LED显示管
 * @param[in]    index   0~4 respectively represent the 5 LED display tubes from the lowest 
                         position to the highest position;0~4分别表示最低位到最高位的5个LED显示管                        
 */
void led_dig_enable(uint8_t index);
/**
 * @brief     LED display tube disable, 5 LED display tubes in total from 0 to 4
 *            ;LED显示管禁用，0~4共5个LED显示管
 * @param[in]    index   0~4 respectively represent the 5 LED display tubes from the lowest 
                         position to the highest position;0~4分别表示最低位到最高位的5个LED显示管                        
 */
void led_dig_disable(uint8_t index);
/**
 * @brief     Disable all LED display tubes
 *            ;禁用所有的LED显示管                       
 */
void led_dig_disable_all(void);

/**
 * @brief     LED digital tube displays numbers
 *            ;LED数码管显示数字
 * @param[in]    number   数字0~9;Numbers 0~9                       
 */
void led_display_number(uint8_t number);
/**
 * @brief     LED digital display speed
 *            ;LED数码管显示速度
 * @param[in]    speed   Speed value, supports up to 5 positive floating-point numbers
                         ;速度值，最多支持5位的正浮点数                      
 */
void led_display_speed(float speed);
/**
 * @brief     LED digital tube displays floating-point numbers
 *            ;LED数码管显示浮点数
 * @param[in]    number  Floating point data, supporting up to 5 positive floating-point numbers
                         ;浮点数据，最多支持5位的正浮点数                      
 */
void led_display_float(float number);
/**
 * @brief     LED digital tube displays decimal point
 *            ;LED数码管显示小数点                       
 */
void led_diplay_point(void);
/**
 * @brief     Initialize LED digital display function
 *            ;初始化LED数码管显示功能                      
 */
void init_led_display(void);
#endif /* GPIO_LED_H */
