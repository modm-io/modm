<!--webignore-->
# modm: a barebone embedded library generator
<p align="center">
<a href="http://modm.io"><img src="https://raw.githubusercontent.com/modm-io/modm/develop/docs/src/images/logo.svg?sanitize=true" style="max-width:100%;"></a>
</p>
<!--/webignore-->

modm (pronounced like dial-up "modem") is a toolbox for
building custom C++17 libraries tailored to your embedded device.
modm generates startup code, HALs and their implementations, communication
protocols, drivers for external devices, BSPs, etc… in a modular, customizable
process that you can fine-tune to your needs.

<!--webignore-->
- [This project has a homepage](http://modm.io).<!--/webignore-->
- Check out our [install instructions][install] and our [getting started guide][guide].
- Feast your eyes on [lots of working examples][examples].
- Our CI checks every contribution for regressions.
- We care [about testing modm][testing].
- [API reference is available here][api-docs].

<!-- - [Check out our latest quarterly release][releases] with a [beautiful changelog][changelog]. -->

This project also has a [technical blog][blog] to document larger design concepts.

- You found a bug? [Open up an issue, we don't bite][issues].
- You want to contribute? [Read the contribution guidelines][contrib] and [open a pull request][prs].

<!-- - You want to port modm? [Read our porting guide][porting]. -->

modm is optimized for the harsh requirements of the [Eurobot competition][eurobot],
where our robots need to run reliably and completely autonomously for the game's
100 second duration. Our robots contain a lot of different microcontrollers, some
without a lot of resources, so modm needs to fulfill a diverse set of objectives,
like small code size with small memory consumption, predictable program flow,
extreme portability.

The library source code is licensed as MPLv2 with any external source code under
compatible licenses (BSD, Apache2, MIT). So feel free to fork this project and
adapt it to your needs. The only thing we ask of you is to contribute your changes
back so everyone can benefit.

Please clone modm recursively, you need all the submodules:

```sh
git clone --recurse-submodules https://github.com/modm-io/modm.git
```

## Features

- Efficient and fast object-oriented C++17 API.
- Support for hundreds of AVR and ARM Cortex-M microcontrollers from Atmel and ST.
- Build system agnostic: Choose SCons, CMake or use your own.
- Data-driven, target-specific HAL generation using the lbuild engine.
- No memory allocations in HAL with very low overall RAM consumption.
- Highly-configurable modules with sensible defaults and lots of documentation.
- Cross platform peripheral interfaces incl. bit banging:
    - GPIO and GPIO expanders.
    - ADC and Analog.
    - UART, I<sup>2</sup>C, SPI, CAN.
- Interfaces and drivers for many external I<sup>2</sup>C and SPI sensors and devices.
- Debug/logging system with IOStream and printf interface.
- Lightweight, stackless threads and resumable functions using cooperative multitasking.
- Functional (partial) libstdc++ implementation for AVRs.
- Integration of useful third-party software: FreeRTOS, ROSserial, CMSIS-DSP.
- Useful filter, interpolation and geometric algorithms.
- Lightweight unit testing system (suitable for AVRs).
- Hundreds of tests to ensure correct functionality.


## Targets

modm can generate code for <!--avrcount-->506<!--/avrcount--> AVR,
<!--samcount-->163<!--/samcount--> SAM and <!--stmcount-->1959<!--/stmcount-->
STM32 devices, however, there are different levels of support and testing.

<center>

| Device Family | Support | Device Family | Support | Device Family | Support |
|:--------------|:--------|:--------------|:--------|:--------------|:--------|
| AVR           | ★★★     | STM32F0       | ★★★★    | STM32F1       | ★★★★    |
| STM32F2       | ★★★★    | STM32F3       | ★★★★★   | STM32F4       | ★★★★★   |
| STM32F7       | ★★★★    | STM32L1       | ★★★★    | STM32L4       | ★★★★    |
| STM32L4+      | ★★★★    | STM32G0       | ★★★★    | STM32G4       | ★★★★    |
| SAMD21        | ★★      |               |         |               |         |

</center>

All of these targets are compiling and booting correctly
(★)
and have GPIO and UART working
(★★).
Most targets have support for basic peripherals, like I2C, SPI and ADC
(★★★)
as well as complicated peripherals, like Timers, CAN and external memory
(★★★★).
We also use a few targets in everyday development, which are very well tested
(★★★★★).

We are only a small team of developers and are limited in the amount of targets
we can support and test in hardware. We have prepared, but currently not finished
support for STM32F2, STM32L0 and STM32L1. [Open an issue][issues] to ask if your
specific target is supported out-of-the-box and what you can do if it's not.


### Boards

We have out-of-box support for [several development boards][devboards] including
documentation.

<center>
<!--bsptable-->
<table>
<tr>
<td align="center">AL-AVREB-CAN</td>
<td align="center">Arduino NANO</td>
<td align="center">Arduino UNO</td>
<td align="center">Black Pill</td>
</tr><tr>
<td align="center">Blue Pill</td>
<td align="center">DISCO-F051R8</td>
<td align="center">DISCO-F072RB</td>
<td align="center">DISCO-F100RB</td>
</tr><tr>
<td align="center">DISCO-F303VC</td>
<td align="center">DISCO-F407VG</td>
<td align="center">DISCO-F429ZI</td>
<td align="center">DISCO-F469NI</td>
</tr><tr>
<td align="center">DISCO-F746NG</td>
<td align="center">DISCO-F769NI</td>
<td align="center">DISCO-L152RC</td>
<td align="center">DISCO-L476VG</td>
</tr><tr>
<td align="center">FEATHER-M0</td>
<td align="center">MEGA-2560-PRO</td>
<td align="center">MINI-F401</td>
<td align="center">MINI-F411</td>
</tr><tr>
<td align="center">NUCLEO-F031K6</td>
<td align="center">NUCLEO-F042K6</td>
<td align="center">NUCLEO-F103RB</td>
<td align="center">NUCLEO-F303K8</td>
</tr><tr>
<td align="center">NUCLEO-F303RE</td>
<td align="center">NUCLEO-F401RE</td>
<td align="center">NUCLEO-F411RE</td>
<td align="center">NUCLEO-F429ZI</td>
</tr><tr>
<td align="center">NUCLEO-F446RE</td>
<td align="center">NUCLEO-F746ZG</td>
<td align="center">NUCLEO-G071RB</td>
<td align="center">NUCLEO-G474RE</td>
</tr><tr>
<td align="center">NUCLEO-L152RE</td>
<td align="center">NUCLEO-L432KC</td>
<td align="center">NUCLEO-L476RG</td>
<td align="center">OLIMEXINO-STM32</td>
</tr><tr>
<td align="center">SAMD21-MINI</td>
<td align="center">STM32F030F4P6-DEMO</td>
</tr>
</table>
<!--/bsptable-->
</center>


### Drivers

We also have a number of completely target-independent [drivers for external
devices][drivers] connected via I<sup>2</sup>C, SPI, UART, BitBang, etc.
Most of these also give you access to the entire device so you
can easily configure them for you specific needs.

<center>
<!--drivertable-->
<table>
<tr>
<td align="center">AD7280A</td>
<td align="center">AD7928</td>
<td align="center">ADNS9800</td>
<td align="center">ADS7843</td>
<td align="center">AMS5915</td>
<td align="center">APA102</td>
</tr><tr>
<td align="center">SPI-FLASH</td>
<td align="center">BME280</td>
<td align="center">BMP085</td>
<td align="center">BNO055</td>
<td align="center">CAT24AA</td>
<td align="center">DRV832X</td>
</tr><tr>
<td align="center">DS1302</td>
<td align="center">DS1631</td>
<td align="center">DS18B20</td>
<td align="center">EA-DOG</td>
<td align="center">ENCODER-OUTPUT</td>
<td align="center">FT245</td>
</tr><tr>
<td align="center">FT6X06</td>
<td align="center">HCLAx</td>
<td align="center">HD44780</td>
<td align="center">HMC58x</td>
<td align="center">HMC6343</td>
<td align="center">HX711</td>
</tr><tr>
<td align="center">I2C-EEPROM</td>
<td align="center">ITG3200</td>
<td align="center">L3GD20</td>
<td align="center">LAWICEL</td>
<td align="center">LIS302DL</td>
<td align="center">LIS3DSH</td>
</tr><tr>
<td align="center">LIS3MDL</td>
<td align="center">LM75</td>
<td align="center">LP503X</td>
<td align="center">LSM303A</td>
<td align="center">LSM6DS33</td>
<td align="center">LTC2984</td>
</tr><tr>
<td align="center">MAX6966</td>
<td align="center">MAX7219</td>
<td align="center">MCP23X17</td>
<td align="center">MCP2515</td>
<td align="center">NOKIA5110</td>
<td align="center">NRF24</td>
</tr><tr>
<td align="center">TFT-DISPLAY</td>
<td align="center">PAT9125EL</td>
<td align="center">PCA8574</td>
<td align="center">PCA9535</td>
<td align="center">PCA9548A</td>
<td align="center">PCA9685</td>
</tr><tr>
<td align="center">SIEMENS-S65</td>
<td align="center">SIEMENS-S75</td>
<td align="center">SK6812</td>
<td align="center">SK9822</td>
<td align="center">SSD1306</td>
<td align="center">SX1276</td>
</tr><tr>
<td align="center">TCS3414</td>
<td align="center">TCS3472</td>
<td align="center">TLC594X</td>
<td align="center">TMP102</td>
<td align="center">TMP175</td>
<td align="center">VL53L0</td>
</tr><tr>
<td align="center">VL6180</td>
<td align="center">WS2812</td>
</tr>
</table>
<!--/drivertable-->
</center>

<!--webignore-->
Please see [our examples for a complete list][examples] of tested targets and drivers.


## How can I contribute?

The easiest way for you and the best way for us to see if something is unclear
or missing, is if you use the library and give us some feedback by
[filing a bug report][issues] or if you have a fix already, [opening a pull request][prs].

See [CONTRIBUTING.md][contrib] for our contribution guidelines.

<!--authors-->
The modm project is maintained by
Niklas Hauser ([\@salkinium](https://github.com/salkinium)) with significant contributions from
Sascha Schade ([\@strongly-typed](https://github.com/strongly-typed)),
Fabian Greif ([\@dergraaf](https://github.com/dergraaf)),
Kevin Läufer ([\@ekiwi](https://github.com/ekiwi)),
Martin Rosekeit ([\@thundernail](https://github.com/thundernail)),
Daniel Krebs ([\@daniel-k](https://github.com/daniel-k)),
Georgi Grinshpun ([\@georgi-g](https://github.com/georgi-g)) and
<!--authorcount-->33<!--/authorcount-->  more contributors.
<!--/authors-->

## Folder structure

<dl>
<dt>examples</dt>
<dd>
  Example projects that show the usage of parts of the modm library. These
  projects are always up to date and are tested to compile by our CI.
</dd>

<dt>src</dt>
<dd>
  This folder contains the actual source code of modm grouped into several modules.
</dd>

<dt>test</dt>
<dd>
  Contains hundreds of tests for making sure modm works the way we expect it to.
</dd>

<dt>docs</dt>
<dd>
  General documentation about this library and how to use it.
</dd>

<dt>ext</dt>
<dd>
  Third-party code used in this library.
</dd>

<dt>tools</dt>
<dd>
  Support tools, scripts and files.
</dl>
<!--/webignore-->

<!--links-->
[blog]:            https://blog.salkinium.com
[changelog]:       https://github.com/modm-io/modm/tree/develop/docs/CHANGELOG.md
[circle_ci]:       https://circleci.com/gh/modm-io/workflows/modm/tree/develop
[contrib]:         https://github.com/modm-io/modm/tree/develop/CONTRIBUTING.md
[devboards]:       https://github.com/modm-io/modm/tree/develop/src/modm/board
[drivers]:         https://github.com/modm-io/modm/tree/develop/src/modm/driver
[eurobot]:         https://www.eurobot.org/
[examples]:        https://github.com/modm-io/modm/tree/develop/examples
[guide]:           https://modm.io/guide/getting-started
[install]:         https://modm.io/guide/installation
[issues]:          https://github.com/modm-io/modm/issues
[library-builder]: https://github.com/dergraaf/library-builder
[modm-devices]:    https://github.com/modm-io/modm-devices
[porting]:         https://github.com/modm-io/modm/tree/develop/docs/PORTING.md
[prs]:             https://github.com/modm-io/modm/pulls
[rca_ev]:          https://www.roboterclub.rwth-aachen.de/
[reference]:       https://modm.io/reference/api
[releases]:        https://github.com/modm-io/modm/releases
[testing]:         https://modm.io/guide/testing
[api-docs]:        https://docs.modm.io/
<!--/links-->
