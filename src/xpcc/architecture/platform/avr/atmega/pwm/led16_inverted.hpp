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
 * $Id: led16_inverted.hpp 612 2011-10-01 21:27:09Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATMEGA__PWM_LED16_INVERTED_HPP
#define XPCC_ATMEGA__PWM_LED16_INVERTED_HPP

#include <xpcc/driver/pwm/led.hpp>
#include <xpcc/architecture/driver/atomic/lock.hpp>
#include <xpcc/math/utils/operator.hpp>

namespace xpcc
{
	namespace atmega
	{
		namespace pwm
		{
			/**
			 * \brief Basic alpha corrected LED driver using 16bit hardware PWM
			 *
			 * This class supplies basic methods to fade an LED to any brightness
			 * level.
			 * A 1000 value long lookup-table stores the alpha corrected PWM values,
			 * so that no calculation has to be done at run time.
			 *
			 * This class needs to be provided with an output compare register of
			 * a 16bit timer, that has to be set up seperately in non-PWM mode, to
			 * be able to use all Output Compare interrupts, where the LED is set.
			 * When the OVF interrupt occurs, the timer value is manually written
			 * to 0xEFFF and the LED is reset.
			 * This is why the values in the look up table are so high.
			 *
			 * To syncronize the brightness of the connected LED with other LEDs,
			 * limit the global brightness of the LED in the contructor.
			 *
			 * This class uses a 10kHz clock for the fading timer.
			 * You have to set up xpcc::FlexClock at ~10kHz for that!
			 *
			 * \warning You cannot use this class for LEDs connected to the
			 *			Output Compare pins, use Led16.
			 * \warning This class uses additional 2000 bytes of flash memory
			 *			to store the look up table!
			 *
			 * Example:
			 * \code
			 * // set up xpcc::FlexClock at 10kHz
			 * ISR(TIMERn_COMPx_vect) {
			 *	   xpcc::FlexClock::increment();
			 * }
			 *
			 * // Output Compare Interrupt
			 * ISR(TIMERn_COMPx_vect) {
			 *     Led::set();
			 * }
			 * // Overflow Interrupt
			 * ISR(TIMERn_OVF_vect) {
			 *     Led::reset();
			 *	   TCNTn = 0xFFFF - 4096; // = 0xEFFF
			 * }
			 * \endcode
			 *
			 * \see Led8
			 * \see Led16
			 *
			 * \author	Niklas Hauser
			 * \ingroup atmega_pwm
			 */
			class Led16Inverted : public ::xpcc::pwm::Led
			{
			private:
				const uint16_t maximum;
				volatile uint16_t* compareRegister;
				uint16_t value;
				xpcc::Timeout<xpcc::FlexClock> timer;
				bool fading;
				uint16_t fadeTime;
				uint16_t startValue;
				uint16_t endValue;
				
			public:
				/**
				 * \param	outputCompare	register of a 16bit timer
				 * \param	maximum			brightness of the LED
				 */
				Led16Inverted(volatile uint16_t* outputCompare, const float maximum=1.0f);
				
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
				 * \param fadeTime	specify the fade up time in ms
				 */
				void
				on(uint16_t fadeTime=70);
				
				/**
				 * Mimmics the behaviour of normal lamps, which take a small amount
				 * of time until fully extinguishing.
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
				setValue(uint16_t brightness);
				
				void
				fadeTo(uint16_t time, uint16_t brightness);
			};
		}
	}
	
	namespace pwm
	{
		/**
		 * Contains the lookup table for the alpha corrected pwm values.
		 */
		EXTERN_FLASH_STORAGE(uint16_t PwmTable16Inverted[1000]);
	}
}

#include <xpcc/driver/pwm/led16_inverted_table.hpp>
#include "led16_inverted_impl.hpp"

#endif	// XPCC_ATMEGA__PWM_LED16_INVERTED_HPP
