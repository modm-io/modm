# Testing xpcc

xpcc has seen some significant changes in the past time and the level of
maturity of xpcc varies between `uncompilable` and `very reliable`.

There are different explicit and implicit test strategies in place for
testing xpcc:

1.	Manual Tests
1.	Manual Integration Tests
1.	Continous Integration
1.	Release Tests
1.	Unit Tests on hosted
1.	Unit Tests on targets

## Manual Tests

When some new drivers were developed the developer often (not always!) tests the new driver with some real hardware (eval board) and a real peripheral at least on one platform with one compiler. The results on different platforms (AVR instead of ARM) and different hardware (eval board) may vary, although by design should not. 

Mostly, these tests are only conducted when the development takes place.

## Manual Integration Tests

xpcc is the major platform for all programming efforts of the Robot Association Aachen (Roboterclub Aachen, RCA) at RWTH Aachen University. Some parts of xpcc run on real robots in real competitions. The maturity of these parts at least for RCA's hardware can be expected to be very high. This includes

* the xpcc communication protocol
* the target platforms used in the specific season, e.g.
  * STM32F407 in seasons 2014ff, but only under very specific test conditions, e.g. 
    * external 8 MHz oscillator
    * core running at exactly 168 MHz
    * only using UART0, not UART1,
    * et cetera
* the hardware drivers build in the robots (e.g. CAN, VL6180, ...)

The test coverage is unknown but the system has proved to be reliable under conditions of the competition. Nevertheless, some unforseen problems may still occur at any time. Always fasten your seatbelt. 

## Continous Integration

xpcc on Github uses TravisCI for CI. A set of test jobs is run with every commit. These include release tests and unit tests.

## Release Test

xpcc covers many different platforms and a huge number of chips. To make sure that at the code at least compiles for all platforms a TravisCI test job is in place as part of the Continous Integration. This only tests compiling on the TravisCI Linux virtual machine. Compiling may fail with different compilers and on different hosts.

To check if simple programs compile for a large set of microcontrollers call

	$ scons check=devices
	
The test programs that are compiled are located in `xpcc/release/tests` and differ for each platform.

To compile all examples from `xpcc/examples`, run

	$ scons check=examples
	
This does **not** include actually running the examples.

To compile device checks and examples simply run

	$ scons check

## Unit Tests on Hosted

Some parts of xpcc which do not access hardware directly (like communication protocols, mathematical and logical functions) are tested with a xpcc specific unit test framework. 

Tests can be found in the `test` direcotry of each component, e.g. `xpcc/src/xpcc/math/filter/test`. These test check for logic errors in the components. They are compiled for the targed `hosted` (your computer) and run to check the expected result against a predifined and expected result. The test coverage of these tests vary.

Every new software component is expected to have these checks in place when contributed to xpcc.

Unit tests for hosted can be run with

	$ scons unittest

## Unit Tests on Target Platform

A very unique feature of the xpcc unit test framework is that the unit tests can be run on the target platform. This matters because in most cases xpcc is used for cross compiling and the target platform differs at least in one of the following features

 * compiler
   * gcc for x86 vs. arm-none-eabi-gcc vs. avr-gcc
 * word size
   * 64 bits vs. 8 or 32 bits
 * endianness
 * presence of a FPU
   * i7 vs. AVR vs. ARM Cortex-M4f

To cross-compile the set of unittests for STM32 run

	$ scons unittest target=stm32

This will create a binary `xpcc/build/unittest_stm32/executable.elf` that can be manually programmed to STM32 F4 Discovery board. After reset, the unit tests are run and the result of the test is echoed to the serial console (USART2 in case of the STM32 F4 Discovery Board, see runner.cpp for details). The test runner is created from `xpcc/templates/unittest/runner_stm32.cpp.in` and located in `xpcc/build/unittest_stm32/runner.cpp`

Theses tests have not been run for a long time at the time of writing this document. These test must be run and interpreted manually.

All platform and hardware drivers still lack these kind of tests, including external stimuli and waveform verification.

	$ scons unittest target=atmega
	$ scons unittest target=atxmega
	
## Conclusions and Outlook

xpcc has a varity of testing strategies in place, is being used on a regular basis by Roboterclub Aachen, receives updates and bug fixes frequently and test coverage is constantly improved.
