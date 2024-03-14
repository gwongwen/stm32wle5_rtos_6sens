# Code for 6sens Omnitilt Project : final code

## Overview
Here is the final code for the 6sens Omnitilt project. It makes it possible to measure seismic measurements and alert in the event of an alert. The data is stored in memory and sent to the Lorawan/5G network. In addition, the battery level and temperature/pressure/humidity of the node are taken every 30 minutes and sent to the network once a day.

## Building and Running
The following commands clean build folder, build and flash the sample:

**Command to use**

west build -t pristine

west build -p always -b stm32wl_dw1000_iot_board applications/stm32wle5_rtos_6sens

west flash --runner stm32cubeprogrammer