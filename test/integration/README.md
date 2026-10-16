# Integration Tests

These projects test or benchmark modm on real hardware or on your computer.
They are compiled by the CI like the examples, but they are not meant as a
starting point for your own code: have a look at the `examples` folder for that.

```sh
# compile all tests for all of their boards and run the hosted ones
python3 tools/scripts/examples_compile.py test/integration
```

The `project.xml` files use the same syntax as the examples to list the boards
a test runs on.


## Test Result

A test includes `integration_test.hpp` and ends with `return finishTest(passed);`,
which prints exactly one of these lines on the logger of the board:

```
INTEGRATION TEST PASSED
INTEGRATION TEST FAILED
```

A test runner only needs to program the board and wait for this line. On hosted
targets the result is also the exit code of the program.

Tests that expect an assertion to fail report their result from an assertion
handler instead: see `etl`, `fiber_overflow` and `assert`.


## Required Hardware

Some tests need more than the board:

| Test               | Requires                                                    |
|:-------------------|:------------------------------------------------------------|
| `i2c_transaction`  | I2C device with address 0x3C on D14/D15, e.g. a SSD1306.     |
| `nrf24_phy`        | Two nRF24L01+ modules, see `radio.hpp`.                      |
| `spi_dma`          | A wire from Mosi (PB5) to Miso (PB4).                        |
| `uart_spi`         | A wire from Tx to Rx, see `main.cpp`.                        |
| `spi_flash`        | SST26VF064B flash chip on the NUCLEO-F429ZI, see `main.cpp`. |
| `spi_stack_flash`  | W25M512VJ flash chip on the NUCLEO-F429ZI, see `main.cpp`.   |
