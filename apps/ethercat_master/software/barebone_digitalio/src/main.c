#include "board.h"
#include "ethercat.h"
#include "hpm_gptmr_drv.h"
#include "ethercat_common.h"

#define DEBUG_ENABLE 0

static void hpm6e00_digital_io_test(void)
{
    while (esc_enter_op && bfin_get_link_status() && !esc_pdo_error) {
        for (uint16_t cnt = 1; cnt <= ec_slavecount; cnt++) {
            uint32_t *output = (uint32_t *)ec_slave[cnt].outputs;
            uint32_t *input = (uint32_t *)ec_slave[cnt].inputs;
            output[0] = 0x0000003 ^ output[0];
#if DEBUG_ENABLE
            printf("e[%d].o:%08x\r\n", cnt, output[0]);
            printf("e[%d].i:%08x\r\n", cnt, input[0]);
#endif
        }
        board_delay_ms(500);
    }
    ethercat_common_stop();
}

int main(void)
{
    static bool ethercat_start = false;

    board_init();
    board_init_led_pins();
    bfin_emac_init();

    /* Initialize a board timer */
    board_timer_create(100, bfin_polling_link_status);

    while (1) {
        if (bfin_get_link_status()) {
            if (ethercat_start == false) {
                if (ethercat_common_start(NULL, NULL) == 0) {
                    ethercat_start = true;
                }
            }

            if (esc_enter_op) {
                hpm6e00_digital_io_test();
            }
        } else {
            ethercat_start = false;
        }
    }
}