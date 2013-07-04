STM32 Alternate Function GPIO Driver
====================================

Targets
-------
* ATmega*

Device Specific Data
--------------------
In your device file description you need to specify the devices
GPIOs like this:

::
	<gpio port="A" id="0" analog="Adc0,Adc1,Adc2">
		<!-- This makes the GPIO connect to all peripheral drivers
			 of a certain kind. For example Uart as well as UartHAL
			 driver for the stm32 platform are both of the type uart
			 and thus use the same alternate function -->
		<af id="1" peripheral-type="timer" peripheral-id="2"/>
		<af id="2" peripheral-type="timer" peripheral-id="5"/>
		<af id="3" peripheral-type="timer" peripheral-id="8"/>
		<!-- According to the development summit 2012 the driver
			 directory is call uard thus the type will always be
			 called uart and never usart! -->
		<af id="7" peripheral-type="uart" peripheral-id="2"/>
		<af id="8" peripheral-type="uart" peripheral-id="4"/>
	</gpio>
	<!-- PD5 is only available on devices with 90 pins or more -->
	<gpio port="D" id="5" pin-count="90+"></gpio>

Use the *pin-count* atrribute to specify on which devices this gpio is
available


Changelog
---------
