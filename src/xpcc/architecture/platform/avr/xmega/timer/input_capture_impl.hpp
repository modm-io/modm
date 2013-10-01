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

#ifndef XPCC_ATXMEGA__TIMER_INPUT_CAPTURE_HPP
	#error	"Don't include this file directly, use 'input_capture.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename TIMER, typename PIN>
TIMER xpcc::xmega::InputCapture<TIMER, PIN>::timer;

template <typename TIMER, typename PIN>
PIN xpcc::xmega::InputCapture<TIMER, PIN>::pin;

template <typename TIMER, typename PIN>
uint16_t xpcc::xmega::InputCapture<TIMER, PIN>::capture(0);

template <typename TIMER, typename PIN>
bool xpcc::xmega::InputCapture<TIMER, PIN>::newData(false);

template <typename TIMER, typename PIN>
register16_t* xpcc::xmega::InputCapture<TIMER, PIN>::captureRegister;

// ----------------------------------------------------------------------------
template <typename TIMER, typename PIN>
bool
xpcc::xmega::InputCapture<TIMER, PIN>::initialize(
		register8_t& eventChannel,
		TC_CLKSEL_t clock,
		input_capture::Mode action,
		timer::Channel timerChannel,
		Configuration pinConfig,
		InputSense inputSense)
{
	// first check if we can actually access the timerChannel
	if (!timer.isType0() && (timerChannel > xpcc::timer::CHANNELB)) {
		return false;
	}
	
	// configure the pin
	pin.setInput(pinConfig);
	
	// override input sense if neccessary
	if (action == input_capture::FREQUENCY) {
		inputSense = RISING;
	}
	else if (action == input_capture::PULSEWIDTH) {
		inputSense = BOTHEDGES;
	}
	pin.configureInputSense(inputSense);
	
	// Get the Event Channel Mux Input Selection from the Pins memory location
	eventChannel = pin.getEventChannelMuxInput();
	
	// Enable the Capture/Compare Channel
	timer.enableCompareCapture(1 << (timerChannel + 4));
	
	// set and enable interrupt level, use the CCA interrupt level
	timer.attachCompareCaptureInterrupt(timerChannel, TC_CCAINTLVL_MED_gc, captureData);
	
	// Get the Event Channel number from its memory location, 8 - 15
	uint8_t evChannel = static_cast<uint8_t>
		(reinterpret_cast<uint16_t>(&eventChannel) - 0x0180);
	// make sure the event source is not negative and wraps around 3 bits
	timer.setEventSource(static_cast<TC_EVSEL_t>((8 + evChannel - timerChannel)%8 + 8));
	
	// Get the Channel CC register
	captureRegister = reinterpret_cast<register16_t*>
		(reinterpret_cast<uint16_t>(&timer.getModuleBase()) + 0x0028 + 2*timerChannel);

	// tell the timer what to do: Input, Frequency or Pulse-width Capture
	timer.setEventAction(static_cast<TC_EVACT_t>(action));
	
	// Enable the timer module
	timer.setClockSource(clock);
	
	return true;
}

template <typename TIMER, typename PIN>
inline void
xpcc::xmega::InputCapture<TIMER, PIN>::captureData()
{
	capture = *captureRegister;
	newData = true;
}

template <typename TIMER, typename PIN>
inline bool
xpcc::xmega::InputCapture<TIMER, PIN>::isNewDataAvailable()
{
	return newData;
}

template <typename TIMER, typename PIN>
inline uint16_t
xpcc::xmega::InputCapture<TIMER, PIN>::getData()
{
	newData = false;
	return capture;
}

