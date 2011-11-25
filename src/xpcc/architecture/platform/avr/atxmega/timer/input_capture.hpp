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

#ifndef XPCC_ATXMEGA__TIMER_INPUT_CAPTURE_HPP
#define XPCC_ATXMEGA__TIMER_INPUT_CAPTURE_HPP

#include <avr/io.h>
#include <xpcc/architecture/platform/avr/atxmega/gpio.hpp>
#include <stdint.h>

#include "timer.hpp"

// older versions of avr-libc provide false register names
#ifndef TC_EVACT_FRQ_gc
#	define TC_EVACT_FRQ_gc		TC_EVACT_FRW_gc
#endif

namespace xpcc
{
	namespace atxmega
	{
		namespace input_capture
		{
			enum Mode
			{
				NORMAL = TC_EVACT_CAPT_gc,
				FREQUENCY = TC_EVACT_FRQ_gc,
				PULSEWIDTH = TC_EVACT_PW_gc
			};
		}
		
		/**
		 * \brief A generic Input Capture implementation.
		 * 
		 * This template takes care of configuring the Pin, the Timer and the Event
		 * System for all three Input Capture Modes.
		 * The result is read and buffered independently to make sure no Error
		 * condition is encountered.
		 * The Timer Channel Interrupt is handled automatically, but may manually 
		 * be overridden.
		 * 
		 * \tparam TIMER an Interrupt Timer/Counter 0 or 1 to configure and use.
		 * \tparam PIN any GPIO Input pin at which the signal is connected
		 *
		 * \ingroup atxmega_timer
		 * \author	Niklas Hauser
		 */
		template < typename TIMER,
				   typename PIN >
		class InputCapture
		{
		public:
			/**
			 * \brief Calculates all the settings to set up the Event System.
			 * 
			 * This methods sets up the Event System to route the Pin Interrupt via
			 * the specified Multiplexer Channel to the Timer Channel and configures
			 * the Timer into the specified Input Capture Mode.
			 * Frequency Capture requires a rising edge trigger, Pulse Width Capture
			 * requires triggering on both edges (rising then falling edge).
			 * You can invert this by configuring the pin as inverted.
			 * In these two modes the Input Sense is automatically overwritten.
			 * 
			 * If you call this with constants, the compiler does all the
			 * calculations for the settings, not the Xmega.
			 * 
			 * \param eventChannel an unused Event System Multiplexer Channel
			 * \param clock Timer prescaler
			 * \param action Input Capture Mode
			 * \param timerChannel an unused Timer Compare/Capture Channel
			 * \param pinConfig configuration of the interrupt pin
			 * \param inputSense trigger sense of the interrupt pin
			 * 					(only available in NORMAL mode)
			 * \return \c false if Channel C or D is requested of a TC type 1.
			 */
			static bool
			initialize(register8_t& eventChannel=EVSYS_CH0MUX,
					TC_CLKSEL_t clock=TC_CLKSEL_DIV1024_gc,
					input_capture::Mode action=input_capture::NORMAL,
					timer::Channel timerChannel=timer::CHANNELA,
					Configuration pinConfig=NORMAL,
					InputSense inputSense=RISING);
			
			inline static bool
			isDataAvailable();
			
			/// \return the buffered, most recent result
			inline static uint16_t
			getData();
			
		private:
			inline static void
			captureData();
			
			static TIMER timer;
			static PIN pin;
			
			static register16_t* captureRegister;
			static uint16_t capture;
			static bool newData;
		};
	}
}

#include "input_capture_impl.hpp"

#endif // XPCC_ATXMEGA__TIMER_INPUT_CAPTURE_HPP
