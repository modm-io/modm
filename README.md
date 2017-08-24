# xpcc: C++ microcontroller framework <span style="float:right;"><a href="https://travis-ci.org/roboterclubaachen/xpcc" style="border-bottom:none">![Build Status](https://travis-ci.org/roboterclubaachen/xpcc.svg?branch=develop)</a></span>

The xpcc framework consists of powerful hardware abstraction layers for many
different microcontrollers, a set of drivers for various external targets and a general purpose toolbox for building hardware orientated applications.

The main goal of xpcc is to provide a usable API for barebone microcontroller programming,
which is efficient enough to be deployed on a small ATtiny, yet powerful enough to make
use of advanced capabilities found on the 32bit ARM Cortex-M.

xpcc is [battle-tested](#who-we-are) in the real-world, highly competitive
environment of [Eurobot][].
It is the foundation of all of [@RCA_eV][rca]'s robot code, and is the
culmination of many years worth of effort, experience and improvements.

- [This project has a homepage](http://xpcc.io) with [install instructions][install] and a [getting started guide][guide].
- [Feast your eyes on lots of working examples][examples].
- [API reference is available here][reference].
- [Check out our latest quarterly release][releases] with a [beautiful changelog](CHANGELOG.md).
- [We have continuous integration as well][travis_ci].
- [And we care a lot about testing](TESTING.md).

This project also has [guide for developers][guide] as well as a [technical blog][blog] to document larger design concepts.

- You have questions? [Ask them on our mailing list][mailing_list]
or [have a look at the archive][mailing_archive].
- You found a bug? [Open up an issue, we don't bite][issues].
- You want to contribute? [Read the contribution guidelines][contributing] and [open a pull request so we can merge it][prs].
- You want to port xpcc? [Read our porting guide][porting].

The source code is freely available under a 3-clause BSD license, so feel
free to fork this project and adapt it to your needs.
The only thing we ask of you is to contribute your changes back.
That way everyone can profit.

## Features

- Efficient and fast object-oriented C++11 API.
- Support of AVR and ARM Cortex-M based microcontrollers from Atmel, ST and NXP.
- Build system based on SCons and extendable using Python.
- Data-driven HAL generation using Jinja2 template engine.
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
| LPC11C24   | [LPCxpresso][]          | &#9733;&#9733;                      |

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

While the xpcc API is designed to be portable, we are only a small team of developers and are limited in the amount of platforms we can support and test in hardware.
The following microcontrollers should be able to compile, but *have not been tested extensively* in hardware:

- All AT90 targets
- All ATtiny targets
- All ATmega targets
- All STM32F0 targets
- All STM32F1 targets
- All STM32F3 targets
- All STM32F4 targets
- All STM32F7 targets

There are more platforms which we have prepared, but currently not finished support for (Xmega, STM32F2, STM32L).
[Drop us an email][mailing_list] to ask if your specific target is supported out-of-the-box and what you can do if it's not.

## Documentation

The project homepage is located at [xpcc.io](http://xpcc.io).  
Complete installation instructions for our build system and toolchains can [be found here][install].

The API documentation is created from doxygen files in the doc-folder and can be found [online here](http://xpcc.io/api/modules.html).  
If you want to have up-to-date documentation of the develop branch, execute `scons doc`
in the xpcc root directory and the current development documentation will be available
in `xpcc/doc/build/api/index.html`.

xpcc also has [a blog for in-depth write ups][blog] of the programming techniques used.

## How can I contribute?

The easiest way for you and the best way for us to see if something is unclear or missing, is if you use the library and [give us some feedback](http://mailman.rwth-aachen.de/mailman/listinfo/xpcc-dev).

You may of course [file a bug report](https://github.com/roboterclubaachen/xpcc/issues) or if you have a fix already, [open a pull request](https://github.com/roboterclubaachen/xpcc/pulls).

See [CONTRIBUTING.md](CONTRIBUTING.md) for our contributing guidelines.

## Who we are

During the last decade the [Roboterclub Aachen e.V.][rca_ev] has developed a software library for communication among components that are distributed on PCs and microcontrollers. This library was used in autonomous robots for the [Eurobot competition][eurobot].

In 2009, xpcc became a separate project and since then focussed on a new approach to cross target microcontroller libraries. Over the years xpcc grew from a communication library to a general purpose framework suitable for all kinds of embedded applications.

The xpcc project is maintained by
Niklas Hauser ([@salkinium](https://github.com/salkinium)) with significant contributions from
Sascha Schade ([@strongly-typed](https://github.com/strongly-typed)),
Kevin Läufer ([@ekiwi](https://github.com/ekiwi)),
Fabian Greif ([@dergraaf](https://github.com/dergraaf)),
Martin Rosekeit ([@thundernail](https://github.com/thundernail)),
Daniel Krebs ([@daniel-k](https://github.com/daniel-k)) and
Georgi Grinshpun ([@georgi-g](https://github.com/georgi-g)).

## Folder structure

<dl>
<dt>build</dt>
<dd>
  Temporary folder, build files created during building the unit tests etc.
  go in here. Can be deleted to save disk space.
</dd>

<dt>doc</dt>
<dd>
  Documentation, mainly the doxygen documentation. The online documentation
  is build by running `doxygen doxyfile` in this folder or `scons doc` in
  the root folder.
</dd>

<dt>examples</dt>
<dd>
  Example projects that show the usage of parts of the xpcc library. These
  projects are always up to date and are tested to compile before a
  release.
</dd>

<dt>release</dt>
<dd>
  Among other things some test programs which are run before a release to
  check that everything is compiling correctly.
</dd>

<dt>scons</dt>
<dd>
  SCons tools (e.g. xpcc.py for the build process, avr.py for compiling for
  AVR microcontrollers, arm.py for compiling for Cortex-M3/4, etc.)

  Look at the SConstruct files in the example folder and the online
  documentation to see how to use the provided scons tools.
</dd>

<dt>src</dt>
<dd>
  Source files for the xpcc library
</dd>

<dt>templates</dt>
<dd>
  Template files for xpcc, Unit-Tests etc.
</dd>

<dt>tools</dt>
<dd>
  Supporting tools for the usage of the library.

  For example the communication builder, used to generate header files for the
  communication architecture from XML-files.
</dd>
</dl>

[prs]: https://github.com/roboterclubaachen/xpcc/pulls
[contributing]: https://github.com/roboterclubaachen/xpcc/tree/develop/CONTRIBUTING.md
[porting]: https://github.com/roboterclubaachen/xpcc/blob/develop/PORTING.md
[issues]: https://github.com/roboterclubaachen/xpcc/issues
[releases]: https://github.com/roboterclubaachen/xpcc/releases
[rca_ev]: http://www.roboterclub.rwth-aachen.de/
[eurobot]: http://www.eurobot.org/
[travis_ci]: https://travis-ci.org/roboterclubaachen/xpcc
[testing]: http://xpcc.io/guide/testing
[mailing_archive]: https://www.mail-archive.com/xpcc-dev@lists.rwth-aachen.de
[examples]: https://github.com/roboterclubaachen/xpcc/tree/develop/examples
[mailing_list]: http://mailman.rwth-aachen.de/mailman/listinfo/xpcc-dev
[guide]: http://xpcc.io/guide/getting-started
[install]: http://xpcc.io/installation
[reference]: http://xpcc.io/reference/api
[blog]: http://blog.xpcc.io
[rca]: http://www.roboterclub.rwth-aachen.de

[Arduino Uno]: https://www.arduino.cc/en/Main/ArduinoBoardUno
[Nucleo F031K6]: http://www.st.com/en/evaluation-tools/nucleo-f031k6.html
[STM32F072 Discovery]: http://www.st.com/en/evaluation-tools/32f072bdiscovery.html
[STM32F1 Discovery]: http://www.st.com/en/evaluation-tools/stm32vldiscovery.html
[Nucleo F103RB]: http://www.st.com/en/evaluation-tools/nucleo-f103rb.html
[STM32F1 Blue Pill]: http://wiki.stm32duino.com/index.php?title=Blue_Pill
[STM32F3 Discovery]: http://www.st.com/en/evaluation-tools/stm32f3discovery.html
[Nucleo F303K8]: http://www.st.com/en/evaluation-tools/nucleo-f303k8.html
[STM32F4 Discovery]: http://www.st.com/en/evaluation-tools/stm32f4discovery.html
[Nucleo F401RE]: http://www.st.com/en/evaluation-tools/nucleo-f401re.html
[Nucleo F411RE]: http://www.st.com/en/evaluation-tools/nucleo-f411re.html
[STM32F429 Discovery]: http://www.st.com/en/evaluation-tools/32f429idiscovery.html
[Nucleo F429ZI]: http://www.st.com/en/evaluation-tools/nucleo-f429zi.html
[STM32F469 Discovery]: http://www.st.com/en/evaluation-tools/32f469idiscovery.html
[STM32F746 Discovery]: http://www.st.com/en/evaluation-tools/32f746gdiscovery.html
[STM32F769 Discovery]: http://www.st.com/en/evaluation-tools/32f769idiscovery.html
[LPCxpresso]: https://www.lpcware.com/LPCXpressoV1Boards
[STM32L476 Discovery]: http://www.st.com/en/evaluation-tools/32l476gdiscovery.html
[Nucleo L476RG]: http://www.st.com/en/evaluation-tools/nucleo-l476rg.html

