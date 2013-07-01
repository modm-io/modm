// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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
	namespace gpio
	{
		/**
		 * \brief	Dummy implementation of an I/O pin
		 * 
		 * This class can be used when a pin is not required. All functions
		 * are dummy functions which do nothing. Unused::read() will always
		 * return \c false.
		 * 
		 * For example when
		 * creating a software SPI with the xpcc::SoftwareSpi class and
		 * the return channel (MISO - Master In Slave Out) is not needed, a
		 * good way is to use this class as a parameter when defining the
		 * SPI class.
		 * 
		 * Example:
		 * \code
		 * #include <xpcc/architecture/driver/gpio.hpp>
		 * #include <xpcc/driver/software_spi.hpp>
		 * 
		 * namespace pin
		 * {
		 *     GPIO__OUTPUT(Clk, D, 7);
		 *     GPIO__OUTPUT(Mosi, D, 5);
		 * }
		 * 
		 * SoftwareSpi< pin::Clk, pin::Mosi, xpcc::gpio::Unused > Spi;
		 * 
		 * ...
		 * Spi::write(0xaa);
		 * \endcode
		 * 
		 * \ingroup	gpio
		 */
		class Unused
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
		 * #include <xpcc/architecture/driver/gpio.hpp>
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
		template <typename Pin>
		class Invert
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
		template <typename T3 = Unused,
				  typename T2 = Unused,
				  typename T1 = Unused,
				  typename T0 = Unused>
		class Nibble
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
			
			/**
			 * \brief	Read data
			 * 
			 * Only the lower four bits are used, the higher four bits will
			 * always be zero.
			 */
			static uint8_t
			read()
			{
				uint8_t value = 0;
				
				if (T3::read()) { value |= 0x08; }
				if (T2::read()) { value |= 0x04; }
				if (T1::read()) { value |= 0x02; }
				if (T0::read()) { value |= 0x01; }
				
				return value;
			}
			
			/**
			 * \brief	Write data
			 * 
			 * Only the lower four bits are used.
			 */
			static void
			write(uint8_t data)
			{
				if (data & 0x08) { T3::set(); } else { T3::reset(); }
				if (data & 0x04) { T2::set(); } else { T2::reset(); }
				if (data & 0x02) { T1::set(); } else { T1::reset(); }
				if (data & 0x01) { T0::set(); } else { T0::reset(); }
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
		 * \ingroup	gpio
		 */
		template <typename T7 = Unused,
				  typename T6 = Unused,
				  typename T5 = Unused,
				  typename T4 = Unused,
				  typename T3 = Unused,
				  typename T2 = Unused,
				  typename T1 = Unused,
				  typename T0 = Unused>
		class Port
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
				
				if (T7::read()) { value |= 0x80; }
				if (T6::read()) { value |= 0x40; }
				if (T5::read()) { value |= 0x20; }
				if (T4::read()) { value |= 0x10; }
				if (T3::read()) { value |= 0x08; }
				if (T2::read()) { value |= 0x04; }
				if (T1::read()) { value |= 0x02; }
				if (T0::read()) { value |= 0x01; }
				
				return value;
			}
			
			static void
			write(uint8_t data)
			{
				if (data & 0x80) { T7::set(); } else { T7::reset(); }
				if (data & 0x40) { T6::set(); } else { T6::reset(); }
				if (data & 0x20) { T5::set(); } else { T5::reset(); }
				if (data & 0x10) { T4::set(); } else { T4::reset(); }
				if (data & 0x08) { T3::set(); } else { T3::reset(); }
				if (data & 0x04) { T2::set(); } else { T2::reset(); }
				if (data & 0x02) { T1::set(); } else { T1::reset(); }
				if (data & 0x01) { T0::set(); } else { T0::reset(); }
			}
		};
	}
}

#endif // XPCC__GPIO_HPP
