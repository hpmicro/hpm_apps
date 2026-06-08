#include "board.h"
#include <hpm_lvgl.h>
#include "ui.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "hpm_gpio_drv.h"
#include "string.h"
#include "hpm_gptmr_drv.h"
#include "hpm_debug_console.h"
#include "hpm_clock_drv.h"
#include "hpm_lcdc_drv.h"
#include "ec_master.h"
#include "data_common.h"
#include "hpm_uart_drv.h"
#include "shell.h"

ATTR_PLACE_AT_FAST_RAM_BSS ec_master_t g_ec_master;
data_pack_t data_pack = { 0 };

unsigned char cherryecat_eepromdata[2048]; // EEPROM data buffer, please generate by esi_parse.py

static void lvgl_task(void *pvParameters)
{
    (void)pvParameters;
    uint32_t delay;

    board_init_cap_touch();
    board_init_lcd();

    hpm_lvgl_init();

    ui_init(NULL);

    while (1) {
        delay = lv_timer_handler();
        vTaskDelay(delay);
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_CONSOLE_UART_IRQ, shell_uart_isr)

#define task_start_PRIORITY (configMAX_PRIORITIES - 2U)

static void task_start(void *param);

int main(void)
{
    board_init();

    if (pdPASS != xTaskCreate(task_start, "task_start", 1024U, NULL, task_start_PRIORITY, NULL)) {
        printf("Task start creation failed!\r\n");
        while (1) {
        };
    }

    vTaskStartScheduler();
    for (;;) {
    }
    return 0;
}

static void task_start(void *param)
{
    (void)param;

    printf("Try to initialize the uart\r\n"
           "  if you are using the console uart as the shell uart\r\n"
           "  failure to initialize may result in no log\r\n");

    uart_config_t shell_uart_config = { 0 };
    uart_default_config(BOARD_CONSOLE_UART_BASE, &shell_uart_config);
    shell_uart_config.src_freq_in_hz = clock_get_frequency(BOARD_CONSOLE_UART_CLK_NAME);
    shell_uart_config.baudrate = 115200;

    if (status_success != uart_init(BOARD_CONSOLE_UART_BASE, &shell_uart_config)) {
        /* uart failed to be initialized */
        printf("Failed to initialize uart\r\n");
        while (1) {
        };
    }

    printf("Initialize shell uart successfully\r\n");

    /* default password is : 12345678 */
    /* shell_init() must be called in-task */
    if (0 != shell_init(BOARD_CONSOLE_UART_BASE, false)) {
        /* shell failed to be initialized */
        printf("Failed to initialize shell\r\n");
        while (1) {
        };
    }

    printf("Initialize shell successfully\r\n");

    /* irq must be enabled after shell_init() */
    uart_enable_irq(BOARD_CONSOLE_UART_BASE, uart_intr_rx_data_avail_or_timeout);
    intc_m_enable_irq_with_priority(BOARD_CONSOLE_UART_IRQ, 1);

    printf("Enable shell uart interrupt\r\n");

    data_pack.cur_index = 0;
    data_pack.speed = 0;
    data_pack.position = 0;
    data_pack.mode = MODE_CSP;

    ec_master_cmd_init(&g_ec_master);
    ec_master_init(&g_ec_master, 0);

    if (xTaskCreate(lvgl_task, "lvgl", 2048, NULL, 5, NULL) != pdPASS) {
        printf("Task creation failed!.\n");
        for (;;) {
        }
    }

    printf("Exit start task\r\n");

    vTaskDelete(NULL);
}

CSH_CMD_EXPORT(ethercat, );
