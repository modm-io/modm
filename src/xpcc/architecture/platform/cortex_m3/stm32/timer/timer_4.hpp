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

#ifndef XPCC_STM32__TIMER_4_HPP
#define XPCC_STM32__TIMER_4_HPP

#include <stdint.h>
#include "timer_base.hpp"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Timer 4
		 * 
		 * Interrupt handler:
		 * \code
		 * extern "C" void
		 * TIM4_IRQHandler(void)
		 * {
		 *     Timer4::acknowledgeInterrupt(Timer4::...);
		 *     
		 *     ...
		 * }
		 * \endcode
		 * 
		 * @ingroup		stm32
		 */
		class Timer4 : public GeneralPurposeTimer
		{
		public:
			enum Remap
			{
				NO_REMAP = 0,
				FULL_REMAP = AFIO_MAPR_TIM4_REMAP,
			};
			static const uint32_t remapMask = AFIO_MAPR_TIM4_REMAP;
			
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
				TIM4->CR1 &= ~TIM_CR1_CEN;
			}
			
			static inline void
			resume()
			{
				TIM4->CR1 |= TIM_CR1_CEN;
			}
			
			/**
			 * Configure as Up/Down-Counter
			 */
			static void
			configureCounter(Mode mode = PERIODIC, Direction dir = UP_COUNTING);
			
			/**
			 * Configure as Encoder
			 * 
			 * The encoder channels A and B must be connected to Timer
			 * Channel 1 and 2 of the Timer (e.g. TIM3_CH1 and TIM3_CH2).
			 */
			static void
			configureEncoder();
			
			/**
			 * Configure in PWM Mode
			 * 
			 */
			static void
			configurePwm(PwmMode pwm = EDGE_ALIGNED, Direction dir = UP_COUNTING);
			
			// TODO
			//static void
			//configureOutputCompare();
			
			// TODO
			//static void
			//configureInputCapture();
			
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
				// prescaler value is \p prescaler - 1 (see Datasheet)
				TIM4->PSC = prescaler - 1;
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
				TIM4->ARR = overflow;
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
			 * @brief	Reset the counter, and update the prescaler and
			 * 			overflow values.
			 *
			 * This will reset the counter to 0 in up-counting mode (the
			 * default) or to the maximal value in down-counting mode. It will
			 * also update the timer's prescaler and overflow values if you
			 * have set them up to be changed using setPrescaler() or
			 * setOverflow() (or setPeriod()).
			 */
			static inline void
			refresh()
			{
				// Generate Update Event to apply the new settings for ARR
				TIM4->EGR |= TIM_EGR_UG;
			}
			
			/**
			 * Get the counter value
			 */
			static inline uint16_t
			getValue()
			{
				return TIM4->CNT;
			}
			
			/**
			 * Set a new counter value
			 */
			static inline void
			setValue(uint16_t value)
			{
				TIM4->CNT = value;
			}
			
			//static void
			//configureInputChannel(uint32_t channel, );
			
			/**
			 * @param	channel	[1..4]
			 */
			static void
			configureOutputChannel(uint32_t channel, OutputCompareMode mode);
			
			/**
			 * @param	channel	[1..4]
			 */
			static inline void
			setCompareChannel(uint32_t channel, uint16_t value)
			{
				*(&TIM4->CR1 + ((channel - 1) * 2)) = value;
			}
			
			/**
			 * @param	channel	[1..4]
			 */
			static inline uint16_t
			getCompareChannel(uint32_t channel)
			{
				return *(&TIM4->CR1 + ((channel - 1) * 2));
			}
			
			/** Enable interrupt handler */
			static void
			enableInterrupt(Interrupt interrupt);
			
			/** Disable interrupt handler */
			static void
			disableInterrupt(Interrupt interrupt)
			{
				TIM4->DIER &= ~interrupt;
			}
			
			static Interrupt
			getInterruptCause();
			
			/**
			 * Reset interrupt flags
			 * 
			 * You need to call this function at the beginning of the
			 * interrupt handler function. Preferably as the first command.
			 */
			static void
			acknowledgeInterrupt(Interrupt interrupt)
			{
				// Flags are cleared by writing a zero to the flag position.
				// Writing a one is ignored.
				TIM4->SR = ~interrupt;
			}
			
			/**
			 * Remap compare channels to other locations.
			 * 
			 * The pins are not automatically initialized. This has to be
			 * done by the user.
			 */
			static inline void
			remapPins(Remap mapping)
			{
				AFIO->MAPR = (AFIO->MAPR & ~remapMask) | mapping;
			}
		};
	}
}

#endif // XPCC_STM32__TIMER_4_HPP