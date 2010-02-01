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

#ifndef XPCC__MCP2515_HPP
#define XPCC__MCP2515_HPP

#include <stdint.h>
#include <xpcc/hal/flash/flash_pointer.hpp>
#include <xpcc/hal/time/delay.hpp>

#include "can.hpp"

/**
 * \ingroup	    driver
 * \name		Bits des Filters fuer den MCP2515 umformatieren
 * 
 * \code
 * FLASH(uint8_t can_filter[]) =
 * {
 * 		MCP2515_FILTER_EXTENDED(0),	// Filter 0
 *  	MCP2515_FILTER_EXTENDED(0),	// Filter 1
 *  	
 *  	MCP2515_FILTER_EXTENDED(0),	// Filter 2
 *  	MCP2515_FILTER_EXTENDED(0),	// Filter 3
 *  	MCP2515_FILTER_EXTENDED(0),	// Filter 4
 *  	MCP2515_FILTER_EXTENDED(0),	// Filter 5
 *  	
 *  	MCP2515_FILTER_EXTENDED(0),	// Mask 0
 * 		MCP2515_FILTER_EXTENDED(0),	// Mask 1
 * };
 * \endcode
 * 
 * \warning	Do not use this macro for variables, only for static values
 *			known at compile-time.
 */
//\{
#if defined(__DOXYGEN__)

	#define	MCP2515_FILTER_EXTENDED(id)
	#define	MCP2515_FILTER(id)

#else

	#define MCP2515_FILTER_EXTENDED(id)	\
			(uint8_t)  ((uint32_t) (id) >> 21), \
			(uint8_t)((((uint32_t) (id) >> 13) & 0xe0) | (1<<3) | \
				(((uint32_t) (id) >> 16) & 0x3)), \
			(uint8_t)  ((uint32_t) (id) >> 8), \
			(uint8_t)  ((uint32_t) (id))
	
	#define	MCP2515_FILTER(id) \
			(uint8_t)((uint32_t) id >> 3), \
			(uint8_t)((uint32_t) id << 5), \
			0, \
			0
#endif
//\}

namespace xpcc
{
	/**
	 * 
	 * \brief	Driver for a MPC2515 CAN Controller
	 * 
	 * \tparam	SPI		TODO
	 * \tparam	CS		TODO
	 * \tparam	INT		TODO
	 * 
	 * If you want to activate the internal pullup for the INT pin you
	 * need to do this by yourself before calling the initialize method!
	 * 
	 * \ingroup	driver
	 */
	template < typename SPI,
			   typename CS,
			   typename INT >
	class Mcp2515
	{
	public:
		static bool
		initialize(Can::Bitrate bitrate);
		
		static void
		setFilter(FlashPointer<uint8_t> filter);
		
		static void
		setMode(Can::Mode mode);
		
		
		static inline bool
		isMessageAvailable();
		
		static bool
		getMessage(Can::Message& message);
		
		static bool
		canSend();
		
		static bool
		sendMessage(const Can::Message& message);
		
	protected:
		static void
		writeRegister(uint8_t address, uint8_t data);
		
		static uint8_t
		readRegister(uint8_t address);
		
		static void
		bitModify(uint8_t address, uint8_t mask, uint8_t data);
		
		static uint8_t
		readStatus(uint8_t type);
		
		static inline void
		writeIdentifier(const uint32_t& identifier, bool isExtendedFrame);
		
		static inline bool
		readIdentifier(uint32_t& identifier);
	};
}

#include "mcp2515_impl.hpp"

#endif // XPCC__MCP2515_HPP
