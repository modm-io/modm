[![Build Status](https://travis-ci.org/roboterclubaachen/xpcc.svg?branch=develop)](https://travis-ci.org/roboterclubaachen/xpcc)

xpcc: C++ microcontroller framework
===================================

The xpcc framework consists of powerful hardware abstraction layers for many
different microcontrollers, a set of drivers for various external chips,
a communication library for transparent communication over CAN, TCP/IP and TIPC
and a general purpose toolbox for building hardware orientated applications.

The main goal of xpcc is to provide a simple API for barebone microcontroller programming,
which is efficient enough to be deployed on a small ATtiny, yet powerful enough to make
use of advanced capabilities found on the 32bit ARM Cortex-M.

TL;DR:
- [This project has a homepage](http://xpcc.io) with [install instructions](http://xpcc.io/install.html) and [a technical blog](http://blog.xpcc.io).
- [Feast your eyes on lots of working examples](examples).
- [API documentation is available too](http://xpcc.io/api/modules.html).
- [We have Continuous Integration as well](https://travis-ci.org/roboterclubaachen/xpcc).
- You have questions? [Ask them on our mailing list](http://mailman.rwth-aachen.de/mailman/listinfo/xpcc-dev)
or [have a look at the archive](http://blog.gmane.org/gmane.comp.hardware.arm.cortex.xpcc.devel).
- You found a bug? [Open up an issue, we don't bite](https://github.com/roboterclubaachen/xpcc/issues).
- You want to contribute? [Read the contribution guidelines](CONTRIBUTING.md) and [open a pull request so we can merge it](https://github.com/roboterclubaachen/xpcc/pulls).

The source code is freely available, so adapt it to your needs.
The only thing we ask of you is to contribute your changes back.
That way everyone can profit from it.

Have fun!

Main features
-------------

- efficient object-oriented C++11 API:
  - new- and delete-operators
  - STL-containers
  - IO-stream interface to hardware (console, RS232, CAN)
- support of AVR and ARM Cortex-M based microcontrollers from Atmel, ST and NXP,
- build system based on SCons and Jinja2 template engine,
- cross platform peripheral interfaces (incl. bit banging):
  - GPIO,
  - ADC,
  - UART, I2C, SPI,
  - CAN
- write once, run anywhere external IC drivers using these interfaces,
- Debug/Logging system,
- lightweight, non-blocking workflow using timers and cooperative multitasking  with protothreads and resumable functions,
- integration of RTOS (currently only FreeRTOS and boost::thread),
- useful mathematical and geometric algorithms optimized for microcontrollers,
- lightweight unit testing system (suitable for 8-bit microcontrollers),
- graphical user interface for small screens.


Hardware support
----------------

Here is a list of supported **and tested** microcontrollers (with development boards):

- AT90CAN
- ATtiny44, ATtiny85
- ATmega16, ATmega164, ATmega644
- ATmega328p (Arduino Uno)
- ATmega1280 (Arduino Mega)
- STM32F072 (STM32F072 Discovery Board)
- STM32F100 (STM32F1 Discovery Board)
- STM32F103 (STM32F103 Nucleo Board)
- STM32F303 (STM32F3 Discovery Board)
- STM32F407 (STM32F4 Discovery Board)
- STM32F429 (STM32F429 Discovery Board)
- STM32F469 (STM32F469 Discovery Board)
- LPC11C24 (LPCxpresso Board)
- LPC1115 (LPCxpresso Board)

Please see [our examples for a complete list](examples) of tested projects.

While the xpcc API is designed to be extremely portable, we are only a small team of developers and are limited in the amount of platforms we can support and test in hardware.
The following microcontrollers should be able to compile, but *have not been tested extensively* in hardware:

- All AT90 chips
- All ATtiny chips
- All ATmega chips
- All STM32F3 chips
- All STM32F4 chips

There are more platforms which we have prepared, but currently not finished support for (Xmega, STM32F0, STM32F1, STM32F2, STM32F7).

[Drop us an email](http://mailman.rwth-aachen.de/mailman/listinfo/xpcc-dev), if you want to get your hands dirty and help us finish the support.

Get the code
------------

We recommend cloning the source code using Git:

    $ git clone git://github.com/roboterclubaachen/xpcc.git xpcc
    $ cd xpcc


Use our virtual machine
-----------------------

If you do not want to install the compiler and build system tools on you computer,
or you run a OS for which these tools are not available (Windows), use our [headless virtual machine][rca-vm] (size: ~1.2GB) for [VirtualBox][] instead.
It is managed using [Vagrant][] and contains all software required for compiling xpcc:

    $ cd xpcc
    $ vagrant up
    $ vagrant ssh

You may then compile examples or your own code using VBs shared folders:

    $ cd /vagrant
    $ cd examples/arduino_uno/basic/blink
    $ scons

If you want to program the examples onto hardware, don't forget to forward your USB
port to the virtual machine in the "Ports" settings tab of your virtual machine in Virtualbox.
Depending on your programmer, you might need to install the [Extension Pack](virtualbox).

Once your programmer is connected to the virtual machine, you may simply program your board using:

	$ scons program


Documentation
-------------

The project homepage is located at [xpcc.io](http://xpcc.io).  
Complete installation instructions for our build system and toolchains can [be found here](http://xpcc.io/install.html).

The API documentation is created from doxygen files in the doc-folder and can be found [online here](http://xpcc.io/api/).  
If you want to have up-to-date documentation of the develop branch, execute `scons doc`
in the xpcc root directory and the current development documentation will be available
in `xpcc/doc/build/api/index.html`.

xpcc also has [a blog for in-depth write ups](http://blog.xpcc.io) of the programming techniques used.

How can I contribute?
---------------------

The easiest way for you and the best way for us to see if something is unclear or missing, is if you use the library and [give us some feedback](http://mailman.rwth-aachen.de/mailman/listinfo/xpcc-dev).

You may of course [file a bug report](https://github.com/roboterclubaachen/xpcc/issues) or if you have a fix already, [open a pull request](https://github.com/roboterclubaachen/xpcc/pulls).

See [CONTRIBUTING.md](CONTRIBUTING.md) for our contributing guidelines.

Folder structure
----------------

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

<dt>tests</dt>
<dd>
  Test programs, used during the development of the library. May be
  out of date or not working at all.
</dd>

<dt>tools</dt>
<dd>
  Supporting tools for the usage of the library.

  For example the communication builder, used to generate header files for the
  communication architecture from XML-files.
</dd>
</dl>



[virtualbox]: https://www.virtualbox.org/wiki/Downloads
[vagrant]: http://www.vagrantup.com/downloads.html
[rca-vm]: https://github.com/roboterclubaachen/rca-vm
