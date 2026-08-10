# STM32H7 Ethernet Tests

These tests exercise the MAC/DMA, PHY, frame APIs, descriptor rings, checksum
handling, malformed traffic, and recovery. Compile checks cover the MII and RMII
APIs. Hardware tests run on a NUCLEO-H753ZI using RMII.

Hardware tests use LAN8742A near end loopback. This exercises the PHY and RMII
data path rather than the MAC internal loopback.

Disconnect the RJ45 cable before running the hardware suite. Teardown disables
loopback and restores auto negotiation.

Traffic tests cover 10/full and 100/full. Half duplex modes only test
configuration because LAN8742A RMII loopback is not reliable in those modes.
Active link transitions require an external link partner.

From the repository root:

```sh
cd test
make compile-nucleo-h753zi-eth
make run-nucleo-h753zi-eth
```

The run target programs the board and reports results through its default serial
connection.
