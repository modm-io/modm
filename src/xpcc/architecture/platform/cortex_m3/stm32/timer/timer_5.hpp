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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__TIMER_5_HPP
#define XPCC_STM32__TIMER_5_HPP

#include <stdint.h>
#include "general_purpose_timer.hpp"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Timer 5
		 * 
		 * Interrupt handler:
		 * \code
		 * extern "C" void
		 * TIM5_IRQHandler(void)
		 * {
		 *     ...
		 * }
		 * \endcode
		 * 
		 * @ingroup		stm32
		 */
		class Timer5 : public GeneralPurposeTimer
		{
		public:
			/**
			 * Enables the clock for the timer and resets all settings
			 */
			static void
			enable();
			
			/**
			 * Disable clock.
			 * 
			 * All settings are ignored in this mode.
			 */
			static void
			disable();
			
			static inline void
			pause()
			{
				TIM5->CR1 &= ~TIM_CR1_CEN;
			}
			
			static inline void
			resume()
			{
				TIM5->CR1 |= TIM_CR1_CEN;
			}
			
			static void
			configure();
			
			/**
			 * Set new prescaler
			 * 
			 * The prescaler can divide the counter clock frequency by any
			 * factor between 1 and 65536. The new prescaler ratio is taken
			 * into account at the next update event.
			 * 
			 * @see		refresh()
			 */
			static inline void
			setPrescaler(uint16_t prescaler)
			{
				// Because a prescaler of zero is not possible the actual
				// prescaler is \p prescaler - 1 (see Datasheet)
				TIM5->PSC = prescaler - 1;
			}
			
			/**
			 * Set overflow.
			 * 
			 * This sets the maximum counter value of the timer.
			 * The timer is blocked if \p overflow is set to zero.
			 * 
			 * Takes effect at next update event.
			 *
			 * @see		refresh()
			 */
			static inline void
			setOverflow(uint16_t overflow)
			{
				TIM5->ARR = overflow;
			}
			
			/**
			 * Set period in microseconds
			 * 
			 * Changes prescaler and overflow values.
			 * Takes effect at next update event.
			 * 
			 * @param	microseconds	Requested period in microseconds
			 * @param	autoRefresh		Update the new value immediately and
			 * 							reset the counter value. 
			 * 
			 * @return	New overflow value.
			 * 
			 * @see		refresh()
			 */
			static uint16_t
			setPeriod(uint32_t microseconds, bool autoRefresh = true);
			
			/**
			 * Get the counter value
			 */
			static inline uint16_t
			getValue()
			{
				return TIM5->CNT;
			}
			
			/**
			 * @brief	Reset the counter, and update the prescaler and
			 * 			overflow values.
			 *
			 * This will reset the counter to 0 in upcounting mode (the
			 * default). It will also update the timer's prescaler and
			 * overflow, if you have set them up to be changed using
			 * setPrescaler() or setOverflow().
			 */
			static inline void
			refresh()
			{
				// Genererate Update Event to apply the new settings for ARR
				TIM5->EGR |= TIM_EGR_UG;
			}
			
			/** Enable interrupt handler */
			static void
			enableInterrupt(Interrupt interrupt);
			
			/** Disable interrupt handler */
			static void
			disableInterrupt(Interrupt interrupt);
			
			static void
			acknowledgeInterrupt(Interrupt interrupt);
		};
	}
}

#endif // XPCC_STM32__TIMER_5_HPP