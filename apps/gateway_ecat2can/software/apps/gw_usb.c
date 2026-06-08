/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "gw_usb.h"

void gw_usb_thread(void *argument)
{
    (void)argument;

    board_init_usb(HPM_USB0);
    intc_set_irq_priority(IRQn_USB0, 2);

    /* default password is : 12345678 */
    if (osOK != shell_init(0, HPM_USB0_BASE, true)) {
        LOG_E("Failed to initialize usb shell\r\n");
    }

    osThreadExit();
}


static int gw_usb_sh_test(int argc, char **argv)
{
    chry_shell_t *csh = (void *)argv[argc + 1];
    csh_printf(csh, "test: \r\n");
    csh_printf(csh, "argc=<%d>\r\n", argc);
    for (uint8_t i = 0; i < argc; i++) {
        csh_printf(csh, "argv[%d]:0x%08x=<%s>\r\n", i, (uintptr_t)argv[i], argv[i]);
    }

    csh_printf(csh, "argv[%d]=<0x%08x>\r\n", argc, argv[argc]);
    csh_printf(csh, "argv[%d]=<0x%08x>\r\n\r\n", argc + 1, argv[argc + 1]);

    return 0;
}
CSH_CMD_EXPORT(gw_usb_sh_test,);

static int toggle_led(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    //board_led_toggle();
    return 0;
}
CSH_CMD_EXPORT(toggle_led,);

static int write_led(int argc, char **argv)
{
    chry_shell_t *csh = (void *)argv[argc + 1];
    if (argc < 2) {
        csh_printf(csh, "usage: write_led <status>\r\n\r\n");
        csh_printf(csh, "  status    0 or 1\r\n\r\n");
        return -1;
    }

    //board_led_write(!board_get_led_gpio_off_level() ^ (atoi(argv[1]) == 0));
    return 0;
}
CSH_CMD_EXPORT(write_led,);
