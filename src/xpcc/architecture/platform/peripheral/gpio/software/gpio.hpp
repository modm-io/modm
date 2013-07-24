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

#ifndef XPCC__SOFTWARE_GPIO_HPP
#define XPCC__SOFTWARE_GPIO_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/peripheral/gpio.hpp>

/**
 * 
 * \see		driver
 * \author	Fabian Greif
 * \author	Niklas Hauser
 */

namespace xpcc
{
	/**
	 * \brief	Dummy implementation of an I/O pin
	 *
	 * This class can be used when a pin is not required. All functions
	 * are dummy functions which do nothing. Unused::read() will always
	 * return \c false.
	 *
	 * For example when
	 * creating a software SPI with the xpcc::SoftwareSpiMaster class and
	 * the return channel (MISO - Master In Slave Out) is not needed, a
	 * good way is to use this class as a parameter when defining the
	 * SPI class.
	 *
	 * Example:
	 * \code
	 * #include <xpcc/architecture/platform.hpp>
	 *
	 * namespace pin
	 * {
	 *     GPIO__OUTPUT(Clk, D, 7);
	 *     GPIO__OUTPUT(Mosi, D, 5);
	 * }
	 *
	 * SoftwareSpiMaster< pin::Clk, pin::Mosi, xpcc::GpioUnused > Spi;
	 *
	 * ...
	 * Spi::write(0xaa);
	 * \endcode
	 *
	 * \ingroup	gpio
	 */
	class GpioUnused : GpioIO
	{
	public:
		ALWAYS_INLINE static void
		setOutput()
		{
		}

		ALWAYS_INLINE static void
		setOutput(bool)
		{
		}

		ALWAYS_INLINE static void
		setInput()
		{
		}

		ALWAYS_INLINE static void
		set()
		{
		}

		ALWAYS_INLINE static void
		set(bool)
		{
		}

		ALWAYS_INLINE static void
		reset()
		{
		}

		ALWAYS_INLINE static void
		toggle()
		{
		}

		/// Returns \c false
		ALWAYS_INLINE static bool
		read()
		{
			return false;
		}
	};

	/**
	 * \brief	Invert a pin
	 *
	 * This template can be used the invert the logic level of a normal
	 * pin template.
	 *
	 * Example:
	 * \code
	 * #include <xpcc/architecture/peripheral/gpio.hpp>
	 * #include <xpcc/driver/software_spi.hpp>
	 *
	 * namespace pin
	 * {
	 *     GPIO__OUTPUT(Led, B, 0);
	 * }
	 *
	 * typedef xpcc::gpio::Invert< pin::Led > Led;
	 *
	 * ...
	 * Led::reset();
	 * \endcode
	 *
	 * \ingroup	gpio
	 */
	template < class Pin >
	class GpioInverted : GpioIO
	{
	public:
		ALWAYS_INLINE static void
		setOutput()
		{
			Pin::setOutput();
		}

		ALWAYS_INLINE static void
		setOutput(bool value)
		{
			Pin::setOutput(!value);
		}

		ALWAYS_INLINE static void
		setInput()
		{
			Pin::setInput();
		}

		ALWAYS_INLINE static void
		set()
		{
			Pin::reset();
		}

		ALWAYS_INLINE static void
		set(bool value)
		{
			Pin::set(!value);
		}

		ALWAYS_INLINE static void
		reset()
		{
			Pin::set();
		}

		ALWAYS_INLINE static void
		toggle()
		{
			Pin::toggle();
		}

		ALWAYS_INLINE static bool
		read()
		{
			return !Pin::read();
		}
	};

	/**
	 * \brief	Generic implementation of a Nibble composed of four
	 * 			independent pins.
	 *
	 * When possible preferred the GPIO__NIBBLE_LOW() or GPIO__NIBBLE_HIGH()
	 * macros over this class as they are much faster and require less code.
	 *
	 * \see		GPIO__NIBBLE_LOW(), GPIO__NIBBLE_HIGH()
	 * \ingroup	gpio
	 */
	template < class T3 = GpioUnused,
			   class T2 = GpioUnused,
			   class T1 = GpioUnused,
			   class T0 = GpioUnused >
	class SoftwareGpioNibble : GpioNibble
	{
	public:
		ALWAYS_INLINE static void
		setOutput()
		{
			T3::setOutput();
			T2::setOutput();
			T1::setOutput();
			T0::setOutput();
		}

		ALWAYS_INLINE static void
		setInput()
		{
			T3::setInput();
			T2::setInput();
			T1::setInput();
			T0::setInput();
		}

		static uint8_t
		read()
		{
			uint8_t value = 0;
			
			if (T3::read()) { value |= 0b1000; }
			if (T2::read()) { value |= 0b0100; }
			if (T1::read()) { value |= 0b0010; }
			if (T0::read()) { value |= 0b0001; }
			
			return value;
		}

		static void
		write(uint8_t data)
		{
			T3::set(data & 0b1000);
			T2::set(data & 0b0100);
			T1::set(data & 0b0010);
			T0::set(data & 0b0001);
		}
	};

	/**
	 * \brief	Create a 8-bit port from arbitrary pins.
	 *
	 * Be aware that this method is slow, because for every write or read
	 * cycle, every of the eight pins have to be read/written
	 * individually.
	 *
	 * This class is included here because it can be very useful sometimes,
	 * for example when connecting a LCD where speed is not the main
	 * concern.
	 *
	 * When possible preferred the GPIO__PORT() or GPIO__OCTET()
	 * macros over this class as they are much faster and require less code.
	 *
	 * \see		GPIO__PORT(), GPIO__OCTET()
	 * \ingroup	gpio
	 */
	template <typename T7 = GpioUnused,
			  typename T6 = GpioUnused,
			  typename T5 = GpioUnused,
			  typename T4 = GpioUnused,
			  typename T3 = GpioUnused,
			  typename T2 = GpioUnused,
			  typename T1 = GpioUnused,
			  typename T0 = GpioUnused>
	class SoftwareGpioOctet : GpioOctet
	{
	public:
		static void
		setOutput()
		{
			T7::setOutput();
			T6::setOutput();
			T5::setOutput();
			T4::setOutput();
			T3::setOutput();
			T2::setOutput();
			T1::setOutput();
			T0::setOutput();
		}

		static void
		setInput()
		{
			T7::setInput();
			T6::setInput();
			T5::setInput();
			T4::setInput();
			T3::setInput();
			T2::setInput();
			T1::setInput();
			T0::setInput();
		}
		
		static uint8_t
		read()
		{
			uint8_t value = 0;
			
			if (T7::read()) { value |= 0b10000000; }
			if (T6::read()) { value |= 0b01000000; }
			if (T5::read()) { value |= 0b00100000; }
			if (T4::read()) { value |= 0b00010000; }
			if (T3::read()) { value |= 0b00001000; }
			if (T2::read()) { value |= 0b00000100; }
			if (T1::read()) { value |= 0b00000010; }
			if (T0::read()) { value |= 0b00000001; }
			
			return value;
		}
		
		static void
		write(uint8_t data)
		{
			T7::set(data & 0b10000000);
			T6::set(data & 0b01000000);
			T5::set(data & 0b00100000);
			T4::set(data & 0b00010000);
			T3::set(data & 0b00001000);
			T2::set(data & 0b00000100);
			T1::set(data & 0b00000010);
			T0::set(data & 0b00000001);
		}
	};

	/**
	 * \brief	Create a 16-bit port from arbitrary pins.
	 *
	 * Be aware that this method is slow, because for every write or read
	 * cycle, every of the sixteen (!) pins have to be read/written
	 * individually.
	 *
	 * This class is included here because it can be very useful sometimes,
	 * for example when connecting a LCD where speed is not the main
	 * concern.
	 *
	 * When possible preferred the GPIO__PORT()
	 * macros over this class as they are much faster and require less code.
	 *
	 * \see		GPIO__PORT()
	 * \ingroup	gpio
	 */
	template <typename T15 = GpioUnused,
			  typename T14 = GpioUnused,
			  typename T13 = GpioUnused,
			  typename T12 = GpioUnused,
			  typename T11 = GpioUnused,
			  typename T10 = GpioUnused,
			  typename T9  = GpioUnused,
			  typename T8  = GpioUnused,
			  typename T7  = GpioUnused,
			  typename T6  = GpioUnused,
			  typename T5  = GpioUnused,
			  typename T4  = GpioUnused,
			  typename T3  = GpioUnused,
			  typename T2  = GpioUnused,
			  typename T1  = GpioUnused,
			  typename T0  = GpioUnused>
	class SoftwareGpioWord : GpioWord
	{
	public:
		static void
		setOutput()
		{
			T15::setOutput();
			T14::setOutput();
			T13::setOutput();
			T12::setOutput();
			T11::setOutput();
			T10::setOutput();
			T9::setOutput();
			T8::setOutput();
			T7::setOutput();
			T6::setOutput();
			T5::setOutput();
			T4::setOutput();
			T3::setOutput();
			T2::setOutput();
			T1::setOutput();
			T0::setOutput();
		}

		static void
		setInput()
		{
			T15::setInput();
			T14::setInput();
			T13::setInput();
			T12::setInput();
			T11::setInput();
			T10::setInput();
			T9::setInput();
			T8::setInput();
			T7::setInput();
			T6::setInput();
			T5::setInput();
			T4::setInput();
			T3::setInput();
			T2::setInput();
			T1::setInput();
			T0::setInput();
		}

		static uint16_t
		read()
		{
			uint16_t value = 0;

			if (T15::read()) { value |= 0b1000000000000000; }
			if (T14::read()) { value |= 0b0100000000000000; }
			if (T13::read()) { value |= 0b0010000000000000; }
			if (T12::read()) { value |= 0b0001000000000000; }
			if (T11::read()) { value |= 0b0000100000000000; }
			if (T10::read()) { value |= 0b0000010000000000; }
			if (T9::read())  { value |= 0b0000001000000000; }
			if (T8::read())  { value |= 0b0000000100000000; }
			if (T7::read())  { value |= 0b0000000010000000; }
			if (T6::read())  { value |= 0b0000000001000000; }
			if (T5::read())  { value |= 0b0000000000100000; }
			if (T4::read())  { value |= 0b0000000000010000; }
			if (T3::read())  { value |= 0b0000000000001000; }
			if (T2::read())  { value |= 0b0000000000000100; }
			if (T1::read())  { value |= 0b0000000000000010; }
			if (T0::read())  { value |= 0b0000000000000001; }

			return value;
		}

		static void
		write(uint16_t data)
		{
			T15::set(data & 0b1000000000000000);
			T14::set(data & 0b0100000000000000);
			T13::set(data & 0b0010000000000000);
			T12::set(data & 0b0001000000000000);
			T11::set(data & 0b0000100000000000);
			T10::set(data & 0b0000010000000000);
			T9::set (data & 0b0000001000000000);
			T8::set (data & 0b0000000100000000);
			T7::set (data & 0b0000000010000000);
			T6::set (data & 0b0000000001000000);
			T5::set (data & 0b0000000000100000);
			T4::set (data & 0b0000000000010000);
			T3::set (data & 0b0000000000001000);
			T2::set (data & 0b0000000000000100);
			T1::set (data & 0b0000000000000010);
			T0::set (data & 0b0000000000000001);
		}
	};
}

#endif // XPCC__SOFTWARE_GPIO_HPP
