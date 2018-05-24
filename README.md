<!--webignore-->
# modm: a barebone embedded library generator
<p align="center">
<a href="http://modm.io"><img src="https://raw.githubusercontent.com/modm-io/modm/develop/docs/src/images/logo.svg?sanitize=true" style="max-width:100%;"></a>
</p>
<!--/webignore-->

modm (Modular Object-oriented Development for Microcontrollers) is a toolbox for
building custom C++14 libraries tailored to your embedded device.
modm generates startup code, HALs and their implementations, communication
protocols, drivers for external devices, BSPs, etc… in a modular, customizable
process that you can fine-tune to your needs.

<!--webignore-->
- [This project has a homepage](http://modm.io).<!--/webignore-->
- Check out our [install instructions][install] and our [getting started guide][guide].
- Feast your eyes on [lots of working examples][examples].
<!-- - [API reference is available here][reference]. -->
<!-- - [Check out our latest quarterly release][releases] with a [beautiful changelog][changelog]. -->
- Our CIs check every contribution for regressions: <a href="https://circleci.com/gh/modm-io/workflows/modm/tree/develop" style="border-bottom:none">![Build Status](https://circleci.com/gh/modm-io/modm/tree/develop.svg?style=shield)</a> <a href="https://gitlab.com/modm-io/modm/pipelines">![Build Status](https://gitlab.com/modm-io/modm/badges/develop/build.svg)> </a>
- We care [about testing modm][testing].

This project also has a [technical blog][blog] to document larger design concepts.

- You found a bug? [Open up an issue, we don't bite][issues].
- You want to contribute? [Read the contribution guidelines][contributing] and [open a pull request][prs].
<!-- - You want to port modm? [Read our porting guide][porting]. -->

modm is optimized for the harsh requirements of the [Eurobot competition][eurobot],
where our robots need to run reliably and completely autonomously for the game's
100 second duration. Our robots contain a lot of different microcontrollers, some
without a lot of resources, so modm needs to fulfill a diverse set of objectives,
like small code size with small memory consumption, predictable program flow,
extreme portability.

The source code is freely available under the MPLv2 license, so feel
free to fork this project and adapt it to your needs.
The only thing we ask of you is to contribute your changes back so everyone benefits.

Please clone modm recursively, you will need all the submodules.

```sh
git clone --recursive https://github.com/modm-io/modm.git
```

## Features

- Efficient and fast object-oriented C++14 API.
- Support for hundreds of AVR and ARM Cortex-M microcontrollers from Atmel and ST.
- Build system agnostic: We use SCons by default, but you don't have to.
- Data-driven HAL generation using the library-builder engine.
- No memory allocations in HAL with very low overall RAM consumption.
- Highly-configurable modules with sensible defaults and lots of documentation.
- Cross platform peripheral interfaces incl. bit banging:
    - GPIO and GPIO expanders.
    - ADC and Analog.
    - UART, I<sup>2</sup>C, SPI, CAN.
- Interfaces and drivers for many external I2C and SPI sensors and devices.
- Debug/logging system with IOStream interface.
- Lightweight, stackless threads and resumable functions using cooperative multitasking.
- Useful filter, interpolation and geometric algorithms.
- Lightweight unit testing system (suitable for AVRs).
- Graphical user interface for small binary and color displays.
- Hundreds of tests to ensure correct functionality.


## Targets

modm can generate code for <!--avrcount-->76<!--/avrcount--> AVR  and
<!--stmcount-->858<!--/stmcount--> STM32 devices, however,
there are different levels of support and testing.

<center>

| Device Family | Support                             | Device Family | Support                             |
|:--------------|:------------------------------------|:--------------|:------------------------------------|
| AVR           | &#9733;&#9733;&#9733;               | STM32F3       | &#9733;&#9733;&#9733;&#9733;&#9733; |
| STM32F0       | &#9733;&#9733;&#9733;&#9733;        | STM32F4       | &#9733;&#9733;&#9733;&#9733;&#9733; |
| STM32F1       | &#9733;&#9733;&#9733;&#9733;        | STM32F7       | &#9733;&#9733;&#9733;&#9733;        |
| STM32F2       | &#9733;&#9733;&#9733;               | STM32L4       | &#9733;&#9733;&#9733;&#9733;        |

</center>

All of these targets are compiling and booting correctly
(&#9733;)
and have GPIO and UART working
(&#9733;&#9733;).
Most targets have support for basic peripherals, like I2C, SPI and ADC
(&#9733;&#9733;&#9733;)
as well as complicated peripherals, like Timers, CAN and external memory
(&#9733;&#9733;&#9733;&#9733;).
We also use a few targets in everyday development, which are very well tested
(&#9733;&#9733;&#9733;&#9733;&#9733;).

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
<td align="center">Arduino Uno</td>
<td align="center">Blue Pill</td>
<td align="center">DISCO-F051R8</td>
</tr><tr>
<td align="center">DISCO-F072RB</td>
<td align="center">DISCO-F100RB</td>
<td align="center">DISCO-F303VC</td>
<td align="center">DISCO-F407VG</td>
</tr><tr>
<td align="center">DISCO-F429ZI</td>
<td align="center">DISCO-F469NI</td>
<td align="center">DISCO-F746NG</td>
<td align="center">DISCO-F769NI</td>
</tr><tr>
<td align="center">DISCO-L476VG</td>
<td align="center">NUCLEO-F031K6</td>
<td align="center">NUCLEO-F103RB</td>
<td align="center">NUCLEO-F303K8</td>
</tr><tr>
<td align="center">NUCLEO-F401RE</td>
<td align="center">NUCLEO-F411RE</td>
<td align="center">NUCLEO-F429ZI</td>
<td align="center">NUCLEO-L476RG</td>
</tr><tr>
<td align="center">OLIMEXINO-STM32</td>
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
<td align="center">AMSYS5915</td>
<td align="center">SPI-FLASH</td>
</tr><tr>
<td align="center">BME280</td>
<td align="center">BMP085</td>
<td align="center">DS1302</td>
<td align="center">DS1631</td>
<td align="center">DS18B20</td>
<td align="center">EA-DOG</td>
</tr><tr>
<td align="center">FT245</td>
<td align="center">FT6X06</td>
<td align="center">HCLAx</td>
<td align="center">HD44780</td>
<td align="center">HMC58x</td>
<td align="center">HMC6343</td>
</tr><tr>
<td align="center">I2C-EEPROM</td>
<td align="center">ITG3200</td>
<td align="center">L3GD20</td>
<td align="center">LAWICEL</td>
<td align="center">LIS302DL</td>
<td align="center">LIS3DSH</td>
</tr><tr>
<td align="center">LM75</td>
<td align="center">LSM303A</td>
<td align="center">LTC2984</td>
<td align="center">MAX6966</td>
<td align="center">MAX7219</td>
<td align="center">MCP23X17</td>
</tr><tr>
<td align="center">MCP2515</td>
<td align="center">NOKIA5110</td>
<td align="center">NRF24</td>
<td align="center">TFT-DISPLAY</td>
<td align="center">PCA8574</td>
<td align="center">PCA9535</td>
</tr><tr>
<td align="center">PCA9685</td>
<td align="center">SIEMENS-S65</td>
<td align="center">SIEMENS-S75</td>
<td align="center">SSD1306</td>
<td align="center">TCS3414</td>
<td align="center">TCS3472</td>
</tr><tr>
<td align="center">TMP102</td>
<td align="center">TMP175</td>
<td align="center">VL53L0</td>
<td align="center">VL6180</td>
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

See [CONTRIBUTING.md][contributing] for our contribution guidelines.

<!--authors-->
The modm project is maintained by
Niklas Hauser ([@salkinium](https://github.com/salkinium)) with significant contributions from
Sascha Schade ([@strongly-typed](https://github.com/strongly-typed)),
Fabian Greif ([@dergraaf](https://github.com/dergraaf)),
Kevin Läufer ([@ekiwi](https://github.com/ekiwi)),
Martin Rosekeit ([@thundernail](https://github.com/thundernail)),
Daniel Krebs ([@daniel-k](https://github.com/daniel-k)),
Georgi Grinshpun ([@georgi-g](https://github.com/georgi-g)) and 
[<!--authorcount-->20<!--/authorcount--> more contributors](https://github.com/modm-io/modm/blob/develop/AUTHORS).
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
[blog]:            http://blog.salkinium.com
[changelog]:       https://github.com/modm-io/modm/tree/develop/docs/CHANGELOG.md
[circle_ci]:       https://circleci.com/gh/modm-io/workflows/modm/tree/develop
[contributing]:    https://github.com/modm-io/modm/tree/develop/CONTRIBUTING.md
[devboards]:       https://github.com/modm-io/modm/tree/develop/src/modm/board
[drivers]:         https://github.com/modm-io/modm/tree/develop/src/modm/driver
[eurobot]:         http://www.eurobot.org/
[examples]:        https://github.com/modm-io/modm/tree/develop/examples
[guide]:           http://modm.io/guide/getting-started
[install]:         http://modm.io/guide/installation
[issues]:          https://github.com/modm-io/modm/issues
[library-builder]: https://github.com/dergraaf/library-builder
[modm-devices]:    https://github.com/modm-io/modm-devices
[porting]:         https://github.com/modm-io/modm/tree/develop/docs/PORTING.md
[prs]:             https://github.com/modm-io/modm/pulls
[rca_ev]:          http://www.roboterclub.rwth-aachen.de/
[reference]:       http://modm.io/reference/api
[releases]:        https://github.com/modm-io/modm/releases
[testing]:         http://modm.io/guide/testing
<!--/links-->
