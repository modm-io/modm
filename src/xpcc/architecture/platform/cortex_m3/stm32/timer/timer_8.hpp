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

#ifndef XPCC_STM32__TIMER_8_HPP
#define XPCC_STM32__TIMER_8_HPP

#include "timer_base.hpp"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Advanced Control Timer 8
		 * 
		 * 
		 * Interrupt handler for High Density:
		 * - TIM8_BRK_IRQHandler
		 * - TIM8_UP_IRQHandler
		 * - TIM8_TRG_COM_IRQHandler
		 * - TIM8_CC_IRQHandler
		 * 
		 * Interrupt handler for XL Density:
		 * - TIM8_BRK_TIM12_IRQn
		 * - TIM8_UP_TIM13_IRQn
		 * - TIM8_TRG_COM_TIM14_IRQn
		 * - TIM8_CC_IRQn
		 * 
		 * 
		 * Example:
		 * \code
		 * extern "C" void
		 * TIM8_UP_IRQHandler(void)
		 * {
		 *     Timer8::acknowledgeInterrupt(Timer8::...);
		 *     
		 *     ...
		 * }
		 * \endcode
		 * 
		 * \warning	The Timer has much more possibilities than presented by this
		 * 			interface (e.g. Input Capture, Trigger for other Timers, DMA).
		 * 			It might be expanded in the future.
		 * 
		 * @author		Fabian Greif
		 * @ingroup		stm32
		 */
		class Timer8 : public AdvancedControlTimer
		{
			
			
		public:
			static void
			enable();
			
			static void
			disable();
			
			static inline void
			pause()
			{
				TIM8->CR1 &= ~TIM_CR1_CEN;
			}
			
			static inline void
			start()
			{
				TIM8->CR1 |= TIM_CR1_CEN;
			}
			
			static void
			setMode(Mode mode);
			
			static inline void
			setPrescaler(uint16_t prescaler)
			{
				// Because a prescaler of zero is not possible the actual
				// prescaler value is \p prescaler - 1 (see Datasheet)
				TIM8->PSC = prescaler - 1;
			}
			
			static inline void
			setOverflow(uint16_t overflow)
			{
				TIM8->ARR = overflow;
			}
			
			static uint16_t
			setPeriod(uint32_t microseconds, bool autoApply = true);
			
			static inline void
			applyAndReset()
			{
				// Generate Update Event to apply the new settings for ARR
				TIM8->EGR |= TIM_EGR_UG;
			}
			
			static inline uint16_t
			getValue()
			{
				return TIM8->CNT;
			}
			
			static inline void
			setValue(uint16_t value)
			{
				TIM8->CNT = value;
			}
			
			static inline void
			enableOutput()
			{
				TIM8->BDTR |= TIM_BDTR_MOE;
			}
			
		public:
			// TODO
			//static void
			//configureInputChannel(uint32_t channel, );
			
			static void
			configureOutputChannel(uint32_t channel, OutputCompareMode mode,
					uint16_t compareValue);
			
			static inline void
			setCompareValue(uint32_t channel, uint16_t value)
			{
				*(&TIM8->CCR1 + ((channel - 1) * 2)) = value;
			}
			
			static inline uint16_t
			getCompareValue(uint32_t channel)
			{
				return *(&TIM8->CCR1 + ((channel - 1) * 2));
			}
			
		public:
			static void
			enableInterrupt(Interrupt interrupt);
			
			static void
			disableInterrupt(Interrupt interrupt)
			{
				TIM8->DIER &= ~interrupt;
			}
			
			static Interrupt
			getInterruptCause();
			
			static void
			acknowledgeInterrupt(Interrupt interrupt)
			{
				// Flags are cleared by writing a zero to the flag position.
				// Writing a one is ignored.
				TIM8->SR = ~interrupt;
			}
		};
	}
}

#endif // XPCC_STM32__TIMER_8_HPP