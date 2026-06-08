# HPM5E系列：用于EtherCAT-CAN协议转换的CAN终端设备

## 依赖SDK1.10.0

## 概述

本例程使用HPM5E系列芯片的can功能，主要作为gateway_ecat_master例程的can设备，本例程使用hpm5e00evk板子。

特点：
1. 接收ecat主站发送经ecat网关转换后的can报文
2. 在数码管显示接收can报文数量
3. 通过按键将can报文的数量以can报文的形式发送到ecat网关，ecat网关再将该can报文转换为ecat报文发送到主站
4. 支持USB SH 命令行，用户可通过可自行添加USB SH命令功能

## 例程说明 

### 环境

#### SDK版本

V1.10.0

#### BOARD

HPM5E00EVK

详细信息请参考SDK hpm5e00evk board

### 软件配置

#### A. 生成工程
- 通过HPM SDK Project Generator生成segger工程
![generate_project](doc/api/assets/generate_project.png)

### Ecat主站使用说明

#### 设备介绍

![device](doc/api/assets/device.png)
本例程主要作为gateway_ecat_master例程的can设备。上电后，数码管显示00000，当gateway_ecat_master发送一帧数据，经过gateway_ecat2can转换为can报文后发送给本设备，数码管增加1。此时按下图中箭头所指按键，即可将当前数码管显示的数值以can报文的形式发送到ecat网关，ecat网关再将该can报文转换为ecat报文发送到主站，gateway_ecat_master设备会在相应的位置显示与数码管一致的数据。
具体使用参考gateway_ecat_master例程


