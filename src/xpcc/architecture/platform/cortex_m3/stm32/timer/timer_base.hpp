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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__GENERAL_PURPOSE_TIMER_HPP
#define XPCC_STM32__GENERAL_PURPOSE_TIMER_HPP

#include <stdint.h>
#include "../device.h"

namespace xpcc
{
	namespace stm32
	{
		class BasicTimer
		{
		public:
			enum Mode
			{
				UP_COUNTER = 0,
				ONE_SHOT_UP_COUNTER = TIM_CR1_OPM,
			};

			enum Interrupt
			{
				UPDATE_INTERRUPT = TIM_DIER_UIE,
				UPDATE_DMA_REQUEST_INTERRUPT = TIM_DIER_UDE,
			};

		public:
			/**
			 * Enables the clock for the timer and resets all settings
			 *
			 * Has to be called before calling any other function from this
			 * class! Otherwise the settings won't have any effect.
			 */
			static void
			enable();

			/**
			 * Disable clock.
			 *
			 * All settings are ignored in this mode and the timer is
			 * shut down. Calling any function other than enable() won't
			 * have any effect.
			 */
			static void
			disable();

			/**
			 * Pause timer operation
			 *
			 * All settings are stored but the timer operation is suspend until
			 * resume() is called.
			 */
			static void
			pause();

			/**
			 * Re-enable timer operations
			 *
			 * Has to called after the initial configuration to start timer
			 * or after pause() to restart the timer.
			 */
			static void
			start();

			/**
			 * Set operation mode of the timer
			 *
			 * In Encoder mode the encoder channels A and B must be connected
			 * to channel 1 and 2 of the timer (e.g. TIM{{ id }}_CH1 and TIM{{ id }}_CH2).
			 */
			static void
			setMode(Mode mode);

			/**
			 * Set new prescaler
			 *
			 * The prescaler can divide the counter clock frequency by any
			 * factor between 1 and 65536. The new prescaler ratio is taken
			 * into account at the next update event.
			 *
			 * @see		applyAndReset()
			 */
			static inline void
			setPrescaler(uint16_t prescaler);

			/**
			 * Set overflow.
			 *
			 * This sets the maximum counter value of the timer.
			 * The timer is blocked if \p overflow is set to zero.
			 *
			 * Takes effect at next update event.
			 *
			 * @see		applyAndReset()
			 */
			static inline void
			setOverflow(uint16_t overflow);

			/**
			 * Set period in microseconds
			 *
			 * Changes prescaler and overflow values.
			 * Takes effect at next update event.
			 *
			 * @param	microseconds	Requested period in microseconds
			 * @param	autoApply		Update the new value immediately and
			 * 							reset the counter value.
			 *
			 * @return	New overflow value.
			 *
			 * @see		applyAndReset()
			 */
			static uint16_t
			setPeriod(uint32_t microseconds, bool autoApply = true);

			/**
			 * @brief	Reset the counter, and update the prescaler and
			 * 			overflow values.
			 *
			 * Generates an Update-Event without generating an
			 * Update-Interrupt.
			 *
			 * This will reset the counter to 0 in up-counting mode (the
			 * default) or to the maximal value in down-counting mode. It will
			 * also update the timer's prescaler and overflow values if you
			 * have set them up to be changed using setPrescaler() or
			 * setOverflow() (or setPeriod()).
			 *
			 * An Update-Event is also generated when the timer reaches its
			 * maximal (up-counting) or minimal (down-counting) value. The
			 * settings for Prescaler, Overflow and Compare values are applied
			 * then without calling this function.
			 */
			static inline void
			applyAndReset();

			/**
			 * Get the counter value
			 */
			static inline uint16_t
			getValue();

			/**
			 * Set a new counter value
			 */
			static inline void
			setValue(uint16_t value);

			/** Enable interrupt handler */
			static void
			enableInterrupt(Interrupt interrupt);

			/** Disable interrupt handler */
			static void
			disableInterrupt(Interrupt interrupt);

			static Interrupt
			getInterruptCause();

			/**
			 * Reset interrupt flags
			 *
			 * You need to call this function at the beginning of the
			 * interrupt handler function. Preferably as the first command.
			 */
			static void
			acknowledgeInterrupt(Interrupt interrupt);
		};

		class GeneralPurposeTimer : public BasicTimer
		{
		public:
			enum Interrupt
			{
				UPDATE_INTERRUPT = TIM_DIER_UIE,
				CAPTURE_COMPARE_1_INTERRUPT = TIM_DIER_CC1IE,
				CAPTURE_COMPARE_2_INTERRUPT = TIM_DIER_CC2IE,
				CAPTURE_COMPARE_3_INTERRUPT = TIM_DIER_CC3IE,
				CAPTURE_COMPARE_4_INTERRUPT = TIM_DIER_CC4IE,
				COM_INTERRUPT = TIM_DIER_COMIE,
				TRIGGER_INTERRUPT = TIM_DIER_TIE,
				BREAK_INTERRUPT = TIM_DIER_BIE,
				UPDATE_DMA_REQUEST_INTERRUPT = TIM_DIER_UDE,
				CAPTURE_COMPARE_1_DMA_REQUEST_INTERRUPT = TIM_DIER_CC1DE,
				CAPTURE_COMPARE_2_DMA_REQUEST_INTERRUPT = TIM_DIER_CC2DE,
				CAPTURE_COMPARE_3_DMA_REQUEST_INTERRUPT = TIM_DIER_CC3DE,
				CAPTURE_COMPARE_4_DMA_REQUEST_INTERRUPT = TIM_DIER_CC4DE,
				COM_DMA_REQUEST_INTERRUPT = TIM_DIER_COMDE,
				TRIGGER_DMA_REQUEST_INTERRUPT = TIM_DIER_TDE,
			};

			enum Mode
			{
				UP_COUNTER = 0,
				DOWN_COUNTER = TIM_CR1_DIR,
				ONE_SHOT_UP_COUNTER = TIM_CR1_OPM,
				ONE_SHOT_DOWN_COUNTER = TIM_CR1_DIR | TIM_CR1_OPM,

				// Counter counts up and down alternatively
				CENTER_ALIGNED_1 =                 TIM_CR1_CMS_0,	///< Output compare flags only set when counting down
				CENTER_ALIGNED_2 = TIM_CR1_CMS_1, 					///< Output compare flags only set when counting up
				CENTER_ALIGNED_3 = TIM_CR1_CMS_1 | TIM_CR1_CMS_0,	///< Output compare flags set when counting up and down (default)

				ENCODER,
			};

			enum OutputCompareMode
			{
				/// Output is independent from the compare result
				OUTPUT_INACTIVE = 0,

				/**
				 * PWM Mode 1.
				 *
				 * While up-counting, channel is active as long as
				 * count is less than channel capture/compare register, else
				 * inactive.
				 * In downcounting, channel is	inactive as long as count exceeds
				 * capture/compare register, else active
				 */
				OUTPUT_PWM = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1,

				/**
				 * PWM mode 2.
				 *
				 * In upcounting, channel is inactive as long as
				 * count is less than capture/compare register, else active.
				 * In downcounting, channel is active as long as count exceeds
				 * capture/compare	register, else inactive.
				 */
				OUTPUT_PWM2 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0,
			};

		public:
			// TODO implement this function
			//static void
			//configureInputChannel(uint32_t channel, ...);

			/**
			 * Configure output channel 1..4.
			 *
			 * @param	channel			[1..4]
			 * @param	mode			Output compare mode
			 * @param	compareValue	Preloaded output compare value (can be
			 * 							changed later via setCompareValue())
			 */
			static void
			configureOutputChannel(uint32_t channel, OutputCompareMode mode,
					uint16_t compareValue);

			/**
			 * Set compare value for channel 1..4.
			 *
			 * @param	channel	[1..4]
			 * @param	value	Compare value
			 */
			static inline void
			setCompareValue(uint32_t channel, uint16_t value);

			/**
			 * Read compare value for channel 1..4.
			 *
			 * @param	channel	[1..4]
			 * @return	Current compare value
			 */
			static inline uint16_t
			getCompareValue(uint32_t channel);
		};

		class AdvancedControlTimer : public GeneralPurposeTimer
		{
		public:
			/**
			 * Configure output channel 1..4.
			 *
			 * @param	channel			[1..4]
			 * @param	mode			Output compare mode
			 * @param	compareValue	Preloaded output compare value (can be
			 * 							changed later via setCompareValue())
			 */
			// TODO complementary outputs
			//static void
			//configureOutputChannel(uint32_t channel, OutputCompareMode mode,
			//		uint16_t compareValue);

			// TODO Repetition Counter (TIM1_RCR)

			/**
			 * Enable output pins
			 */
			static void
			enableOutput();
		};
	}
}

#endif // XPCC_STM32__GENERAL_PURPOSE_TIMER_HPP
