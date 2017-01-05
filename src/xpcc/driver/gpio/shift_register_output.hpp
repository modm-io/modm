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

#ifndef MODM_SHIFT_REGISTER_OUTPUT_HPP
#define MODM_SHIFT_REGISTER_OUTPUT_HPP

#include <modm/architecture/utils.hpp>
#include <modm/architecture/interface/gpio.hpp>

namespace modm
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

#endif	// MODM_SHIFT_REGISTER_OUTPUT_HPP
