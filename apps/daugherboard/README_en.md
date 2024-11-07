# HPM Daughter Board Series

## Overview

HPM series MCU is a high-performance real-time RISC-V microcontroller from Shanghai HPM Semiconductor Technology Co., Ltd., which provides great computing power and efficient control capability for industrial automation and edge computing applications. Shanghai HPM  Semiconductor has currently released multiple series of high-performance microcontroller products, such as , HPM5300, HPM6700/6400, HPM6300, HPM6200,, HPM6E00 etc.    

In order to help users develop the performance of Xianji MCU more comprehensively, HPM has launched a series of daughter boards that are used in conjunction with HPMEVK development boards. Some daughter boards are not sold and only provide hardware design materials including hardware design drawings, user guides, and software testing routines. 


## HPM6E00EVKSRAMSDRAM Daughter Board

This example project demonstrates how to use the HPM6E00EVK_SRAM daughter board with the HPM6E00EVK to read and write to an external asynchronous SRAM memory device using the PPI peripheral.


### Hardware Support

Please read hardware schematic [HPM6E00EVKSRAMSDRAMRevA](doc/HPM6E00EVKSRAMSDRAMRevA.pdf)   
Please See the hardware engineering path：(hpm_apps\apps\daugherboard\hardware\HPM6E00\HPM6E00EVKSRAMDRAMRev-Project Files) 



#### Device Connection

- Connect PC USB to DEBUG Type-C interface
- Connect the debugger to the JTAG interface
- Insert this daughter board source into the corresponding interface of the HPM6E00EVK.
 
- The connection diagram is as follows: 
![Test connection diagram](doc/api/assets/test_connection_diagram.png) 
The mainboard model is HPM6E00EVK,For details about how to use the mainboard, see:[HPM6E00EVK_User Guide](doc/HPM6E00EVK_UG_V1.0.pdf)


#### Port Settings

- Serial port baud rate is set to ``115200bps``, with ``one stop bit`` and ``no parity bit``

#### Create Project

- windows GUI create project  
![window gui create project](doc/api/assets/create_project.png)

- windows cmd create project  
Please read [HPM6E00EVK_User Guide](doc/HPM6E00EVK_UG_V1.0.pdf) as reference.

###  Test samples

####  async_sram(PPI ASYNC SRAM samples)

For more detailed information on using this routine, please refer to the following path:(hpm_sdk\samples\drivers\ppi\async_sram)

#### Running the example

When the project runs correctly, the serial port terminal will output the following information:
```
hpm6e00evk clock summary
==============================
cpu0:		 600000000Hz
cpu1:		 600000000Hz
ahb:		 200000000Hz
axif:		 200000000Hz
axis:		 200000000Hz
axic:		 200000000Hz
axin:		 100000000Hz
xpi0:		 133333333Hz
femc:		 166666666Hz
mchtmr0:	 24000000Hz
mchtmr1:	 24000000Hz
==========
[11:33:17.359]收←◆====================

----------------------------------------------------------------------
$$\   $$\ $$$$$$$\  $$\      $$\ $$\
$$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
$$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
$$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
$$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$
[11:33:17.392]收←◆ /  $$ |
$$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
$$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
\__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
----------------------------------------------------------------------
ppi sram example
comparison test: from 0xf8000000 to 0xf8020000

[11:33:17.438]收←◆**  u32 comparison succeeded
**  u16 comparison succeeded

[11:33:17.502]收←◆**  u8 comparison succeeded

```  
## API

:::{eval-rst}

About software API: `API doc <../../_static/apps/daugherboard/html/index.html>`_ 。
:::