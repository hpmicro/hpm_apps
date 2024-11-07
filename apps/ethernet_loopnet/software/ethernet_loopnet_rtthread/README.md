# 网络连通性测试

## 概述

本示例演示了基于RT-Thread Enet的网络连通性测试

## 硬件设置

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
* 使用以太网线缆连接PC以太网端口和开发板RGMII或RMII端口

## 运行示例

* 编译下载程序
* 串口终端显示

```console

 \ | /
- RT -     Thread Operating System
 / | \     5.0.1 build Jul 25 2023 16:20:40
 2006 - 2022 Copyright by RT-Thread team
lwIP-2.1.2 initialized!
[4] I/sal.skt: Socket Abstraction Layer initialize success.
msh />[1011] I/NO_TAG: PHY Status: Link up
[1015] I/NO_TAG: PHY Speed: 100Mbps
[1019] I/NO_TAG: PHY Duplex: full duplex
```

## 功能验证

### 1. IP分配查询

```console
msh />ifconfig
network interface device: ET (Default)
MTU: 1500
MAC: 98 2c bc b1 9f 17
FLAGS: UP LINK_UP INTERNET_DOWN DHCP_ENABLE ETHARP BROADCAST
ip address: 192.168.100.6
gw address: 192.168.100.1
net mask  : 255.255.255.0
dns server #0: 192.168.100.1
dns server #1: 0.0.0.0

```

### 2. PING测试

  （1）Windows系统中，打开cmd, 运行ping

```console
C:\Users>ping 192.168.100.6

正在 Ping 192.168.100.6 具有 32 字节的数据:
来自 192.168.100.6 的回复: 字节=32 时间<1ms TTL=255
来自 192.168.100.6 的回复: 字节=32 时间<1ms TTL=255
来自 192.168.100.6 的回复: 字节=32 时间<1ms TTL=255
来自 192.168.100.6 的回复: 字节=32 时间<1ms TTL=255

192.168.100.6 的 Ping 统计信息:
    数据包: 已发送 = 4，已接收 = 4，丢失 = 0 (0% 丢失)，
往返行程的估计时间(以毫秒为单位):
    最短 = 0ms，最长 = 0ms，平均 = 0ms
```

  （2）开发板Ping PC

```console
msh />ping 192.168.100.8
ping: not found specified netif, using default netdev ET.
60 bytes from 192.168.100.8 icmp_seq=0 ttl=64 time=0 ms
60 bytes from 192.168.100.8 icmp_seq=1 ttl=64 time=0 ms
60 bytes from 192.168.100.8 icmp_seq=2 ttl=64 time=0 ms
60 bytes from 192.168.100.8 icmp_seq=3 ttl=64 time=0 ms

```
