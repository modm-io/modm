// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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
 * $Id: shift_register_input.hpp 610 2011-09-21 22:31:12Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SHIFT_REGISTER_INPUT_HPP
#define XPCC__SHIFT_REGISTER_INPUT_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/driver/gpio.hpp>

namespace xpcc
{
	/**
	 * \brief	Shift register
	 * 
	 * TODO Untested!
	 * 
	 * \verbatim
	    74HC165
         _____
  Load -|  U  |- Vcc
   Clk -|     |- ClkInh (-> low)
    P4 -|     |- P3
    P5 -|     |- P2
    P6 -|     |- P1
    P7 -|     |- P0
 !Dout -|     |- Din
   Gnd -|_____|- Dout (MISO)
\endverbatim
	 * 
	 * f_clk max. is 25 MHz at 4,5V and 5 MHz at 2V.
	 * 
	 * TODO
	 * 
	 * \warning	The SPI interface must operate at mode 1! For most other
	 * 			devices you need mode 0.
	 * 
	 * \tparam	Spi			SPI interface
	 * \tparam	Load		HC165 load input (Pin 1)
	 * \tparam	N			Number of HC165 Chips
	 * 
	 * \author	Fabian Greif
	 * \ingroup	gpio
	 */
	template <typename Spi,	typename Load, size_t N>
	class ShiftRegisterInput
	{
	public:
		void
		initialize();
		
		inline const uint8_t&
		operator [] (uint_fast8_t port) const;
		
		inline uint8_t&
		operator [] (uint_fast8_t port);
		
		void
		update();
		
	protected:
		static Spi spi;
		static Load load;
		
		static uint8_t cache[N];
	};
}

#include "shift_register_input_impl.hpp"

#endif	// XPCC__SHIFT_REGISTER_INPUT_HPP
