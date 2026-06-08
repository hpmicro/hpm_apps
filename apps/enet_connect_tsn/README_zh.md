# HPM6E系列：将TSN作为交换机的多端口以太网方案

## 依赖SDK1.11.0

## 概述

本方案使用HPM6E系列MCU的ENET功能代替TSW的CPU PORT功能，可以帮助用户快速搭建带交换机的以太网通信系统。系统框图如下所示。
![system block diagram](doc/api/assets/sys_diagram.png)
其中P1 P2为TSW的P1 P2 PORT，作为对外的以太网PORT。TSW P3端口通过电阻直连到ENET端口，将ENET端口作为对内的端口。
本方案中，既可以利用TSW的硬件交换功能，串联组网，又可以利用ENET的稳定性进行以太网通信。

特点：
1. 两个对外端口均支持10M/100M/1000M以太网通信，且自动协商通信速率。
2. 集成FreeRTOS，支持多任务并发运行，并集成LWIP协议栈。
3. 支持单网卡模式，即从外部看，本设备作为一个网络设备，拥有一套独立的MAC IP。对于两个对外的网口，可以使用任意一个网口进行通信。
4. 支持双网卡模式，即从外部看，本设备作为两个网络设备，拥有两套独立的MAC IP。对于两个对外的网口，每个网口均可独立通信。
5. 本方案硬件带SSD1306 OLED显示，并且集成了U8G2库。
6. 支持USB SH 命令行，用户可通过可自行添加USB SH命令功能

## 例程说明 
本方案硬件为[HPM6E00EnetConnectTsnRevB](hardware/HPM6E00EnetConnectTsnRevB.pdf)，如下图所示。
![hardware](doc/api/assets/hardware.png)

| 编号 | 器件 | 描述 |
| --- | ---     | --- |
| 1   | OLED显示屏 | 双色，SSD306驱动芯片 |
| 2 5 | YT8531C   | PHY芯片，支持10M/100M/1000M以太网通信速率 |
| 3 6 | H5007NL   | 千兆网络变压器 |
| 4 7 | RJ45      | RJ45以太网端口 |
| 8   | MX25L12833FM2I-10G | FLASH存储器 |
| 9   | 33欧姆电阻 | TSW P3端口到ENET端口的电阻 |
| 10  | HPM6E80IVM1 | MCU |
| 11  | Type-C接口 | USB 功能 |
| 12  | Type-C接口 | USB转串口功能，LOG输出 |


### 软件
#### 软件架构
1. 软件代码均在software目录下。
    - apps 为应用目录，包含所有应用代码。
    - drivers 为驱动目录，包含所有驱动代码。
2. 带有FreeRTOS任务，支持多任务并发运行，集成LWIP协议栈，支持以太网通信。
    - main.c 为主程序，包含任务创建和调度。
    - FreeRTOSConfig.h 用于配置FreeRTOS参数
    - lwipopts_app.h 及  drivers\enet_tsn\ports\lwipopts.h 用于配置LWIP协议栈参数
    - csh_config.h 及 shell.h 用于配置USB SH 命令行参数
    - drivers\enet_tsn\common\netconf.h 用于配置网络参数，包括MAC IP、网关 IP、子网掩码等。
#### 主要软件功能设计
1. TSW 与 ENET 均工作在RGMII模式。且TSW P3口与ENET端口均在千兆模式，通过外部端口自协商确定通信速率后，可选择性打开或关闭TSW P1 P2 PORT的存储转发模式。如果检测到对外对口为1000M，TSW P1 P2 PORT将关闭存储转发模式，否则将打开存储转发模式。
2. 单网卡模式
![single network](doc/api/assets/single_net.png)
该模式下，本设备作为一个网络设备，拥有一套独立的MAC IP。对于两个对外的网口，可以使用任意一个网口进行通信。
3. 双网卡模式
![double network](doc/api/assets/double_net.png)
该模式下，本设备作为两个网络设备，拥有两套独立的MAC IP。对于两个对外的网口，每个网口均可独立通信。
4. 支持U8G2库，用于控制OLED显示。
    在drivers\drv_oled.c内已经注册了U8G2库需要的驱动函数
    ```
        u8g2_Setup_ssd1306_128x64_noname_f(
        &u8g2,
        U8G2_R0,
        drv_u8x8_byte_4wire_spi,
        drv_u8x8_gpio_and_delay
        );
    ```
    如果需要使用U8G2库，需要配置CMakeLists.txt文件.
    set(CONFIG_A_U8G2 1)
    set(CONFIG_A_SSD1306 1)
    sdk_compile_definitions(-DUSE_U8G2=1)
    默认会编译U8G2库，但是不使用U8G2库的函数。
    默认显示HPM LOGO。
#### 生成工程
- 通过HPM SDK Project Generator生成segger工程
![generate_project](doc/api/assets/generate_project.png)
board选择hpm6e_enet_connect_tsn, 应用选择enet_connect_tsn。

### 例程测试
1. 单网卡模式测试
    - 配置
    生成工程前需要配置CMakeLists.txt文件.
    sdk_compile_definitions(-D_ENET_PORT_COUNT=1)
    如果需要使用TCP_ECHO功能，需要配置CMakeLists.txt文件.
    sdk_compile_definitions(-D_TCP_ECHO=1)
    如果使用UDP_ECHO功能，需要配置CMakeLists.txt文件.
    sdk_compile_definitions(-D_TCP_ECHO=0)
    配置好CMakeLists.txt文件后，重新生成工程。
    - 测试
    单网卡模式下, 默认使用drivers\enet_tsn\common\netconf.h文件内的MAC IP、网关 IP、子网掩码等。
        ```
        /* MAC ADDRESS */
        #ifndef MAC0_CONFIG
        #define MAC0_CONFIG 98:2c:bc:b1:9f:27
        #endif

        /* Static IP ADDRESS */
        #ifndef IP0_CONFIG
        #define IP0_CONFIG 192.168.100.10
        #endif

        /* Netmask */
        #ifndef NETMASK0_CONFIG
        #define NETMASK0_CONFIG 255.255.255.0
        #endif

        /* Gateway Address */
        #ifndef GW0_CONFIG
        #define GW0_CONFIG 192.168.100.1
        #endif
        ```
    1. 单网卡模式下，使能UDP_ECHO功能
    编译下载到硬件板子后，通过网线一端连接PC机，一端连接任意一个板子上的网口，PC机的IP需要与板子上的IP在同一子网内，使用NetAssist工具连接，设置远程主机为板子上的IP，端口为5001，如下图所示。
    ![single_net_udp](doc/api/assets/net0_udp.png)

    2. 单网卡模式下，使能TCP_ECHO功能
    编译下载到硬件板子后，通过网线一端连接PC机，一端连接任意一个板子上的网口，PC机的IP需要与板子上的IP在同一子网内，使用NetAssist工具连接，设置远程主机为板子上的IP，端口为5001，如下图所示。
    ![single_net_tcp](doc/api/assets/net0_tcp.png)

2. 双网卡模式测试
    - 配置
    生成工程前需要配置CMakeLists.txt文件.
    sdk_compile_definitions(-D_ENET_PORT_COUNT=2)
    如果需要使用TCP_ECHO功能，需要配置CMakeLists.txt文件.
    sdk_compile_definitions(-D_TCP_ECHO=1)
    如果使用UDP_ECHO功能，需要配置CMakeLists.txt文件.
    sdk_compile_definitions(-D_TCP_ECHO=0)
    配置好CMakeLists.txt文件后，重新生成工程。
    - 测试
    双网卡模式下, 默认使用drivers\enet_tsn\common\netconf.h文件内的MAC IP、网关 IP、子网掩码等。
        ```
        /* MAC ADDRESS */
        #ifndef MAC0_CONFIG
        #define MAC0_CONFIG 98:2c:bc:b1:9f:27
        #endif

        #ifndef MAC1_CONFIG
        #define MAC1_CONFIG 98:2c:bc:b1:9f:37
        #endif

        /* Static IP ADDRESS */
        #ifndef IP0_CONFIG
        #define IP0_CONFIG 192.168.100.10
        #endif

        #ifndef IP1_CONFIG
        #define IP1_CONFIG 192.168.200.10
        #endif

        /* Netmask */
        #ifndef NETMASK0_CONFIG
        #define NETMASK0_CONFIG 255.255.255.0
        #endif

        #ifndef NETMASK1_CONFIG
        #define NETMASK1_CONFIG 255.255.255.0
        #endif

        /* Gateway Address */
        #ifndef GW0_CONFIG
        #define GW0_CONFIG 192.168.100.1
        #endif

        #ifndef GW1_CONFIG
        #define GW1_CONFIG 192.168.200.1
        #endif
        ```
    1. 双网卡模式下，使能UDP_ECHO功能
    编译下载到硬件板子后，通过两根网线连接PC机和板子上的两个网口，PC机的每个口的IP需要与板子上对应口的IP在同一子网内，使用NetAssist工具连接，设置远程主机，默认P1口的IP为192.168.100.10，端口为5001，P2口的IP为192.168.200.10，端口为5002，如下图所示。
    ![double_net0_udp](doc/api/assets/net0_udp.png)
    ![double_net1_udp](doc/api/assets/net1_udp.png)

    2. 双网卡模式下，使能TCP_ECHO功能
    编译下载到硬件板子后，通过两根网线连接PC机和板子上的两个网口，PC机的每个口的IP需要与板子上对应口的IP在同一子网内，使用NetAssist工具连接，设置远程主机，默认P1口的IP为192.168.100.10，端口为5001，P2口的IP为192.168.200.10，端口为5002，如下图所示。
    ![double_net0_tcp](doc/api/assets/net0_tcp.png)
    ![double_net1_tcp](doc/api/assets/net1_tcp.png)

