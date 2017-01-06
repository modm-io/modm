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

#ifndef MODM_MCP23S08_HPP
#define MODM_MCP23S08_HPP

#include <modm/architecture/utils.hpp>
#include <modm/architecture/interface/gpio.hpp>

namespace modm
{
	/**
	 * \brief	8-Bit I/O Expander with Serial Interface
	 * 
	 * A1 and A0 need to be tided low.
	 * 
	 * \author	Fabian Greif
	 * \ingroup	driver_gpio
	 */
	template <typename Spi, typename Cs, typename Int>
	class Mcp23s08
	{
	public:
		static void
		initialize();
		
		/**
		 * @brief	Configure pins
		 * 
		 * @param	inputMask	1=Input, 0=Output
		 * @param	pullupMask	1=Pullup, 0=Pullup-Disabled
		 */
		static void
		configure(uint8_t inputMask, uint8_t pullupMask);
		
		//void
		//configureInterrupt();
		
		static uint8_t
		read();
		
		static void
		write(uint8_t output);
		
	protected:
		enum RegisterAddress
		{
			MCP_IODIR = 0x00,		///< Port direction (1=input, 0=output)
			MCP_IPOL = 0x01,		///< Invert polarity
			MCP_GPINTEN = 0x02,		///< Enable interrupt
			MCP_DEFVAL = 0x03,		///< Compare register for interrupt
			MCP_INTCON = 0x04,
			MCP_IOCON = 0x05,		///< Configuration
			MCP_GPPU = 0x06,		///< Enable pullups
			MCP_INTF = 0x07,		///< Interrupt flag register
			MCP_INTCAP = 0x08,		///< Interrupt capture register
			MCP_GPIO = 0x09,		///< Port values
			MCP_OLAT = 0x0a			///< Output latch register
		};
		
		enum RW
		{
			WRITE = 0,
			READ = 1
		};
		
		static const uint8_t deviceAddress = 0x40;
		
		static Spi spi;
		static Cs cs;
		static Int interrupt;
	};
}

#include "mcp23s08_impl.hpp"

#endif	// MODM_MCP23S08_HPP
