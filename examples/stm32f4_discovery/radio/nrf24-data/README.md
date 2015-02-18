# nrf24 data layer example

 This example showcases a simple usage of the data layer implementation for
the nRF24L01+ radio modules.

You need 2 STM32F4 Discovery boards wired up like it's described at the top of
each main.cpp inside 'rx' and 'tx' folder.


## Expected output of tx:

[...]
Send packet
Send packet
Send packet
[...]


## Expected output of rx:

[...]
Received packet from 0x11
Data: 00 00 02 34
Received packet from 0x11
Data: 00 00 02 35
Rx is still alive
Received packet from 0x11
Data: 00 00 02 36
[...]