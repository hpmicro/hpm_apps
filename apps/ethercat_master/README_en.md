# Ethercat master

## Depend on SDK1.9.0

## Overview

This chapter mainly introduces the use of the open-source EtherCAT master named **SOEM**. Due to the restrictions of the **SOEM** open-source license, the source code will not be included in the SDK. Please download the source code yourself and install it in the following directory.

SOEM download url：https://github.com/hpmicro/ethercat_soem/releases.

![soem](doc/api/assets/soem.png)

This barebone example needs to be used with the HPM6E00EVK ethercat/ecat_io demo in hpm_sdk.
This cia402 example needs to be used with the HPM6E00EVK ethercat/ecat_cia402 demo in hpm_sdk.

## Feature support

- Multi-Slave Scanning: Supports scanning and configuration of multiple EtherCAT slave devices.
- Multi-Slave DC Synchronization: Enables Distributed Clocks (DC) synchronization across multiple slaves, including local clock calibration for precise timing alignment.
- CoE (CANopen over EtherCAT) Support: Implements CANopen application layer protocols for parameter configuration and process data exchange.
- PDO Mapping: Configures Process Data Objects (PDOs) to define real-time data exchange between master and slaves.
- Real-Time PDO Execution: PDO communication is executed within timer interrupts to ensure deterministic timing and enhanced real-time performance.

## Demo

### common init

- init soem master

```
    if (ec_init(NULL) > 0) {
        printf("ec_init succeeded.\n");
    ....

```

- find ethercat slave and config

```

    /* find and auto-config slaves */
    if (ec_config_init(FALSE) > 0) {
        printf("%d slaves found and configured.\n", ec_slavecount);
    ....
```

- enable DC and calibrate system time

```
    ec_configdc();
    ec_dcsync0(1, TRUE, 1 * 1000000, 20000); // SYNC0 on slave 1

    uint32 dc_time;
    for (uint16_t i = 0; i < 16000; i++) {
        dc_time = 0;
        ec_FRMW(ec_slave[1].configadr, ECT_REG_DCSYSTIME, 4, &dc_time, EC_TIMEOUTRET);
    }

```

- enable periodic timer for pdo

### barebone demo

- read slave input and toggle ouput to control led in hpm6e00evk

```
    uint32_t *output = (uint32_t *)ec_slave[1].outputs;
    uint32_t *input = (uint32_t *)ec_slave[1].inputs;
    output[0] = 0x0000003 ^ output[0];
    printf("ec_slave[1].outputs:%08x\r\n", output[0]);
    printf("ec_slave[1].inputs:%08x\r\n", input[0]);
```

### cia402 demo

- config SDO

- control motor to run mode and set speed to 100

```
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
```
## Running the example

The barebore example demonstrates that the HPM6750EVK2 connects two HPM6E00EVKs in series and controls the two leds toggle on the HPM6E00EVK.

![](doc/api/assets/ethercat1.jpg)
![](doc/api/assets/ethercat2.png)

The cia402 example demonstrates that the HPM6750EVK2 control two HPM6E00EVKs with motor running.

![](doc/api/assets/ecat_cia402.gif)

## Build

![windows with sdk_env](doc/api/assets/download.png)

## API

:::{eval-rst}

About software API: `API doc <../../_static/apps/ethercat_master/html/index.html>`_ 。
:::