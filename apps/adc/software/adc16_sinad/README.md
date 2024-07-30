# ADC batch sampling demo
## Overview
***
In this example, ADC16 batch samples several data from the specified channel and outputs them to the serial port terminal.
The default sampling rate is 2MSPS.

The sampled data can be imported into scientific computing software for sinad and thd analysis.

## Device Connection
***
- Connect PC USB to DEBUG Type-C interface
- Connect the debugger to the JTAG interface
- Connect the signal source to the signal input SMA interface

## Port Settings
***
- Serial port baud rate is set to ``115200bps``, with ``one stop bit`` and ``no parity bit``

## Running the example
***
When the project runs correctly, the serial port terminal will output the following information:
```
==============================
 hpm6750_adc_evk clock summary
==============================
cpu0:		 648000000Hz
cpu1:		 648000000Hz
axi0:		 200000000Hz
axi1:		 200000000Hz
axi2:		 200000000Hz
ahb:		 200000000Hz
mchtmr0:	 24000000Hz
mchtmr1:	 24000000Hz
xpi0:		 133333333Hz
xpi1:		 400000000Hz
femc:		 166666666Hz
display:	 74250000Hz
cam0:		 59400000Hz
cam1:		 59400000Hz
jpeg:		 200000000Hz
pdma:		 200000000Hz
==============================
hpm_sdk: 1.2.0

----------------------------------------------------------------------
$$\   $$\ $$$$$$$\  $$\      $$\ $$\
$$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
$$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
$$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
$$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
$$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
$$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
\__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
----------------------------------------------------------------------
This is an ADC16 demo for sinad test:
adc data buff is full, buffer start addr:1080014. end addr:10e4010
Ch: 0 val: 62161 0%
 Ch: 0 val: 62087 0%
 Ch: 0 val: 62116 0%
 Ch: 0 val: 62086 0%
 Ch: 0 val: 61961 0%
 Ch: 0 val: 62077 0%
 Ch: 0 val: 61972 0%
 Ch: 0 val: 61927 0%
 Ch: 0 val: 61886 0%
 Ch: 0 val: 61848 0%
 Ch: 0 val: 61804 0%
 Ch: 0 val: 61755 0%
 Ch: 0 val: 61709 0%
 Ch: 0 val: 61663 0%
 Ch: 0 val: 61612 0%
 Ch: 0 val: 61564 0%
 Ch: 0 val: 61517 0%
 Ch: 0 val: 61457 0%
 Ch: 0 val: 61403 0%
 ```

