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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__GPIO_HPP
#define XPCC__GPIO_HPP

#include <stdint.h>

#include <xpcc/utils/macros.hpp>

/**
 * \ingroup		architecture
 * \defgroup	gpio	General purpose input and/or output pins
 */

namespace xpcc
{
	/**
	 * \ingroup	gpio
	 * \brief	General purpose input and/or output pins
	 */
	namespace gpio
	{
		/**
		 * \ingroup	gpio
		 */
		typedef enum
		{
			INPUT,
			OUTPUT,
		} Mode;
		
		/**
		 * \brief	Dummy implementation of an I/O pin
		 * 
		 * \ingroup	gpio
		 * \author	Fabian Greif
		 */
		class Unused
		{
		public:
			ALWAYS_INLINE static void
			setOutput()
			{
			}
			
			ALWAYS_INLINE static void
			setInput()
			{
			}
			
			ALWAYS_INLINE static void
			high()
			{
			}
			
			ALWAYS_INLINE static void
			low()
			{
			}
			
			ALWAYS_INLINE static void
			toggle()
			{
			}
			
			ALWAYS_INLINE static bool
			read()
			{
				return false;
			}
			
			ALWAYS_INLINE static void
			set(bool status)
			{
				(void) status;
			}
		};
		
		/**
		 * \brief	Generic implementation of a Nibble composed of four
		 * 			independent pins.
		 * 
		 * Generally speaking the GPIO__NIBBLE_LOW/HIGH macros should be
		 * prefered because they are faster and require less code.
		 * 
		 * \see		GPIO__NIBBLE_LOW(), GPIO__NIBBLE_HIGH()
		 * \ingroup	gpio
		 * \author	Fabian Greif
		 */
		template <typename T3 = Unused,
				  typename T2 = Unused,
				  typename T1 = Unused,
				  typename T0 = Unused>
		class Nibble
		{
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
			 * Only the lower four bits are used, the higher will always
			 * be zero.
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
				if (data & 0x08) { T3::high(); } else { T3::low(); }
				if (data & 0x04) { T2::high(); } else { T2::low(); }
				if (data & 0x02) { T1::high(); } else { T1::low(); }
				if (data & 0x01) { T0::high(); } else { T0::low(); }
			}
		};
		
		/**
		 * 
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
			ALWAYS_INLINE static void
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
			
			ALWAYS_INLINE static void
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
				if (data & 0x80) { T7::high(); } else { T7::low(); }
				if (data & 0x40) { T6::high(); } else { T6::low(); }
				if (data & 0x20) { T5::high(); } else { T5::low(); }
				if (data & 0x10) { T4::high(); } else { T4::low(); }
				if (data & 0x08) { T3::high(); } else { T3::low(); }
				if (data & 0x04) { T2::high(); } else { T2::low(); }
				if (data & 0x02) { T1::high(); } else { T1::low(); }
				if (data & 0x01) { T0::high(); } else { T0::low(); }
			}
		};
	}
}

#if defined(__AVR__)
	#if defined(__AVR_XMEGA__)
		#include <xpcc/architecture/atxmega/gpio.hpp>
	#else
		#include <xpcc/architecture/atmega/gpio.hpp>
	#endif
#else
	#error "No definitions for this target available!"
#endif

#endif // XPCC__GPIO_HPP
