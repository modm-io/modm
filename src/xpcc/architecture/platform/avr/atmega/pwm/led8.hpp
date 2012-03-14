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

#ifndef XPCC_ATMEGA__PWM_LED8_HPP
#define XPCC_ATMEGA__PWM_LED8_HPP

#include <xpcc/driver/pwm/led.hpp>
#include <xpcc/math/utils/operator.hpp>

namespace xpcc
{
	namespace atmega
	{
		namespace pwm
		{
			/**
			 * \brief Basic alpha corrected LED driver using 8bit hardware PWM
			 *
			 * This class supplies basic methods to fade an LED to any brightness
			 * level.
			 * A lookup-table stores the alpha corrected PWM values, so that no
			 * calculation has to be done at run time.
			 *
			 * This class needs to be provided with an output compare register of
			 * an 8bit timer, that has to be set up seperately, preferably in Fast 
			 * PWM Mode and with the LED connected to the Output Compare pins.
			 *
			 * To syncronize the brightness of the connected LED with other LEDs,
			 * limit the global brightness of the LED in the contructor.
			 *
			 * This class uses a 10kHz clock for the fading timer.
			 * You have to set up xpcc::FlexClock at ~10kHz for that!
			 *
			 * Make sure that when \c OCRnx reads \c 0 the LED does not get switched
			 * on during the COMPx interrupt, as it can cause flickering.
			 * This can be done in software by either not switching the LED on in
			 * the overflow interrupt, or by switching the compare interrupt off.
			 *
			 * \warning Do not expect too much when using this class to smoothly
			 *			fade an LED in low light situations, as the resolution
			 *			is only 1/256. Use Led16 for high performance.
			 *
			 * Example (LED connected to any IO pin):
			 * \code
			 * // Output Compare Interrupt
			 * ISR(TIMERn_COMPx_vect) {
			 *     Led::reset();
			 * }
			 * // Overflow Interrupt
			 * // do not switch LED on, when OCRxn reads 0
			 * ISR(TIMERn_OVF_vect) {
			 *     if (OCRnx) Led::set();
			 * }
			 * \endcode
			 *
			 * Example (LED connected to Output Compare pin):
			 * \code
			 * // Overflow Interrupt
			 * // do not switch LED on, when OCRxn reads 0
			 * ISR(TIMERn_OVF_vect) {
			 *     if (OCRnx) switchCompareInterruptOff();
			 *     else switchCompareInterruptOn();
			 * }
			 * \endcode
			 *
			 * \see Led16
			 * \see Led16Inverted
			 *
			 * \author	Niklas Hauser
			 * \ingroup atmega_pwm
			 */
			class Led8 : public ::xpcc::pwm::Led
			{
			private:
				const uint8_t maximum;
				volatile uint8_t* compareRegister;
				uint8_t value;
				xpcc::Timeout<xpcc::FlexClock> timer;
				bool fading;
				uint16_t fadeTime;
				uint8_t startValue;
				uint8_t endValue;
				
			public:
				/**
				 * \param	outputCompare	register of a 8bit timer
				 * \param	maximum			brightness of the LED
				 */
				Led8(volatile uint8_t* outputCompare, const float maximum=1.0f);
				
				/**
				 * Set the brightness of the LED as a normalized float.
				 */
				void ALWAYS_INLINE
				setValue(float brightness);
				
				/**
				 * \return the normalized brightness of the LED.
				 */
				float
				getValue();
				
				/**
				 * \return \c true if LED is currently fading to another brightness,
				 *		   \c false if otherwise
				 */
				bool
				isFading();
				
				/**
				 * Fade to a normalized brightness in the specified ms.
				 */
				void ALWAYS_INLINE
				fadeTo(uint16_t time, float brightness);
				
				/**
				 * Mimmics the behaviour of normal lamps, which take a small amount
				 * of time until achiving full brightness.
				 * \param fade		set to false to switch the LED on without fading
				 * \param fadeTime	specify the fade up time in ms
				 */
				void
				on(uint16_t fadeTime=70);
				
				/**
				 * Mimmics the behaviour of normal lamps, which take a small amount
				 * of time until fully extinguishing.
				 * \param fade		set to false to switch the LED off without fading
				 * \param fadeTime	specify the fade down time in ms
				 */
				void
				off(uint16_t fadeTime=70);
				
				/**
				 * Must be called periodically, preferably in the main while loop.
				 */
				bool
				run();
				
			private:
				void
				setValue(uint8_t brightness);
				
				void
				fadeTo(uint16_t time, uint8_t brightness);
			};
		}
	}
	
	namespace pwm
	{
		/**
		 * Contains the lookup table for the alpha corrected pwm values.
		 */
		EXTERN_FLASH_STORAGE(uint8_t PwmTable8[256]);
	}
}

#include <xpcc/driver/pwm/led8_table.hpp>
#include "led8_impl.hpp"

#endif	// XPCC_ATMEGA__PWM_LED8_HPP
