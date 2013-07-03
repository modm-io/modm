// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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

#ifndef XPCC__PERIPHERAL_HPP
#define XPCC__PERIPHERAL_HPP

#include <stdint.h>
#include <cstddef>

/**
 * \ingroup 	architecture
 * \defgroup	peripheral	Peripherals
 * 
 * All hardware peripherals with common interfaces.
 */

namespace xpcc
{
	/**
	 * \brief	Peripheral class
	 *
	 * This class defines acts as a base class for all classes describing the
	 * public interface of common peripheral drivers.
	 * As there is no implementation given, the classes specific to the platform
	 * inherit from their respective base classes and must shadow the methods of
	 * them.
	 *
	 * The inheritance is only visible for the documenation, it is completely
	 * removed during compile time keeping a possible error at platform level.
	 * This is safe, because only one platform can be compiled at once.
	 *
	 * This way, no virtual functions are needed and no overhead is generated,
	 * but we still have clean inheritance in the documentation.
	 * There is no need to document the platform specific implementation, since
	 * it is taken from the base class.
	 *
	 * \ingroup peripheral
	 * \author	Niklas Hauser
	 */
	class Peripheral
	{
#ifdef __DOXYGEN__
		/**
		 * \brief initializes the peripheral, must be called before use.
		 */
		static void
		initialize();

		/**
		 * \brief configures a peripheral for a specific purpose
		 */
		static void
		configurePurpose();

		/**
		 * \brief sets a parameter
		 */
		static void
		setParameter();

		/**
		 * \brief gets a parameter
		 */
		static void
		getParameter();

		/**
		 * \brief acknowledges an interrupt flag
		 *
		 * We use acknowledge here, because it describes the intention rather
		 * than the actual implementation.
		 */
		static void
		acknowledgeInterruptFlag();
#endif
	};
}

#include "peripheral/gpio.hpp"
#include "peripheral/uart.hpp"
#include "peripheral/one_wire.hpp"
#include "peripheral/spi.hpp"
#include "peripheral/i2c.hpp"

#endif	// XPCC__PERIPHERAL_HPP
