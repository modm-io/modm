// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_DOUBLE_INDICATOR_HPP
#define XPCC_UI_DOUBLE_INDICATOR_HPP

#include <stdint.h>
#include "led.hpp"

namespace xpcc
{

namespace ui
{

/**
 * LED Double Indicator
 *
 * @author	Niklas Hauser
 * @ingroup led
 */
class DoubleIndicator
{
	xpcc::Timeout<> timer;
	Led& led;
	uint16_t on1;
	uint16_t pause1;
	uint16_t on2;
	uint16_t pause2;
	uint8_t onFade;
	uint8_t offFade;
	uint8_t counter;
	bool isBlinking;
	bool isCounting;

	enum
	{
		FIRST_FLASH,
		FIRST_BREAK,
		SECOND_FLASH,
		SECOND_BREAK
	} state;

public:
	/**
	 * @param	led
	 * 		an object that inherits from xpcc::ui::Led
	 * @param	period
	 * 		of the indicating cycle in ms
	 * @param	on1
	 * 		length of the first "on" part in percent
	 * @param	pause
	 * 		length of the first "off" part in percent
	 * @param	on2
	 * 		length of the second "on" part in percent
	 * @param	onFade
	 * 		time in ms until the LED is fully on
	 * @param	offFade
	 * 		time in ms until the LED is fully off
	 */
	DoubleIndicator(Led& led, uint16_t period=1000, float on1=0.1f,
					float pause=0.2f, float on2=0.1f,
					uint8_t onFade=60, uint8_t offFade=90)
	:	led(led), on1(period * on1), pause1(period * pause),
	on2(period * on2), pause2(period - on1 - pause1 - on2),
	onFade(onFade), offFade(offFade), counter(0),
	isBlinking(false), isCounting(false), state(FIRST_FLASH)
	{
	}

	/// set new period
	inline void
	setPeriod(uint16_t period, float on1=0.1f,
			float pause=0.2f, float on2=0.1f)
	{
		this->on1 = period * on1;
		this->pause1 = period * pause;
		this->on2 = period * on2;
		this->pause2 = period - this->on1 - this->pause1 - this->on2;
	}

	/// start indicating for ever
	ALWAYS_INLINE void
	start()
	{
		state = FIRST_FLASH;
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
		led.run();

		if (timer.isExpired() && (isBlinking || state == FIRST_BREAK || state == SECOND_BREAK))
		{
			switch (state)
			{
				case FIRST_FLASH:
					led.on(onFade);

					timer.restart(on1);
					state = FIRST_BREAK;
					break;

				case FIRST_BREAK:
					led.off(offFade);

					timer.restart(pause1);
					state = SECOND_FLASH;
					break;

				case SECOND_FLASH:
					led.on(onFade);

					if (isCounting && !--counter) {
						isBlinking = false;
						isCounting = false;
					}

					timer.restart(on2);
					state = SECOND_BREAK;
					break;

				case SECOND_BREAK:
					led.off(offFade);

					timer.restart(pause2);

				default:
					state = FIRST_FLASH;
					break;
			}
		}
	}
};

}

}

#endif	// XPCC_UI_DOUBLE_INDICATOR_HPP
