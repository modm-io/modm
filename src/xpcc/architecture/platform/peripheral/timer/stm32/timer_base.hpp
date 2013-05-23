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

#ifndef XPCC_STM32__TIMER_BASE_HPP
#define XPCC_STM32__TIMER_BASE_HPP

#include <stdint.h>
#include "../../../external.hpp"

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
				INTERRUPT_UPDATE = TIM_DIER_UIE,
			};

			enum InterruptFlag
			{
				FLAG_UPDATE = TIM_SR_UIF,
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
			 * start() is called.
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
			
			/**
			 * Enables or disables the Interrupt Vector.
			 * 
			 * \param	enable
			 * 				Enable/disable the interrupt vector
			 * \param	priority
			 * 				Priority of the interrupt vector
			 * 				(0=highest to 15=lowest).
			 */
			static void
			enableInterruptVector(bool enable, uint8_t priority);
			
			/**
			 * Enables interrupts. Don't forget to enable the Interrupt Vector.
			 * 
			 * \param	interrupt	
			 * 				Interrupts to enable
			 * 
			 * \see		enableInterruptVector()
			 */
			static void
			enableInterrupt(Interrupt interrupt);

			/**
			 * Disables interrupts.
			 *
			 * \param	interrupt
			 * 				Interrupts to disable
			 */
			static void
			disableInterrupt(Interrupt interrupt);

			/**
			 * Returns a bitmap of the enum StateFlag. Use this method while
			 * executing an interrupt or in other situations where you want to
			 * know which of the flags are set.
			 *
			 * If a flag is a cause of an enabled interrupt (and the Interrupt
			 * Vector is enabled) then interrupt will be triggered as long
			 * the flag is set.
			 *
			 * \warning		You cannot use a switch statement for the returned
			 * 				value, since multiple flags may be set!
			 *
			 * \code
			 * InterruptFlag flags = Timer{{ id }}::getInterruptFlags()
			 * if (flags & Timer{{ id }}::FLAG_XX){
			 *     // Flag was set, clear the flag since flags are set by
			 *     // hardware, but have to be cleared by software
			 *     Timer{{ id }}::resetInterruptFlags(TIM{{ id }}::FLAG_XX)
			 * }
			 * \endcode
			 */
			static InterruptFlag
			getInterruptFlags();
			
			/**
			 * Clears one or multiple flags.
			 *
			 * \param flags		Bitmap of StateFlag
			 */
			static void
			resetInterruptFlags(InterruptFlag flags);
		};

		class GeneralPurposeTimer : public BasicTimer
		{
		public:
			enum DmaRequestEnable
			{
				UPDATE_DMA_REQUEST_INTERRUPT = TIM_DIER_UDE,
				CAPTURE_COMPARE_1_DMA_REQUEST_INTERRUPT = TIM_DIER_CC1DE,
				CAPTURE_COMPARE_2_DMA_REQUEST_INTERRUPT = TIM_DIER_CC2DE,
				CAPTURE_COMPARE_3_DMA_REQUEST_INTERRUPT = TIM_DIER_CC3DE,
				CAPTURE_COMPARE_4_DMA_REQUEST_INTERRUPT = TIM_DIER_CC4DE,
				COM_DMA_REQUEST_INTERRUPT = TIM_DIER_COMDE,
				TRIGGER_DMA_REQUEST_INTERRUPT = TIM_DIER_TDE,
			};

			enum Interrupt
			{
				INTERRUPT_UPDATE = TIM_DIER_UIE,
				INTERRUPT_CAPTURE_COMPARE_1 = TIM_DIER_CC1IE,
				INTERRUPT_CAPTURE_COMPARE_2 = TIM_DIER_CC2IE,
				INTERRUPT_CAPTURE_COMPARE_3 = TIM_DIER_CC3IE,
				INTERRUPT_CAPTURE_COMPARE_4= TIM_DIER_CC4IE,
				INTERRUPT_COM = TIM_DIER_COMIE,
				INTERRUPT_TRIGGER = TIM_DIER_TIE,
				INTERRUPT_BREAK = TIM_DIER_BIE,
			};

			enum InterruptFlag
			{
				FLAG_UPDATE = TIM_SR_UIF,
				FLAG_CAPTURE_COMPARE_1 = TIM_SR_CC1IF,
				FLAG_CAPTURE_COMPARE_2 = TIM_SR_CC2IF,
				FLAG_CAPTURE_COMPARE_3 = TIM_SR_CC3IF,
				FLAG_CAPTURE_COMPARE_4 = TIM_SR_CC4IF,
				FLAG_COM = TIM_SR_COMIF,
				FLAG_TRIGGER = TIM_SR_TIF,
				FLAG_BREAK = TIM_SR_BIF,
				FLAG_OVERCAPTURE_1 = TIM_SR_CC1OF,
				FLAG_OVERCAPTURE_2 = TIM_SR_CC2OF,
				FLAG_OVERCAPTURE_3 = TIM_SR_CC3OF,
				FLAG_OVERCAPTURE_4 = TIM_SR_CC4OF,
			};

			enum SlaveModeTrigger
			{
			};

			enum SlaveMode
			{
				SLAVE_DISABLED = 0,
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
			};

			enum OutputCompareMode
			{
				/// Output is independent from the compare result
				OUTPUT_INACTIVE = 0,
				/// Output is forced high on match
				OUTPUT_HIGH_ON_MATCH = TIM_CCMR1_OC1M_0,
				/// Output is forced low on match
				OUTPUT_LOW_ON_MATCH = TIM_CCMR1_OC1M_1,
				/// Output is toggled on match
				OUTPUT_TOGGLE = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0,
				/// Output is forced low
				OUTPUT_FORCE_LOW = TIM_CCMR1_OC1M_2,
				/// Output is forced high
				OUTPUT_FORCE_HIGH = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0,

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

			enum OutputComparePreload
			{
				PRELOAD_REGISTER_DISABLED = 0,
				PRELOAD_REGISTER_ENABLED  = TIM_CCMR1_OC1PE,
			};


			enum OutputComparePolarity
			{
				OUTPUT_ACTIVE_HIGH = 0,
				OUTPUT_ACTIVE_LOW = TIM_CCER_CC1P,
			};

			/**
			 * If more than one Compare Channel is available they are always paired.
			 * 		(A channel with an odd number is paired with the next even numbered channel)
			 * It is possible to choose as trigger for the input capture functionality either
			 * the own timer input or the input associated with the paired channel.
			 *
			 * For working with Internal Triggers, TRC can also be selected.
			 */
			enum InputCaptureMapping
			{
				INPUT_OWN 	= 1,
				INPUT_OTHER	= 2,
				INPUT_TRC 	= 3,
			};

			enum InputCapturePolarity
			{
				INPUT_RISING	= 0,
				INPUT_FALLING	= TIM_CCER_CC1P,
				INPUT_BOTH		= TIM_CCER_CC1NP | TIM_CCER_CC1P,
			};

			enum InputCapturePrescaler
			{
				INPUT_DIV1	= 0,
				INPUT_DIV2	= 1,
				INPUT_DIV4	= 2,
				INPUT_DIV8	= 3,
			};

			enum PinState
			{
				DISABLE = 0,
				ENABLE = 1,
			};

			/* Different Resolution Depending on DeadTime[7:5]: */
			enum DeadTimeResolution
			{
				FROM_0_125NS_STEP = 0,									//0xx
				FROM_16US_250NS_STEP = TIM_BDTR_DTG_7,					//10x
				FROM_32US_1US_STEP = TIM_BDTR_DTG_7 | TIM_BDTR_DTG_6,	//110
				FROM_64US_2US_STEP = TIM_BDTR_DTG_7 | TIM_BDTR_DTG_6	//111
													| TIM_BDTR_DTG_5,
			};

		public:
			/**
			 * Set operation mode of the timer
			 *
			 * In Encoder mode the encoder channels A and B must be connected
			 * to channel 1 and 2 of the timer (e.g. TIM2_CH1 and TIM2_CH2).
			 */
			static void
			setMode(Mode mode, SlaveMode slaveMode = SLAVE_DISABLED,
					SlaveModeTrigger slaveModeTrigger = (SlaveModeTrigger) 0);
			
		public:
			/**
			 * TODO Description
			 */
			void
			configureInputChannel(uint32_t channel, InputCaptureMapping input,
					InputCapturePrescaler prescaler, InputCapturePolarity polarity, uint8_t filter);

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
			/*
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

#endif // XPCC_STM32__TIMER_BASE_HPP
