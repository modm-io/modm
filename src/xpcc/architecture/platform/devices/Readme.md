# Device Files

The device file describes the device metadata and peripheral tree in XML.
Information from one device can be extracted by providing the device identifier.

## Device Identifier

The device identifier is the canonical representation of the device name within the device file.
It consists out of the following elements with examples for device names *stm32f407vg* and *atmega328p*:

| Element  | STM32 | AVR    |
|----------|-------|--------|
| platform | stm32 | avr    |
| family   | f4    | atmega |
| name     | 407   | 328    |
| type     |       | p      |
| pin-id   | v     |        |
| size-id  | g     | 32     |

Notice that type and pin-id have different interpretations on the STM32 and AVR platform, which reflects in the device file format.

## Naming Scheme

All device files can be currently found in the `xpcc/architecture/platform/xml/`, with the naming schemes dependend on architecture.
Examples include:

- avr:
	- AT90: `at90` + `sizes-type`
		- `at9032_64_128-can.xml` for **AT90CAN(32|64|128)**
	- ATtiny, ATmega: `atmega` + `sizes-types`
		- `atmega48_88_168_328-a_none_p_pa.xml` for **ATMEGA(48|88|169|328)(a|p|pa)?**
	- ATxmega: `xmega` + `sizes-package-types`
		- `xmega64_128-a1-none_u.xml` for **ATXMEGA(64|128)A1(U)?**
- stm32:
	- STM32F: `stm32f` + `names-pins-sizes`
		- `stm32f405_407_415_417-i_r_v_z-e_g.xml` for **STM32F(405|407|415|417)(I|R|V|Z)(E|G)**


## File Format

The device files encode information about one or several very similar devices using a tree representation.
The device tree root declares its device scope using OR'ed device identifier elements.

Here is the *ATmega328* device family as an example:

```xml
<device platform="avr" family="atmega" name="48|88|168|328" size_id="4|8|16|32" type="a|none|p|pa">
  ...
</device>
```

This looks very similar for the *STM32F407* device family:

```xml
<device platform="stm32" family="f4" name="405|407|415|417" pin_id="i|r|v|z" size_id="e|g">
  ...
</device>
```

### Available Elements

Inside the device tag, the following elements describe the device:

| Element        | Description                                    |
|----------------|------------------------------------------------|
| flash          | size of available non-volatile memory in bytes |
| ram            | size of available volatile memory in bytes     |
| eeprom         | size of available eeprom in bytes (avr-only)   |
| linkerscript   | name of linkerscript (cortex-m-only)           |
| core           | name of CPU core                               |
| mcu            | avrdude name of device (avr-only)              |
| pin-count      | number of pins on device                       |
| header         | required device header(s)                      |
| define         | required device define(s)                      |
| driver         | a peripheral driver implementation             |

For example, all devices in the STM32F407 family have 192kB of RAM:

```xml
<ram>196608</ram>
```

However, each element may also declare its device scope by including device identifier attributes.
To distinguish filterable device identifier attributes from declaration attributes, they must be prefixed with `device-`.

For the ATmega328 family several RAM sizes exist depending on device name:

```xml
<ram device-name="48">512</ram>
<ram device-name="88|168">1024</ram>
<ram device-name="328">2048</ram>
```

You are even able to use the value of these elements to further limit the scope of new elements.

An example of this can be seen in the core driver file (discussed later), which uses the core element as device-scope:

```xml
<template core="cortex-m3|cortex-m4|cortex-m4f">hard_fault_handler.cpp.in</template>
```

As the device file is declarative, the filtering engine will resolve all dependencies recursively and throw an error in case of unresolved elements or circular referencing.

### Available Drivers

The driver elements link the device hardware peripheral with a software implementation, which are found in `xpcc/architecture/platform/driver/`.
Each driver is divided into several hardware dependend implementations.

A selection of the most important drivers are:

	peripheral
	|-- adc
	|-- can
	|-- clock
	|-- fsmc
	|-- gpio
	|   |-- at90_tiny_mega
	|   |-- software
	|   `-- stm32
	|-- i2c
	|-- spi
	|   |-- at90_tiny_mega
	|   |-- at90_tiny_mega_uart
	|   |-- software
	|   |-- stm32
	|   `-- stm32_uart
	|-- timer
	`-- uart

Notice the `software` folders. These are available for all platforms and are automatically included.

Also, notice how there exist two implementations for **spi** for the **stm32** and **at90_tiny_mega** platform.
One is for using the dedicated SPI hardware, and the other is for the UART hardware in SPI mode, however, both conform to the same SPI interface:

```xml
<driver type="spi" name="stm32" instances="1,2,3"/>
<driver type="spi" name="stm32_uart" instances="1,2,3,4,5,6"/>
```

Of course, drivers may also declare device scope. For the STM32F407 family, the FSMC is not available for pin-id `R` (=64 pins):

```xml
<driver device-pin-id="i|v|z" type="fsmc" name="stm32"/>
```

Any childen of a driver element will be available inside the driver instance template. Except for the reserved name `parameter`, there are no limitations on data structure or naming.

For example, the GPIO driver needs to know which pins are available for the requested device (with the `gpio` element), and what alternate functions to connect to them (with the `af` element).

This information is encoded as driver element children (here for the ATmega328 device family):

```xml
<driver type="gpio" name="at90_tiny_mega">
  <gpio port="B" id="0" pcint="0"/>
  ...
  <gpio port="D" id="1" pcint="17">
	<af peripheral="Uart0" type="out" name="Txd"/>
  </gpio>
  <gpio port="D" id="2" pcint="18" extint="0"/>
  ...
</driver>
```

Of course, device scope may also be declared for individual pins and even alternate functions (here for the STM32F407 device family):

```xml
<driver type="gpio" name="stm32">
  ...
  <gpio device-pin-id="i|v|z" port="E" id="1">
	<af id="12" peripheral="Fsmc" name="Nbl1"/>
  </gpio>
  <gpio device-pin-id="i|v|z" port="E" id="2">
	<af id="12" peripheral="Fsmc" name="A23"/>
  </gpio>
  <gpio device-pin-id="i|v|z" port="E" id="3">
	<af id="12" peripheral="Fsmc" name="A19"/>
  </gpio>
  <gpio device-pin-id="i|v|z" port="E" id="4">
	<af id="12" peripheral="Fsmc" name="A20"/>
  </gpio>
  ...
</driver>
```

This structure is then available as a dictionary inside the device driver template files.

## Driver Files

The device file only describes which folder contains the driver implementation and metadata for its implementation.
It does not specify which files are part of the implementation, and how multiple instances of a driver are created.

This is done using the `driver.xml` file inside each driver implemenation.
For the `at90_tiny_mega` GPIO driver it contains:

```xml
<driver type="gpio" name="at90_tiny_mega">
	<static>gpio_define.h</static>
	<template>gpio.hpp.in</template>
	...
</driver>
```

Here the elements stand for:
- `<static>`: copy source file without modification,
- `<template>`: generate source file `gpio.hpp` from `gpio.hpp.in` template with the device file GPIO data

Let's look at how to generate several driver instances using a driver file.
Here is the `at90_tiny_meta` UART driver file:

```xml
<driver type="uart" name="at90_tiny_mega">
	<template instances="0,1,2,3" out="uart{{id}}.hpp">uart.hpp.in</template>
	...
</driver>
```

Here the driver file has the capability to generate the instances 0,1,2 and 3 of the UART classes, and write them into the format specified by the `out` attribute.
The `instances` attribute can be used to generate different instances out of different template files as shown in the `stm32` UART driver file:

```xml
<driver type="uart" name="stm32">
	<template instances="1,2,3,6" out="usart_{{id}}.hpp">uart.hpp.in</template>
	<template instances="4,5" out="uart_{{id}}.hpp">uart.hpp.in</template>
	...
</driver>
```

Note, that only the instances declared in the device file will actually be generated!

So for the ATmega328, only UART instance 0 will be generated, since that has been declared in the device file:

```xml
<driver type="uart" name="at90_tiny_mega" instances="0"/>
```

### Custom elements

In the driver file, custom device-scoped elements may be declared, as in the `at90_tiny_mega` GPIO driver file:

```xml
<driver type="gpio" name="at90_tiny_mega">
	...
	<notoggle device-family="atmega" device-name="8|16|32|64|128|162|8515|8535">True</notoggle>
	<notoggle device-family="attiny" device-name="26">True</notoggle>
	...
</driver>
```

Here the `notoggle` element will be added to the substitution dictionary only for the specified devices.

### Parameters

In order to customize drivers further parameters may be declared.
There are currently three types available:
- `int`: has a minimum and a maximum value
- `bool`: true/false
- `enum`: semicolon-separated values

All buffered UART implementations have two parameters named `tx_buffer` and `rx_buffer`, which are of type `int` and set the size of the atomic transmit and receive buffer.

```xml
<driver type="uart" name="at90_tiny_mega">
	...
	<parameter name="tx_buffer" type="int" min="1" max="254">8</parameter>
	<parameter name="rx_buffer" type="int" min="1" max="254">4</parameter>
</driver>
```

Each UART instance receives their own two independent parameters with these default values.

The `stm32` core driver features the other two types:

```xml
<driver type="core" name="cortex">
	<parameter name="allocator" type="enum" values="newlib;block_allocator">
		block_allocator
	</parameter>
	<parameter name="enable_gpio" type="bool">true</parameter>
	...
</driver>
```

These parameter may be overwritten in either the device file, or in the `project.cfg` in the section `[parameters]`, which follows the naming scheme `type.name.instance.parameter`:

```python
[parameters]
uart.at90_tiny_mega.0.tx_buffer = 200
uart.at90_tiny_mega.0.rx_buffer = 100
```

## Generator

We designed the device file format to use this form of XML to not only be human readable, but much more important, to be human-writeable.

We decided to group multiple devices of the same functionality family into one device file, instead of providing one device file for each of these devices.

This made accessing the devices files computationally more complex, since we have to evaluate the device-scope, however, it dramatically reduces redundancies and allows a much more natural description of devices in this language.

While the first device files were still written manually, a device file generator soon became necessary to allow inclusion of many devices and to help with refactoring during API changes.

This generator uses multiple manufacturer provided description files to extract data about multiple devices and compress them into one device file, while preserving readability and correctness.

Further information about the device file generator can be found in the Readme at `/tools/device_file_generator`.
