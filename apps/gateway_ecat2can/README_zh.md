# HPM Ethercat转Can之网关

## 依赖SDK1.10.0

## 概述

本例程使用HPM6E系列芯片的Ethercat通信及Can通信功能，并基于ETG的5001.5000 ethercat转can协议，实现Ethercat通信转can通信功能。

特点：
1. 基于ETG的5001.5000 ethercat转can协议，协议内容参考《ETG5001_3_V0i1i2_S_D_MDP_Gateways》的5000协议
2. 支持设置Can参数
3. 支持触发和循环发送两种模式
4. 支持整帧或以字节方式发送或接收报文
5. 支持远程帧 标准帧 远程帧
6. 支持USB SH 命令行
7. 支持module slot方式，动态修改PDO映射

## 例程说明 

### 环境

#### SDK版本

V1.10.0

#### BOARD

HPM6E00EVK

### 软件配置

#### A. SSC代码生成
- 新建SSC项目，选择Gateway2
![SSC_new](doc/api/assets/SSC_new.png)
- 如果没有Gateway2配置，依次点击Tool->Options->Configurations,并导入software/apps/ecat/gateway_apps_config.xml
![SSC_config](doc/api/assets/SSC_config.png)
- 依次点击Tool->Application->Import,导入software/apps/ecat/gateway_apps.xlsx
![SSC_import](doc/api/assets/SSC_import.png)
- 生成SSC代码，依次点击Project->Create new Slave Files,生成代码，同时生成gateway_apps.xml文件。Source Folder：hpm_apps\apps\gateway_ecat2can\software\apps\ecat\Src\
![SSC_create](doc/api/assets/SSC_create.png)

注：可以直接使用gateway_apps.esp,该文件已经配置好相关参数，可以直接生成代码。

#### B. 生成工程
- 通过HPM SDK Project Generator生成segger工程
![SSC_project](doc/api/assets/Project_create.png)

### TwinCAT做主机使用说明
- TwinCAT在OP状态
![TwinCat_OP](doc/api/assets/TwinCat_OP.png)
按照5001.5000协议，RxMessage_1为接收标准can帧，TxMessage_1为发送标准can帧，RxMessage1_1为接收扩展can帧，TxMessage1_1为发送扩展can帧。
- 发送标准CAN帧,需要写入TxMessage_1，如发送 1 2 3 4 5 6 7 8 标准CAN帧 id=0
![TwinCat_Send_Can](doc/api/assets/TwinCat_Send_Can.png)
- 接收标准CAN帧,需要读取RxMessage_1
![TwinCat_Recv_Can](doc/api/assets/TwinCat_Recv_Can.png)
- 通过Trigger方式发送
当写入TxMessage_1后，通过主站写0x8000索引的32子索引的bit4为1，代表TriggerMode，bit2每写一次1，即可触发发送。
![Trigger_Mode](doc/api/assets/Trigger_Mode.png)
- 通过Cycle方式发送
当写入TxMessage_1后，通过主站写0x8000索引的32子索引的bit4为0，代表CycleMode，根据37子索引的值确定循环发送时间，即可循环发送。
![Cycle_Mode](doc/api/assets/Cycle_Mode.png)
- 设置Can参数
通过主站写0xF800索引的2子索引设置Can波特率
![Can_Param](doc/api/assets/Can_Param.png)
注：设置COE需要先将从设备切换到PREOP状态。
值与波特率对应关系
```
            switch (ParameterSet0xF800.Baudrate)
            {
                case 0: //1M
                mq_msg.param = 1000000;
                break;
                case 1: //800k
                mq_msg.param = 800000;
                break;
                case 2: //500k
                mq_msg.param = 500000;
                break;
                case 3: //250k
                mq_msg.param = 250000;
                break;
                case 4: //125k
                mq_msg.param = 125000;
                break;
                case 5: //100k
                mq_msg.param = 100000;
                break;
                case 6: //50k
                mq_msg.param = 50000;
                break;
                case 7: //20k
                mq_msg.param = 20000;
                break;
                case 8: //10k
                mq_msg.param = 10000;
                break;
                case 255: //defined in bustiming register
                mq_msg.param = 255;
                break;
                default:
                mq_msg.param = 1000000;
                break;
            }
```

