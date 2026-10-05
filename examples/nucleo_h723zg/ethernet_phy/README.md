# LAN8720A/LAN8742A polling example

This example demonstrates the shared non-blocking PHY state machine. Change the
`Phy` alias in `main.cpp` to select LAN8720A or LAN8742A and set its strapped
Clause-22 address. The selected address is not assumed by the MAC.

Before running it, adapt the RMII pins, provide a stable 50 MHz reference clock,
and implement any board-specific hardware-reset GPIO sequence. A PHY interrupt
is optional because this example polls link state.
