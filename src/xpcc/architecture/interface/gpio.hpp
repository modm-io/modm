/*
 * Copyright (c) 2013-2016, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GPIO_HPP
#define XPCC_GPIO_HPP

#include <stdint.h>
#include <modm/architecture/utils.hpp>

/**
 * @ingroup		interface
 * @defgroup	gpio	General purpose input and/or output pins (GPIO)
 *
 * These classes are used to create architecture independent
 * definitions for hardware pins which then can be used as template
 * parameters for miscellaneous device drivers.
 *
 * @warning	When you use the pins directly outside any device driver class you
 * 			have to remember to initialize them first. Call setInput() or
 * 			setOutput() before the first use, otherwise the result is undefined
 * 			and most likely not what you expect!
 *
 * Example:
 * @code
 * #include <modm/architecture/platform.hpp>
 * #include <modm/driver/lcd/st7036.hpp>
 *
 * typedef GpioOutputD7 Clk;
 * typedef GpioOutputD5 Mosi;
 *
 * typedef GpioOutputD4 LcdCs;
 * typedef GpioOutputD0 LcdRs;
 *
 * // Create a new type for the software SPI for the display
 * typedef xpcc::SoftwareSimpleSpi< Clk, Mosi > SpiInterface;
 *
 * // Create a instance of the ST7036 based display class
 * xpcc::St7036< SpiInterface,
 *               LcdCs,
 *               LcdRs > display;
 *
 * ...
 * display.initialize();
 * display.setPosition(0, 0);
 * display.write("Hallo Welt!");
 * @endcode
 *
 * This example can be compiled for every AVR device without any change!
 *
 * The pins don't need to initialized here because the initialize()
 * method of the display does this for us.
 *
 * @see xpcc::St7036
 *
 * Creating a simple flashing light:
 * @code
 * #include <modm/architecture/platform.hpp>
 * #include <modm/architecture/driver/delay.hpp>
 *
 * typedef GpioOutputB0 Led;
 *
 * int
 * main()
 * {
 *     Led::setOutput();
 *     Led::set();
 *
 *     while (1)
 *     {
 *         Led::toggle();
 *         xpcc::delayMilliseconds(500);
 *     }
 * }
 * @endcode
 *
 * This will generate nearly optimal code. As all methods are `static` and
 * `inline` no function call is generated but the call is mapped directly
 * to a `sbi` assembler instruction!
 *
 * The generated code for the example above (ATmega8):
 * @verbatim
 ...
82: 20 9a          sbi  0x04, 0        ; Led::setOutput();
84: 28 9a          sbi  0x05, 0        ; Led::set();

86: 41 e0          ldi  r20, 0x01      ; Prelude for toggle and delay
88: 29 e9          ldi  r18, 0x99
8a: 35 e1          ldi  r19, 0x15

8c: 85 b1          in   r24, 0x05      ; Led::toggle();
8e: 84 27          eor  r24, r20
90: 85 b9          out  0x05, r24

92: c9 01          movw r24, r18       ; xpcc::delayMilliseconds(500);
94: 01 97          sbiw r24, 0x01
96: f1 f7          brne .-4            ; 0x94 <main+0x12>

98: f9 cf          rjmp .-14           ; 0x8c <main+0xa>
@endverbatim
 *
 * Or for the ATxmega128a1:
 * @verbatim
 ...
22e: 81 e0         ldi  r24, 0x01
230: 80 93 21 06   sts  0x0621, r24    ; Led::setOutput();
234: 80 93 25 06   sts  0x0625, r24    ; Led::set();

238: 41 e0         ldi  r20, 0x01      ; Prelude for toggle and delay
23a: 20 ee         ldi  r18, 0xE0
23c: 3e e2         ldi  r19, 0x2E

23e: 40 93 27 06   sts  0x0627, r20    ; Led::toggle();

242: c9 01         movw r24, r18       ; xpcc::delayMilliseconds(500);
244: 01 97         sbiw r24, 0x01
246: f1 f7         brne .-4            ; 0x244 <main+0x16>

248: fa cf         rjmp .-12           ; 0x23e <main+0x10>
@endverbatim
 *
 * As you can see, no function call whatsoever is involved!
 *
 * @see		driver
 * @author	Fabian Greif
 * @author	Niklas Hauser
 */

namespace xpcc
{

struct Gpio
{
	/**
	 * These constants refer to the *logical* state of the GPIO.
	 * The physical state is determined by the configuration and external connection.
	 * @{
	 */
	static constexpr bool Low = false;
	static constexpr bool High = true;
	/// @}

	/// Declares the direction of a GPIO
	enum class
	Direction
	{
		In = 0,			//< GPIO is an Input
		Out = 1,		//< GPIO is an Output
		InOut = 2,		//< GPIO is both Input and Output
		Special = 3,	//< GPIO is configured with a special purpose
	};
};

/**
 * Input interface of an I/O pin.
 *
 * The pin is not automatically set as an input after startup,
 * so remember to call `setInput()` after startup!
 *
 * @ingroup	gpio
 */
class GpioInput : public Gpio
{
public:
	/// Declares the Pin direction
	static constexpr Direction direction = Direction::In;

#ifdef __DOXYGEN__
	/// configure pin as input
	static void
	setInput();

	/// read input
	static bool
	read();
#endif
};

/**
 * Output interface of an I/O pin.
 *
 * The pin is not automatically set as an output after startup,
 * so remember to call `setOutput()` after startup!
 *
 * @ingroup	gpio
 */
class GpioOutput : public Gpio
{
public:
	/// Declares the Pin direction
	static constexpr Direction direction = Direction::Out;

#ifdef __DOXYGEN__
	/// configure pin as output
	static void
	setOutput();

	/// configure pin as output and set high or low
	static void
	setOutput(bool value);

	/// set output to high level
	static void
	set();

	/// set output to high or low level
	static void
	set(bool value);

	/// set output to low level
	static void
	reset();

	/// toggle output level
	static void
	toggle();

	/// Returns the set logical output state of the pin.
	/// This may be different from the physical state!
	static bool
	isSet();
#endif
};

/**
 * Input/Output interface of an I/O pin.
 *
 * @ingroup	gpio
 */
class GpioIO : public GpioOutput, public GpioInput
{
public:
	/// Declares the Pin direction
	static constexpr Direction direction = Direction::InOut;

#ifdef __DOXYGEN__
	/// returns the pin direction at run-time
	static Direction
	getDirection();
#endif
};

/**
 * Input/output interface of a set of up to 16 I/O pins.
 *
 * This class provides the interface for up to 16 parallel IOs.
 * Be aware that the interface is the same regardless of the actual
 * mapping of the IOs.
 * For example, when a high octet `0xf400` is physically configured, the
 * data will still be written and read as a low octet `0x00f4`.
 *
 * @note `DataOrder` is used to specify the bit order of the write and read data.
 *       When set to Reversed the bit order of `data` is reversed whenever a write
 *       or read to/from the port occurs. So a physical configuration of `0b11100100`
 *       returns `0b00100111` when read, or data `0xe4` is written as a high octet
 *       `0x2700`.
 *
 * @note `DataOrder` is a template Parameter
 * @ingroup	gpio
 */
class GpioPort
{
public:
	/// This enum can be used to reverse the bit order of the Data.
	enum class
	DataOrder : bool
	{
		Normal = false, ///< bit order of data is the same as bit order of port
		Reversed = true ///< bit order of data is the reverse bit order of port
	};
#ifdef __DOXYGEN__
	/// This holds the width of the port as number of bits
	/// and can be used in drivers to assert the correct requirements
	static constexpr uint8_t width;

	/// may also be uint8_t, depending on port width
	using PortType = uint16_t;

	/// returns the chosen data order
	static constexpr DataOrder
	getDataOrder();

public:
	static void
	setOutput();

	static void
	setInput();

	/// The read data is always right-aligned regardless of physical mapping.
	static PortType
	read();

	/// The write data is always right-aligned regardless of physical mapping.
	static void
	write(PortType data);

	/// Toggle the output states of all pins
	static void
	toggle();
#endif
};

} // namespace xpcc

#endif // XPCC__GPIO_HPP
