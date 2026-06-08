#ifndef __BOOT_USER_H
#define __BOOT_USER_H

#include <stdbool.h>

// pbutn
void bootuser_init_gpio_pins(void);

void bootuser_board_init(void);

bool bootuser_board_enter_bootloader(void);

int bootuser_check_enter_mode(void);

int bootuser_verfiy_goto_app(bool verfiy_enable, bool butn_enable);

#endif //__BOOT_USER_H