// coding: utf-8

#ifndef XPCC__XMEGA_TIMER_INPUT_CAPTURE_HPP
#define XPCC__XMEGA_TIMER_INPUT_CAPTURE_HPP

#include <avr/io.h>
#include <stdint.h>
#include <xpcc/architecture/driver/atxmega/timer.hpp>

namespace xpcc
{
	namespace input_capture {
		enum Mode
		{
			NORMAL = TC_EVACT_CAPT_gc,
			FREQUENCY = TC_EVACT_FRW_gc,
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
	 * \ingroup atxmega_timer
	 * \author	Niklas Hauser
	 * \tparam TIMER an Interrupt Timer/Counter 0 or 1 to configure and use.
	 * \tparam PIN any GPIO Input pin at which the signal is connected
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
		 * 
		 * If you call this with constants, the compiler does all the
		 * calculations for the settings, not the Xmega.
		 * 
		 * \param eventChannel an unused Event System Multiplexer Channel
		 * \param clock Timer prescaler
		 * \param action Input Capture Mode
		 * \param timerChannel an unused Timer Compare/Capture Channel
		 * \return \c false if Channel C or D is requested of a TC type 1.
		 */
		static bool
		initialize(register8_t& eventChannel=EVSYS_CH0MUX,
				   TC_CLKSEL_t clock=TC_CLKSEL_DIV1024_gc,
				   input_capture::Mode action=input_capture::NORMAL,
				   timer::Channel timerChannel=timer::CHANNELA);
		
		inline static bool
		isDataAvailable();
		
		/// \returns the buffered, most recent result
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

#include "input_capture_impl.hpp"

#endif // XPCC__XMEGA_TIMER_INPUT_CAPTURE_HPP
