STM32 Alternate Function GPIO Driver
====================================

Targets
-------
* STM32F2
* STM32F3
* STM32F4

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


GPIO Ports
----------

Each port bit of the General Purpose IO (GPIO) Ports, can be
individually configured by software in several modes:
- Input floating
- Input (with pull-up or pull-down)
- Analog
- Output open-drain (with pull-up or pull-down)
- Output push-pull (with pull-up or pull-down)
- Alternate function push-pull (with pull-up or pull-down)
- Alternate function open-drain (with pull-up or pull-down)

GPIOx_MODER (32-bit)
  00: Input
  01: General purpose output mode
  10: Alternate function mode
  11: Analog mode
GPIOx_OTYPER (16-bit)
  0: Output push-pull
  1: Output open-drain
GPIOx_OSPEEDR (32-bit)
  00: 2 MHz
  01: 25 MHz
  10: 50 MHz
  11: 100 MHz
GPIOx_PUPDR (32-bit)
  00: No pull-up, pull-down
  01: Pull-up
  10: Pull-down
  11: Reserved

GPIOx_IDR = input state (read-only) (16-bit)
GPIOx_ODR = output state (16-bit)

GPIOx_BSRR = set and reset register (32-bit)
- BSRR(i) sets
- BSRR(i+16) resets

How To Use
----------
	GPIO__INPUT(Button, A, 0);

	Button::setInput();
	Button::setInput(xpcc::stm32::PULLUP);
	Button::setInput(xpcc::stm32::PULLDOWN);
	Button::setAnalogInput();
	Button::setAlternateFunction();
	Button::setAlternateFunction(xpcc::stm32::PULLDOWN);
	if (Button::read()) {
	    ...
	}



	GPIO__OUTPUT(Led, C, 12);

	Led::setOutput();
	Led::setOutput(xpcc::stm32::PUSH_PULL);
	Led::setOutput(xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_10MHZ);
	Led::setOutput(xpcc::stm32::OPEN_DRAIN);

	Led::setAlternateFunction(); 
	Led::setAlternateFunction(xpcc::stm32::PUSH_PULL); 
	Led::setAlternateFunction(xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ); 
	Led::setAlternateFunction(xpcc::stm32::OPEN_DRAIN);

	Led::set();
	Led::reset();

	Led::toggle();


Changelog
---------
