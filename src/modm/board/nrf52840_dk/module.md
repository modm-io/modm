# nRF52840-DK

Nordic Semiconductor nRF52840 Development Kit.

This BSP `Board::initialize()` sets the board up with these settings:

- 64MHz CPU clock frequency.
- SysTick configured for the selected system clock.
- MODM_LOGGER initialized at 115.2kBaud on UARTE0 (`P0.06` TX, `P0.08` RX).
- Four active-low LEDs on `P0.13..P0.16`.
- Four active-low buttons on `P0.11`, `P0.12`, `P0.24`, and `P0.25` with pull-ups.

[Product Link](https://www.nordicsemi.com/Products/Development-hardware/nrf52840-dk)
