# Explore the Examples

To get a quick feel about modm's APIs you can look at and experiment
[with our examples][examples], especially if you have a development board that
modm supports out-of-box.

Make sure you've [installed all tools required for building modm][installation]
and you've cloned the modm repository *recursively*:

```sh
git clone --recurse-submodules --jobs 8 https://github.com/modm-io/modm.git
```


## TL;DR

Change directory into any of the examples and compile it:

```sh
# cd into any example you like
cd modm/examples/gpio/blinky
# generate modm library (call only once)
lbuild build
# compile the example
scons -j8
# Connect your development board and upload the firmware
scons program
```

To debug with GDB in TUI mode. Make sure to change to the debug profile:

```sh
# compile and upload debug profile
scons program profile=debug
# launch OpenOCD and GDB for debugging
scons debug profile=debug
```

To generate your target specific Doxygen documentation:

```sh
(cd modm/docs && doxygen doxyfile.cfg)
# open modm/docs/html/index.html
```

To remove it all:

```sh
# Remove build artifacts
scons -c
# Remove generated files
lbuild clean
```

Have a look at the [build system documentation][build_docs] and the
[online documentation][docs].


## Structure

The examples are sorted by what they show, not by the board they run on:

| Folder          | Examples for                                                   |
|:----------------|:---------------------------------------------------------------|
| `core`          | Fibers, assertions, fault handling, allocators, internal flash, watchdogs, debug channels. |
| `gpio`          | Blinking LEDs, buttons, ports, external interrupts.             |
| `analog`        | Internal ADCs, DACs and comparators.                            |
| `timer`         | Timers, PWM and encoders.                                       |
| `logging`       | Logging and printing via UART.                                  |
| `i2c`           | I2C peripherals and drivers for I2C devices.                    |
| `spi`           | SPI peripherals and drivers for SPI devices.                    |
| `can`           | CAN peripherals and drivers for CAN devices.                    |
| `driver`        | Drivers for devices on other or board-specific connections.     |
| `ui`            | Displays, graphics, touch and animations.                       |
| `communication` | Protocols and radios: AMNB, SAB, XPCC, Ethernet, nRF24.         |
| `ext`           | External libraries: FreeRTOS, TinyUSB, LVGL, ETL, nanopb and more.       |

An example that runs on several boards lists all of them in its `project.xml`.
The suffix of an example like `analog/dac_dma_g4` names the device family or
platform that this example is specific to.


## Choosing Your Board

Most examples run on several boards. Their `project.xml` lists all of them and
you choose yours by moving the comment:

```xml
<library>
  <extends>modm:nucleo-l476rg</extends>
  <!-- <extends>modm:blue-pill-f103</extends> -->
  <!-- <extends>modm:hosted</extends> -->
  <options>
    <!-- Required for modm:blue-pill-f103 -->
    <!-- <option name="modm:build:openocd.cfg">interface/stlink.cfg</option> -->
  </options>
  <modules>
    <!-- Required for modm:nucleo-l476rg -->
    <module>modm:platform:adc:1</module>
    <!-- Required for modm:blue-pill-f103 -->
    <!-- <module>modm:platform:adc:2</module> -->
    <module>modm:build:scons</module>
  </modules>
</library>
```

- Every `<extends>` is one board that this example supports.
- `modm:hosted` compiles the example for your computer.
- A comment that names boards applies to the lines directly below it: these
  options, collectors and modules are only for these boards. If you switch
  boards, also switch which of these lines are commented out.
- All other lines apply to every board.

Our CI reads the same comments to compile every example for all of its boards:

```sh
# compile one example for all of its boards
python3 tools/scripts/examples_compile.py examples/gpio/blinky
# compile all examples, but only for AVR targets
python3 tools/scripts/examples_compile.py examples --target '^at'
```

Projects that test or benchmark modm rather than show how to use it are located
in `test/integration`.


## Noteworthy Examples

We have hundreds of examples, here are some of our favorite ones:

<!--checkrepourls-->
- Getting started:
[Blinky for every board](https://github.com/modm-io/modm/tree/develop/examples/gpio/blinky),
[Logging](https://github.com/modm-io/modm/tree/develop/examples/logging/logger),
[Printf formatting](https://github.com/modm-io/modm/tree/develop/examples/logging/printf),
[Button & Serial on Arduino](https://github.com/modm-io/modm/tree/develop/examples/gpio/digital_read_serial).
- Core features:
[Fibers](https://github.com/modm-io/modm/tree/develop/examples/core/fiber),
[Fibers on multiple cores](https://github.com/modm-io/modm/tree/develop/examples/core/mcfiber),
[Assertions](https://github.com/modm-io/modm/tree/develop/examples/core/assert),
[Hard Fault with CrashCatcher and GDB](https://github.com/modm-io/modm/tree/develop/examples/core/hard_fault),
[Internal Flash Programming](https://github.com/modm-io/modm/tree/develop/examples/core/flash),
[Vector table in RAM](https://github.com/modm-io/modm/tree/develop/examples/core/vector_table_ram),
[Multi-heap with external 16MB memory](https://github.com/modm-io/modm/tree/develop/examples/core/tlsf-allocator),
[Logging via ITM](https://github.com/modm-io/modm/tree/develop/examples/core/itm),
[Logging via RTT](https://github.com/modm-io/modm/tree/develop/examples/core/rtt).
- Peripherals:
[ADC](https://github.com/modm-io/modm/tree/develop/examples/analog/adc_basic),
[DAC with DMA](https://github.com/modm-io/modm/tree/develop/examples/analog/dac_dma),
[CAN](https://github.com/modm-io/modm/tree/develop/examples/can/can),
[FDCAN](https://github.com/modm-io/modm/tree/develop/examples/can/fdcan),
[UART](https://github.com/modm-io/modm/tree/develop/examples/logging/uart).
- Displays and user interfaces:
[Drawing on display](https://github.com/modm-io/modm/tree/develop/examples/ui/display_f4),
[Touchscreen inputs](https://github.com/modm-io/modm/tree/develop/examples/ui/touchscreen_f4_stm32f469_discovery),
[Game of Life in Color with Multitouch](https://github.com/modm-io/modm/tree/develop/examples/ui/game_of_life),
[Timer & LED Animations](https://github.com/modm-io/modm/tree/develop/examples/ui/timer),
[SSD1306 OLED display](https://github.com/modm-io/modm/tree/develop/examples/ui/ssd1306),
[HD44780 over I2C-GPIO expander](https://github.com/modm-io/modm/tree/develop/examples/ui/hd44780_f4).
- Sensors:
[BMP085/BMP180 barometer](https://github.com/modm-io/modm/tree/develop/examples/i2c/barometer_bmp085_bmp180),
[BMP180/BME280 barometer](https://github.com/modm-io/modm/tree/develop/examples/i2c/environment),
[TMP102 temperature sensor](https://github.com/modm-io/modm/tree/develop/examples/i2c/tmp102),
[VL6180 time-of-flight distance sensor](https://github.com/modm-io/modm/tree/develop/examples/i2c/distance_vl6180),
[VL53L0 time-of-flight distance sensor](https://github.com/modm-io/modm/tree/develop/examples/i2c/distance_vl53l0),
[TCS3414 color sensor](https://github.com/modm-io/modm/tree/develop/examples/i2c/colour_tcs3414),
[ADNS9800 motion sensor](https://github.com/modm-io/modm/tree/develop/examples/spi/adns_9800).
- External libraries:
[TinyUSB CDC and MSC](https://github.com/modm-io/modm/tree/develop/examples/ext/usb),
[TinyUSB DFU](https://github.com/modm-io/modm/tree/develop/examples/ext/usb_dfu),
[FreeRTOS](https://github.com/modm-io/modm/tree/develop/examples/ext/freertos),
[ETL](https://github.com/modm-io/modm/tree/develop/examples/ext/etl).
<!--/checkrepourls-->


## Copy Carefully

When copying from our examples make sure to set the repository path correctly!
All example `modm/examples/**/project.xml` files are missing this path, since we
set it in the inherited base `modm/examples/lbuild.xml` configuration.
You must also add the `modm:docs` module manually if you want it.

The `modm/examples/lbuild.xml` file:

```xml
<library>
  <!-- This is the default lbuild configuration file for every
       example in this folder. It only defaults the common settings,
       so that this isn't duplicated in every single example.
       When you write your own application, you must set this
       path yourself! -->
  <repositories>
    <repository><path>../repo.lb</path></repository>
  </repositories>
  <modules>
    <module>modm:docs</module>
  </modules>
</library>
```

[examples]: https://github.com/modm-io/modm/tree/develop/examples
[installation]: https://modm.io/guide/installation
[make]: https://modm.io/reference/module/modm-build-make
[build_docs]: https://modm.io/reference/build-systems/
[docs]: https://modm.io/reference/documentation/
