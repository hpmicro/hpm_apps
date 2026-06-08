/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "drv_log.h"

static char drv_log_buf[1024];
static char drv_log_transfer_buf[16];
static hpm_log_engine_t *drv_log_engine;

static void drv_log_isr(void)
{
    /* Get the UART interrupt ID */
    uint8_t irq_id = uart_get_irq_id(BOARD_CONSOLE_UART_BASE);

    /* Check if the interrupt is due to a transmit slot becoming available */
    if (irq_id == uart_intr_id_tx_slot_avail) {
        /* Disable the transmit slot available interrupt */
        uart_disable_irq(BOARD_CONSOLE_UART_BASE, uart_intr_tx_slot_avail);
        /* Notify the log engine that the transfer is finished */
        hpm_log_engine_transfer_finish(drv_log_engine);
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_CONSOLE_UART_IRQ, drv_log_isr)

static void drv_log_transfer_start(const char *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        uart_write_byte(BOARD_CONSOLE_UART_BASE, (uint8_t)buf[i]);

    uart_enable_irq(BOARD_CONSOLE_UART_BASE, uart_intr_tx_slot_avail);
}
static void drv_log_transfer_block(const char *buf, uint32_t len)
{
    uart_send_data(BOARD_CONSOLE_UART_BASE, (uint8_t *)buf, len);
}
void drv_log_init(void)
{
    /* Configuration structure for the log engine */
    hpm_log_engine_config_t cfg = {
        /* Function to start the data transfer */
       .transfer_start = drv_log_transfer_start,
        /* Function to perform a blocking data transfer */
       .transfer_block = drv_log_transfer_block,
        /* Buffer used for data transfer */
       .transfer_buf = drv_log_transfer_buf,
        /* Maximum size of a single transfer */
       .max_transfer_size = sizeof(drv_log_transfer_buf),
        /* FIFO buffer for logging */
       .fifo_buf = drv_log_buf,
        /* Size of the FIFO buffer */
       .fifo_buf_size = sizeof(drv_log_buf),
    };

    /* Initialize the logging system */
    hpm_log_init(NULL);
    /* Create a log engine instance with the specified configuration */
    drv_log_engine = hpm_log_engine_create(&cfg);
    /* Enable the log engine */
    hpm_log_engine_enable(drv_log_engine);
    /* Enable the UART console interrupt with a priority of 1 */
    intc_m_enable_irq_with_priority(BOARD_CONSOLE_UART_IRQ, 1);
}