#include "board.h"
#include "hpm_gpio_drv.h"
#include "string.h"
#include "ethercat.h"
#include "cia402_def.h"
#include "hpm_gptmr_drv.h"
#include "ethercat_common.h"

#define DEBUG_ENABLE 1

void config_sdo(uint16_t slave)
{
    uint32_t pid_data;
    int size;
    int ret;

    // pid_data = 0x16020001;
    // ec_SDOwrite(slave, 0x1c12, 0x00, TRUE, 4, &pid_data, EC_TIMEOUTRXM);
    // pid_data = 0x1a020001;
    // ec_SDOwrite(slave, 0x1c13, 0x00, TRUE, 4, &pid_data, EC_TIMEOUTRXM);

    pid_data = 0;
    size = 4;
    ec_SDOread(slave, 0x1c12, 0x00, TRUE, &size, &pid_data, EC_TIMEOUTRXM);
    printf("rxpdo assign:%08x\r\n", pid_data);
    pid_data = 0;
    size = 4;
    ec_SDOread(slave, 0x1c13, 0x00, TRUE, &size, &pid_data, EC_TIMEOUTRXM);
    printf("txpdo assign:%08x\r\n", pid_data);

    pid_data = 0;
    size = 4;
    ec_SDOread(slave, 0x6502, 0x00, FALSE, &size, &pid_data, EC_TIMEOUTRXM);
    printf("objSupportedDriveModes:%04x\r\n", pid_data);

    pid_data = CYCLIC_SYNC_VELOCITY_MODE;
    ec_SDOwrite(slave, 0x6060, 0x00, FALSE, 4, &pid_data, EC_TIMEOUTRXM);
    pid_data = 0;
    size = 4;
    ec_SDOread(slave, 0x6060, 0x00, FALSE, &size, &pid_data, EC_TIMEOUTRXM);
    printf("objModesOfOperation:%04x\r\n", pid_data);

    pid_data = 0;
    size = 4;
    ec_SDOread(slave, 0x6061, 0x00, FALSE, &size, &pid_data, EC_TIMEOUTRXM);
    printf("objModesOfOperationDisplay:%04x\r\n", pid_data);
}

void sdo_config()
{
    for (int slc = 1; slc <= ec_slavecount; slc++) {
        config_sdo(slc);
    }
}

void hpm6e00_cia402_test(void)
{
    uint16_t status;

    while (esc_enter_op && bfin_get_link_status() && !esc_pdo_error) {
        for (uint16_t cnt = 1; cnt <= ec_slavecount; cnt++) {
            TCiA402PDO1602 *output = (TCiA402PDO1602 *)ec_slave[cnt].outputs;
            TCiA402PDO1A02 *input = (TCiA402PDO1A02 *)ec_slave[cnt].inputs;

            status = input->ObjStatusWord;

            switch (status & STATUSWORD_STATE_MASK) {
                case STATUSWORD_STATE_NOTREADYTOSWITCHON:
                    break;
                case STATUSWORD_STATE_SWITCHEDONDISABLED:
                    output->ObjControlWord = CONTROLWORD_COMMAND_SHUTDOWN; // next state STATE_READY_TO_SWITCH_ON
                    break;
                case STATUSWORD_STATE_READYTOSWITCHON:
                    output->ObjControlWord = CONTROLWORD_COMMAND_SWITCHON; // next state STATE_SWITCHED_ON
                    break;
                case STATUSWORD_STATE_SWITCHEDON:
                    output->ObjControlWord = CONTROLWORD_COMMAND_SWITCHON_ENABLEOPERATION; // next state STATE_OPERATION_ENABLED
                    break;
                case STATUSWORD_STATE_OPERATIONENABLED:
                    output->ObjControlWord = CONTROLWORD_COMMAND_ENABLEOPERATION; // next state STATE_OPERATION_ENABLED
                    output->ObjTargetVelocity = 100;
                    break;
                default:
                    break;
            }
#if DEBUG_ENABLE
            printf("Slave:%d, StatusWord:%04x, Position:%d\n", cnt, input->ObjStatusWord, input->ObjPositionActualValue);
#endif
        }
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
                if (ethercat_common_start(sdo_config) == 0) {
                    ethercat_start = true;
                }
            }

            if (esc_enter_op) {
                hpm6e00_cia402_test();
            }
        } else {
            ethercat_start = false;
        }
    }
}