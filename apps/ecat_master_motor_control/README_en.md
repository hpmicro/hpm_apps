# EtherCAT Master-Based Integrated Display and Control Solution for Motors

## Depends on SDK 1.11.0

## Overview

This solution is built on HPM series MCUs from HPMicro, integrating EtherCAT master technology and the LVGL graphics interface to provide a complete EtherCAT master motor control solution.

This solution integrates the following core capabilities:
- **EtherCAT Master**: Industrial-grade EtherCAT master based on open-source CherryECAT
- **CIA402 Motor Control**: Supports CSP (Cyclic Synchronous Position) and CSV (Cyclic Synchronous Velocity) motion control modes
- **LVGL UI**: Intuitive touch-screen control interface with real-time display of motor status and motion parameters
- **FreeRTOS**: Ensures real-time performance and stability for motor control
- **Shell CLI**: Supports command-line debugging and configuration

## Core Features

### EtherCAT Master Features
- **Asynchronous Queue Transfer**: One transfer can carry multiple datagrams
- **Zero-Copy Technology**: Directly uses Ethernet TX/RX buffers
- **Hot-Plug Support**: Automatically scans the bus and updates slave info when topology changes
- **Automatic State Monitoring**: Real-time monitoring of slave status

### Motor Control Features
- **CIA402 Standard Protocol**: Compliant with industrial standard motor control interfaces
- **Multiple Control Modes**:
  - CSP (Cyclic Synchronous Position)
  - CSV (Cyclic Synchronous Velocity)
- **Device Switching**: Supports switching control between multiple motor devices
- **Real-Time Feedback**: Real-time display of position, velocity, and status
- **Fault Diagnostics**: Connection state monitoring and error indication

### UI Features
- **Modern Design**: Smooth touch interface based on LVGL v9
- **Speed Control**: Arc slider to adjust target speed
- **Position Control**: Arc slider to adjust target position
- **Real-Time Waveform**: Chart component showing motion curves
- **Device Management**: Supports multi-device selection and status display
- **Button Controls**: Quick actions for start/stop and forward/reverse

## Hardware Requirements

### Controller Board Requirements
- **MCU**: HPM6800EVK
- **Display**: 1280x800 resolution touch screen
- **Ethernet**: Interface supporting EtherCAT communication
- **Debug Interface**: JTAG/SWD
- **Serial Port**: UART

### Slave Device Requirements
- EtherCAT servo drives supporting CIA402 protocol

## Software Architecture

### System Framework
```
┌─────────────────────────────────────────┐
│         LVGL UI Layer                   │
│  (Touch Control + Real-time Display)    │
└─────────────┬───────────────────────────┘
              │
┌─────────────┴───────────────────────────┐
│      Application Layer                  │
│  (CIA402 Control Logic + Data Handling) │
└─────────────┬───────────────────────────┘
              │
┌─────────────┴───────────────────────────┐
│      CherryECAT Master                  │
│  (PDO Comm + DC Sync + State Machine)   │
└─────────────┬───────────────────────────┘
              │
┌─────────────┴───────────────────────────┐
│      FreeRTOS + Ethernet Driver         │
│  (Task Scheduling + Network Driver)     │
└─────────────────────────────────────────┘
```

### Task Structure
- **LVGL Task**: UI refresh and touch event handling
- **EtherCAT Task**: Periodic PDO communication and status monitoring
- **Shell Task**: Command-line interaction and debugging

## Device Connections

### Hardware Connection Diagram
```
[PC Debug Tool] ──USB──> [HPM Board] ──JTAG──> [Debugger]
                         │
                         │ EtherCAT
                         ├──> [Servo Drive 1] ──> [Motor 1]
                         │
                         ├──> [Servo Drive 2] ──> [Motor 2]
                         │
                         └──> [Servo Drive N] ──> [Motor N]
```
![connect](doc/api/assets/connect.png)

### Connection Steps
1. Connect PC USB to board DEBUG Type-C interface
2. Connect debugger to JTAG interface
3. Connect EtherCAT slave devices (servo drives)
4. Connect motors to the drives
5. Connect display to the board (if using UI)
6. Power on the system

## Port Settings

- **Baud Rate**: 115200bps
- **Stop Bits**: 1
- **Parity**: None
- **Data Bits**: 8

## Project Creation
![generate_project](doc/api/assets/generate_project.png)

## Runtime

### System Startup

#### Serial Log for Successful Project Initialization (No Slaves Connected):

![start_log](doc/api/assets/log.png)

#### Serial Log for Successful Slave Connection:

![lvgl_log](doc/api/assets/ecat_log.png)

### UI Display

The touch screen will display the motor control interface, including the following elements:

**Top Status Bar**:
- HPMicro logo
- Start button
- Forward/Reverse buttons
- Control mode dropdown box

**Device Information Module**:

**Control Area**:
  - Draggable slider (0-100)
  - Slide right to control speed or position
  - Real-time value display
  
**Bottom In/Output Control Buttons**:
- INPUT mode selection button
- OUTPUT mode selection button
- 12 bit input/output buttons

![lvgl_log](doc/api/assets/ui.png)

## Features

### 1. Device Management
- Automatically scans all slaves on the EtherCAT bus
- Supports hot-plug detection
- Supports multi-device switching control
- Real-time display of device count and current selection

### 2. Control Exclusivity
- Speed control and position control are mutually exclusive (only one mode can be active at a time)
- Interface automatically switches to display corresponding slave device info

### 3. Visual Feedback

- Visual feedback when buttons are pressed
- Waveforms for target and actual values are displayed in different colors

### 4. Data Acquisition
- Waveform chart displays motor motion status in real-time
- Real-time display of slave status information


