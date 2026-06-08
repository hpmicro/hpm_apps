#ifndef _USER_LVGL_PORT_H
#define _USER_LVGL_PORT_H

#include "lvgl.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void user_lvgl_init(void);
void user_lvgl_wait_vsync(void);

#ifdef __cplusplus
}
#endif

#endif
