/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_gpiom_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_dma_drv.h"
#include "hpm_dmamux_drv.h"
#include "timer_led.h"

#define BIT(n) (1UL << (n))
#define ABS(n) ((n) >= 0 ? (n) : (-n))
static unsigned char P_RGB_TEST[UCS1903_LED_COUNT][3] = {0};

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(4)
uint32_t ucs1903_data_buf[DATA_PLACEHOLDER_COUNT + UCS1903_DATA_COUNT];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(4)
uint32_t ucs1903_data_length;

static volatile bool dma_transfer_done;
static volatile bool dma_transfer_error;
/*
* PINs must belong to the same group of pins, for example, PA0 to 31 / PB0 to 31 / PC0 to 31 / PD0 to 31
* ;PINs 必须属于同一组管脚，例如：PA0~31 / PB0~31 / PC0~31 / PD0~31 ...
* The following array is the num of the pin pin, for example, PD11 PD14 PD15 PD16 PD19 PD20 PD23 PD24 PD25
* ;下面数组为pin脚的num，例如：PD11 PD14 PD15 PD16 PD19 PD20 PD23 PD24 PD25
*/
const uint8_t ucs1903_channel_pin_num[32] = {16, 19, 20, 23, 24, 25, 28, 29, 30, 31};

void ucs1903_init_gpio_pins(void)
{
    uint32_t pad_ctl = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);

    HPM_IOC->PAD[IOC_PAD_PD16].FUNC_CTL = IOC_PD16_FUNC_CTL_GPIO_D_16;
    HPM_IOC->PAD[IOC_PAD_PD16].PAD_CTL = pad_ctl;

    HPM_IOC->PAD[IOC_PAD_PD19].FUNC_CTL = IOC_PD19_FUNC_CTL_GPIO_D_19;
    HPM_IOC->PAD[IOC_PAD_PD19].PAD_CTL = pad_ctl;

    HPM_IOC->PAD[IOC_PAD_PD20].FUNC_CTL = IOC_PD20_FUNC_CTL_GPIO_D_20;
    HPM_IOC->PAD[IOC_PAD_PD20].PAD_CTL = pad_ctl;

    HPM_IOC->PAD[IOC_PAD_PD23].FUNC_CTL = IOC_PD23_FUNC_CTL_GPIO_D_23;
    HPM_IOC->PAD[IOC_PAD_PD23].PAD_CTL = pad_ctl;

    HPM_IOC->PAD[IOC_PAD_PD24].FUNC_CTL = IOC_PD24_FUNC_CTL_GPIO_D_24;
    HPM_IOC->PAD[IOC_PAD_PD24].PAD_CTL = pad_ctl;

    HPM_IOC->PAD[IOC_PAD_PD25].FUNC_CTL = IOC_PD25_FUNC_CTL_GPIO_D_25;
    HPM_IOC->PAD[IOC_PAD_PD25].PAD_CTL = pad_ctl;

    HPM_IOC->PAD[IOC_PAD_PD28].FUNC_CTL = IOC_PD28_FUNC_CTL_GPIO_D_28;
    HPM_IOC->PAD[IOC_PAD_PD28].PAD_CTL = pad_ctl;

    HPM_IOC->PAD[IOC_PAD_PD29].FUNC_CTL = IOC_PD29_FUNC_CTL_GPIO_D_29;
    HPM_IOC->PAD[IOC_PAD_PD29].PAD_CTL = pad_ctl;

    HPM_IOC->PAD[IOC_PAD_PD30].FUNC_CTL = IOC_PD30_FUNC_CTL_GPIO_D_30;
    HPM_IOC->PAD[IOC_PAD_PD30].PAD_CTL = pad_ctl;

    HPM_IOC->PAD[IOC_PAD_PD31].FUNC_CTL = IOC_PD31_FUNC_CTL_GPIO_D_31;
    HPM_IOC->PAD[IOC_PAD_PD31].PAD_CTL = pad_ctl;
}

void ucs1903_gpio_init(void)
{
    uint8_t index;
    for (index = 0; index < USC1903_CHANNEL_COUNT; index++)
    {
        gpio_set_pin_output(USC1903_USE_GPIO, GPIO_DO_GPIOD, ucs1903_channel_pin_num[index]);
        gpio_write_pin(USC1903_USE_GPIO, GPIO_DO_GPIOD, ucs1903_channel_pin_num[index], 0);
    }
}

static void isr_dma(void)
{
    volatile hpm_stat_t stat;

    stat = dma_check_transfer_status(USC1903_USE_DMA, USC1903_USE_DMA_CHANN);
    if (0 != (stat & DMA_CHANNEL_STATUS_TC))
    {
        gptmr_stop_counter(USC1903_USE_GPTIMER, USC1903_USE_GPTIMER_CH);
        dmamux_disable_channel(USC1903_USE_DMAMUX, USC1903_USE_DMAMUX_CHANN);
        // gptmr_channel_enable_dma_request(USC1903_USE_GPTIMER, USC1903_USE_GPTIMER_CH, false);
        dma_transfer_done = true;
    }
    else if (0 != (stat & DMA_CHANNEL_STATUS_ERROR))
    {
        gptmr_stop_counter(USC1903_USE_GPTIMER, USC1903_USE_GPTIMER_CH);
        dmamux_disable_channel(USC1903_USE_DMAMUX, USC1903_USE_DMAMUX_CHANN);
        // gptmr_channel_enable_dma_request(USC1903_USE_GPTIMER, USC1903_USE_GPTIMER_CH, false);
        dma_transfer_done = true;
        dma_transfer_error = true;
    }
}

SDK_DECLARE_EXT_ISR_M(USC1903_USE_DMA_IRQ, isr_dma)

void ucs1903_timer_config(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(USC1903_USE_GPTIMER, &config);
    clock_add_to_group(USC1903_USE_GPTIMER_CLK_NAME, 0);
    gptmr_freq = clock_get_frequency(USC1903_USE_GPTIMER_CLK_NAME);

    config.reload = (gptmr_freq / UCS1903_TIMER_FREQ) - 1;
    config.mode = gptmr_work_mode_no_capture;
    config.dma_request_event = gptmr_dma_request_on_reload;

    gptmr_channel_config(USC1903_USE_GPTIMER, USC1903_USE_GPTIMER_CH, &config, false);
}

void ucs1903_dma_config(void)
{
    dma_channel_config_t ch_config = {0};

    intc_m_enable_irq_with_priority(USC1903_USE_DMA_IRQ, 1);
    dma_reset(USC1903_USE_DMA);

    dma_default_channel_config(USC1903_USE_DMA, &ch_config);

    ch_config.src_addr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&ucs1903_data_buf[0]);

    ch_config.dst_addr = (uint32_t)&USC1903_USE_GPIO->DO[GPIO_DO_GPIOD].VALUE;
    ch_config.src_width = DMA_TRANSFER_WIDTH_WORD; // 32位
    ch_config.dst_width = DMA_TRANSFER_WIDTH_WORD; // 32位
    ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    ch_config.size_in_byte = ucs1903_data_length * sizeof(uint32_t);
    ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    ch_config.src_burst_size = 0;

    if (status_success != dma_setup_channel(USC1903_USE_DMA, USC1903_USE_DMA_CHANN, &ch_config, true))
    {
        printf(" dma setup channel failed\n");
        return;
    }
    dmamux_config(USC1903_USE_DMAMUX, USC1903_USE_DMAMUX_CHANN, HPM_DMA_SRC_GPTMR0_1, false);

    gptmr_channel_enable_dma_request(USC1903_USE_GPTIMER, USC1903_USE_GPTIMER_CH, true);

    gptmr_channel_reset_count(USC1903_USE_GPTIMER, USC1903_USE_GPTIMER_CH);
    gptmr_start_counter(USC1903_USE_GPTIMER, USC1903_USE_GPTIMER_CH);

    dmamux_enable_channel(USC1903_USE_DMAMUX, USC1903_USE_DMAMUX_CHANN);
}

void ucs1903_rgb_convert(uint32_t count, const uint8_t data[][3])
{
    if (count > UCS1903_LED_COUNT)
    {
        printf("error:data overflow!\n");
        return;
    }
    uint32_t i = 0, j = 8, n = 0, index = 0;
    uint8_t tmp = 0;
    ucs1903_data_buf[0] = 0;
    ucs1903_data_length = DATA_PLACEHOLDER_COUNT;
    for (i = 0; i < count; i++)
    {
        for (n = 0; n < 3; n++)
        {
            if (data[i][0] == data[i][1] && data[i][1] == data[i][2])
                tmp = data[i][n];
            else
            {
                if (n == 0)
                    tmp = data[i][n];
                else
                    tmp = data[i][n] / 2;
            }
            for (j = 8; j > 0; j--)
            {
                if (tmp & BIT(j - 1))
                {
                    // 1
                    for (index = 0; index < UCS1903_SPLIT_COUNT; index++)
                    {
                        if (index <= UCS1903_1_HIGH_COUNT - 1)
                        {
                            ucs1903_data_buf[ucs1903_data_length++] = 0xFFFFFFFF;
                        }
                        else
                        {
                            ucs1903_data_buf[ucs1903_data_length++] = 0;
                        }
                    }
                }
                else
                {
                    // 0
                    for (index = 0; index < UCS1903_SPLIT_COUNT; index++)
                    {
                        if (index <= USC1903_0_HIGH_COUNT - 1)
                        {
                            ucs1903_data_buf[ucs1903_data_length++] = 0xFFFFFFFF;
                        }
                        else
                        {
                            ucs1903_data_buf[ucs1903_data_length++] = 0;
                        }
                    }
                }
            }
        }
    }
}

void light_strip_color_set(uint32_t count, const unsigned char data[][3], unsigned char flag)
{

    if (flag == 1 && data != NULL)
    {
        memcpy(P_RGB_TEST, data, count * 3);
    }
    ucs1903_rgb_convert(count, data);
}

int set_light_colour(int r, int g, int b, unsigned char bright, int length, unsigned char flag)
{
    int i;
    unsigned char RGB_TEST[UCS1903_LED_COUNT][3] = {0};
    for (i = 0; i < length; i += 1)
    {
        if (flag == 1)
        {
            RGB_TEST[i][0] = (r * bright) / 100;
            RGB_TEST[i][1] = (g * bright) / 100;
            RGB_TEST[i][2] = (b * bright) / 100;
        }
        else
        {
            RGB_TEST[i][0] = (P_RGB_TEST[i][0] * bright) / 100;
            RGB_TEST[i][1] = (P_RGB_TEST[i][1] * bright) / 100;
            RGB_TEST[i][2] = (P_RGB_TEST[i][2] * bright) / 100;
        }
    }

    light_strip_color_set(length, RGB_TEST, flag);
    return 0;
}

void ucs1903_dma_transfer(void)
{
    dma_transfer_done = false;
    dma_transfer_error = false;
    ucs1903_dma_config();
}

uint32_t run_times;
void clear_cycle(void)
{
    write_csr(CSR_MCYCLE, 0);
}
uint32_t read_cycles(void)
{
    uint32_t cycles;
    cycles = read_csr(CSR_MCYCLE);
    return cycles;
}

void ucs1903_dma_send(int r, int g, int b, unsigned char bright, int length, unsigned char flag)
{
    clear_cycle();
    set_light_colour(r, g, b, bright, length, flag);

    // for (uint32_t i = 0; i < UCS1903_DATA_COUNT; i += 2)
    // {
    //     ucs1903_data_buf[i] = 0xFFFFFFFF;
    //     ucs1903_data_buf[i + 1] = 0;
    // }
    // ucs1903_data_length = UCS1903_DATA_COUNT;

    ucs1903_dma_transfer();
    run_times = read_cycles();
    printf("total times:%d tick.\r\n", run_times);

    while (!dma_transfer_done)
    {
        __asm("nop");
    }

    if (dma_transfer_error)
    {
        printf(" chained transfer failed\n");
    }
    printf("dma transfer down!\n");
}

int main(void)
{
    board_init();
    ucs1903_init_gpio_pins();
    ucs1903_gpio_init();
    printf("ucs1903 led of timer dma gpio\n");
    ucs1903_timer_config();

    while (1)
    {
        ucs1903_dma_send(255, 0, 0, 100, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(0, 255, 0, 100, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(0, 0, 255, 100, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(255, 255, 0, 100, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(0, 255, 255, 100, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(255, 0, 255, 100, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(255, 255, 255, 10, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(255, 255, 255, 20, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(255, 255, 255, 30, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(255, 255, 255, 50, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(255, 255, 255, 70, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(255, 255, 255, 80, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(255, 255, 255, 90, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(255, 255, 255, 100, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);

        ucs1903_dma_send(0, 0, 0, 0, UCS1903_LED_COUNT, 1);
        board_delay_ms(1000);
    }

    return 0;
}
