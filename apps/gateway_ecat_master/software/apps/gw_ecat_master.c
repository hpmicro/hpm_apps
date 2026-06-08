/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "gw_ecat_master.h"
#include "ec_eeprom.h" // cherryecat_eepromdata[2048] EEPROM data buffer, please generate by esi_parse.py

static ec_master_t g_ec_master;
static ec_slave_config_t slave_gw_config __attribute__((section(".fast_ram")));
gw_can_tx_std can_tx_std = {0};
gw_can_tx_ext can_tx_ext = {0};
gw_can_rx_std can_rx_std = {0};
gw_can_rx_ext can_rx_ext = {0};
static uint32_t last_can_tx_count = 0;
static uint32_t can_tx_count = 0;
static uint32_t last_can_rx_count = 0;
static uint32_t can_rx_count = 0;

static ec_pdo_entry_info_t eni_0001_1600[] = {
    { 0x7000, 0x01, 0x10 },  // TxCounter
    { 0x7000, 0x02, 0x10 },  // RxCounter
    { 0x7000, 0x03, 0x10 },  // NumOfTxMessage
    { 0x7000, 0x04, 0x60 },  // TxMessage1
    { 0x7001, 0x01, 0x10 },  // TxCounter
    { 0x7001, 0x02, 0x10 },  // RxCounter
    { 0x7001, 0x03, 0x10 },  // NumOfTxMessage
    { 0x7001, 0x04, 0x80 },  // TxMessage1
};

static ec_pdo_entry_info_t eni_0001_1a00[] = {
    { 0x6000, 0x01, 0x10 },  // TxCounter
    { 0x6000, 0x02, 0x10 },  // RxCounter
    { 0x6000, 0x03, 0x10 },  // NumOfRxMessage
    { 0x6000, 0x04, 0x10 },  // TxTransNum
    { 0x6000, 0x05, 0x50 },  // RxMessage1
    { 0x6001, 0x01, 0x10 },  // TxCounter
    { 0x6001, 0x02, 0x10 },  // RxCounter
    { 0x6001, 0x03, 0x10 },  // NumOfRxMessage
    { 0x6001, 0x04, 0x10 },  // TxTransNum
    { 0x6001, 0x05, 0x70 },  // RxMessage1
};

static ec_pdo_info_t eni_0001_rxpdos[] = {
    { 0x1600, 8, &eni_0001_1600[0] },
};

static ec_pdo_info_t eni_0001_txpdos[] = {
    { 0x1a00, 10, &eni_0001_1a00[0] },
};

static ec_sync_info_t eni_0001_syncs[] = {
    { 2, EC_DIR_OUTPUT, 1, eni_0001_rxpdos },
    { 3, EC_DIR_INPUT, 1, eni_0001_txpdos },
};

static void gw_ecat_master_coe_read(ec_master_t *master, uint32_t slave_idx, uint16_t index, uint8_t subindex, uint8_t *buf, uint32_t *size)
{
    int ret;
    uint32_t actual_size;
    static ec_datagram_t datagram;

    ec_datagram_init(&datagram, 512);
    ret = ec_coe_upload(master,
                        slave_idx,
                        &datagram,
                        index,
                        subindex,
                        buf,
                        4,
                        &actual_size,
                        false);
    if (ret < 0) {
        LOG_E("Slave %u coe_read failed: %d\n", slave_idx, ret);
    }
    *size = actual_size;
    ec_datagram_clear(&datagram);
}

static void gw_ecat_master_coe_write(ec_master_t *master, uint32_t slave_idx, uint16_t index, uint8_t subindex, uint8_t *buf, uint32_t size)
{
    int ret;
    static ec_datagram_t datagram;

    ec_datagram_init(&datagram, 512);

    ret = ec_coe_download(master,
                          slave_idx,
                          &datagram,
                          index,
                          subindex,
                          buf,
                          size,
                          false);
    if (ret < 0) {
        LOG_E("Slave %u coe_write failed: %d\n", slave_idx, ret);
    }
    ec_datagram_clear(&datagram);
}

static void gw_ecat_master_set_can_baudrate(uint8_t baudrate_index)
{
    for(uint32_t i=0;i<g_ec_master.slave_count;i++) {
        gw_ecat_master_coe_write(&g_ec_master, i, 0xF800, 2, (uint8_t *)&baudrate_index, 1);
    }
}

static void gw_ecat_master_set_mode(uint8_t mode)
{
    for(uint32_t i=0;i<g_ec_master.slave_count;i++) {
        uint16_t read_data = 0;
        uint32_t size;
        gw_ecat_master_coe_read(&g_ec_master, i, 0x8000, 32, (uint8_t *)&read_data, &size);
        read_data &= ~(1<<4);
        read_data |= mode<<4;
        gw_ecat_master_coe_write(&g_ec_master, i, 0x8000, 32, (uint8_t *)&read_data, 2);
    }
}

static void gw_ecat_master_set_trigger(uint8_t trigger)
{
    for(uint32_t i=0;i<g_ec_master.slave_count;i++) {
        uint16_t read_data = 0;
        uint32_t size;
        gw_ecat_master_coe_read(&g_ec_master, i, 0x8000, 32, (uint8_t *)&read_data, &size);
        read_data &= ~(1<<2);
        read_data |= trigger<<2;
        gw_ecat_master_coe_write(&g_ec_master, i, 0x8000, 32, (uint8_t *)&read_data, 2);
        read_data &= ~(1<<2);
        gw_ecat_master_coe_write(&g_ec_master, i, 0x8000, 32, (uint8_t *)&read_data, 2);
    }
}

static void gw_ecat_master_set_frame_mode(uint8_t frame_mode)
{
    for(uint32_t i=0;i<g_ec_master.slave_count;i++) {
        uint16_t read_data = 0;
        uint32_t size;
        gw_ecat_master_coe_read(&g_ec_master, i, 0x8000, 32, (uint8_t *)&read_data, &size);
        read_data &= ~(1<<3);
        read_data |= frame_mode<<3;
        gw_ecat_master_coe_write(&g_ec_master, i, 0x8000, 32, (uint8_t *)&read_data, 2);
    }
}

static void gw_ecat_master_set_cycle_time(uint32_t cycle_time)
{
    for(uint32_t i=0;i<g_ec_master.slave_count;i++) {
        gw_ecat_master_coe_write(&g_ec_master, i, 0x8000, 37, (uint8_t *)&cycle_time, 4);
    }
}

static void gw_ecat_pdo_callback(ec_slave_t *slave, uint8_t *output, uint8_t *input)
{

}

void gw_ecat_master_handle(gw_mq_msg msg)
{
    gw_mq_msg mq_msg;
    uint8_t state = 0;
    if(g_ec_master.slave_count>0) {
        for(uint32_t i=0;i<g_ec_master.slave_count;i++) {
            ec_slave_t *slave = g_ec_master.slaves+i;
            state |= slave->current_state;
        }
    }
	switch (msg.submsg_id)
	{
		case GW_MQ_SUBMSG_ECAT_UPDATE_ID:
            static uint8_t last_state = 0;
            if(g_ec_master.slave_count==0) {
                state = EC_SLAVE_STATE_INIT;
                ec_memset((uint8_t *)&can_rx_std, 0, 10);
                ec_memset((uint8_t *)&can_rx_ext, 0, 14);
            }
            if(state != last_state) {
                mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_SET_STATE_ID;
                mq_msg.param = state;
                gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
                last_state = state;
                if(state == EC_SLAVE_STATE_INIT) {
                    mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
                    mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_SET_STOP_ID;
                    gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
                }
            }
            if(state == EC_SLAVE_STATE_OP) {
                for(uint32_t i=0;i<g_ec_master.slave_count;i++) {
                    uint8_t *buffer_out = ec_master_get_slave_domain_output(&g_ec_master, i);
                    if(buffer_out) {
                        ec_memcpy(buffer_out+6, (uint8_t *)&can_tx_std, 12);
                        ec_memcpy(buffer_out+24, (uint8_t *)&can_tx_ext, 16);
                    }
                }
                for(uint32_t i=0;i<g_ec_master.slave_count;i++) {
                    uint8_t *buffer_in = ec_master_get_slave_domain_input(&g_ec_master, i);
                    if(buffer_in) {
                        can_tx_count = *((uint16_t*)buffer_in);
                        can_rx_count = *((uint16_t*)(buffer_in+2));
                        ec_memcpy((uint8_t *)&can_rx_std, buffer_in+8, 10);
                        ec_memcpy((uint8_t *)&can_rx_ext, buffer_in+26, 14);
                    }
                }
            }

            static uint8_t slves_num = 0;
            if(slves_num != g_ec_master.slave_count) {
                slves_num = g_ec_master.slave_count;
                mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_SET_SLAVES_NUM_ID;
                mq_msg.param = slves_num;
                gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
            }
            if(last_can_tx_count != can_tx_count) {
                last_can_tx_count = can_tx_count;
                mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_SET_TXNUM_ID;
                mq_msg.param = last_can_tx_count;
                gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
            }
            if(last_can_rx_count != can_rx_count) {
                last_can_rx_count = can_rx_count;
                mq_msg.msg_id = GW_MQ_MSG_LCD_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_LCD_SET_RXNUM_ID;
                mq_msg.param = last_can_rx_count;
                gw_mq_send(GW_MQ_LCD_ID, &mq_msg, 0);
            }
			break;
        case GW_MQ_SUBMSG_ECAT_START_ID:
            if(g_ec_master.slave_count == 0) {
                LOG_W("No slave found!\r\n");
                break;
            }
            slave_gw_config.dc_assign_activate = 0x300;
            slave_gw_config.dc_sync[0].cycle_time = 1000 * 1000;
            slave_gw_config.dc_sync[0].shift_time = 1000000;
            slave_gw_config.dc_sync[1].cycle_time = 0;
            slave_gw_config.dc_sync[1].shift_time = 0;
            slave_gw_config.sync = eni_0001_syncs;
            slave_gw_config.sync_count = sizeof(eni_0001_syncs) / sizeof(ec_sync_info_t);
            slave_gw_config.pdo_callback = gw_ecat_pdo_callback;

            for (uint32_t i = 0; i < g_ec_master.slave_count; i++) {
                if (g_ec_master.slaves[i].sii.vendor_id != 0x0048504D) { // HPMicro
                    LOG_E("Unsupported slave found: vendor_id=0x%08x\n", g_ec_master.slaves[i].sii.vendor_id);
                }
                g_ec_master.slaves[i].config = &slave_gw_config;
            }
            g_ec_master.cycle_time = 1000 * 1000;       // cycle time in ns
            g_ec_master.shift_time = 1000 * 1000 * 0.2; // 20% shift time in ns
            g_ec_master.dc_sync_with_dc_ref_enable = false;       // enable DC sync with dc reference clock
            ec_master_start(&g_ec_master);
            break;
        case GW_MQ_SUBMSG_ECAT_STOP_ID:
            ec_master_stop(&g_ec_master);
            break;
        case GW_MQ_SUBMSG_ECAT_SET_CAN_BAUDRATE_ID:
            if(state == EC_SLAVE_STATE_PREOP) {
                gw_ecat_master_set_can_baudrate(msg.param);
            }
            break;
        case GW_MQ_SUBMSG_ECAT_SET_MODE_ID:
            if(state == EC_SLAVE_STATE_PREOP) {
                gw_ecat_master_set_mode(msg.param);
            }
            break;
        case GW_MQ_SUBMSG_ECAT_SET_TRIGGER_ID:
                gw_ecat_master_set_trigger(msg.param);
            break;
        case GW_MQ_SUBMSG_ECAT_SET_CYCLE_TIME_ID:
            if(state == EC_SLAVE_STATE_PREOP) {
                gw_ecat_master_set_cycle_time(msg.param);
            }
            break;
        case GW_MQ_SUBMSG_ECAT_SET_FRAME_MODE_ID:
            if(state == EC_SLAVE_STATE_PREOP) {
                gw_ecat_master_set_frame_mode(msg.param);
            }
            break;
		default:
			break;
	}
}


void gw_ecat_master_thread(void *argument)
{
    (void)argument;

    osStatus_t ret;
    gw_mq_msg mq_msg;
    
    LOG_D("gw_ecat_master_thread run\r\n");    

    ec_master_cmd_init(&g_ec_master);
    ec_master_init(&g_ec_master, 0);

    for(;;)
    {
        ret = gw_mq_recv(GW_MQ_ECAT_ID, &mq_msg, osWaitForever);

        if(ret < 0) {
            continue;
            LOG_E("%s mq recv fail!\r\n",__func__);
        }

        switch (mq_msg.msg_id)
        {
            case GW_MQ_MSG_ECAT_ID:
                gw_ecat_master_handle(mq_msg);
                break;
            default:
                break;
        }
    }
}