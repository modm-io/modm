// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_PULSE_HPP
#define XPCC_UI_PULSE_HPP

#include <stdint.h>
#include "led.hpp"

namespace xpcc
{
namespace ui
{
/**
 * LED Pulser
 *
 * This class fades an LED up and down during the period.
 *
 * @see		Led
 * @author	Niklas Hauser
 * @ingroup led
 */
class Pulse
{
	Led& led;
	uint16_t halfPeriod;
	uint8_t counter;
	bool pulseDirection;
	bool isPulsing;
	bool isCounting;

public:
	/**
	 * @param	led
	 * 		an object that inherits from xpcc::ui::Led
	 * @param	period
	 *		pulse cycle period in ms.
	 */
	Pulse(Led& led, uint16_t period=1000)
	:	led(led), halfPeriod(period/2), counter(0),
		pulseDirection(false), isPulsing(false), isCounting(false)
	{
	}

	/// set new period
	inline void
	setPeriod(uint16_t period)
	{
		halfPeriod = period / 2;
	}

	/// start pulsing forever
	ALWAYS_INLINE void
	start()
	{
		isPulsing = true;
		isCounting = false;
	}

	/// Stops pulsing after finishing the current cycle
	ALWAYS_INLINE void
	stop()
	{
		isPulsing = false;
	}

	ALWAYS_INLINE bool
	isRunning()
	{
		return isPulsing;
	}

	/// Pulses a number of times and then stops
	inline void
	pulse(uint8_t times=1)
	{
		if (times) {
			counter = times;
			isPulsing = true;
			isCounting = true;
		}
		else isPulsing = false;
	}

	/// Must be called at least every ms
	void
	run()
	{
		led.update();

		if (!led.isFading() && (isPulsing || !pulseDirection))
		{
			if (pulseDirection) {
				led.on(halfPeriod);

				if (isCounting && !--counter) {
					isPulsing = false;
					isCounting = false;
				}
			}
			else {
				led.off(halfPeriod);
			}
			pulseDirection = !pulseDirection;
		}
	}
};

}

}

#endif	// XPCC_UI_PULSE_HPP
