# HPM5E00EVK

## Overview

The HPM5E00 is a single-core MCU with high computing power and abundant peripheral resources, 512KB of RAM on the chip and 1MB flash.

The HPM5E00EVK is an evaluation board for the HPM6E00 series MCU, providing a range of interfaces including ADC input with SMA interface, SDM input with SMA interface, motor control interface (QEO/QEI/SEI/PWM/ADC), CAN interface, Ethernet interface, EtherCAT interfaces, USB interface, audio interface, PPI/FREMC interface, and Raspberry Pi interface. It also integrates an onboard debugger FT2232 for easy debugging by users.

 ![hpm500evk](doc/hpm5e00evk.png "hpm5e00evk")

## 板上硬件资源

- HPM5E31 MCU (1MB Flash)
- Ethernet
  - 1000 Mbits PHY
- EtherCAT
  - 2 port
- USB
  - USB type C (USB 2.0 OTG) connector x1
- Motor
- AD sensor
  - NSI1306W25
- Others
  - RGB LED
  - CAN
- Attention
  - **When using PPI peripherals, please insert the corresponding expansion board into the PPI interface**
    - The standard expansion board for HPM5E00EVK evaluation purposes is as follows, with a 16 bits SRAM
      ![hpm5e00evk_ext](doc/hpm5e00evk_ext.png "hpm5e00evk_ext")
    - The PPI interface has high flexibility. If you need to evaluate other parallel port devices, such as FPGA, ASYNC SRAM, etc., you can design your own expansion board or contact us.
  - **When using SDM peripheral and AD sensor(NSI1306W25) on board, please connect JP4、JP5、JP6 and disconnect J3**

## DIP Switch SW2

- Bit 1 and 2 controls boot mode

| Bit[2:1] | Description                  |
| -------- | ---------------------------- |
| OFF, OFF | Boot from Quad SPI NOR flash |
| ON, OFF  | ISP                          |

(lab_hpm6e00_evk_board)=

## Button

(**lab_hpm6e00_evk_board_buttons**)=

| Name         | FUNCTIONS                                      |
| ------------ | ---------------------------------------------- |
| PB24 (KEYA) | GPIO ButtonA                             |
| PB25 (KEYB) | GPIO ButtonB                             |
| WBUTN (WKUP) | WAKE UP Button                          |
| RESETN (RESET) | Reset Button                            |

## Plug-in

- Debug interface select

| Function  | Position   | Note |
| --------- | ------ |------|
| Debug interface select  | J17 | All connected:use the on-board ft2232, All disconnected: use JTAG interface |

- PPI/FEMC interface

| Function  | Position   | Note |
| --------- | ------ |------|
| PPI interface | CN1 | Connecting to PPI expansion board |

## Pin Description

- UART0 Pin:

The UART0 use for core0 debugger console:

| Function | Pin  | Position |
| -------- | ---- |  --------- |
| UART0.TX | PA00 | DEBUGUART0 |
| UART0.RX | PA01 | DEBUGUART0 |

- UART4 Pin:

 The UART4 is used for some functional testing using UART, such as MICROROS_UART, USB_CDC_ACM_UART, MODBUS_RTU, lin etc.

| Function | Pin  | Position | Remark   |
| -------- | ---- |  --------- | ------ |
| UART4.TX | PC16 | P5[8]  |
| UART4.RX | PC17 | P5[10] |
| UART4.break | PD13 | P5[18] | generate uart break signal |

- CAN Pin

| Function      | Postion   |
| --------- | ------ |
| CAN_H  | J7[0] |
| CAN_L  | J7[2] |

- ADC Pin

| Function  | Pin   | Postion   |
| --------- | ------ | ------ |
| ADC16 Input | PF26 | J3[6] |
| SDM ADC Input  |  | J13 |

- ACMP

| Function  | Pin   | Postion   |
| --------- | -------- | -------- |
| CMP0.INN6 | PF26 | J3[6]  |

- Quadrature Encoder Pin. Note: QEO ABZ pins should be connected as documented here

| Function      | Postion   |
| --------- | ------ |
| QEI.A / HALL.U  | J3[1] |
| QEI.B / HALL.V  | J3[3] |
| QEI.Z / HALL.W  | J3[5] |
| QEO.A  | J3[24] |
| QEO.B  | J3[22] |
| QEO.Z  | J3[26] |

- PWM Output Pin

| Function      | Postion   |
| --------- | ------ |
| PWM.WL  | J3[12] |
| PWM.WH  | J3[11] |
| PWM.VL / PWM1.P3 | J3[10] |
| PWM.VH / PWM1.P2 | J3[9] |
| PWM.UL / PWM1.P1 | J3[8] |
| PWM.UH / PWM1.P0 | J3[7] |

- QEIV2 Sin/Cos Pin

| Function      | Postion   | Note |
| ---------- | ------ | ------  |
| ADC2.INA11  | J3[15] | ADC_IW (Cos) |
| ADC0.INA14  | J3[13] | ADC_IU (Sin) |

- PLB Pulse Output Pin

| Function      | Postion   |
| ---- | ------ |
| PLB.PULSE_OUT  | J3[9] |

- PLB LIN Clock Pin

This pin is used for UART LIN Slave baudrate adaptive demo to detect the clock of RX signal
| Function | Postion   |
| ---- | ------ |
| PLB.TRGM_IN  | J3[5] |


- Tamper Pin

| Function | Pin    | Position |  Mode  |
|----------|--------|--------|---------|
| TAMP.04  | PZ04   | P5[18] | Active Mode |
| TAMP.05  | PZ05   | P5[33] | Active Mode |
| TAMP.03  | PZ03   | P5[7]  | Passive Mode |

- LOBS Trig Pin

|  Pin   | Position   |
|--------|--------|
| PF26   | P5[23] |

- GPTMR Pin:

| Function      | Position | Remark   |
| ------------- | ----- | ------ |
| GPTMR0.CAPT_2 | P5[7] |
| GPTMR0.COMP_2 | P5[26] | BLCK of i2s emulation |
| GPTMR0.COMP_3 | P5[29] | LRCK of i2s emulation |
| GPTMR1.COMP_3 | P5[11] | MCLK of i2s emulation |

- CS Pin of i2s emulation

| Function | Position    |  Remark |
| ---- | ----- | ------|
| PD24  | P5[38] | the pin that controls the SPI slave CS |

- SPI Pin：

| Function  | Pin  | Position |
| --------- | ---- | ------- |
| SPI1.CSN  | PC11 | P5[24]  |
| SPI1.SCLK | PC10 | P5[23]  |
| SPI1.MISO | PC12 | P5[21]  |
| SPI1.MOSI | PC13 | P5[19]  |

- I2C Pin：

| Function | Position |
| -------- | ------ |
| I2C0.SCL | P5[5] |
| I2C0.SDA | P5[3] |

- ECAT:

| Function        | Pin | Position   |
| ----------- | ---- | ------ |
| Port0 | - | P1 |
| Port1 | - | P2 |
| Port2 | - | via Extension board |
| Input IO(2bit) | PD06/PD12 | SW6 |
| Output IO(2bit) | PC23/PC24 | LED6/LED7 |
