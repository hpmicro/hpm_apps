# EtherCAT 主站

## 依赖SDK1.10.0

## 概述

本节主要基于开源 ethercat 主站 [CherryECAT](https://github.com/cherry-embedded/CherryECAT)，演示在先楫 MCU 上运行 ethercat 主站功能。

## 特性

- ~ 4K ram，~40K flash（24K + 16K shell cmd, including log）
- 异步队列式传输（一次传输可以携带多个 datagram）
- 零拷贝技术：直接使用 enet tx/rx buffer 填充和解析 ethercat 数据
- 支持热插拔
	- 自动扫描总线
	- 拓扑结构发生变化时自动更新 Slave 信息
- 支持自动监控 Slave 状态
- 支持分布式时钟
- 支持 CANopen over EtherCAT (COE)
- 支持 File over EtherCAT(FOE)
- 支持 Ethernet over EtherCAT(EOE)
- 支持 Slave SII 读写
- 支持 Slave 寄存器读写
- 支持多主站
- **最小 PDO cyclic time < 40 us (实际数值受主站硬件和从站硬件影响)**
- **DC 抖动 < 3us (实际数值受主站硬件和从站硬件影响)**
- 支持 ethercat 命令行交互，参考 IgH

下图展示 dc 抖动 < 3us （hpm6800evk + flash_xip）:
![ethercat](doc/api/assets/ethercat_dc.png)

## 运行现象

默认 demo 使用命令行来操作 ethercat 从站的功能。

![ethercat](doc/api/assets/ethercat.png)
![ethercat](doc/api/assets/ethercat1.png)
![ethercat](doc/api/assets/ethercat2.png)
![ethercat](doc/api/assets/ethercat3.png)
![ethercat](doc/api/assets/ethercat4.png)
![ethercat](doc/api/assets/ethercat5.png)
![ethercat](doc/api/assets/ethercat6.png)
![ethercat](doc/api/assets/ethercat7.png)

- 启动 pdo 1ms 周期性传输

```
ec_start 1000
```

- 搭配  ecat_io 例程控制 LED 开启和关闭

```
ethercat pdo_write -p 0 0 0x00
ethercat pdo_write -p 0 0 0x01
ethercat pdo_write -p 0 0 0x02
ethercat pdo_write -p 0 0 0x03
```

- 搭配  ecat_coe 例程控制电机，使用 CSV 模式

```
ethercat pdo_write -p 0 0 0x0f // motor on
ethercat pdo_write -p 0 2 0xFF000000 // change ObjTargetVelocity to 255, hexdata from low byte to high byte
ethercat pdo_read -p 0 // get ObjStatusWord and ObjPositionActualValue
ethercat pdo_write -p 0 0 0x00 // motor off
```

## API

参考 CherryECAT API ：https://cherryecat.readthedocs.io/zh-cn/latest/api.html
