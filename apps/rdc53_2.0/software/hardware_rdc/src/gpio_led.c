/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "gpio_led.h"

uint8_t led_dig_ctr[5] = {LED_DIG_4,LED_DIG_3,LED_DIG_2,LED_DIG_1,LED_DIG_0};

void init_led_pins(void)
{
    uint32_t pad_ctl = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_HYS_SET(1);
    HPM_IOC->PAD[IOC_PAD_PA29].FUNC_CTL = IOC_PA29_FUNC_CTL_GPIO_A_29;
    HPM_IOC->PAD[IOC_PAD_PA29].PAD_CTL = pad_ctl;

    HPM_IOC->PAD[IOC_PAD_PA30].FUNC_CTL = IOC_PA30_FUNC_CTL_GPIO_A_30;
    HPM_IOC->PAD[IOC_PAD_PA30].PAD_CTL = pad_ctl;

    HPM_IOC->PAD[IOC_PAD_PA31].FUNC_CTL = IOC_PA31_FUNC_CTL_GPIO_A_31;
    HPM_IOC->PAD[IOC_PAD_PA31].PAD_CTL = pad_ctl;

    gpio_set_pin_input(LED_STATUS_CTRL, LED_STATUS_INDEX, LED_BLUE_WORK_PIN);
    gpio_set_pin_input(LED_STATUS_CTRL, LED_STATUS_INDEX, LED_GREEN_OK_PIN);
    gpio_set_pin_input(LED_STATUS_CTRL, LED_STATUS_INDEX, LED_RED_ERROR_PIN);

    gpio_set_pin_output(LED_STATUS_CTRL, LED_STATUS_INDEX, LED_BLUE_WORK_PIN);
    gpio_set_pin_output(LED_STATUS_CTRL, LED_STATUS_INDEX, LED_GREEN_OK_PIN);
    gpio_set_pin_output(LED_STATUS_CTRL, LED_STATUS_INDEX, LED_RED_ERROR_PIN);

    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_GREEN_OK_PIN, LED_OFF);
    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_BLUE_WORK_PIN, LED_OFF);
    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_RED_ERROR_PIN, LED_OFF);
}
void led_red_error(void)
{
    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_GREEN_OK_PIN, LED_OFF);
    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_BLUE_WORK_PIN, LED_OFF);
    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_RED_ERROR_PIN, LED_ON);
}
void led_green_ok(void)
{
    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_BLUE_WORK_PIN, LED_OFF);
    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_RED_ERROR_PIN, LED_OFF);
    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_GREEN_OK_PIN, LED_ON);
}
void led_blue_work(void)
{
    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_RED_ERROR_PIN, LED_OFF);
    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_GREEN_OK_PIN, LED_OFF);
    gpio_write_pin(LED_STATUS_CTRL, LED_STATUS_INDEX,LED_BLUE_WORK_PIN, LED_ON);
}


void init_led_diplay_pins(void)
{
    HPM_PIOC->PAD[IOC_PAD_PY00].FUNC_CTL = PIOC_PY00_FUNC_CTL_SOC_GPIO_Y_00;
    HPM_PIOC->PAD[IOC_PAD_PY01].FUNC_CTL = PIOC_PY01_FUNC_CTL_SOC_GPIO_Y_01;
    HPM_PIOC->PAD[IOC_PAD_PY02].FUNC_CTL = PIOC_PY02_FUNC_CTL_SOC_GPIO_Y_02;
    HPM_PIOC->PAD[IOC_PAD_PY03].FUNC_CTL = PIOC_PY03_FUNC_CTL_SOC_GPIO_Y_03;
    HPM_PIOC->PAD[IOC_PAD_PY04].FUNC_CTL = PIOC_PY04_FUNC_CTL_SOC_GPIO_Y_04;
    HPM_PIOC->PAD[IOC_PAD_PY05].FUNC_CTL = PIOC_PY05_FUNC_CTL_SOC_GPIO_Y_05;
    HPM_PIOC->PAD[IOC_PAD_PY06].FUNC_CTL = PIOC_PY06_FUNC_CTL_SOC_GPIO_Y_06;
    HPM_PIOC->PAD[IOC_PAD_PY07].FUNC_CTL = PIOC_PY07_FUNC_CTL_SOC_GPIO_Y_07;
    
    HPM_IOC->PAD[IOC_PAD_PY00].FUNC_CTL = IOC_PY00_FUNC_CTL_GPIO_Y_00;
    HPM_IOC->PAD[IOC_PAD_PY01].FUNC_CTL = IOC_PY01_FUNC_CTL_GPIO_Y_01;
    HPM_IOC->PAD[IOC_PAD_PY02].FUNC_CTL = IOC_PY02_FUNC_CTL_GPIO_Y_02;
    HPM_IOC->PAD[IOC_PAD_PY03].FUNC_CTL = IOC_PY03_FUNC_CTL_GPIO_Y_03;
    HPM_IOC->PAD[IOC_PAD_PY04].FUNC_CTL = IOC_PY04_FUNC_CTL_GPIO_Y_04;
    HPM_IOC->PAD[IOC_PAD_PY05].FUNC_CTL = IOC_PY05_FUNC_CTL_GPIO_Y_05;
    HPM_IOC->PAD[IOC_PAD_PY06].FUNC_CTL = IOC_PY06_FUNC_CTL_GPIO_Y_06;
    HPM_IOC->PAD[IOC_PAD_PY07].FUNC_CTL = IOC_PY07_FUNC_CTL_GPIO_Y_07;
    
    gpio_set_pin_output(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_A_PIN);
    gpio_set_pin_output(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_F_PIN);
    gpio_set_pin_output(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_B_PIN);
    gpio_set_pin_output(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_E_PIN);
    gpio_set_pin_output(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_D_PIN);
    gpio_set_pin_output(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_DP_PIN);
    gpio_set_pin_output(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_C_PIN);
    gpio_set_pin_output(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_G_PIN);

}
void init_led_dig_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PA10].FUNC_CTL = IOC_PA10_FUNC_CTL_GPIO_A_10;
    HPM_IOC->PAD[IOC_PAD_PA12].FUNC_CTL = IOC_PA12_FUNC_CTL_GPIO_A_12;
    HPM_IOC->PAD[IOC_PAD_PA13].FUNC_CTL = IOC_PA13_FUNC_CTL_GPIO_A_13;
    HPM_IOC->PAD[IOC_PAD_PA14].FUNC_CTL = IOC_PA14_FUNC_CTL_GPIO_A_14;
    HPM_IOC->PAD[IOC_PAD_PA15].FUNC_CTL = IOC_PA15_FUNC_CTL_GPIO_A_15;

    gpio_set_pin_output(LED_POWER_CTRL, LED_POWER_INDEX, LED_DIG0_PIN);
    gpio_set_pin_output(LED_POWER_CTRL, LED_POWER_INDEX, LED_DIG1_PIN);
    gpio_set_pin_output(LED_POWER_CTRL, LED_POWER_INDEX, LED_DIG2_PIN);
    gpio_set_pin_output(LED_POWER_CTRL, LED_POWER_INDEX, LED_DIG3_PIN);
    gpio_set_pin_output(LED_POWER_CTRL, LED_POWER_INDEX, LED_DIG4_PIN);

}

void led_dig_enable(uint8_t index)
{
    for (int i = 0;i < LED_NUMBER;i++)
    {
        if (index != i)
        {
            gpio_write_pin(LED_POWER_CTRL, LED_POWER_INDEX,led_dig_ctr[i], LED_DIG_DISABLE);
        }
    }
    gpio_write_pin(LED_POWER_CTRL, LED_POWER_INDEX,led_dig_ctr[index], LED_DIG_ENABLE);
}
void led_dig_disable(uint8_t index)
{
    gpio_write_pin(LED_POWER_CTRL, LED_POWER_INDEX,led_dig_ctr[index], LED_DIG_DISABLE);
}
void led_dig_disable_all(void)
{
    gpio_write_pin(LED_POWER_CTRL, LED_POWER_INDEX,LED_DIG_0, LED_OFF);
    gpio_write_pin(LED_POWER_CTRL, LED_POWER_INDEX,LED_DIG_1, LED_OFF);
    gpio_write_pin(LED_POWER_CTRL, LED_POWER_INDEX,LED_DIG_2, LED_OFF);
    gpio_write_pin(LED_POWER_CTRL, LED_POWER_INDEX,LED_DIG_3, LED_OFF);
    gpio_write_pin(LED_POWER_CTRL, LED_POWER_INDEX,LED_DIG_4, LED_OFF);
}
uint8_t led_number_array[10][8] = {
    {LED_ON,LED_ON,LED_ON,LED_ON,LED_ON,LED_ON,LED_OFF,LED_OFF},//0
    {LED_OFF,LED_ON,LED_ON,LED_OFF,LED_OFF,LED_OFF,LED_OFF,LED_OFF},//1
    {LED_ON,LED_ON,LED_OFF,LED_ON,LED_ON,LED_OFF,LED_ON,LED_OFF},//2
    {LED_ON,LED_ON,LED_ON,LED_ON,LED_OFF,LED_OFF,LED_ON,LED_OFF},//3
    {LED_OFF,LED_ON,LED_ON,LED_OFF,LED_OFF,LED_ON,LED_ON,LED_OFF},//4
    {LED_ON,LED_OFF,LED_ON,LED_ON,LED_OFF,LED_ON,LED_ON,LED_OFF},//5
    {LED_ON,LED_OFF,LED_ON,LED_ON,LED_ON,LED_ON,LED_ON,LED_OFF},//6
    {LED_ON,LED_ON,LED_ON,LED_OFF,LED_OFF,LED_OFF,LED_OFF,LED_OFF},//7
    {LED_ON,LED_ON,LED_ON,LED_ON,LED_ON,LED_ON,LED_ON,LED_OFF},//8
    {LED_ON,LED_ON,LED_ON,LED_ON,LED_OFF,LED_ON,LED_ON,LED_OFF},//9
};
void led_diplay_point(void)
{
    gpio_write_pin(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_DP_PIN,LED_ON);
}
void led_display_number(uint8_t number)
{
    gpio_write_pin(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_A_PIN,led_number_array[number][0]);
    gpio_write_pin(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_B_PIN,led_number_array[number][1]);
    gpio_write_pin(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_C_PIN,led_number_array[number][2]);
    gpio_write_pin(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_D_PIN,led_number_array[number][3]);
    gpio_write_pin(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_E_PIN,led_number_array[number][4]);
    gpio_write_pin(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_F_PIN,led_number_array[number][5]);
    gpio_write_pin(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_G_PIN,led_number_array[number][6]);
    gpio_write_pin(LED_DISPLAY_CTRL, LED_DISPLAY_INDEX, LED_DISPLAY_DP_PIN,LED_OFF);

}
void led_display_float(float number)
{
    int data;
    uint8_t display_number;
    int index = 0;
    if (number < 1.0f)
    {
        data = number * (float)MULTI_COFFI;
        for (index = 0;index < DECIMAL_NUMBER;index++)
        {
            display_number = data % 10;
            led_dig_enable(index);
            led_display_number(display_number);
            data /= 10;
            board_delay_ms(DISPLAY_DELAY_MS);
        }
        led_dig_enable(DECIMAL_NUMBER);
        led_display_number(0);
        led_diplay_point(); 
        board_delay_ms(DISPLAY_DELAY_MS);
        led_dig_disable(DECIMAL_NUMBER);
    }else if (number < 1000.0f)
    {
        data = number * (float)MULTI_COFFI;
        for (index = 0;index < DECIMAL_NUMBER;index++)
        {
            display_number = data % 10;
            led_dig_enable(index);
            led_display_number(display_number);
            data /= 10;
            board_delay_ms(DISPLAY_DELAY_MS);
        }
        display_number = data % 10;
        led_dig_enable(index);
        led_display_number(display_number);
        led_diplay_point();
        board_delay_ms(DISPLAY_DELAY_MS);
        led_dig_disable(index);
        while (data /= 10)
        {       
            led_dig_enable(++index);
            display_number = data % 10; 
            led_display_number(display_number);
            board_delay_ms(DISPLAY_DELAY_MS);
        }
        led_dig_disable(index);     
    }else if (number < 10000.0f)
    {
        data = number * 10.0f;
        display_number = data % 10;
        led_dig_enable(index);
        led_display_number(display_number);
        board_delay_ms(DISPLAY_DELAY_MS);
        led_dig_disable(index);
        while (data /= 10)
        {           
            led_dig_enable(++index);
            display_number = data % 10; 
            led_display_number(display_number);
            if (index == 1)
            {
                led_diplay_point();
            }
            board_delay_ms(DISPLAY_DELAY_MS);
        }
        led_dig_disable(index);
    }else{
        data = (int)number;
        while (index < LED_NUMBER)
        {
            display_number = data % 10;
            led_dig_enable(index++);
            led_display_number(display_number);
            board_delay_ms(DISPLAY_DELAY_MS);
            data /= 10;
        }
        led_dig_disable(index - 1);
    }
}
void led_display_speed(float speed)
{
    for (int i = 0;i < DISPLAY_COUNT;i++)
    {
        led_display_float(speed);
    }
}
void init_led_display(void)
{
    init_led_pins();
    init_led_diplay_pins();
    init_led_dig_pins();
}


