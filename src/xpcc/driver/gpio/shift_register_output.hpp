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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SHIFT_REGISTER_OUTPUT_HPP
#define XPCC__SHIFT_REGISTER_OUTPUT_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/interface/gpio.hpp>

namespace xpcc
{
	/**
	 * \brief	Shift register
	 * 
	 * TODO Untested!
	 * 
	\verbatim
	
		  74HC595
	       _____
	  P1 -|  U  |- Vcc
	  P2 -|     |- P0
	  P3 -|     |- Din (MOSI)
	  P4 -|     |- !OE (Output Enable -> low)
	  P5 -|     |- Store
	  P6 -|     |- Clk
	  P7 -|     |- !Reset (-> low)
	 Gnd -|_____|- Dout     
	\endverbatim
	 * 
	 * f_clk max. is 25 MHz at 4,5V and 5 MHz at 2V.
	 * 
	 * \tparam	Spi		SPI interface
	 * \tparam	Store	HC595 store input (Pin 12)
	 * \tparam	N		Number of HC595 Chips
	 * 
	 * \author	Fabian Greif
	 * \ingroup	driver_gpio
	 */
	template <typename Spi, typename Store, size_t N>
	class ShiftRegisterOutput
	{
	public:
		/**
		 * \brief	Initialize pins
		 * 
		 * Starts an initial transfer to set the outputs to known levels. If
		 * you don't want all Ports to be low set the values via set()
		 * beforehand.
		 */
		static void
		initialize();
		
		inline const uint8_t&
		operator [] (uint_fast8_t port) const;
		
		inline uint8_t&
		operator [] (uint_fast8_t port);
		
		/**
		 * \brief	Transfer the internal values to the outputs
		 */
		static void
		update();
		
	protected:
		static Spi spi;
		static Store store;
		
		static uint8_t cache[N];
	};
}

#include "shift_register_output_impl.hpp"

#endif	// XPCC__SHIFT_REGISTER_OUTPUT_HPP
