#include "board.h"
#include "ethercat.h"
#include "hpm_gptmr_drv.h"
#include "ethercat_common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cia402_def.h"
#include "shell.h"

SDK_DECLARE_EXT_ISR_M(BOARD_CONSOLE_UART_IRQ, shell_uart_isr)

#define DEBUG_ENABLE        0

#define task1_PRIORITY      (configMAX_PRIORITIES - 1U)
#define task_start_PRIORITY (configMAX_PRIORITIES - 2U)

#define MOTOR_MODE_CSV_CSP  0
#define MOTOR_MODE_CSP      1
#define MOTOR_MODE_CSV      2

volatile uint8_t motor_mode = MOTOR_MODE_CSV;

static void task_start(void *param);

void config_sdo1(uint16_t slave)
{
    uint32_t pid_data;
    int size;
    int ret;

    switch (motor_mode) {
        case MOTOR_MODE_CSV_CSP:
            pid_data = 0x16000001;
            ec_SDOwrite(slave, 0x1c12, 0x00, TRUE, 4, &pid_data, EC_TIMEOUTRXM);
            pid_data = 0x1a000001;
            ec_SDOwrite(slave, 0x1c13, 0x00, TRUE, 4, &pid_data, EC_TIMEOUTRXM);

            break;
        case MOTOR_MODE_CSP:
            pid_data = 0x16010001;
            ec_SDOwrite(slave, 0x1c12, 0x00, TRUE, 4, &pid_data, EC_TIMEOUTRXM);
            pid_data = 0x1a010001;
            ec_SDOwrite(slave, 0x1c13, 0x00, TRUE, 4, &pid_data, EC_TIMEOUTRXM);

            break;
        case MOTOR_MODE_CSV:
            pid_data = 0x16020001;
            ec_SDOwrite(slave, 0x1c12, 0x00, TRUE, 4, &pid_data, EC_TIMEOUTRXM);
            pid_data = 0x1a020001;
            ec_SDOwrite(slave, 0x1c13, 0x00, TRUE, 4, &pid_data, EC_TIMEOUTRXM);

            break;

        default:
            break;
    }

    pid_data = 0;
    size = 4;
    ec_SDOread(slave, 0x1c12, 0x00, TRUE, &size, &pid_data, EC_TIMEOUTRXM);
    printf("rxpdo assign:%08x\r\n", pid_data);
    pid_data = 0;
    size = 4;
    ec_SDOread(slave, 0x1c13, 0x00, TRUE, &size, &pid_data, EC_TIMEOUTRXM);
    printf("txpdo assign:%08x\r\n", pid_data);
}

void config_sdo2(uint16_t slave)
{
    uint32_t pid_data;
    int size;
    int ret;

    switch (motor_mode) {
        case MOTOR_MODE_CSV_CSP:
            break;
        case MOTOR_MODE_CSP:
            pid_data = CYCLIC_SYNC_POSITION_MODE;
            ec_SDOwrite(slave, 0x6060, 0x00, FALSE, 4, &pid_data, EC_TIMEOUTRXM);
            pid_data = 0;
            size = 4;
            ec_SDOread(slave, 0x6060, 0x00, FALSE, &size, &pid_data, EC_TIMEOUTRXM);
            printf("objModesOfOperation:%04x\r\n", pid_data);

            break;
        case MOTOR_MODE_CSV:
            pid_data = CYCLIC_SYNC_VELOCITY_MODE;
            ec_SDOwrite(slave, 0x6060, 0x00, FALSE, 4, &pid_data, EC_TIMEOUTRXM);
            pid_data = 0;
            size = 4;
            ec_SDOread(slave, 0x6060, 0x00, FALSE, &size, &pid_data, EC_TIMEOUTRXM);
            printf("objModesOfOperation:%04x\r\n", pid_data);

            break;

        default:
            break;
    }

    pid_data = 0;
    size = 4;
    ec_SDOread(slave, 0x6502, 0x00, FALSE, &size, &pid_data, EC_TIMEOUTRXM);
    printf("objSupportedDriveModes:%04x\r\n", pid_data);

    pid_data = 0;
    size = 4;
    ec_SDOread(slave, 0x6061, 0x00, FALSE, &size, &pid_data, EC_TIMEOUTRXM);
    printf("objModesOfOperationDisplay:%04x\r\n", pid_data);
}

void sdo_prev_config()
{
    for (int slc = 1; slc <= ec_slavecount; slc++) {
        config_sdo1(slc);
    }
}

void sdo_post_config()
{
    for (int slc = 1; slc <= ec_slavecount; slc++) {
        config_sdo2(slc);
    }
}

static void task_start(void *param)
{
    (void)param;

    printf("Try to initialize the uart\r\n"
           "  if you are using the console uart as the shell uart\r\n"
           "  failure to initialize may result in no log\r\n");

    uart_config_t shell_uart_config = { 0 };
    uart_default_config(BOARD_CONSOLE_UART_BASE, &shell_uart_config);
    shell_uart_config.src_freq_in_hz = clock_get_frequency(BOARD_CONSOLE_UART_CLK_NAME);
    shell_uart_config.baudrate = 115200;

    if (status_success != uart_init(BOARD_CONSOLE_UART_BASE, &shell_uart_config)) {
        /* uart failed to be initialized */
        printf("Failed to initialize uart\r\n");
        while (1) {
        };
    }

    printf("Initialize shell uart successfully\r\n");

    /* default password is : 12345678 */
    /* shell_init() must be called in-task */
    if (0 != shell_init(BOARD_CONSOLE_UART_BASE, false)) {
        /* shell failed to be initialized */
        printf("Failed to initialize shell\r\n");
        while (1) {
        };
    }

    printf("Initialize shell successfully\r\n");

    /* irq must be enabled after shell_init() */
    uart_enable_irq(BOARD_CONSOLE_UART_BASE, uart_intr_rx_data_avail_or_timeout);
    intc_m_enable_irq_with_priority(BOARD_CONSOLE_UART_IRQ, 1);

    printf("Enable shell uart interrupt\r\n");

    bfin_emac_init();

    /* Initialize a board timer */
    board_timer_create(100, bfin_polling_link_status);

    printf("Exit start task\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    board_init();
    board_init_led_pins();

    if (pdPASS != xTaskCreate(task_start, "task_start", 1024U, NULL, task_start_PRIORITY, NULL)) {
        printf("Task start creation failed!\r\n");
        while (1) {
        };
    }

    vTaskStartScheduler();
    for (;;) {
        ;
    }
}

static void task1(void *pvParameters)
{
    (void)pvParameters;

    int ret = ethercat_common_start(sdo_prev_config, sdo_post_config);
    if (ret < 0) {
        printf("EtherCAT master start failed with error code: %d\n", ret);
    } else {
    }

    vTaskDelete(NULL);
}

static int esc_start(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!strcmp(argv[1], "csv_csp")) {
        motor_mode = MOTOR_MODE_CSV_CSP;
    } else if (!strcmp(argv[1], "csp")) {
        motor_mode = MOTOR_MODE_CSP;
    } else if (!strcmp(argv[1], "csv")) {
        motor_mode = MOTOR_MODE_CSV;
    }

    printf("EtherCAT master started with mode: %s\n", argv[1]);
    if (xTaskCreate(task1, "task1", configMINIMAL_STACK_SIZE + 2048U, NULL, task1_PRIORITY, NULL) != pdPASS) {
        printf("Task1 creation failed!.\n");
        for (;;) {
            ;
        }
    }

    return 0;
}
CSH_CMD_EXPORT(esc_start, );

static int esc_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    TCiA402PDO1601 *csp_output;
    TCiA402PDO1602 *csv_output;

    switch (motor_mode) {
        case MOTOR_MODE_CSV_CSP:
            break;
        case MOTOR_MODE_CSP:
            for (uint16_t cnt = 1; cnt <= ec_slavecount; cnt++) {
                csp_output = (TCiA402PDO1601 *)ec_slave[cnt].outputs;

                csp_output->ObjControlWord = CONTROLWORD_COMMAND_SHUTDOWN;
            }

            break;
        case MOTOR_MODE_CSV:
            for (uint16_t cnt = 1; cnt <= ec_slavecount; cnt++) {
                csv_output = (TCiA402PDO1602 *)ec_slave[cnt].outputs;

                csv_output->ObjControlWord = CONTROLWORD_COMMAND_SHUTDOWN;
            }
            break;

        default:
            break;
    }

    vTaskDelay(100);

    ethercat_common_stop();
    return 0;
}
CSH_CMD_EXPORT(esc_stop, );

static int esc_motor_run(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    TCiA402PDO1601 *csp_output;
    TCiA402PDO1602 *csv_output;

    switch (motor_mode) {
        case MOTOR_MODE_CSV_CSP:
            break;
        case MOTOR_MODE_CSP:
            for (uint16_t cnt = 1; cnt <= ec_slavecount; cnt++) {
                csp_output = (TCiA402PDO1601 *)ec_slave[cnt].outputs;

                csp_output->ObjControlWord = CONTROLWORD_COMMAND_ENABLEOPERATION;
            }

            break;
        case MOTOR_MODE_CSV:
            for (uint16_t cnt = 1; cnt <= ec_slavecount; cnt++) {
                csv_output = (TCiA402PDO1602 *)ec_slave[cnt].outputs;

                csv_output->ObjControlWord = CONTROLWORD_COMMAND_ENABLEOPERATION;
            }
            break;

        default:
            break;
    }

    return 0;
}
CSH_CMD_EXPORT(esc_motor_run, );

static int esc_motor_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    TCiA402PDO1601 *csp_output;
    TCiA402PDO1602 *csv_output;

    switch (motor_mode) {
        case MOTOR_MODE_CSV_CSP:
            break;
        case MOTOR_MODE_CSP:
            for (uint16_t cnt = 1; cnt <= ec_slavecount; cnt++) {
                csp_output = (TCiA402PDO1601 *)ec_slave[cnt].outputs;

                csp_output->ObjControlWord = CONTROLWORD_COMMAND_SHUTDOWN;
            }

            break;
        case MOTOR_MODE_CSV:
            for (uint16_t cnt = 1; cnt <= ec_slavecount; cnt++) {
                csv_output = (TCiA402PDO1602 *)ec_slave[cnt].outputs;

                csv_output->ObjControlWord = CONTROLWORD_COMMAND_SHUTDOWN;
            }
            break;

        default:
            break;
    }

    return 0;
}
CSH_CMD_EXPORT(esc_motor_stop, );

static int esc_motor_control(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    TCiA402PDO1601 *csp_output;
    TCiA402PDO1602 *csv_output;

    int speed_pos = atoi(argv[1]);

    switch (motor_mode) {
        case MOTOR_MODE_CSV_CSP:
            break;
        case MOTOR_MODE_CSP:
            for (uint16_t cnt = 1; cnt <= ec_slavecount; cnt++) {
                csp_output = (TCiA402PDO1601 *)ec_slave[cnt].outputs;

                csp_output->ObjTargetPosition = speed_pos;
            }

            break;
        case MOTOR_MODE_CSV:
            for (uint16_t cnt = 1; cnt <= ec_slavecount; cnt++) {
                csv_output = (TCiA402PDO1602 *)ec_slave[cnt].outputs;

                csv_output->ObjTargetVelocity = speed_pos;
            }
            break;

        default:
            break;
    }

    return 0;
}
CSH_CMD_EXPORT(esc_motor_control, );
