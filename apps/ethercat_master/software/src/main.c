#include "board.h"
#include "hpm_gpio_drv.h"
#include "string.h"
#include "ethercat.h"

#define SYNC0TIME 10000 // 8000

uint32 network_configuration(void)
{
    ///* Do we got expected number of slaves from config */
    // if (ec_slavecount < NUMBER_OF_SLAVES)
    //    return 0;

    ///* Verify slave by slave that it is correct*/
    if (strcmp(ec_slave[1].name, "digital_io"))
        return 0;
    // else if (strcmp(ec_slave[EL4001_1].name,"EL4001"))
    //    return 0;
    // else if (strcmp(ec_slave[EL3061_1].name,"EL3061"))
    //    return 0;
    // else if (strcmp(ec_slave[EL1008_1].name,"EL1008"))
    //    return 0;
    // else if (strcmp(ec_slave[EL1008_2].name,"EL1008"))
    //    return 0;
    // else if (strcmp(ec_slave[EL2622_1].name,"EL2622"))
    //    return 0;
    // else if (strcmp(ec_slave[EL2622_2].name,"EL2622"))
    //    return 0;
    // else if (strcmp(ec_slave[EL2622_3].name,"EL2622"))
    //    return 0;
    // else if (strcmp(ec_slave[EL2622_4].name,"EL2622"))
    //    return 0;

    return 1;
}

char IOmap[512];
int dorun = 0;

static void hpm6e00_digital_io_test(void)
{
    int cnt, i, j, slc;

    printf("Starting simple test\n");

    /* initialise SOEM */
    if (ec_init(NULL) > 0) {
        printf("ec_init succeeded.\n");

        /* find and auto-config slaves */
        if (ec_config_init(FALSE) > 0) {
            printf("%d slaves found and configured.\n", ec_slavecount);

            /* Check network  setup */
            if (network_configuration()) {
                for (slc = 1; slc <= ec_slavecount; slc++) {
                    printf("Found %s at position %d\n", ec_slave[slc].name, slc);
                }
                slc = 0;
                ec_configdc();
                ec_dcsync0(0, TRUE, SYNC0TIME, 250000); // SYNC0 on slave 1

                /* Run IO mapping */
                ec_config_map(&IOmap);

                printf("Slaves mapped, state to SAFE_OP.\n");
                /* wait for all slaves to reach SAFE_OP state */
                ec_statecheck(slc, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE);
                ec_readstate();
                printf("Slave 0 State=0x%04x\r\n", ec_slave[0].state);
                printf("Slave 1 State=0x%04x\r\n", ec_slave[1].state);

                /* Print som information on the mapped network */
                for (cnt = 1; cnt <= ec_slavecount; cnt++) {
                    printf("\nSlave:%d\n Name:%s\n Output size: %dbits\n Input size: %dbits\n State: %d\n Delay: %d[ns]\n Has DC: %d\n",
                           cnt, ec_slave[cnt].name, ec_slave[cnt].Obits, ec_slave[cnt].Ibits,
                           ec_slave[cnt].state, ec_slave[cnt].pdelay, ec_slave[cnt].hasdc);
                    printf(" Configured address: %x\n", ec_slave[cnt].configadr);
                    printf(" Outputs address: %x\n", ec_slave[cnt].outputs);
                    printf(" Inputs address: %x\n", ec_slave[cnt].inputs);

                    for (j = 0; j < ec_slave[cnt].FMMUunused; j++) {
                        printf(" FMMU%1d Ls:%x Ll:%4d Lsb:%d Leb:%d Ps:%x Psb:%d Ty:%x Act:%x\n", j,
                               (int)ec_slave[cnt].FMMU[j].LogStart, ec_slave[cnt].FMMU[j].LogLength, ec_slave[cnt].FMMU[j].LogStartbit,
                               ec_slave[cnt].FMMU[j].LogEndbit, ec_slave[cnt].FMMU[j].PhysStart, ec_slave[cnt].FMMU[j].PhysStartBit,
                               ec_slave[cnt].FMMU[j].FMMUtype, ec_slave[cnt].FMMU[j].FMMUactive);
                    }
                    printf(" FMMUfunc 0:%d 1:%d 2:%d 3:%d\n",
                           ec_slave[cnt].FMMU0func, ec_slave[cnt].FMMU1func, ec_slave[cnt].FMMU2func, ec_slave[cnt].FMMU3func);
                }

                printf("Request operational state for all slaves\n");
                ec_slave[slc].state = EC_STATE_OPERATIONAL;
                board_delay_us(20);
                /* send one valid process data to make outputs in slaves happy*/
                ec_send_processdata();
                ec_receive_processdata(EC_TIMEOUTRET);
                /* request OP state for all slaves */
                ec_writestate(slc);
                /* wait for all slaves to reach OP state */
                /* wait for all slaves to reach OP state */
                ec_statecheck(slc, EC_STATE_OPERATIONAL, EC_TIMEOUTSTATE);
                do {
                    ec_slave[slc].state = EC_STATE_OPERATIONAL;
                    ec_send_processdata();
                    ec_receive_processdata(EC_TIMEOUTRET);
                    ec_statecheck(slc, EC_STATE_OPERATIONAL, 50000);
                } while (ec_slave[slc].state != EC_STATE_OPERATIONAL);
                printf("Slave 0 State=0x%04x\r\n", ec_slave[slc].state);

                if (ec_slave[slc].state == EC_STATE_OPERATIONAL) {
                    dorun = 1;
                    printf("Operational state reached for all slaves.\n");
                } else {
                    printf("Not all slaves reached operational state.\n");
                    ec_readstate();
                    for (i = 1; i <= ec_slavecount; i++) {
                        if (ec_slave[i].state != EC_STATE_OPERATIONAL) {
                            printf("Slave %d State=0x%04x StatusCode=0x%04x\n",
                                   i, ec_slave[i].state, ec_slave[i].ALstatuscode);
                        }
                    }
                }

                while (1) {
                    if (dorun == 1) {
                        ec_send_processdata();
                        ec_receive_processdata(EC_TIMEOUTRET);

                        for (cnt = 1; cnt <= ec_slavecount; cnt++) {
                            uint32_t *output = (uint32_t *)ec_slave[cnt].outputs;
                            uint32_t *input = (uint32_t *)ec_slave[cnt].inputs;
                            output[0] = 0x0000003 ^ output[0];
                            printf("ec_slave[%d].outputs:%08x\r\n", cnt, output[0]);
                            printf("ec_slave[%d].inputs:%08x\r\n", cnt, input[0]);
                        }
                    }
                    if (!bfin_get_link_status()) {
                        return;
                    }
                    board_delay_us(50000);
                }
            } else {
                printf("Mismatch of network units!\n");
            }
        } else {
            printf("No slaves found!\n");
        }
        printf("End simple test, close socket\n");
        board_delay_us(1500000);
        /* stop SOEM, close socket */
        ec_close();
    } else {
        printf("ec_init failed");
    }
    printf("End program\n");
}

int main(void)
{
    board_init();
    board_init_led_pins();
    bfin_emac_init();

    /* Initialize a board timer */
    board_timer_create(100, bfin_polling_link_status);

    while (1) {
        if (bfin_get_link_status()) {
            hpm6e00_digital_io_test();
        }
    }
}