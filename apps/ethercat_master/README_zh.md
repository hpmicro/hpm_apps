# Ethercat master

## 概述

本节主要介绍基于开源 Ethercat 主站 **SOEM** 的使用。由于 **SOEM** 开源协议的限制，故源代码不会放在 SDK 中，请自行下载源代码并安装到下面目录。

SOEM 下载链接：https://github.com/hpmicro/ethercat_soem/releases。

![soem](doc/api/assets/soem.png)

本节例程需要搭配 HPM6E00EVK 和 hpm_sdk 中 ethercat/ecat_io 例程使用。

## 例程

- 初始化 soem 主站

```
    if (ec_init(NULL) > 0) {
        printf("ec_init succeeded.\n");
    ....

```

- 查找 ethercat 从站设备并进行一些配置

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

- 读取 IO 输入值，并反转输出 IO，用于控制 HPM6E00EVK 两个 led 反转

```
    ec_send_processdata();
    ec_receive_processdata(EC_TIMEOUTRET);
    uint32_t *output = (uint32_t *)ec_slave[1].outputs;
    uint32_t *input = (uint32_t *)ec_slave[1].inputs;
    output[0] = 0x0000003 ^ output[0];
    printf("ec_slave[1].outputs:%08x\r\n", output[0]);
    printf("ec_slave[1].inputs:%08x\r\n", input[0]);
```

## 运行现象

示例演示 HPM6750EVK2 串联两个 HPM6E00EVK, 并控制 HPM6E00EVK 上两个 led 交替闪烁。

![](doc/api/assets/ethercat1.jpg)
![](doc/api/assets/ethercat2.png)

## 工程构建

![windows 下使用 sdk_env 构建](doc/api/assets/download.png)

## API

:::{eval-rst}

关于软件API 请查看 `方案API 文档 <../../_static/apps/ethercat_master/html/index.html>`_ 。
:::
