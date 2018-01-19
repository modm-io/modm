# modm: a modular C++14 library for barebone programming

modm (Modular Object-oriented Development for Microcontrollers) is a toolbox for building custom libraries tailored to your embedded device.
modm generates startup code, HALs and their implementations, communication protocols, drivers for external devices, BSPs, etc… in a modular, customizable process that you can fine-tune for your needs.

- [This project has a homepage](http://modm.io) with [install instructions][install] and a [getting started guide][guide].
- [Feast your eyes on lots of working examples][examples].
- [API reference is available here][reference].
- [Check out our latest quarterly release][releases] with a [beautiful changelog](CHANGELOG.md).
- [We have continuous integration as well][circle_ci].
- [And we care a lot about testing](TESTING.md).

This project also has [guide for developers][guide] as well as a [technical blog][blog] to document larger design concepts.

- You found a bug? [Open up an issue, we don't bite][issues].
- You want to contribute? [Read the contribution guidelines][contributing] and [open a pull request so we can merge it][prs].
- You want to port modm? [Read our porting guide][porting].

The source code is freely available under the MPLv2 license, so feel
free to fork this project and adapt it to your needs.
The only thing we ask of you is to contribute your changes back.
That way everyone can profit.

<a href="https://circleci.com/gh/modm-io/workflows/modm/tree/develop" style="border-bottom:none">![Build Status](https://circleci.com/gh/modm-io/modm/tree/develop.svg?style=shield)</a>

## Features

- Efficient and fast object-oriented C++14 API.
- Support for hundreds of AVR and ARM Cortex-M microcontrollers from Atmel and ST.
- Build system agnostic: We use SCons by default, but you don't have to.
- [Data-driven][modm-devices] HAL generation using the [library-builder engine][library-builder].
- No memory allocations in HAL with very low overall RAM consumption.
- Cross platform peripheral interfaces incl. bit banging:
    - GPIO & GPIO expanders
    - ADC
    - UART, I2C, SPI
    - CAN
- Interfaces for external I2C and SPI device drivers.
- Debug/logging system with IOStream interface.
- Lightweight, stackless threads and resumable functions using cooperative multitasking.
- Useful mathematical and geometric algorithms optimized for microcontrollers.
- Lightweight unit testing system (suitable for AVRs).
- Graphical user interface for small binary displays.

## Supported hardware

Here is a list of supported **and tested** microcontrollers and development boards:

| Controller | Development Board       | Support                             |
|:-----------|:------------------------|:------------------------------------|
| AT90can    | custom                  | &#9733;&#9733;&#9733;               |
| ATtiny44a  | custom                  | &#9733;&#9733;&#9733;               |
| ATtiny85   | custom                  | &#9733;&#9733;&#9733;               |
| ATmega328p | [Arduino Uno][]         | &#9733;&#9733;&#9733;               |
| STM32F031  | [Nucleo F031K6][]       | &#9733;&#9733;&#9733;&#9733;        |
| STM32F051  | [STM32F0 Discovery][]   | &#9733;&#9733;                      |
| STM32F072  | [STM32F072 Discovery][] | &#9733;&#9733;&#9733;&#9733;        |
| STM32F100  | [STM32F1 Discovery][]   | &#9733;&#9733;&#9733;               |
| STM32F103  | [Nucleo F103RB][]       | &#9733;&#9733;&#9733;               |
| STM32F103  | [STM32F1 Blue Pill][]   | &#9733;&#9733;&#9733;               |
| STM32F303  | [STM32F3 Discovery][]   | &#9733;&#9733;&#9733;&#9733;&#9733; |
| STM32F303  | [Nucleo F303K8][]       | &#9733;&#9733;&#9733;&#9733;        |
| STM32F407  | [STM32F4 Discovery][]   | &#9733;&#9733;&#9733;&#9733;&#9733; |
| STM32F401  | [Nucleo F401RE][]       | &#9733;&#9733;&#9733;&#9733;        |
| STM32F411  | [Nucleo F411RE][]       | &#9733;&#9733;&#9733;&#9733;        |
| STM32F429  | [STM32F429 Discovery][] | &#9733;&#9733;&#9733;&#9733;&#9733; |
| STM32F429  | [Nucleo F429ZI][]       | &#9733;&#9733;&#9733;&#9733;        |
| STM32F469  | [STM32F469 Discovery][] | &#9733;&#9733;&#9733;&#9733;        |
| STM32F746  | [STM32F746 Discovery][] | &#9733;&#9733;&#9733;&#9733;        |
| STM32F769  | [STM32F769 Discovery][] | &#9733;&#9733;&#9733;&#9733;        |
| STM32L476  | [STM32L476 Discovery][] | &#9733;&#9733;                      |
| STM32L476  | [Nucleo L476RG][]       | &#9733;&#9733;                      |

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

Please see [our examples for a complete list][examples] of tested projects.

### Your target

While the modm API is designed to be portable, we are only a small team of developers and are limited in the amount of platforms we can support and test in hardware.
The following microcontrollers should be able to compile, but *have not been tested extensively* in hardware:

- All STM32F targets.
- Most AVR targets, except Xmega.
- Some STM32L targets.

There are more platforms which we have prepared, but currently not finished support for (Xmega, STM32F2, STM32L).
[Open an issue][issues] to ask if your specific target is supported out-of-the-box and what you can do if it's not.

## Documentation

The project homepage is located at [modm.io](http://modm.io).
Complete installation instructions for our build system and toolchains can [be found here][install].

To get source-code documentation, include the `:docs` module (or execute `lbuild build --module ":docs"`), then call `doxygen doxyfile` inside the generated `docs` folder. The documentation for your target and configuration will then be available in `docs/api/index.html`.

## How can I contribute?

The easiest way for you and the best way for us to see if something is unclear or missing, is if you use the library and give us some feedback by [filing a bug report][issues] or if you have a fix already, [opening a pull request][prs].

See [CONTRIBUTING.md][contributing] for our contributing guidelines.

## Who we are

During the last decade the [Roboterclub Aachen e.V.][rca_ev] has developed a software library for communication among components that are distributed on PCs and microcontrollers. This library was used in autonomous robots for the [Eurobot competition][eurobot].

In 2009, xpcc became a separate project and since then focussed on a new approach to cross target microcontroller libraries. Over the years xpcc grew from a communication library to a general purpose framework suitable for all kinds of embedded applications.

In 2016-2018, Niklas and Fabian refactored xpcc into modm and relicensed it from BSD to MPLv2.

The modm project is maintained by
Niklas Hauser ([@salkinium](https://github.com/salkinium)) with significant contributions from
Sascha Schade ([@strongly-typed](https://github.com/strongly-typed)),
Kevin Läufer ([@ekiwi](https://github.com/ekiwi)),
Fabian Greif ([@dergraaf](https://github.com/dergraaf)),
Martin Rosekeit ([@thundernail](https://github.com/thundernail)),
Daniel Krebs ([@daniel-k](https://github.com/daniel-k)) and
Georgi Grinshpun ([@georgi-g](https://github.com/georgi-g)).

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

[blog]: http://blog.salkinium.com
[contributing]: CONTRIBUTING.md
[eurobot]: http://www.eurobot.org/
[examples]: https://github.com/modm-io/modm/tree/develop/examples
[guide]: http://modm.io/guide/getting-started
[install]: http://modm.io/installation
[issues]: https://github.com/modm-io/modm/issues
[porting]: docs/PORTING.md
[prs]: https://github.com/modm-io/modm/pulls
[rca_ev]: http://www.roboterclub.rwth-aachen.de/
[reference]: http://modm.io/reference/api
[releases]: https://github.com/modm-io/modm/releases
[testing]: docs/TESTING.md
[circle_ci]: https://circleci.com/gh/modm-io/workflows/modm
[modm-devices]: https://github.com/modm-io/modm-devices
[library-builder]: https://github.com/dergraaf/library-builder

[Arduino Uno]: https://www.arduino.cc/en/Main/ArduinoBoardUno
[STM32F1 Blue Pill]: http://wiki.stm32duino.com/index.php?title=Blue_Pill
[Nucleo F031K6]: http://www.st.com/en/evaluation-tools/nucleo-f031k6.html
[Nucleo F103RB]: http://www.st.com/en/evaluation-tools/nucleo-f103rb.html
[Nucleo F303K8]: http://www.st.com/en/evaluation-tools/nucleo-f303k8.html
[Nucleo F401RE]: http://www.st.com/en/evaluation-tools/nucleo-f401re.html
[Nucleo F411RE]: http://www.st.com/en/evaluation-tools/nucleo-f411re.html
[Nucleo F429ZI]: http://www.st.com/en/evaluation-tools/nucleo-f429zi.html
[Nucleo L476RG]: http://www.st.com/en/evaluation-tools/nucleo-l476rg.html
[STM32F0 Discovery]: http://www.st.com/en/evaluation-tools/stm32f0discovery.html
[STM32F072 Discovery]: http://www.st.com/en/evaluation-tools/32f072bdiscovery.html
[STM32F1 Discovery]: http://www.st.com/en/evaluation-tools/stm32vldiscovery.html
[STM32F3 Discovery]: http://www.st.com/en/evaluation-tools/stm32f3discovery.html
[STM32F4 Discovery]: http://www.st.com/en/evaluation-tools/stm32f4discovery.html
[STM32F429 Discovery]: http://www.st.com/en/evaluation-tools/32f429idiscovery.html
[STM32F469 Discovery]: http://www.st.com/en/evaluation-tools/32f469idiscovery.html
[STM32F746 Discovery]: http://www.st.com/en/evaluation-tools/32f746gdiscovery.html
[STM32F769 Discovery]: http://www.st.com/en/evaluation-tools/32f769idiscovery.html
[STM32L476 Discovery]: http://www.st.com/en/evaluation-tools/32l476gdiscovery.html
