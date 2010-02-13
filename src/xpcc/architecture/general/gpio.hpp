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
			set()
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
			
			ALWAYS_INLINE static bool
			read()
			{
				return false;
			}
			
			ALWAYS_INLINE static void
			write(bool status)
			{
				(void) status;
			}
		};
		
		/**
		 * \brief	Generic implementation of a Nibble composed of four
		 * 			independent pins.
		 * 
		 * Generally speaking the CREATE_LOW/HIGH_NIBBLE macros should be
		 * prefered because they are faster and require less code.
		 * 
		 * \see		CREATE_LOW_NIBBLE(), CREATE_HIGH_NIBBLE()
		 * \ingroup	gpio
		 * \author	Fabian Greif
		 */
		template <typename D3, typename D2, typename D1, typename D0>
		class Nibble
		{
			ALWAYS_INLINE static void
			setOutput()
			{
				D3::setOutput();
				D2::setOutput();
				D1::setOutput();
				D0::setOutput();
			}
			
			ALWAYS_INLINE static void
			setInput()
			{
				D3::setInput();
				D2::setInput();
				D1::setInput();
				D0::setInput();
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
				
				if (D3::read()) { value |= 0x08; }
				if (D2::read()) { value |= 0x04; }
				if (D1::read()) { value |= 0x02; }
				if (D0::read()) { value |= 0x01; }
				
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
				if (data & 0x08) { D3::set(); } else { D3::reset(); }
				if (data & 0x04) { D2::set(); } else { D2::reset(); }
				if (data & 0x02) { D1::set(); } else { D1::reset(); }
				if (data & 0x01) { D0::set(); } else { D0::reset(); }
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
