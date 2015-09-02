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


Main Features
-------------

- efficient object-oriented C++11 API:
  - new- and delete-operators
  - STL-containers
  - IO-stream interface to hardware (console, RS232, CAN)
- support of AVR and ARM Cortex-M based microcontrollers from Atmel, ST and NXP,
- build system based on SCons and Jinja2 template engine,
- cross platform peripheral interfaces (incl. bit banging):
  - GPIO, 
  - UART, I2C, SPI,
  - CAN
- write once, run anywhere external IC drivers using these interfaces,
- Debug/Logging system,
- lightweight, non-blocking workflow using timers and cooperative multitasking,
- integration of RTOS (currently only FreeRTOS and boost::thread),
- useful mathematical algorithms optimized for microcontrollers,
- lightweight unit testing system (suitable for 8-bit microcontrollers),
- graphical user interface for small screens.


Hardware support
----------------

Here is a list of supported **and tested** microcontrollers:

- AT90CAN family
- ATtiny44 family
- ATtiny85 family
- ATmega644 family
- ATmega328p family (Arduino Uno)
- ATmega1280 family (Arduino Mega)
- STM32F101 family
- STM32F100 family (STM32F1 Discovery Board)
- STM32F303 family (STM32F3 Discovery Board)
- STM32F407 family (STM32F4 Discovery Board)
- STM32F429 family (STM32F429 Discovery Board)
- STM32F401 family (STM32F401 Nucleo Board)
- LPC11C24 (LPCxpresso Board)
- LPC1115 (LPCxpresso Board)

Please see [our examples](https://github.com/roboterclubaachen/xpcc/tree/develop/examples) 
for a complete list of tested projects.

While the xpcc API is designed to be extremely portable, we are only a small team of developers and
therefore are limited in the amount of platforms we can support.
The following microcontrollers should be able to compile, but *have not been tested extensively* in hardware:

- All AT90's
- All ATtiny's
- All ATmega's

There are more platforms which we have prepared, but currently not finished support for (Xmega, STM32F{0,2}).
A complete support list of tested and/or compilable and/or potentially portable microcontrollers is available in our
[device file section](https://github.com/roboterclubaachen/xpcc/tree/develop/src/xpcc/architecture/platform/devices).

Drop us an email, if you want to get your hands dirty and help us finish the support.


Documentation
-------------

The project homepage is located at [xpcc.io](http://xpcc.io).  
Complete installation instructions for our build system and toolchains can [be found here](http://xpcc.io/install.html).

The API documentation is created from doxygen files in the doc-folder and can be found [online here](http://xpcc.io/api/).  
If you want to have up-to-date documentation of the develop branch, execute `scons doc`
in the xpcc root directory and the current development documentation will be available
in `xpcc/doc/build/api/index.html`.

xpcc also has [a blog for in-depth write ups](http://blog.xpcc.io) of the programming techniques used.


Get the Code
------------

We recommend cloning the source code using Git:

    $ git clone git://github.com/roboterclubaachen/xpcc.git xpcc
    $ cd xpcc


Use our virtual machine
-----------------------

We configured a [headless virtual machine][rca-vm] for VirtualBox (size: ~1.2GB), which
is managed using Vagrant and contains all software required for compiling xpcc.

If you do not want to install the compiler and build system tools on you computer,
or you run a OS for which these tools are not available (Windows), you can install
[VirtualBox][] and [Vagrant][] instead and use the VM like this:

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


Development Model
-----------------
The new things we are working on are created on feature branches, and as
soon as they compile, merged into the `develop` branch.
We do annual releases on our `master` branch. They contain the code that has
been tested on our robot for the past season.

So if you want to have a somewhat stable and tested codebase and you are
satisfied with the features it contains, we recommend you use one of our
releases. On the other hand, if you want to use the most recent code
(and don't mind us breaking APIs now and then), or if you want
to develop for xpcc, you should use the `develop` branch.


How can I contribute?
---------------------

Use the library and give us some feedback. That's the easiest way for you and
the best way for us to see if something is unclear or missing.

There is a low-traffic mailing list just waiting to answer your questions:
[xpcc-dev](http://mailman.rwth-aachen.de/mailman/listinfo/xpcc-dev)

If you need a specific feature just drop us a mail and we will look into it.

If you want to contribute to the library be sure to read the developer section,
to familiarize yourself with
[the](https://github.com/roboterclubaachen/xpcc-doc/blob/master/source/developer/coding_conventions.rst)
[coding](https://github.com/roboterclubaachen/xpcc-doc/blob/master/source/developer/coding_conventions.cpp)
[convention](https://github.com/roboterclubaachen/xpcc-doc/blob/master/source/developer/coding_conventions.hpp),
[design decisions](https://github.com/roboterclubaachen/xpcc-doc/blob/master/source/developer/design_decisions.rst)
etc.
The source code is freely available, so feel free to adapt it to your needs.
The only thing we ask you to do is to contribute your changes back.
That way everybody can profit from it.

Have fun!


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
