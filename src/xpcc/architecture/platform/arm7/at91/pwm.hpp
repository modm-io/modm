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

#ifndef XPCC_AT91__PWM_HPP
#define XPCC_AT91__PWM_HPP

#include "aic.hpp"
#include "device.h"

namespace xpcc
{
	namespace at91
	{
		/**
		 * @brief	Pulse Width Modulation Controller (PWM)
		 * 
		 * The PWM controller provides four channels of 16-bit PWM.
		 * 
		 * @see		PwmChannel
		 * 
		 * @date	July 2011
		 * @author	Fabian Greif
		 * @ingroup	at91
		 */
		class Pwm
		{
		public:
			enum Clock
			{
				CLOCK_A,
				CLOCK_B,
			};
			
			enum Prescaler
			{
				PRESCALER_1,
				PRESCALER_2,
				PRESCALER_4,
				PRESCALER_8,
				PRESCALER_16,
				PRESCALER_32,
				PRESCALER_64,
				PRESCALER_128,
				PRESCALER_256,
				PRESCALER_512,
				PRESCALER_1024
			};
			
			enum Channel
			{
				CHANNEL_0 = (1 << 0),
				CHANNEL_1 = (1 << 1),
				CHANNEL_2 = (1 << 2),
				CHANNEL_3 = (1 << 3),
			};
			
		public:
			/**
			 * @brief	Enable PWM clock
			 * 
			 * The PWM is not continuously clocked. The programmer must first
			 * enable the PWM clock. However, if the application does not require
			 * PWM operations, the PWM clock can be stopped when not needed
			 * and be restarted later. In this case, the PWM will resume
			 * its operation where it left off.
			 * 
			 * Configuring the PWM does not require the PWM clock to be enabled.
			 */
			static void
			enable();
			
			/**
			 * @brief	Disable PWM clock
			 */
			static void
			disable();
			
			/**
			 * @brief	Configure clock generator A or B
			 */
			static void
			configureClockGenerator(Clock clock, Prescaler prescaler, uint8_t divideFactor);
			
			/**
			 * 
			 * @param	mask
			 * @see		Pwm::Channel
			 */
			static void
			enableChannels(uint32_t mask);
			
			/**
			 * @brief	
			 * @param	mask	Channels to disable
			 * @see		Pwm::Channel
			 */
			static void
			disableChannels(uint32_t mask);
			
			/**
			 * @brief	Enable interrupt for a channel
			 * 
			 * All channel interrupts use a common interrupt handler.
			 * 
			 * @param	mask	Channel mask (see Pwm::Channel)
			 * 
			 * @see		Pwm::Channel
			 * @see		Pwm::registerInterruptHandler()
			 */
			inline void
			enableInterrupt(uint32_t mask)
			{
				AT91C_BASE_PWMC->PWMC_IER = mask;
			}
			
			/**
			 * @brief	Disable interrupt for a channel
			 * 
			 * @param	mask	Channel mask (see Pwm::Channel)
			 * @see		Pwm::Channel
			 */
			inline void
			disableInterrupt(uint32_t mask)
			{
				AT91C_BASE_PWMC->PWMC_IDR = mask;
			}
			
			/**
			 * @brief	Set a interrupt handler function
			 * 
			 * The handler function needs to read PIT_PIVR in order to
			 * acknowledge the interrupt.
			 */
			static void
			registerInterruptHandler(Aic::Handler function);
		};
		
		/**
		 * @brief	PWM Channel
		 * 
		 * @date	July 2011
		 * @author	Fabian Greif
		 * @ingroup	at91
		 */
		class PwmChannel
		{
		public:
			/**
			 * @see		Pwm::configureClockGenerator() for CLOCK_A and CLOCK_B
			 */
			enum Clock
			{
				PRESCALER_1,	///< F_CPU / 1
				PRESCALER_2,	///< F_CPU / 2
				PRESCALER_4,	///< F_CPU / 4
				PRESCALER_8,	///< F_CPU / 8
				PRESCALER_16,	///< F_CPU / 16
				PRESCALER_32,	///< F_CPU / 32
				PRESCALER_64,	///< F_CPU / 64
				PRESCALER_128,	///< F_CPU / 128
				PRESCALER_256,	///< F_CPU / 256
				PRESCALER_512,	///< F_CPU / 512
				PRESCALER_1024,	///< F_CPU / 1024
				CLOCK_A,		///< Clock A from the PWM clock generator
				CLOCK_B			///< Clock A from the PWM clock generator
			};
			
			/**
			 * @brief	Channel configuration
			 * 
			 * - The PWM might be left or center aligned.
			 * - Inverted Mode: PWM starts with high level, otherwise it
			 *   starts with low level
			 */
			enum Configuration
			{
				LEFT_ALIGNED = 0,					//!< LEFT_ALIGNED
				CENTER_ALIGNED = AT91C_PWMC_CALG,	//!< CENTER_ALIGNED
				INVERTED_MODE = AT91C_PWMC_CPOL,	//!< INVERTED_MODE
				NORMAL_MODE = 0,					//!< NORMAL_MODE
			};
			
		public:
			/**
			 * @brief	Access a PWM channel
			 * 
			 * @param	channel		Channel number [0..3]
			 */
			PwmChannel(uint32_t channel);
			
			/**
			 * @brief	Configure PWM channel
			 * 
			 * If the period is left aligned the counter counts from 0 to
			 * \c period and is reset. The resulting PWM frequency is:
			 * 
			 * \code
			 * (Prescaler * period) / f_cpu
			 * \code
			 * 
			 * If the period is center aligned the counter counts upwards to
			 * \c period and than downwards back to zero. Therefore the
			 * resulting PWM frequency is:
			 * 
			 * \code
			 * (2 * Prescaler * period) / f_cpu
			 * \endcode
			 * 
			 * The internal counter is reset to 0 zero by this function.
			 * 
			 * @param	clock				Clock source
			 * @param	period				PWM period
			 * @param	configurationMask	see PwmChannel::Configuration
			 */
			void
			configure(Clock clock, uint32_t period, uint32_t configurationMask);
			
			/**
			 * @brief	Set a new duty cycle
			 * 
			 * The duty cycle is updated at the next start of a period.
			 * 
			 * From the Errata Sheet:
			 * Setting Channel Duty Cycle at 0 in center aligned mode or
			 * at 0 or 1 in left aligned mode may change the polarity of the
			 * signal.
			 * 
			 * Problem Fix/Workaround:
			 * - Do not set PWM_CDTYx at 0 in center aligned mode.
			 * - Do not set PWM_CDTYx at 0 or 1 in left aligned mode.
			 * 
			 * @param	dutyCycle		Must be in range 1-2 to \c period
			 * 							(set during configure())
			 * @see		configure()
			 */
			inline void
			setDutyCycle(uint32_t dutyCycle)
			{
				base->PWMC_CUPDR = dutyCycle;
			}
			
		protected:
			AT91S_PWMC_CH *base;
		};
	}
}

#endif // XPCC_AT91__PWM_HPP
