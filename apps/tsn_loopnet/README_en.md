# HPM TSN Ring Network Communication Solution

## Depend on SDK1.10.0

## Overview

This example program utilizes the Time-Sensitive Networking (TSN) functionality of HPM6E series chips, implementing ring network communication and hardware forwarding through TSN's CB protocol and switch function, with forwarding latency reaching the microsecond level.

## HPM6E00EVK Ring Network Topology
![tsn_loop_net](doc/api/assets/tsn_loop_net.png)

Example Program Description:
1. The entire system consists of only one master device, and the rest are slave devices. Both the master and slave devices use HPM6E00EVK development boards, which are connected in a ring via network cables through the two IN and OUT network ports.
2. Each master or slave device has only one Media Access Control (MAC) address, and no MAC addresses can be duplicated.
3.Each master or slave device has only one Internet Protocol (IP) address, and no IP addresses can be duplicated. Additionally, all devices must be on the same network segment.
4. The master device establishes a User Datagram Protocol (UDP) server, bound to IP_ADDR_ANY (any address), with the port number configurable via macros. Slave devices establish UDP clients, whose port numbers can either be fixed (bound) or unfixed.
5. The TSN functionality of HPM6E series chips includes three external ports and one internal port. The IN and OUT network ports on the HPM6E00EVK development board correspond to two external ports (P1 and P2) of TSN respectively. This example program uses one internal port and two external ports.
    Master device configuration: Broadcast frames can be forwarded from the internal port, unknown frames can be forwarded from ports P1 and P2, and internal frames are not allowed to be forwarded. A lookup table is also configured to ensure that frames with the local MAC address as the destination can be forwarded from the internal port.
    Slave device configuration: Broadcast frames can be forwarded from ports P1 and P2, unknown frames can be forwarded from ports P1 and P2, and internal frames are not allowed to be forwarded. A lookup table is also configured to ensure that frames with the local MAC address as the destination can be forwarded from the internal port.
    These two configurations ensure that the Address Resolution Protocol (ARP) operates unidirectionally, preventing broadcast storms and reducing the load on the internal port (i.e., reducing CPU pressure).
6. When the master or slave device sends data, the CB function adds an r-tag to the data frame and transmits it simultaneously from the two external ports. When receiving data, if duplicate frames are detected, the switch function discards them. This mechanism ensures that the internal port only receives one valid data frame and no duplicate frames.
7. Communication logic: After the device is powered on, the UDP client first sends one data frame. Upon receiving the data, the master device records the client's IP address and port number and notifies the client to stop sending. By default, the master device can store information of up to 16 slave devices, meaning a maximum of 16 slave devices can be connected in series.
8. After a slave device receives 1000 data frames, the RGB light changes its state once. The data reception rate of the slave device can be verified by observing the state of the RGB light.
9. The total latency from the master sending data to Slave 3 receiving it is 21 microseconds, including 3 forwarding delays, 3 transmission line delays, and the MCU's processing delay, etc.

## Example Program Details

### Environment

#### SDK Version

V1.10.0

#### BOARD

HPM6E00EVK_RevB

### Software Configuration

#### A. Software Description

tsn_loopnet_master: Master device program
tsn_loopnet_slave: Slave device program

#### B. Modification of MAC and IP Addresses for Master/Slave Devices

Modify the IP address and the 5th byte of the MAC address in netconf.h.

Default IP address and 5th byte of MAC address for the master device:
```
#ifndef IP_CONFIG
#define IP_CONFIG 192.168.100.10
#endif
#define MAC5 0x40
```
Default IP addresses and 5th bytes of MAC addresses for slave devices:
```
#ifndef IP_CONFIG
#define IP_CONFIG 192.168.100.11
#endif
#define MAC5 0x41

#ifndef IP_CONFIG
#define IP_CONFIG 192.168.100.12
#endif
#define MAC5 0x42
```
#### C. Modification of Ports for Master/Slave Devices

Modify the master device port in udp_echo.h.

Default master device port:
```
#ifndef UDP_LOCAL_PORT
#define UDP_LOCAL_PORT (5000U)
#endif
```
Modify the slave device ports in udp_client.h.

Default slave device ports:
```
#ifndef UDP_LOCAL_PORT
#define UDP_LOCAL_PORT (5001U)
#endif
#ifndef UDP_LOCAL_PORT
#define UDP_LOCAL_PORT (5002U)
#endif
```

Notes:Ensure that the IP addresses and MAC addresses of the master device and all slave devices are unique and not duplicated.
The master device and all slave devices must be on the same network segment; otherwise, communication will fail.

## Testing

1. Configure the devices as described above, then power on both the master and slave devices. The master device will automatically detect all slave devices and record their IP addresses and port numbers. It will then send one data frame to each slave device every 1ms by default. Each slave device will change the RGB light state once after receiving 1000 data frames.

2. Disconnection test: Communication remains normal even if one network cable is disconnected.

Note:This example program only detects the PHY connection status of port P2. During the disconnection test, do not disconnect the network cable of port P2, otherwise the slave device will go offline directly.

## API

:::{eval-rst}

About software API:  `API doc <../../_static/apps/tsn_loopnet/html/index.html>`_ 。
:::