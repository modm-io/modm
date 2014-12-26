STM32 Timer Driver
==================


Targets
-------
* STM32F1
* STM32F2
* STM32F3
* STM32F4
* possibly other targets from the STM32 platform

Device Specific Data
--------------------
You need to specify which timer instances are available on your particular
device like so:

::
	<driver type="timer" name="stm32" instances="1,2,3,4,5,6,7,8" />

If some the availability depends for example on the package size, something
like this is possible:

::
	<driver type="timer" name="stm32" instances="1,2,3,4,5,6" />
	<driver type="timer" name="stm32" pin-count="90+" instances="7,8" />

Which would say that Timer7 and Timer8 are only available on devices that have
90 or more pins. (This is only an example and not actually true...)


Changelog
---------


Internal connection
-------------------
APB2: Timer 1, 8-11
APB1: Timer 2-7, 12-14

Timer types
-----------
Basic Timers: 6, 7
General Purpose Timers (1 Compare Channel): 10, 11, 13, 14
General Purpose Timers (2 Compare Channels): 9, 12
General Purpose Timers (Encoder + 4 Compare Channels): 2, 3, 4, 5
Advanced Timers: 1, 8
