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

#ifndef XPCC_AT91__PIT_HPP
#define XPCC_AT91__PIT_HPP

#include <stdint.h>

#include "aic.hpp"
#include "device.h"

namespace xpcc
{
	namespace at91
	{
		/**
		 * \brief	Periodic Interval Timer (PIT)
		 * 
		 * Should be set to a 1 ms period to act as the general time base.
		 * 
		 * \ingroup	at91
		 * \author	Fabian Greif
		 */
		class Pit
		{
		public:
			/**
			 * @brief	Restart timer with a new period
			 * @param	period		New 20-bit counter value. A value of 0
			 * 						sets the period to 1 ms (calculated from
			 * 						F_CPU).
			 * 
			 * The Periodic Interval Timer runs at F_CPU / 16.
			 */
			static void
			start(uint32_t period = 0);
			
			/**
			 * @brief	Stop timer
			 */
			static void
			stop();
			
			/**
			 * @brief	Set a interrupt handler function
			 * 
			 * The handler function needs to read PIT_PIVR in order to
			 * acknowledge the interrupt.
			 */
			static void
			registerInterruptHandler(Aic::Handler function);
			
			/**
			 * @brief	Remove interrupt handler and disable the interrupt
			 */
			static void
			removeInterruptHandler();
			
			static inline void
			acknowledgeInterrupt()
			{
				// read PIVR to reset the interrupt
				(void) AT91C_BASE_PITC->PITC_PIVR;
			}
		};
	}
}

#endif // XPCC_AT91__PIT_HPP
