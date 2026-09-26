# Raw STM32H723 Ethernet example

This example exercises the MAC and DMA through internal MAC loopback without a
network stack. The GPIO list is a valid H723 RMII reference mapping, not a
promise that those signals are wired on every Nucleo, WeAct, or custom board.

The STM32 Ethernet reset and RMII logic may still require a running 50 MHz
reference clock even in internal loopback. Adapt the pins and clock source to
the board before running it. No PHY negotiation is performed.
