# HPM Ethercat转Can之主站

## 依赖SDK1.10.0

## 概述

本例程使用HPM6E系列芯片的以太网功能，并基于cherryecat实现ethercat主站功能。本例程使用hpm6e00 full port板子，该硬件可以显示通信状态，并控制通信过程。

特点：
1. 基于cherryecat主站
2. 使用hpm6e00 full port硬件
3. 支持设置Can参数
4. 支持触发和循环发送两种模式
5. 支持整帧或以字节方式发送或接收报文并显示
6. 支持远程帧 标准帧 远程帧
7. 支持USB SH 命令行

## 例程说明 

### 环境

#### SDK版本

V1.10.0

#### BOARD

HPM6E00_FULL_PORT 

![HPM6E00_FULL_PORT](doc/api/assets/master.png)

详细信息请参考full_port例程。本例程主要使用P3口作为百兆以太网口，屏幕显示，按键控制，USB及UART log口

### 软件配置

#### A. 生成工程
- 通过HPM SDK Project Generator生成segger工程
![generate_project](doc/api/assets/generate_project.png)

### Ecat主站使用说明

#### 系统介绍

![system](doc/api/assets/system.png)
本系统分三个模块，主站，ecat转can网关，can设备。主站即为本例程所示，ecat转can网关参考gateway_ecat2can例程，can设备为hpm5e00evk实现的一个简单can收发设备，用户可使用任意can设备代替。
主站与网关通过网线连接，网关与can设备通过can线连接。

#### 主站显示介绍
- 初始主界面
![dis_init](doc/api/assets/dis_init.png)
主站上电后显示该界面，主要包括Slaves Num(从站数量), State(从站状态, 默认INIT), Trigger(模式，默认为Trigger方式), Tx Num(已发送帧数)，Rx Num(已接收帧数), 启停显示。
- 连接从站主界面
![dis_connect](doc/api/assets/dis_connect.png)
主站连接从站成功后，State显示为PREOP，Slaves Num显示为1。
- 设置界面
![dis_set](doc/api/assets/dis_set.png)
设置界面显示当前can参数，用户可在该界面设置can参数。主要包括Baud Rate(波特率 默认1M，可设置范围：10k, 20k, 50k, 100k, 125k, 250k, 500k, 800k, 1M)，Mode(模式 默认Trigger方式，可设置：Trigger或Cycle)，Cycle Time(循环时间， 默认1000ms), Frame Mode(帧模式， 默认标准帧，可设置：Standard或Extended)。
- 数据界面
![dis_data](doc/api/assets/dis_data.png)
数据界面显示当前发送和接收的数据。其中发送相关的内容可以通过按键设置，接收相关的内容只显示，不能设置。
发送相关：
tnu：发送帧索引
tlen：发送帧长度，范围：1-8
tid：发送数据id，范围：0-0x7FF（标准帧）或0-0x1FFFFFFF（扩展帧）
trtr：发送数据rtr位，0：数据帧，1：远程帧
tidf：发送数据id是否为扩展id，0：标准帧，1：扩展帧
TX行D0-D7为发送数据的字节。
接收相关：
rlen：接收帧长度，范围：1-8
rid：接收数据id，范围：0-0x7FF（标准帧）或0-0x1FFFFFFF（扩展帧）
rrtr：接收数据rtr位，0：数据帧，1：远程帧
ridf：接收数据id是否为扩展id，0：标准帧，1：扩展帧
RX行D0-D7为接收数据的字节。
- 启动界面
![dis_start](doc/api/assets/dis_start.png)

#### 主站按键介绍
![button](doc/api/assets/button.png)
1. 主界面操作
- 长按左键2s可以切换到设置界面，只有在PREOP状态才能切换到设置界面
- 长按右键2s可以切换到数据界面
- 短按右键可以启动或停止主站OP状态，即连接好从站后，通过此动作可以进入OP状态(启动界面)
2. 设置界面操作
- 长按下键2s可以切换到主界面
- 长按右键2s可以切换到数据界面
- 短按左右键选择要修改的项，选中的项会变红底色
- 短按上下键修改参数
主要参数包括：波特率 1000000 800000 500000 250000 125000 100000 50000 20000 10000
             模式 Trigger:每按一次上键主站发送一次数据，Cycle:每隔循环时间主站发送一次数据
             循环时间（ms）
             帧模式 Standard Extended
3. 数据界面操作
- 长按下键2s可以切换到主界面
- 长按左键2s可以切换到设置界面，只有在PREOP状态才能切换到设置界面
- 短按左右键选择要修改的项
- 短按上下键修改发送参数

#### 主站测试
1. trigger方式下，每按一次上键，主站发送一次数据，数据内容通过数据界面设置。每次发送完数据Tx Num加1，can设备的数码管加1.
ecat转can网关log，表示发送帧rtr为0，dlc为1， id为0
```
[D] std tx rtr dlc id:0, 1, 0
[I] Module100_ConfigInterfaceWrite0x8000 index:0x8000, subindex:0x20, dataSize:0x2, bCompleteAccess:0x0
[I] pData[0]:0x10
[I] pData[1]:0x0,
```
2. cycle方式下，每循环一次，主站发送一次数据，数据内容通过数据界面设置。每次发送完数据Tx Num加1，can设备的数码管加1.默认1000ms发送一帧，相应的Tx Num与数码管每隔1s加1.
ecat转can网关log，表示发送帧rtr为0，dlc为1， id为0
```
[D] std tx rtr dlc id:0, 1, 0
```
3. can设备上传数据
can设备发送数据ecat转can网关后，会在主站数据界面显示接收数据的id，rtr，dlc，数据内容。ecat转can网关log，表示接收帧rtr为0，dlc为8， id为0
```
[D] std rx rtr dlc id:0, 8, 0
```

