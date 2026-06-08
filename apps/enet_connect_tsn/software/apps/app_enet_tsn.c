#include "app_enet_tsn.h"

struct netif gnetif[_ENET_PORT_COUNT];

void app_enet_tsn_thread(void *argument)
{
    (void)argument;

    drv_enet_tsn_init();

    /* Initialize LwIP stack */
    tcpip_init(NULL, NULL);

    for (uint8_t i = 0; i < _ENET_PORT_COUNT; i++) 
    {
        netif_config(&gnetif[i], i);
    }

    /* Enable Enet IRQ */
    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT 
        board_enable_enet_irq(BOARD_ENET);
    #endif

    #if defined(LWIP_DHCP) && LWIP_DHCP
    for (uint8_t i = 0; i < _ENET_PORT_COUNT; i++) 
    {
        xTaskCreate(LwIP_DHCP_task, "DHCP", configMINIMAL_STACK_SIZE*2, &gnetif[i], 4, NULL);
    }
    #endif

    TimerHandle_t  port_speed_timer = xTimerCreate(
            "port_speed_timer",          // 定时器名称（仅调试用）
            pdMS_TO_TICKS(1000),      // 定时器周期（1000ms）
            pdTRUE,                   // pdTRUE=周期定时器，pdFALSE=单次
            (void *)0,                // 定时器ID（区分多个定时器，这里传0）
            timer_callback);
    
    xTimerStart(port_speed_timer, pdMS_TO_TICKS(0));

    for (uint8_t i = 0; i < _ENET_PORT_COUNT; i++) 
    {
        #if _TCP_ECHO
            app_tcp_echo_init(&gnetif[i]);
        #else
            app_udp_echo_init(&gnetif[i]);
        #endif
    }

    vTaskDelete(NULL);
}