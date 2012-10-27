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

#ifndef XPCC__CPU_BOARD2_SLAVE_HPP
#define XPCC__CPU_BOARD2_SLAVE_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/platform/avr/xmega.hpp>
#include <xpcc/driver/connectivity/interconnect/interconnect_interface.hpp>

namespace xpcc
{
	/**
	 * \brief	CPU Board 2 Slave
	 * 
	 * <h2>Pin assignment:</h2>
	 * 
	 * InterconnectLink (to Master):
	 * - PD4 - SM  (output  -> PC1)
	 * - PD5 - MS  (input -> PC0)
	 * - PD6 - RXD (USARTD1)
	 * - PD7 - TXD (USARTD1)
	 * 
	 * Other:
	 * - PB0..3 - LEDs
	 * 
	 * On Headers:
	 * - PA0..7
	 * - PC0..7
	 * - PD0..3
	 * 
	 * \ingroup	board
	 * \author	Fabian Greif
	 */
	template <typename Transmit = Empty, typename Receive = Empty>
	class CpuBoard2Slave
	{
	protected:
		// BoardLink at D4-7
		
	public:
		/// Connection to the Slave CPU
		typedef InterconnectInterface<Transmit, Receive> Interconnect;
		
		GPIO__OUTPUT(Led0, B, 0);		///< Green
		GPIO__OUTPUT(Led1, B, 1);		///< Green
		GPIO__OUTPUT(Led2, B, 2);		///< Yellow
		GPIO__OUTPUT(Led3, B, 3);		///< Red
		
		GPIO__NIBBLE_LOW(Leds, B);
		
	public:
		/**
		 * \brief	Initialize all peripheral devices
		 * 
		 * \return	\c true if the initialization was successful, \c false
		 * 			if one of the chips didn't respond.
		 */
		static bool
		initialize();
		
	protected:
		/// Enable internal PLL (x4) to generate the 32 MHz clock frequency
		static void
		enableExternalClock();
	};
}

#include "slave_impl.hpp"

#endif // XPCC__CPU_BOARD2_SLAVE_HPP
