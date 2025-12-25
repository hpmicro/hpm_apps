/*
 * Copyright (c) 2022-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "hpm_soc.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_common.h"
#include "monitor_log.h"
#include "monitor_task.h"
#include "monitor_profile.h"
#include "monitor.h"

typedef struct
{
    uint8_t fifo[MONITOR_RINGBUFFSER_SIZE];
    uint32_t len;
    uint32_t expect_len;
} bingfifo_t;

MONITOR_NOCACHE_RAM_SECTION static bingfifo_t monitor_bingfifo;

#ifdef MONITOR_UART_DMA_ENABLE
MONITOR_NOCACHE_RAM_SECTION MONITOR_ATTR_ALIGN(4) bool monitor_uart_tx_done;
MONITOR_NOCACHE_RAM_SECTION MONITOR_ATTR_ALIGN(8) static dma_linked_descriptor_t tx_descriptors[2];
MONITOR_NOCACHE_RAM_SECTION MONITOR_ATTR_ALIGN(8) static dma_linked_descriptor_t rx_descriptors[2];
MONITOR_NOCACHE_RAM_SECTION MONITOR_ATTR_ALIGN(4) static bool dma_done = true;
MONITOR_NOCACHE_RAM_SECTION MONITOR_ATTR_ALIGN(4) static uint8_t dma_recv_buf[MONITOR_RINGBUFFSER_SIZE];
static dma_channel_config_t first_tx_dma_ch_config;

static int monitor_uart_dma_tx_chain(uint8_t *data, uint32_t len)
{
    uint8_t des_index = 0;
    dma_channel_config_t dma_ch_config;

    dma_default_channel_config(MONITOR_UART_DMA_CONTROLLER, &dma_ch_config);

    dma_ch_config.size_in_byte = len;
    dma_ch_config.src_addr = core_local_mem_to_sys_address(CONFIG_MONITOR_RUNNING_CORE, (uint32_t)data);
    dma_ch_config.dst_addr = (uint32_t)&MONITOR_UART_BASE->THR;
    dma_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.interrupt_mask = DMA_INTERRUPT_MASK_ALL;
    dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&tx_descriptors[des_index + 1]);
    dma_config_linked_descriptor(MONITOR_UART_DMA_CONTROLLER, &tx_descriptors[des_index], MONITOR_UART_TX_DMA_CHN, &dma_ch_config);
    if (des_index == 0)
    {
        memcpy(&first_tx_dma_ch_config, &dma_ch_config, sizeof(dma_channel_config_t));
    }
    des_index++;

    dma_default_channel_config(MONITOR_UART_DMA_CONTROLLER, &dma_ch_config);
    dma_ch_config.size_in_byte = 1;
    dma_ch_config.src_addr = core_local_mem_to_sys_address(CONFIG_MONITOR_RUNNING_CORE, (uint32_t)&dma_done);
    dma_ch_config.dst_addr = core_local_mem_to_sys_address(CONFIG_MONITOR_RUNNING_CORE, (uint32_t)&monitor_uart_tx_done);
    dma_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.interrupt_mask = DMA_INTERRUPT_MASK_ALL;
    dma_ch_config.linked_ptr = NULL;
    dma_config_linked_descriptor(MONITOR_UART_DMA_CONTROLLER, &tx_descriptors[des_index], MONITOR_UART_TX_DMA_CHN, &dma_ch_config);
    des_index++;
    return 0;
}

static uint32_t monitor_get_current_recv_remaining_size(void)
{
    return dma_get_remaining_transfer_size(MONITOR_UART_DMA_CONTROLLER, MONITOR_UART_RX_DMA_CHN);
}

static int monitor_uart_rx_dma_autorun(void)
{
    dma_channel_config_t dma_ch_config;
    dma_default_channel_config(MONITOR_UART_DMA_CONTROLLER, &dma_ch_config);

    dma_ch_config.size_in_byte = MONITOR_RINGBUFFSER_SIZE;
    dma_ch_config.src_addr = (uint32_t)&MONITOR_UART_BASE->RBR;
    dma_ch_config.dst_addr = core_local_mem_to_sys_address(CONFIG_MONITOR_RUNNING_CORE, (uint32_t)&dma_recv_buf[0]);
    dma_ch_config.src_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.dst_width = DMA_TRANSFER_WIDTH_BYTE;
    dma_ch_config.src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T;
    dma_ch_config.src_mode = DMA_HANDSHAKE_MODE_HANDSHAKE;
    dma_ch_config.dst_mode = DMA_HANDSHAKE_MODE_NORMAL;
    dma_ch_config.src_addr_ctrl = DMA_ADDRESS_CONTROL_FIXED;
    dma_ch_config.dst_addr_ctrl = DMA_ADDRESS_CONTROL_INCREMENT;
    dma_ch_config.interrupt_mask = DMA_INTERRUPT_MASK_ALL;
    dma_ch_config.linked_ptr = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&rx_descriptors[0]);
    dma_config_linked_descriptor(MONITOR_UART_DMA_CONTROLLER, &rx_descriptors[0], MONITOR_UART_RX_DMA_CHN, &dma_ch_config);
    if (status_success != dma_setup_channel(MONITOR_UART_DMA_CONTROLLER, MONITOR_UART_RX_DMA_CHN, &dma_ch_config, true))
    {
        MONITOR_LOG_ERR("rx dma setup channel failed\n");
        return -2;
    }

    return 0;
}

static uint32_t uart_dma_recv_polling(uint8_t *data, uint32_t maxsize)
{
    static uint32_t old_recvlen = 0;
    uint32_t size = 0;
    uint32_t new_recvlen = MONITOR_RINGBUFFSER_SIZE - monitor_get_current_recv_remaining_size();

    if (old_recvlen != new_recvlen)
    {
        if (new_recvlen > old_recvlen)
        {
            size = new_recvlen - old_recvlen;
            if (size > maxsize)
            {
                MONITOR_LOG_ERR("BAD! overflow1, overflow data drop!\r\n");
                size = maxsize;
            }
            memcpy(data, dma_recv_buf + old_recvlen, size);
        }
        else
        {
            size = MONITOR_RINGBUFFSER_SIZE - old_recvlen;
            if (size > maxsize)
            {
                MONITOR_LOG_ERR("BAD! overflow2, overflow data drop!\r\n");
                size = maxsize;
                memcpy(data, dma_recv_buf + old_recvlen, size);
            }
            else if (size + new_recvlen > maxsize)
            {
                MONITOR_LOG_ERR("BAD! overflow3, overflow data drop!\r\n");
                memcpy(data, dma_recv_buf + old_recvlen, size);
                memcpy(data + size, dma_recv_buf, maxsize - size);
                size = maxsize;
            }
            else
            {
                memcpy(data, dma_recv_buf + old_recvlen, size);
                memcpy(data + size, dma_recv_buf, new_recvlen);
                size += new_recvlen;
            }
        }
        old_recvlen = new_recvlen;
        return size;
    }
    return 0;
}

static int monitor_uart_tx_dma(uint8_t *data, uint32_t len)
{
    // dma addr must 4byte aligned
    if (((uint32_t)data & 0x3) != 0)
        return -1;
    if (monitor_uart_tx_done)
    {
        monitor_uart_tx_done = false;
        monitor_uart_dma_tx_chain(data, len);
        if (status_success != dma_setup_channel(MONITOR_UART_DMA_CONTROLLER, MONITOR_UART_TX_DMA_CHN, &first_tx_dma_ch_config, true))
        {
            MONITOR_LOG_ERR("tx dma setup channel failed 0\n");
            return -2;
        }
    }
    else
    {
        return -1;
    }
    return 0;
}
#endif

typedef int (*uart_cb_t)(uint8_t *data, uint32_t len, uint32_t *drop_offset, uint32_t *expect_length);

static int uart_channel_output(uint8_t *data, uint16_t length)
{
    if (data == NULL || length <= 0)
        return -1;
#ifdef MONITOR_UART_DMA_ENABLE
    return monitor_uart_tx_dma(data, length);
#else
    if (status_success != uart_send_data(MONITOR_UART_BASE, data, length))
        return -1;
    return 0;
#endif
}

#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_LOG)
static void debug_printf(uint8_t *str)
{
    MONITOR_LOG_DBG("%s, len:%d, data:\r\n", str, monitor_bingfifo.len);
    for (uint32_t i = 0; i < monitor_bingfifo.len; i++)
    {
        MONITOR_LOG_RAW(" %02x", monitor_bingfifo.fifo[i]);
    }
    MONITOR_LOG_RAW("\r\n");
}
#endif

static void uart_channel_handle(uart_cb_t uart_cb)
{
    bool new_data;
    uint32_t drop = 0, expect = 0;
#ifdef MONITOR_UART_DMA_ENABLE
    uint32_t recv_len;
    if (monitor_bingfifo.len >= MONITOR_RINGBUFFSER_SIZE)
    {
        // full data
        new_data = true;
        monitor_bingfifo.expect_len = monitor_bingfifo.len;
    }
    else
    {
        recv_len = uart_dma_recv_polling(monitor_bingfifo.fifo + monitor_bingfifo.len, MONITOR_RINGBUFFSER_SIZE - monitor_bingfifo.len);
        if (recv_len > 0)
        {
            new_data = true;
            monitor_bingfifo.len += recv_len;
#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_LOG)
            debug_printf((uint8_t *)"recv");
#endif
        }
    }

#else
    while (1)
    {
        if (monitor_bingfifo.len >= MONITOR_RINGBUFFSER_SIZE)
        {
            // full data
            new_data = true;
            monitor_bingfifo.expect_len = monitor_bingfifo.len;
        }
        if (uart_receive_byte(MONITOR_UART_BASE, &monitor_bingfifo.fifo[monitor_bingfifo.len]) != status_success)
            break;
        monitor_bingfifo.len++;
        new_data = true;
    }
#endif
    if (new_data && monitor_bingfifo.len >= monitor_bingfifo.expect_len)
    {
        if (uart_cb != NULL)
        {
            uart_cb(monitor_bingfifo.fifo, monitor_bingfifo.len, &drop, &expect);
            MONITOR_LOG_DBG("drop:%d, expect:%d\r\n", drop, expect);
            if (drop > 0)
            {
                if(drop >= monitor_bingfifo.len)
                {
                    monitor_bingfifo.len = 0;
                }
                else
                {
                    memcpy(monitor_bingfifo.fifo, monitor_bingfifo.fifo + drop, drop);
                    monitor_bingfifo.len -= drop;
                }
                monitor_bingfifo.expect_len = monitor_bingfifo.len + expect;
            }
#if (CONFIG_MONITOR_DBG_LEVEL >= MONITOR_DBG_LOG)
            debug_printf((uint8_t *)"last");
#endif
        }
    }
}

static int monitor_uart_init(void)
{
    hpm_stat_t stat;
    uart_config_t config = {0};
    board_init_uart(MONITOR_UART_BASE);
    uart_default_config(MONITOR_UART_BASE, &config);
    
    memset(&monitor_bingfifo, 0, sizeof(monitor_bingfifo));
#ifdef MONITOR_UART_DMA_ENABLE
    memset(dma_recv_buf, 0, sizeof(dma_recv_buf));
    dma_done = true;
    monitor_uart_tx_done = true;

    config.fifo_enable = true;
    config.dma_enable = true;
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;

    intc_m_disable_irq(MONITOR_UART_DMA_IRQ);
    dmamux_config(MONITOR_UART_DMAMUX_CONTROLLER, MONITOR_UART_RX_DMAMUX_CHN, MONITOR_UART_RX_DMA_REQ, true);
    dmamux_config(MONITOR_UART_DMAMUX_CONTROLLER, MONITOR_UART_TX_DMAMUX_CHN, MONITOR_UART_TX_DMA_REQ, true);
    monitor_uart_rx_dma_autorun();
#endif
    config.src_freq_in_hz = clock_get_frequency(MONITOR_UART_CLK_NAME);
    config.baudrate = MONITOR_UART_BAUDRATE;
    stat = uart_init(MONITOR_UART_BASE, &config);
    if (stat != status_success)
    {
        printf("failed to initialize uart\n");
        while (1)
        {
        }
    }
    return 0;
}

int monitor_init(void)
{
    monitor_uart_init();

    monitor_task_init();

    monitor_task_output_register(uart_channel_output);
    return 0;
}

void monitor_handle(void)
{
    monitor_task_handle();
    uart_channel_handle(monitor_task_input_process);
}

bool monitor_send_is_idle(void)
{
    if(monitor_uart_tx_done)
        return true;
    return false;
}