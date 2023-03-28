<!--webignore-->
# modm: a barebone embedded library generator
<p align="center">
<a href="http://modm.io"><img src="https://raw.githubusercontent.com/modm-io/modm/develop/docs/src/images/logo.svg?sanitize=true" style="max-width:100%;"></a>
</p>
<!--/webignore-->

modm (pronounced like dial-up "modem") is a toolbox for
building custom C++23 libraries tailored to your embedded device.
modm generates startup code, HALs and their implementations, communication
protocols, drivers for external devices, and BSPs in a modular, customizable
process that you can fine-tune to your needs.

<!--webignore-->
- [This project has a homepage](http://modm.io).<!--/webignore-->
- [Install the toolchain][install] and [explore our examples][examples].
- You can [discover the modm library][discover] in more detail.
- Easily start your [own completely custom project][custom-project].
- Our CI checks every contribution for regressions.
- We care [about testing modm][testing].
- [API reference is available here][api-docs].
- [We do quarterly releases][releases] with a [curated changelog][changelog].

This project also has a [forum for discussions][discussions] and a
[technical blog][blog] to document larger design concepts.

- You found a bug? [Open up an issue, we don't bite][issues].
- You have a question? [We probably have an answer][questions].
- You require a feature? [Write it down to get things rolling][ideas].
- You want to contribute? [Read the contribution guidelines][contrib] and [open a pull request][prs].

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
git clone --recurse-submodules --jobs 8 https://github.com/modm-io/modm.git
```

## Features

- Efficient and fast object-oriented C++23 API.
- Support for thousands of AVR and ARM Cortex-M microcontrollers from Microchip, STMicroelectronics and Raspberry Pi.
- Build system agnostic: Choose SCons, CMake, Makefile or use your own.
- Modular, data-driven, target-specific HAL generation using the lbuild code generator.
- No memory allocations in HAL with very low overall RAM consumption.
- Highly configurable modules with sensible defaults and lots of documentation.
- Cross-platform peripheral interfaces incl. bit banging:
    - GPIO, External Interrupt and IO expanders.
    - ADC, DAC and Comparators.
    - UART, I<sup>2</sup>C, SPI, CAN and Ethernet.
- Interfaces and drivers for many external I<sup>2</sup>C and SPI sensors and devices.
- Debug/logging system with IOStream and printf interface.
- Cooperative, stackless protothreads and resumable functions.
- Cooperative, stackful fibers and scheduler.
- Functional (partial) libstdc++ implementation for AVRs.
- Useful filter, interpolation and geometric algorithms.
- Lightweight unit testing system (suitable for AVRs).
- Hundreds of tests to ensure correct functionality.
- Integration of useful third-party software:
	- [FreeRTOS][] and [FreeRTOS+TCP][] operating system.
	- [CMSIS][] and [CMSIS-DSP][] interfaces.
	- [ETL][]: Embedded Template Library.
	- [TinyUSB][]: USB Host/Device stack.
	- [FatFS][]: FAT/exFAT filesystem.
	- [ROSserial][]: Embedded ROS client.
	- [CrashCatcher][]: Crash reports for HardFaults.
	- [printf][]: Small printf implementation.
	- [Nanopb][]: Embedded Protocol Buffers.
	- [LVGL][]: Embedded Graphics Library.


## Microcontrollers

modm can create a HAL for <!--allcount-->3734<!--/allcount--> devices of these vendors:

- STMicroelectronics STM32: <!--stmcount-->2929<!--/stmcount--> devices.
- Microchip SAM: <!--samcount-->416<!--/samcount--> devices.
- Microchip AVR: <!--avrcount-->388<!--/avrcount--> devices.
- Raspberry Pi: <!--rpicount-->1<!--/rpicount--> device.

Here is a table with all device families and the peripheral drivers they support:

- ✅ Implemented as a software driver in modm.
- ○ Available in hardware but missing a software driver in modm.
- ✕ Unavailable in hardware or device with that peripheral not supported by modm.

Note that this is a summary overview and your specific device may not have all
the peripherals in this table.
Please [discover modm's peripheral drivers for your specific device][discover].

<center>
<!--alltable-->
<table>
<tr>
<th align="center"></th>
<th align="center" colspan="14">STM32</th>
<th align="center" colspan="4">SAM</th>
<th align="center" colspan="1">RP</th>
<th align="center" colspan="3">AT</th>
</tr><tr>
<th align="left">Peripheral</th>
<th align="center">F0</th>
<th align="center">F1</th>
<th align="center">F2</th>
<th align="center">F3</th>
<th align="center">F4</th>
<th align="center">F7</th>
<th align="center">G0</th>
<th align="center">G4</th>
<th align="center">H7</th>
<th align="center">L0</th>
<th align="center">L1</th>
<th align="center">L4</th>
<th align="center">L5</th>
<th align="center">U5</th>
<th align="center">D1x<br/>D2x<br/>DAx</th>
<th align="center">D5x<br/>E5x</th>
<th align="center">E7x<br/>S7x<br/>V7x</th>
<th align="center">G5x</th>
<th align="center">20</th>
<th align="center">90</th>
<th align="center">Mega</th>
<th align="center">Tiny</th>
</tr><tr>
<td align="left">ADC</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✅</td>
</tr><tr>
<td align="left">CAN</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✕</td>
</tr><tr>
<td align="left">Comparator</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
</tr><tr>
<td align="left">DAC</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">✕</td>
</tr><tr>
<td align="left">DMA</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
</tr><tr>
<td align="left">Ethernet</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
</tr><tr>
<td align="left">External Interrupt</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
</tr><tr>
<td align="left">External Memory</td>
<td align="center">✕</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
</tr><tr>
<td align="left">GPIO</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
</tr><tr>
<td align="left">I<sup>2</sup>C</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
</tr><tr>
<td align="left">Internal Flash</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
</tr><tr>
<td align="left">IWDG</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
</tr><tr>
<td align="left">Random Generator</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
</tr><tr>
<td align="left">SPI</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
</tr><tr>
<td align="left">System Clock</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
</tr><tr>
<td align="left">Timer</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
<td align="center">○</td>
</tr><tr>
<td align="left">UART</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
</tr><tr>
<td align="left">Unique ID</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
</tr><tr>
<td align="left">USB</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">○</td>
<td align="center">✅</td>
<td align="center">✅</td>
<td align="center">✕</td>
<td align="center">✕</td>
<td align="center">✕</td>
</tr>
</table>
<!--/alltable-->
</center>

We are only a small team of developers and are limited in the amount of devices
we can support and test in hardware. [Open a discussion][discussions] to ask if
your specific device is supported out-of-the-box and how you can add it otherwise.


### Boards

We have out-of-box support for many development boards including documentation.

<center>
<!--bsptable-->
<table>
<tr>
<td align="center"><a href="https://modm.io/reference/config/modm-al-avreb-can">AL-AVREB-CAN</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-arduino-nano">Arduino NANO</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-arduino-uno">Arduino UNO</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-black-pill-f103">Black Pill F103</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-black-pill-f401">Black Pill F401</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-black-pill-f411">Black Pill F411</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-blue-pill-f103">Blue Pill F103</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-devebox-stm32f4xx">DEVEBOX-STM32F4XX</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-devebox-stm32h750vb">DEVEBOX-STM32H750VB</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-f051r8">DISCO-F051R8</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-f072rb">DISCO-F072RB</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-f100rb">DISCO-F100RB</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-f303vc">DISCO-F303VC</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-f401vc">DISCO-F401VC</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-f407vg">DISCO-F407VG</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-f411ve">DISCO-F411VE</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-f429zi">DISCO-F429ZI</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-f469ni">DISCO-F469NI</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-f746ng">DISCO-F746NG</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-f769ni">DISCO-F769NI</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-l152rc">DISCO-L152RC</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-disco-l476vg">DISCO-L476VG</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-feather-m0">FEATHER-M0</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-feather-m4">FEATHER-M4</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-feather-rp2040">FEATHER-RP2040</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-mega-2560-pro">MEGA-2560-PRO</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f031k6">NUCLEO-F031K6</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f042k6">NUCLEO-F042K6</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f072rb">NUCLEO-F072RB</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f091rc">NUCLEO-F091RC</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f103rb">NUCLEO-F103RB</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f303k8">NUCLEO-F303K8</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f303re">NUCLEO-F303RE</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f334r8">NUCLEO-F334R8</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f401re">NUCLEO-F401RE</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f411re">NUCLEO-F411RE</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f429zi">NUCLEO-F429ZI</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f439zi">NUCLEO-F439ZI</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f446re">NUCLEO-F446RE</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f446ze">NUCLEO-F446ZE</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f746zg">NUCLEO-F746ZG</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-f767zi">NUCLEO-F767ZI</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-g070rb">NUCLEO-G070RB</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-g071rb">NUCLEO-G071RB</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-g431kb">NUCLEO-G431KB</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-g431rb">NUCLEO-G431RB</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-g474re">NUCLEO-G474RE</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-h723zg">NUCLEO-H723ZG</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-h743zi">NUCLEO-H743ZI</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-l031k6">NUCLEO-L031K6</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-l053r8">NUCLEO-L053R8</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-l152re">NUCLEO-L152RE</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-l432kc">NUCLEO-L432KC</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-l452re">NUCLEO-L452RE</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-l476rg">NUCLEO-L476RG</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-l496zg-p">NUCLEO-L496ZG-P</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-l552ze-q">NUCLEO-L552ZE-Q</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-nucleo-u575zi-q">NUCLEO-U575ZI-Q</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-olimexino-stm32">OLIMEXINO-STM32</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-rp-pico">Raspberry Pi Pico</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-samd21-mini">SAMD21-MINI</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-samd21-xplained-pro">SAMD21-XPLAINED-PRO</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-same54-xplained-pro">SAME54-XPLAINED-PRO</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-same70-xplained">SAME70-XPLAINED</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-samg55-xplained-pro">SAMG55-XPLAINED-PRO</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-samv71-xplained-ultra">SAMV71-XPLAINED-ULTRA</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-srxe">Smart Response XE</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-stm32_f4ve">STM32-F4VE</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/config/modm-stm32f030_demo">STM32F030-DEMO</a></td>
<td align="center"><a href="https://modm.io/reference/config/modm-thingplus-rp2040">THINGPLUS-RP2040</a></td>
</tr>
</table>
<!--/bsptable-->
</center>


### Drivers

We also have a number of completely target-independent drivers for external
devices connected via I<sup>2</sup>C, SPI, UART, BitBang, etc. Most of these
also give you access to the entire device so you can easily configure them for
your specific needs.

<center>
<!--drivertable-->
<table>
<tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ad7280a">AD7280A</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ad7928">AD7928</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-adis16470">ADIS16470</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-adns9800">ADNS9800</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ads101x">ADS101X</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ads7828">ADS7828</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ads7843">ADS7843</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ads816x">ADS816x</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ams5915">AMS5915</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-apa102">APA102</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-as5047">AS5047</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-at24mac402">AT24MAC402</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-block-device-spi-flash">SPI Flash</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-bme280">BME280</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-bmi088">BMI088</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-bmp085">BMP085</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-bno055">BNO055</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-cat24aa">CAT24AA</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-cycle_counter">CYCLE-COUNTER</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-drv832x_spi">DRV832X</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ds1302">DS1302</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ds1631">DS1631</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ds18b20">DS18B20</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ea_dog">EA-DOG</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-encoder_input">Encoder Input</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-encoder_input-bitbang">Encoder Input BitBang</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-encoder_output-bitbang">Encoder Output BitBang</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ft245">FT245</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ft6x06">FT6x06</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-gpio_sampler">Gpio Sampler</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-hclax">HCLAx</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-hd44780">HD44780</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-hmc58x">HMC58x</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-hmc6343">HMC6343</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-hx711">HX711</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-i2c-eeprom">I2C-EEPROM</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ili9341">ILI9341</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-is31fl3733">IS31FL3733</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-itg3200">ITG3200</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ixm42xxx">IXM42XXX</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-l3gd20">L3GD20</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-lan8720a">LAN8720A</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-lawicel">LAWICEL</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-lis302dl">LIS302DL</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-lis3dsh">LIS3DSH</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-lis3mdl">LIS3MDL</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-lm75">LM75</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-lp503x">LP503x</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-lsm303a">LSM303A</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-lsm6ds33">LSM6DS33</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-lsm6dso">LSM6DSO</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ltc2984">LTC2984</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-max31855">MAX31855</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-max31865">MAX31865</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-max6966">MAX6966</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-max7219">MAX7219</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-mcp23x17">MCP23x17</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-mcp2515">MCP2515</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-mcp3008">MCP3008</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-mcp7941x">MCP7941x</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-mcp990x">MCP990X</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-mmc5603">MMC5603</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ms5611">MS5611</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ms5837">MS5837</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-nokia5110">NOKIA5110</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-nrf24">NRF24</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-parallel_tft_display">TFT-DISPLAY</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-pat9125el">PAT9125EL</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-pca8574">PCA8574</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-pca9535">PCA9535</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-pca9548a">PCA9548A</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-pca9685">PCA9685</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-qmc5883l">QMC5883L</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-sh1106">SH1106</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-siemens_s65">SIEMENS-S65</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-siemens_s75">SIEMENS-S75</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-sk6812">SK6812</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-sk9822">SK9822</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ssd1306">SSD1306</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-st7586s">ST7586S</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-st7789">ST7789</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-stts22h">STTS22H</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-stusb4500">STUSB4500</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-sx1276">SX1276</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-sx128x">SX128X</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-tcs3414">TCS3414</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-tcs3472">TCS3472</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-tlc594x">TLC594x</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-tmp102">TMP102</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-tmp12x">TMP12x</a></td>
</tr><tr>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-tmp175">TMP175</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-touch2046">TOUCH2046</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-vl53l0">VL53L0</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-vl6180">VL6180</a></td>
<td align="center"><a href="https://modm.io/reference/module/modm-driver-ws2812">WS2812</a></td>
</tr>
</table>
<!--/drivertable-->
</center>

<!--webignore-->
Please see [our examples for a complete list][examples] of tested targets and drivers.


## How can I contribute?

The easiest way for you and the best way for us to see if something is unclear
or missing, is if you use the library and give us some feedback by [filing a bug
report][issues] or if you have a fix already [opening a pull request][prs].

See [CONTRIBUTING.md][contrib] for our contribution guidelines.

<!--authors-->
The modm project is maintained by
Niklas Hauser ([\@salkinium](https://github.com/salkinium)),
Raphael Lehmann ([\@rleh](https://github.com/rleh)), and
Christopher Durand ([\@chris-durand](https://github.com/chris-durand)) with significant contributions from
Sascha Schade ([\@strongly-typed](https://github.com/strongly-typed)),
Fabian Greif ([\@dergraaf](https://github.com/dergraaf)),
Kevin Läufer ([\@ekiwi](https://github.com/ekiwi)),
Martin Rosekeit ([\@thundernail](https://github.com/thundernail)),
Daniel Krebs ([\@daniel-k](https://github.com/daniel-k)),
Georgi Grinshpun ([\@georgi-g](https://github.com/georgi-g)),
David Hebbeker ([\@dhebbeker](https://github.com/dhebbeker)),
Thorsten Lajewski ([\@TheTh0r](https://github.com/TheTh0r)),
Mike Wolfram ([\@mikewolfram](https://github.com/mikewolfram)),
and [many more contributors][contributors].
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
[changelog]:       https://github.com/modm-io/modm/tree/develop/CHANGELOG.md
[releases]:        https://github.com/modm-io/modm/releases
[contributors]:    https://github.com/modm-io/modm/graphs/contributors
[contrib]:         https://github.com/modm-io/modm/tree/develop/CONTRIBUTING.md
[eurobot]:         https://www.eurobot.org/
[examples]:        https://github.com/modm-io/modm/tree/develop/examples
[discover]:        https://modm.io/guide/discover
[custom-project]:  https://modm.io/guide/custom-project
[install]:         https://modm.io/guide/installation
[issues]:          https://github.com/modm-io/modm/issues
[prs]:             https://github.com/modm-io/modm/pulls
[discussions]:     https://github.com/modm-io/modm/discussions
[questions]:       https://github.com/modm-io/modm/discussions/categories/q-a
[ideas]:           https://github.com/modm-io/modm/discussions/categories/ideas
[testing]:         https://github.com/modm-io/modm/tree/develop/test/README.md
[api-docs]:        https://docs.modm.io/

[FreeRTOS]:        https://www.freertos.org
[FreeRTOS+TCP]:    https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/index.html
[CMSIS]:           https://www.keil.com/pack/doc/CMSIS/General/html/index.html
[CMSIS-DSP]:       https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html
[TinyUSB]:         https://github.com/hathach/tinyusb
[ETL]:             https://www.etlcpp.com
[FatFS]:           http://elm-chan.org/fsw/ff/00index_e.html
[ROSserial]:       https://wiki.ros.org/rosserial
[CrashCatcher]:    https://github.com/adamgreen/CrashCatcher
[printf]:          https://github.com/eyalroz/printf
[Nanopb]:          https://github.com/nanopb/nanopb
[LVGL]:            https://lvgl.io
<!--/links-->
