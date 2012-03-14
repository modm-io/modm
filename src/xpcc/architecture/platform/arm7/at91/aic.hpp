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

#ifndef XPCC_AT91__AIC_HPP
#define XPCC_AT91__AIC_HPP

#include <stdint.h>

namespace xpcc
{
	namespace at91
	{
		/**
		 * \brief	Advanced Interrupt Controller (AIC)
		 * 
		 * \ingroup	at91
		 */
		class Aic
		{
		public:
			typedef void (* Handler)();
			
			enum Mode
			{
				LOW_LEVEL_SENSITIVE = 0,		///< only for external interrupt sources
				NEGATIVE_EDGE_TRIGGERED = 1,	///< only for external interrupt sources
				HIGH_LEVEL_SENSITVE = 2,
				POSITIVE_EDGE_TRIGGERED = 3,
			};
			
		public:
			static void
			initialize();
			
			/**
			 * @brief	Enable interrupt source and register a corresponding
			 * 			handler function
			 * 
			 * @param	id			Interrupt identifier (0..31)
			 * @param	function	Functions pointer to the handler function
			 * @param	priority	Interrupt priority (0=lowest .. 7=highest)
			 * @param	mode		Sensitivity mode of an interrupt
			 */
			static void
			registerHandler(uint32_t id, Handler function,
					uint32_t priority = 0, Mode mode = HIGH_LEVEL_SENSITVE);
			
			/**
			 * @brief	Enable interrupt source
			 * @param	id		Interrupt identifier (0..31)
			 */
			static void
			enableInterrupt(uint32_t id);
			
			/**
			 * @brief	Disable interrupt source
			 * @param	id		Interrupt identifier (0..31)
			 */
			static void
			disableInterrupt(uint32_t id);
			
			/**
			 * @brief	Register a spurious handler
			 * 
			 * Spurious interrupts might appear when the interrupt cause is
			 * gone before the core can react to it. In that case the interrupt
			 * handling is continued normally but the AIC returns the spurious
			 * handler instead of a normal interrupt handler as it doesn't
			 * know what else to do.
			 * 
			 * Nothing needs to be done in the handler, it just have to return
			 * normally.
			 * 
			 * @param	function	Spurious handler function
			 */
			static void
			registerSpuriousHandler(Handler function);
		};
	}
}

#endif // XPCC_AT91__AIC_HPP
