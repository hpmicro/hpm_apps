#include "board.h"
#include "user_lvgl_port.h"
#include "stdio.h"
#include "watch.h"
#include <FreeRTOS.h>
#include <task.h>

#define TASK_LVGL_PRIORITY (configMAX_PRIORITIES - 2U)

static void lvgl_start(void *param);

int main(void)
{
    board_init();

    if (pdPASS != xTaskCreate(lvgl_start, "lvgl_start", 4096U, NULL, TASK_LVGL_PRIORITY, NULL)) {
        printf("Task lvgl creation failed!\r\n");
        while (1) {
        }
    }

    vTaskStartScheduler();
    for (;;) {
    }
}

static void lvgl_start(void *param)
{
    (void)param;

    board_init_lcd();
    user_lvgl_init();
    watch_init(NULL);

    while (1) {
        uint32_t delay = lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(delay));
    }
}
