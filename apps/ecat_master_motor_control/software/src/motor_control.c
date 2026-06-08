/**
 * @file motor_control.c
 * @brief Motor control and EtherCAT implementation
 */

#include "motor_control.h"
#include "data_common.h"
#include "ec_master.h"
#include "cia402_def.h"

static void ec_pdo_callback(ec_slave_t *slave, uint8_t *output, uint8_t *input);

extern data_pack_t data_pack;
extern ec_master_t g_ec_master;

static ec_pdo_entry_info_t coe402_1602[] = {
    { 0x6040, 0x00, 0x10 },
    { 0x60ff, 0x00, 0x20 },
    { 0x0000, 0x00, 0x10 },
};

static ec_pdo_entry_info_t coe402_1a02[] = {
    { 0x6041, 0x00, 0x10 },
    { 0x6064, 0x00, 0x20 },
    { 0x0000, 0x00, 0x10 },
};

static ec_pdo_info_t cia402_csv_rxpdos[] = {
    { 0x1602, 3, &coe402_1602[0] },
};

static ec_pdo_info_t cia402_csv_txpdos[] = {
    { 0x1a02, 3, &coe402_1a02[0] },
};

static ec_sync_info_t cia402_csv_syncs[] = {
    { 2, EC_DIR_OUTPUT, 1, cia402_csv_rxpdos },
    { 3, EC_DIR_INPUT, 1, cia402_csv_txpdos },
};

static ec_pdo_entry_info_t coe402_1601[] = {
    { 0x6040, 0x00, 0x10 },
    { 0x607a, 0x00, 0x20 },
    { 0x0000, 0x00, 0x10 },
};

static ec_pdo_entry_info_t coe402_1a01[] = {
    { 0x6041, 0x00, 0x10 },
    { 0x6064, 0x00, 0x20 },
    { 0x0000, 0x00, 0x10 },
};

static ec_pdo_info_t cia402_csp_rxpdos[] = {
    { 0x1601, 3, &coe402_1601[0] },
};

static ec_pdo_info_t cia402_csp_txpdos[] = {
    { 0x1a01, 3, &coe402_1a01[0] },
};

static ec_sync_info_t cia402_csp_syncs[] = {
    { 2, EC_DIR_OUTPUT, 1, cia402_csp_rxpdos },
    { 3, EC_DIR_INPUT, 1, cia402_csp_txpdos },
};

static ec_pdo_entry_info_t coe402_1600[] = {
    { 0x6040, 0x00, 0x10 },
    { 0x607a, 0x00, 0x20 },
    { 0x60FF, 0x00, 0x20 },
    { 0x6060, 0x00, 0x10 },
};

static ec_pdo_entry_info_t coe402_1a00[] = {
    { 0x6041, 0x00, 0x10 },
    { 0x6064, 0x00, 0x20 },
    { 0x606C, 0x00, 0x20 },
    { 0x6061, 0x00, 0x10 },
};

static ec_pdo_info_t cia402_cspcsv_rxpdos[] = {
    { 0x1600, 3, &coe402_1600[0] },
};

static ec_pdo_info_t cia402_cspcsv_txpdos[] = {
    { 0x1a00, 3, &coe402_1a00[0] },
};

static ec_sync_info_t cia402_cspcsv_syncs[] = {
    { 2, EC_DIR_OUTPUT, 1, cia402_cspcsv_rxpdos },
    { 3, EC_DIR_INPUT, 1, cia402_cspcsv_txpdos },
};

static int32_t internal_target_pos[32] = { 0 };

static void ec_pdo_callback(ec_slave_t *slave, uint8_t *output, uint8_t *input)
{
    if (data_pack.mode == MODE_CSP) {
        int slave_idx = -1;
        /* 只作用于Ui所选择的从机设备 */
        for (uint32_t i = 0; i < g_ec_master.slave_count; i++) {
            if (&g_ec_master.slaves[i] == slave) {
                slave_idx = i;
                break;
            }
        }

        if (slave_idx < 0 || slave_idx >= 32) {
            return;
        }
        int32_t *target_pos_ptr;
        int32_t actual_pos;
        uint16_t status_word;

        TCiA402PDO1601 *out_pdo = (TCiA402PDO1601 *)output;
        TCiA402PDO1A01 *in_pdo = (TCiA402PDO1A01 *)input;
        target_pos_ptr = &out_pdo->ObjTargetPosition;
        actual_pos = in_pdo->ObjPositionActualValue;
        status_word = in_pdo->ObjStatusWord;

        /* 当前从机不是UI所选择的从机 */
        if (slave_idx != data_pack.cur_index) {
            internal_target_pos[slave_idx] = actual_pos;
            *target_pos_ptr = internal_target_pos[slave_idx];
            return;
        }

        /* 确保电机处于工作状态 */
        if ((status_word & STATUSWORD_STATE_MASK) == STATUSWORD_STATE_OPERATIONENABLED) {
            int32_t goal_pos = (int32_t)data_pack.position;
            int32_t diff = goal_pos - internal_target_pos[slave_idx];
            int32_t step = MODE_STEP_SIZE;

            if (abs(diff) <= step) {
                internal_target_pos[slave_idx] = goal_pos;
            } else {
                if (diff > 0) {
                    internal_target_pos[slave_idx] += step;
                } else {
                    internal_target_pos[slave_idx] -= step;
                }
            }
            *target_pos_ptr = internal_target_pos[slave_idx];
        } else {
            /* 不是处于工作状态 */
            internal_target_pos[slave_idx] = actual_pos;
            *target_pos_ptr = actual_pos;
        }
    }
}

void ec_start(void)
{
    static ec_slave_config_t slave_cia402_config;
    uint8_t operation_mode = 0;

    slave_cia402_config.dc_assign_activate = 0x300;

    slave_cia402_config.dc_sync[0].cycle_time = ETHERCAT_MOTOR_CYCLE_TIME_US * 1000;
    slave_cia402_config.dc_sync[0].shift_time = 1000000;
    slave_cia402_config.dc_sync[1].cycle_time = 0;
    slave_cia402_config.dc_sync[1].shift_time = 0;

    if (g_ec_master.slave_count == 0) {
        return;
    }

    switch (data_pack.mode) {
        case MODE_CSP_CSV:
            slave_cia402_config.sync = cia402_cspcsv_syncs;
            slave_cia402_config.sync_count = sizeof(cia402_cspcsv_syncs) / sizeof(ec_sync_info_t);
            operation_mode = CYCLIC_SYNC_VELOCITY_MODE;
            break;
        case MODE_CSP:
            slave_cia402_config.sync = cia402_csp_syncs;
            slave_cia402_config.sync_count = sizeof(cia402_csp_syncs) / sizeof(ec_sync_info_t);
            operation_mode = CYCLIC_SYNC_POSITION_MODE;
            break;
        case MODE_CSV:
            slave_cia402_config.sync = cia402_csv_syncs;
            slave_cia402_config.sync_count = sizeof(cia402_csv_syncs) / sizeof(ec_sync_info_t);
            operation_mode = CYCLIC_SYNC_VELOCITY_MODE;
            break;

        default:
            break;
    }
    slave_cia402_config.pdo_callback = ec_pdo_callback;

    for (uint32_t i = 0; i < g_ec_master.slave_count; i++) {
        if (g_ec_master.slaves[i].sii.vendor_id != 0x0048504D) { // HPMicro
            EC_LOG_ERR("Unsupported slave found: vendor_id=0x%08x\n", g_ec_master.slaves[i].sii.vendor_id);
            return;
        }

        switch (g_ec_master.slaves[i].sii.product_code) {
            case 0x00000003: // CIA402
            case 0x00000004: // CIA402 + FOE
                g_ec_master.slaves[i].config = &slave_cia402_config;
                break;
            default:
                EC_LOG_ERR("Unsupported slave found: product_code=0x%08x\n", g_ec_master.slaves[i].sii.product_code);
                return;
        }

        ec_datagram_t datagram;
        ec_datagram_init(&datagram, 1024);

        int ret = ec_coe_download(&g_ec_master,
                                  i,
                                  &datagram,
                                  0x6060,
                                  0x00,
                                  &operation_mode,
                                  1,
                                  false);

        ec_datagram_clear(&datagram);
        if (ret < 0) {
            EC_LOG_ERR("Failed to set operation mode for slave %u: %d\n", (unsigned int)i, ret);
        } else {
            EC_LOG_INFO("Set operation mode %d for slave %u\n", operation_mode, (unsigned int)i);
        }
    }

    g_ec_master.cycle_time = ETHERCAT_MOTOR_CYCLE_TIME_US * 1000;       // cycle time in ns
    g_ec_master.shift_time = ETHERCAT_MOTOR_CYCLE_TIME_US * 1000 * 0.2; // 20% shift time in ns
    g_ec_master.dc_sync_with_dc_ref_enable = true;                      // enable DC sync with dc reference clock
    ec_master_start(&g_ec_master);
}

void ec_stop(void)
{
    if (g_ec_master.slave_count == 0) {
        return;
    }

    ec_master_stop(&g_ec_master);
}

void motor_control_update(void)
{
    if (g_ec_master.slave_count == 0) {
        return;
    }

    if (data_pack.mode == MODE_CSP_CSV) {
        ((TCiA402PDO1600 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetVelocity = (data_pack.dir == DIR_FORWORD) ? (int32_t)data_pack.speed : (int32_t)(0 - data_pack.speed);
    } else if (data_pack.mode == MODE_CSP) {
        // ((TCiA402PDO1601 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetPosition = (data_pack.dir == DIR_FORWORD) ? (int32_t)data_pack.position : (int32_t)(0 - data_pack.position);
    } else if (data_pack.mode == MODE_CSV) {
        ((TCiA402PDO1602 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetVelocity = (data_pack.dir == DIR_FORWORD) ? (int32_t)data_pack.speed : (int32_t)(0 - data_pack.speed);
    }
}

void motor_control_start(void)
{
    if (g_ec_master.slave_count == 0) {
        return;
    }

    if (data_pack.mode == MODE_CSP_CSV) {
        ((TCiA402PDO1600 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjControlWord = CONTROLWORD_COMMAND_ENABLEOPERATION;
    } else if (data_pack.mode == MODE_CSP) {
        ((TCiA402PDO1601 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjControlWord = CONTROLWORD_COMMAND_ENABLEOPERATION;
    } else if (data_pack.mode == MODE_CSV) {
        ((TCiA402PDO1602 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjControlWord = CONTROLWORD_COMMAND_ENABLEOPERATION;
    }
}

void motor_control_stop(void)
{
    if (g_ec_master.slave_count == 0) {
        return;
    }

    if (data_pack.mode == MODE_CSP_CSV) {
        ((TCiA402PDO1600 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjControlWord = CONTROLWORD_COMMAND_SHUTDOWN;
        ((TCiA402PDO1600 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetVelocity = 0;
    } else if (data_pack.mode == MODE_CSP) {
        ((TCiA402PDO1601 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjControlWord = CONTROLWORD_COMMAND_SHUTDOWN;
        // ((TCiA402PDO1601 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetPosition = 0;
    } else if (data_pack.mode == MODE_CSV) {
        ((TCiA402PDO1602 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjControlWord = CONTROLWORD_COMMAND_SHUTDOWN;
        ((TCiA402PDO1600 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetVelocity = 0;
    }
}

void motor_control_forward(void)
{
    data_pack.dir = DIR_FORWORD;
    motor_control_update();
}

void motor_control_backward(void)
{
    data_pack.dir = DIR_BACKWORD;
    motor_control_update();
}

void motor_control_reset(void)
{
    if (g_ec_master.slave_count == 0) {
        return;
    }

    if (data_pack.mode == MODE_CSP_CSV) {
        ((TCiA402PDO1600 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetVelocity = 0;
    } else if (data_pack.mode == MODE_CSP) {
        // ((TCiA402PDO1601 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetPosition = 0;
    } else if (data_pack.mode == MODE_CSV) {
        ((TCiA402PDO1602 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetVelocity = 0;
    }
}

uint32_t get_actual_value(void)
{
    if (g_ec_master.slave_count == 0) {
        return 0;
    }

    if (data_pack.mode == MODE_CSP_CSV) {
        return abs(((TCiA402PDO1A00 *)ec_master_get_slave_domain_input(&g_ec_master, data_pack.cur_index))->ObjPositionActualValue);
    } else if (data_pack.mode == MODE_CSP) {
        return abs(((TCiA402PDO1A01 *)ec_master_get_slave_domain_input(&g_ec_master, data_pack.cur_index))->ObjPositionActualValue);
    } else if (data_pack.mode == MODE_CSV) {
        return abs(((TCiA402PDO1A02 *)ec_master_get_slave_domain_input(&g_ec_master, data_pack.cur_index))->ObjPositionActualValue);
    }
    return 0;
}

uint32_t get_target_value(void)
{
    if (g_ec_master.slave_count == 0) {
        return 0;
    }

    if (data_pack.mode == MODE_CSP_CSV) {
        return abs(((TCiA402PDO1600 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetVelocity);
    } else if (data_pack.mode == MODE_CSP) {
        return abs(((TCiA402PDO1601 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetPosition);
    } else if (data_pack.mode == MODE_CSV) {
        return abs(((TCiA402PDO1602 *)ec_master_get_slave_domain_output(&g_ec_master, data_pack.cur_index))->ObjTargetVelocity);
    }
    return 0;
}
