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

#ifndef XPCC_STM32__TIMER_6_HPP
#define XPCC_STM32__TIMER_6_HPP

#include "timer_base.hpp"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Basic Timer 6
		 * 
		 * Interrupt handler:
		 * \code
		 * extern "C" void
		 * TIM6_IRQHandler(void)
		 * {
		 *     Timer6::resetInterruptFlags(Timer6::FLAG_UPDATE);
		 *     
		 *     ...
		 * }
		 * \endcode
		 * 
		 * For the STM32F2xx and STM32F4xx:
		 * \code
		 * extern "C" void
		 * TIM6_DAC_IRQHandler(void)
		 * {
		 *     Timer6::resetInterruptFlags(Timer6::FLAG_UPDATE);
		 *     
		 *     ...
		 * }
		 * \endcode
		 * 
		 * @author		Fabian Greif
		 * @ingroup		stm32
		 */
		class Timer6 : public BasicTimer
		{
		public:
			static void
			enable();
			
			static void
			disable();
			
			static inline void
			pause()
			{
				TIM6->CR1 &= ~TIM_CR1_CEN;
			}
			
			static inline void
			start()
			{
				TIM6->CR1 |= TIM_CR1_CEN;
			}
			
			static void
			setMode(Mode mode);
			
			static inline void
			setPrescaler(uint16_t prescaler)
			{
				// Because a prescaler of zero is not possible the actual
				// prescaler value is \p prescaler - 1 (see Datasheet)
				TIM6->PSC = prescaler - 1;
			}
			
			static inline void
			setOverflow(uint16_t overflow)
			{
				TIM6->ARR = overflow;
			}
			
			static uint16_t
			setPeriod(uint32_t microseconds, bool autoApply = true);
			
			static inline void
			applyAndReset()
			{
				// Generate Update Event to apply the new settings for ARR
				TIM6->EGR |= TIM_EGR_UG;
			}
			
			static inline uint16_t
			getValue()
			{
				return TIM6->CNT;
			}
			
			static inline void
			setValue(uint16_t value)
			{
				TIM6->CNT = value;
			}
			
			static void
			enableInterruptVector(bool enable, uint32_t priority);
			
			static inline void
			enableInterrupt(Interrupt interrupt)
			{
				TIM6->DIER |= interrupt;
			}
			
			static inline void
			disableInterrupt(Interrupt interrupt)
			{
				TIM6->DIER &= ~interrupt;
			}
			
			static inline InterruptFlag
			getInterruptFlags()
			{
				return static_cast<InterruptFlag>(TIM6->SR);
			}
			
			static void
			resetInterruptFlags(InterruptFlag interrupt)
			{
				// Flags are cleared by writing a zero to the flag position.
				// Writing a one is ignored.
				TIM6->SR = ~interrupt;
			}
		};
	}
}

#endif // XPCC_STM32__TIMER_6_HPP