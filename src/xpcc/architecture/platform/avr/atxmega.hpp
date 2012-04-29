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
 */
// ----------------------------------------------------------------------------
/**
 * \ingroup		avr
 * \defgroup	atxmega		XMEGA
 */

#ifndef XPCC_ATXMEGA__ATXMEGA_HPP
#define XPCC_ATXMEGA__ATXMEGA_HPP

#include "../avr.hpp"

namespace xpcc
{
	namespace atxmega
	{
		using avr::enableInterrupts;
		using avr::disableInterrupts;
		
		/// Used to set the interrupt level of all modules.
		/// @ingroup	atxmega
		enum InterruptLevel
		{
			INTERRUPT_LEVEL_OFF = 0x00,
			INTERRUPT_LEVEL_LOW = 0x01,
			INTERRUPT_LEVEL_MEDIUM = 0x02,
			INTERRUPT_LEVEL_HIGH = 0x03,
		};
		
		enum InterruptControlLevel
		{
			INTERRUPT_CONTROL_LEVEL_LOW = PMIC_LOLVLEN_bm,
			INTERRUPT_CONTROL_LEVEL_MEDIUM = PMIC_MEDLVLEN_bm,
			INTERRUPT_CONTROL_LEVEL_HIGH = PMIC_HILVLEN_bm,
			
			/// Enable all Interrupt levels
			INTERRUPT_CONTROL_LEVEL_ALL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm,
		};
		
		/**
		 * Enable one or more of the three Interrupt levels.
		 *
		 * In order to work with interrupts on a xMEGA you need to enable
		 * interrupts in general (enableInterrupts()) and enable the
		 * level you wont to work with.
		 *
		 * Example:
		 * \code
		 * // Enable all interrupt levels
		 * xpcc::atxmega::enableInterrupts();
		 * xpcc::atxmega::enableInterruptLevel(xpcc::atxmega::INTERRUPT_LEVEL_ALL);
		 * \endcode
		 *
		 * @ingroup	atxmega
		 */
		static inline void
		enableInterruptLevel(InterruptControlLevel level)
		{
			PMIC_CTRL |= level;
		}
		
		/**
		 * Disable interrupt levels.
		 *
		 * @ingroup	atxmega
		 */
		static inline void
		disableInterruptLevel(InterruptControlLevel level)
		{
			PMIC_CTRL &= ~level;
		}
	}
}

#include "atxmega/uart.hpp"
#include "atxmega/spi.hpp"
#include "atxmega/gpio.hpp"
#include "atxmega/timer.hpp"
#include "atxmega/adc.hpp"
#include "atxmega/clock.hpp"
#include "atxmega/utils.hpp"
#include "atxmega/i2c.hpp"
#include "atxmega/dma.hpp"

#endif
