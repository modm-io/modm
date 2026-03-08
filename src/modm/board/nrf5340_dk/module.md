# nRF5340-DK

Nordic Semiconductor nRF5340 Development Kit.

This BSP supports both cores and `Board::initialize()` configures the selected
core with these settings:

- Application core target: `nrf5340-xxaa@app`
  - 128MHz CPU clock frequency.
  - MODM_LOGGER initialized at 115.2kBaud on UARTE1 (`P1.01` TX, `P1.00` RX).
  - Owns LED1/LED2 and Button1/Button2.
  - Transfers selected GPIO ownership to the network core.

- Network core target: `nrf5340-xxaa@net`
  - 64MHz CPU clock frequency.
  - MODM_LOGGER initialized at 115.2kBaud on UARTE0 (`P0.20` TX, `P0.22` RX).
  - Owns LED3/LED4 and Button3/Button4.

The default board core in [board.xml](src/modm/board/nrf5340_dk/board.xml)
is the application core (`nrf5340-xxaa@app`). To use this board with the network
core you must inherit and then overwrite the the `modm:target` with
`nrf5340-xxaa@net`.

[Product Link](https://www.nordicsemi.com/Products/Development-hardware/nRF5340-DK)
