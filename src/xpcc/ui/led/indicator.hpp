// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_INDICATOR_HPP
#define XPCC_UI_INDICATOR_HPP

#include <stdint.h>
#include "led.hpp"

namespace xpcc
{

namespace ui
{

/**
 * LED Indicator.
 *
 * This class provided smooth on and off phases, much like a
 * blinker/indicator on cars.
 *
 * @see		Led
 * @author	Niklas Hauser
 * @ingroup led
 */
class Indicator
{
	Led& led;
	uint16_t on;
	uint16_t off;
	uint8_t onFade;
	uint8_t offFade;
	uint8_t counter;
	bool blinkDirection;
	bool isBlinking;
	bool isCounting;

	xpcc::Timeout<> timer;

public:
	/**
	 * @param	led
	 * 		an object that inherits from xpcc::ui::Led
	 * @param	period
	 * 		indication cycle period in ms
	 * @param	dutyCycle
	 * 		on part of the period in percent
	 * @param	onFade
	 * 		time in ms until the LED is fully on
	 * @param	offFade
	 * 		time in ms until the LED is fully off
	 */
	Indicator(Led& led, uint16_t period=1000, float dutyCycle=0.45f,
			  uint8_t onFade=75, uint8_t offFade=110)
	:	led(led), on(period * dutyCycle), off(period - on), onFade(onFade), offFade(offFade),
	counter(0), blinkDirection(true), isBlinking(false), isCounting(false)
	{
	}

	/// set new period
	inline void
	setPeriod(uint16_t period, float dutyCycle=0.45f)
	{
		on = period * dutyCycle;
		off = period - on;
	}

	/// start indicating for ever
	ALWAYS_INLINE void
	start()
	{
		isBlinking = true;
		isCounting = false;
	}

	/// Stops indicating after finishing the current cycle
	ALWAYS_INLINE void
	stop()
	{
		isBlinking = false;
	}

	ALWAYS_INLINE bool
	isRunning()
	{
		return isBlinking;
	}

	/// Indicate a number of times and then stop
	inline void
	indicate(uint8_t times=1)
	{
		if (times) {
			counter = times;
			isBlinking = true;
			isCounting = true;
		}
	}

	/// Must be called at least every ms
	void
	run()
	{
		led.update();

		if (timer.isExpired() && (isBlinking || !blinkDirection))
		{
			if (blinkDirection)
			{
				led.on(onFade);

				if (isCounting && !--counter) {
					isBlinking = false;
					isCounting = false;
				}

				timer.restart(on);
			}
			else
			{
				led.off(offFade);

				timer.restart(off);
			}
			blinkDirection = !blinkDirection;
		}
	}
};

}

}

#endif	// XPCC_UI_INDICATOR_HPP
