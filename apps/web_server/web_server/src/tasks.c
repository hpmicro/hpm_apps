/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tasks.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

// LwIP
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/pbuf.h"

// USB RNDIS
#include "usbd_core.h"
#include "usbd_rndis.h"

#include "net.h"
#include "button.h"

#include "hpm_segment_led.h"

/* Global task handle for GPIO task notification */
TaskHandle_t g_gpio_task_handle = NULL;
/* FreeRTOS runtime counter for statistics */
volatile uint32_t g_freertos_run_time_counter = 0;

/* Task priorities */
#define USB_OSAL_PRIORITY (0U)
#define MONGOOSE_PRIORITY (3U)
#define GPIO_PRIORITY     (2U)

/* Forward declarations of task functions */
static void usb_osal_task(void *pvParameters);
static void mongoose_task(void *pvParameters);
static void gpio_task(void *pvParameters);

/* Helper function for LwIP traffic handling */
static void lwip_service_traffic(void);

/* Network interface data (shared with init.c via extern) */
extern struct netif netif_data;

void create_system_tasks(void)
{
    /* Create USB OSAL task */
    if (usb_osal_thread_create("usb_osal_task", 8192U, USB_OSAL_PRIORITY, usb_osal_task, NULL) == NULL) {
        printf("usb_osal_task creation failed!.\n");
        for (;;) {
            ;
        }
    }

    /* Create Mongoose web server task */
    TaskHandle_t htask = NULL;
    xTaskCreate(mongoose_task, "mongoose_task", 8192U / sizeof(StackType_t), NULL, MONGOOSE_PRIORITY, &htask);
    if (htask == NULL) {
        printf("Create thread mongoose_task failed\r\n");
        while (1) {
        }
    }

    /* Create GPIO button handling task */
    xTaskCreate(gpio_task, "gpio_task", 4096U / sizeof(StackType_t), NULL, GPIO_PRIORITY, &g_gpio_task_handle);
    if (g_gpio_task_handle == NULL) {
        printf("Create thread gpio_task failed\r\n");
        while (1) {
        }
    }
}

/**
 * @brief Helper function to service incoming USB RNDIS traffic
 */
static void lwip_service_traffic(void)
{
    err_t err;
    struct pbuf *p;

    p = usbd_rndis_eth_rx();

    if (p != NULL) {
        err = netif_data.input(p, &netif_data);

        if (err != ERR_OK) {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
        }
    }
}

/**
 * @brief USB OSAL task - handles USB RNDIS network traffic
 */
static void usb_osal_task(void *pvParameters)
{
    extern usb_osal_sem_t sema_rndis_data;
    (void)pvParameters;

    printf("[cherry usb rndis device with freertos]: task started.\n");

    while (1) {
        if (usb_osal_sem_take(sema_rndis_data, portMAX_DELAY) != 0) {
            continue;
        }
        tcpip_callback((tcpip_callback_fn)lwip_service_traffic, NULL);
    }
}

/**
 * @brief Mongoose web server task
 */
static void mongoose_task(void *pvParameters)
{
    (void)pvParameters;

    printf("[mongoose server]: task started.\n");

    struct mg_mgr mgr;
    mg_log_set(MG_LL_DEBUG);
    mg_mgr_init(&mgr);
    web_init(&mgr);

    while (1) {
        mg_mgr_poll(&mgr, 5);
        vTaskDelay(5);
    }

    mg_mgr_free(&mgr);
}

/**
 * @brief GPIO button task - handles button events and updates DI state
 */
static void gpio_task(void *pvParameters)
{
    (void)pvParameters;

    for (;;) {
        if (ulTaskGenericNotifyTake(0, pdTRUE, portMAX_DELAY) == pdTRUE) {
            vTaskDelay(pdMS_TO_TICKS(10));
            update_gpio_number();

            printf("Keys detected raw: %d %d %d %d %d %d\n", gpio_number[0], gpio_number[1], gpio_number[2], gpio_number[3], gpio_number[4], gpio_number[5]);

            for (int i = 0; i < 6; i++) {
                if (gpio_number[i]) {
                    di_state ^= (1U << i);
                    gpio_number[i] = 0;
                }
            }

            printf("DI state: 0x%02X\n", di_state);
        }
    }
}

/**
 * @brief EUI (Extended User Interface) interrupt handler
 * Handles button press events and notifies GPIO task
 */
SDK_DECLARE_EXT_ISR_M(BOARD_EUI_IRQ, eui_isr)
void eui_isr(void)
{
    if ((eui_get_irq_status(BOARD_EUI) & eui_irq_area_mask) == eui_irq_area_mask) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        eui_clear_irq_flag(BOARD_EUI, eui_irq_area_mask);

        if (g_gpio_task_handle != NULL) {
            xTaskNotifyFromISR(g_gpio_task_handle, 1UL, eSetBits, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}