# nRF51422-DK

Nordic Semiconductor nRF51422 Development Kit.

This BSP `Board::initialize()` sets the board up with these settings:

- 16MHz CPU clock frequency.
- SysTick configured for the selected system clock.
- MODM_LOGGER initialized at 115.2kBaud on UART0 (`P0.09` TX, `P0.11` RX).
- Four active-low LEDs on `P0.21..P0.24`.
- Four active-low buttons on `P0.17..P0.20` with pull-ups.

[Product Link](https://www.nordicsemi.com/Products/Development-hardware/nRF51422-DK)
