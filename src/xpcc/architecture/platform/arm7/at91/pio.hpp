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

#ifndef XPCC_AT91__PIO_HPP
#define XPCC_AT91__PIO_HPP

#include <stdint.h>

#include "aic.hpp"

namespace xpcc
{
	namespace at91
	{
		enum PioController
		{
			PIOA,		///< Parallel I/O Controller A 
			PIOB		///< Parallel I/O Controller B
		};
		
		/**
		 * \brief	Parallel Input/Output Controller (PIO)
		 * 
		 * All the I/O lines PA0 to PA31 on AT91SAM7S512/256/128/64/321
		 * (PA0 to PA20 on AT91SAM7S32) are 5V-tolerant and all integrate a
		 * programmable pull-up resistor.
		 * 
		 * The PIO lines PA0 to PA3 are high-drive current capable. Each of
		 * these I/O lines can drive up to 16 mA permanently.
		 * 
		 * The remaining I/O lines can draw only 8 mA. However, the total
		 * current drawn by all the I/O lines cannot exceed 150 mA (100 mA for
		 * AT91SAM7S32/16).
		 * 
		 * @see		Datasheet AT91SAM7S: 10.4 PIO Controller A Multiplexing
		 * 
		 * \ingroup	at91
		 * \author	Fabian Greif
		 */
		class Pio
		{
		public:
			/**
			 * @brief	Enable clock for the Input/Output Controller
			 */
			static void
			enable();
			
			static void
			disable();
			
			/**
			 * @brief	Set a interrupt handler function
			 * 
			 * Common handler for all pin change interrupts.
			 * 
			 * When the software reads PIO_ISR, all the interrupts are
			 * automatically cleared. This signifies that all the interrupts
			 * that are pending when PIO_ISR is read must be handled.
			 * 
			 * @param	controller	Selects the PIO controller (only PIOA for AT91SAM7S,
			 * 						PIOA and PIOB for AT91SAM7X)
			 * @param	handler		Interrupt handler
			 */
			static void
			registerInterruptHandler(PioController controller, Aic::Handler handler);
			
			/**
			 * @brief	Remove interrupt handler and disable the interrupt
			 */
			static void
			removeInterruptHandler(PioController controller);
		};
	}
}

#endif // XPCC_AT91__PIT_HPP
