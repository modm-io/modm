# Simple ZeroMQ example with XPCC

## Setup

 * STM32F4 Discovery Board
 * CAN transceiver breakout board with SN65HVD230 
 * CAN2USB

Connect the CAN2USB to your computer and to the CAN transceiver.

## STM32 XPCC publisher

Flash `1_stm32` on the STM32F4 Discovery Board

    cd 1_stm32
    scons program

The board will periodically send XPCC message over the CAN bus to the CAN2USB. The RX LED of CAN2USB should toggle every 1.25 seconds.

## ZeroMQ Gateway on PC

Start ZeroMQ Gateway on PC

    cd 2_zmq_gateway
    scons
    
This will connect to the CAN2USB and receives the CAN frames, reassembles them with XPCC CAN Connector and creates XPCC Header and XPCC Payload. This data is printed on the console. You may need to update the hardcoded USB device (`/dev/tty.usbserial-14112`).

     Debug:   Payload is:      0xDEADBEEF112233445566778802
     Debug:   Header is:       (t=0,a=false,d=05,s=07,i=0B)
     Debug:   Payload size is: 13

The XPCC Header and XPCC Payload is then manually serialised into a ZeroMQ message and published at port 8211 on localhost.

The application is polling the USB which needs a lot of CPU.

Now run as many ZeroMQ subscribers as you like

## Simple Python Subscriber

There is a simple Python subscriber

    cd 3_zmq_app
    python sub.py

The subscriber will just display the raw ZeroMQ message:

    Subscriber received: >>0005070bdeadbeef112233445566778811<<

## C++ XPCC subscriber

There is a C++ XPCC subscriber

	cd 3_zmq_app
	scons
	
It will parse the ZeroMQ message into XPCC Header and XPCC Payload.
	
    Debug:   0MQ size is 17
    Debug:   Header is:       (t=0,a=false,d=05,s=07,i=0B)
    Debug:   Payload size is: 13
    Debug:   Payload is:      0xDEADBEEF112233445566778819
    
## ToDo

* Add a back channel with `PUSH/PULL` ports in ZeroMQ
  * With `PUSH/PULL` the Gateway will open a TCP socket. That avoids potential problems with firewalls.
  * Any client then can send XPCC messages to the CAN bus. The XPCC messages will be fragmented into CAN frames on the Gatway.
* Use XPCC communication packets from XML definitions.
* Use the XPCC Postman and Dispatcher
* Add a sensor (rotary switch / odometry) to the STM32 to simulate odometry data.
