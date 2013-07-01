// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__GPIO_HPP
#define XPCC__GPIO_HPP

#include <stdint.h>
#include <xpcc/architecture/utils.hpp>

/**
 * \ingroup		peripheral
 * \defgroup	gpio	General purpose input and/or output pins (GPIO)
 * 
 * These classes are used to create architecture independent
 * definitions for hardware pins which then can be used as template
 * parameters for miscellaneous device drivers.
 * 
 * \warning	When you use the pins directly outside any device driver class you
 * 			have to remember to initialize them first. Call configure(),
 * 			setInput() or setOutput() before the first use, otherwise the
 * 			result is undefined and most likely not what you expect!
 * 
 * Example:
 * \code
 * #include <xpcc/architecture/platform.hpp>
 * #include <xpcc/driver/software_spi.hpp>
 * #include <xpcc/driver/lcd/st7036.hpp>
 * 
 * GPIO__OUTPUT(Clk, D, 7);
 * GPIO__OUTPUT(Mosi, D, 5);
 *     
 * GPIO__OUTPUT(LcdCs, D, 4);
 * GPIO__OUTPUT(LcdRs, D, 0);
 * 
 * // Create a new type for the software SPI for the display 
 * typedef xpcc::SoftwareSpi< Clk, Mosi, xpcc::gpio::Unused > SpiInterface;
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
 * \endcode
 * 
 * This example can be compiled for every AVR device without any change!
 * 
 * The pins don't need to initialized here because the initialize()
 * method of the display does this for us.
 * 
 * \see xpcc::St7036
 * 
 * Creating a simple flashing light:
 * \code
 * #include <xpcc/architecture/platform.hpp>
 * #include <xpcc/architecture/driver/delay.hpp>
 * 
 * GPIO__OUTPUT(Led, B, 0);
 * 
 * int
 * main(void)
 * {
 *     Led::setOutput();
 *     Led::set();
 *     
 *     while (1)
 *     {
 *         Led::toggle();
 *         xpcc::delay_ms(500);
 *     }
 * }
 * \endcode
 * 
 * This will generate nearly optimal code. As all methods are \c static and
 * \c inline no function call is generated but the call is mapped directly
 * to a \c sbi assembler instruction!
 * 
 * The generated code for the example above (ATmega8):
 * \verbatim
 ...
82: 20 9a          sbi  0x04, 0        ; Led::setOutput();
84: 28 9a          sbi  0x05, 0        ; Led::set();

86: 41 e0          ldi  r20, 0x01      ; Prelude for toggle and delay
88: 29 e9          ldi  r18, 0x99
8a: 35 e1          ldi  r19, 0x15

8c: 85 b1          in   r24, 0x05      ; Led::toggle();
8e: 84 27          eor  r24, r20
90: 85 b9          out  0x05, r24

92: c9 01          movw r24, r18       ; xpcc::delay_ms(500);
94: 01 97          sbiw r24, 0x01
96: f1 f7          brne .-4            ; 0x94 <main+0x12>

98: f9 cf          rjmp .-14           ; 0x8c <main+0xa>
\endverbatim
 * 
 * Or for the ATxmega128a1:
 * \verbatim
 ...
22e: 81 e0         ldi  r24, 0x01
230: 80 93 21 06   sts  0x0621, r24    ; Led::setOutput();
234: 80 93 25 06   sts  0x0625, r24    ; Led::set();

238: 41 e0         ldi  r20, 0x01      ; Prelude for toggle and delay
23a: 20 ee         ldi  r18, 0xE0
23c: 3e e2         ldi  r19, 0x2E

23e: 40 93 27 06   sts  0x0627, r20    ; Led::toggle();

242: c9 01         movw r24, r18       ; xpcc::delay_ms(500);
244: 01 97         sbiw r24, 0x01
246: f1 f7         brne .-4            ; 0x244 <main+0x16>

248: fa cf         rjmp .-12           ; 0x23e <main+0x10>
\endverbatim
 * 
 * As you can see, no function call whatsoever is involved!
 * 
 * \see		driver
 * \author	Fabian Greif
 */

namespace xpcc
{
	struct Gpio
	{
		static const bool LOW = false;
		static const bool HIGH = true;
	};

	/**
	 * \brief	Input interface of an I/O pin
	 *
	 * Even if the wrapped pin can only be used as input it isn't configured that
	 * way from the beginning. So remember to call \b setInput() after startup!
	 *
	 * \ingroup	gpio
	 */
	class GpioInput
	{
	public:
		/// configure pin as input
		static void
		setInput();

		/// read input
		static bool
		read();
	};

	/**
	 * \brief	Output interface of an I/O pin
	 *
	 * Even if the pin can only be used as output it isn't configured that
	 * way from the beginning. So remember to call \b setOutput() after startup!
	 *
	 * \ingroup	gpio
	 */
	class GpioOutput
	{
	public:
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
	};

	/**
	 * \brief	Input/output interface of a set of 4 I/O pins
	 *
	 * This class provides the interface for 4 parallel IOs.
	 * Be aware that the interface is the same regardless of the actual
	 * mapping of the IOs.
	 * For example, when a high nibble (0xf0) is physically configured, the
	 * data will still be written and read as a low nibble (0x0f).
	 *
	 * \ingroup	gpio
	 */
	class GpioNibble
	{
	public:
		static void
		setOutput();

		static void
		setInput();

		/**
		 * \brief	Read data
		 * 
		 * Only the lower four bits are used, the higher four bits will
		 * always be zero.
		 */
		static uint8_t
		read();
		
		/**
		 * \brief	Write data
		 * 
		 * Only the lower four bits are written. The upper bits of data will
		 * be discarded.
		 */
		static void
		write(uint8_t data);
	};

	/**
	 * \brief	Input/output interface of a set of 8 I/O pins
	 *
	 * This class provides the interface for 8 parallel IOs.
	 *
	 * \ingroup	gpio
	 */
	class GpioOctet
	{
	public:
		static void
		setOutput();

		static void
		setInput();

		static uint8_t
		read();

		static void
		write(uint8_t data);
	};

	/**
	 * \brief	Input/output interface of a set of 16 I/O pins
	 *
	 * This class provides the interface for 16 parallel IOs.
	 *
	 * \ingroup	gpio
	 */
	class GpioWord
	{
	public:
		static void
		setOutput();

		static void
		setInput();

		static uint16_t
		read();

		static void
		write(uint16_t data);

	};
}

#endif // XPCC__GPIO_HPP
