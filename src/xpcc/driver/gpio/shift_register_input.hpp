/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SHIFT_REGISTER_INPUT_HPP
#define XPCC_SHIFT_REGISTER_INPUT_HPP

#include <modm/architecture/utils.hpp>
#include <modm/architecture/interface/gpio.hpp>

namespace xpcc
{
	/**
	 * \brief	Shift register
	 * 
	 * TODO Untested!
	 * 
	\verbatim
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
	 * \ingroup	driver_gpio
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

#endif	// XPCC_SHIFT_REGISTER_INPUT_HPP
