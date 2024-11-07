# Ethercat master

## Overview

This chapter mainly introduces the use of the open-source EtherCAT master named **SOEM**. Due to the restrictions of the **SOEM** open-source license, the source code will not be included in the SDK. Please download the source code yourself and install it in the following directory.

SOEM download url：https://github.com/hpmicro/ethercat_soem/releases/tag/v1.3.1.

![soem](doc/api/assets/soem.png)

This example needs to be used with the HPM6E00EVK ethercat/ecat_io demo in hpm_sdk.

## Demo

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

        /* Check network  setup */
        if (network_configuration()) {
            for (slc = 1; slc <= ec_slavecount; slc++) {
                printf("Found %s at position %d\n", ec_slave[slc].name, slc);
            }
    ....
```

- read slave input and toggle ouput to control led in hpm6e00evk

```
    ec_send_processdata();
    ec_receive_processdata(EC_TIMEOUTRET);
    uint32_t *output = (uint32_t *)ec_slave[1].outputs;
    uint32_t *input = (uint32_t *)ec_slave[1].inputs;
    output[0] = 0x0000003 ^ output[0];
    printf("ec_slave[1].outputs:%08x\r\n", output[0]);
    printf("ec_slave[1].inputs:%08x\r\n", input[0]);
```

## Running the example

The example demonstrates that the HPM6750EVK2 connects two HPM6E00EVKs in series and controls the two leds toggle on the HPM6E00EVK.

![](doc/api/assets/ethercat1.jpg)
![](doc/api/assets/ethercat2.png)

## Build

![windows with sdk_env](doc/api/assets/download.png)

## API

:::{eval-rst}

About software API: `API doc <../../_static/apps/ethercat_master/html/index.html>`_ 。
:::