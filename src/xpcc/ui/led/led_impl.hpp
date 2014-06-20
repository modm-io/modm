// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_LED_HPP
#	error	"Don't include this file directly, use 'led.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
xpcc::ui::Led::Led()
:	currentValue(0), startValue(0), endValue(0), deltaValue(0), fadeTime(1),
	 previous(0)
{
}

void inline
xpcc::ui::Led::setBrightness(uint8_t brightness)
{
	fadeTime = 0;
	currentValue = brightness;

	setValue(currentValue);
}

uint8_t ALWAYS_INLINE
xpcc::ui::Led::getBrightness() const
{
	return currentValue;
}

bool ALWAYS_INLINE
xpcc::ui::Led::isFading() const
{
	return (fadeTime != 0);
}

void
xpcc::ui::Led::fadeTo(uint16_t time, uint8_t brightness)
{
	if (brightness == currentValue)
		return;

	if (time == 0) {
		setBrightness(brightness);
		return;
	}

	startValue = static_cast<uint16_t>(currentValue)*100;
	endValue = brightness;
	int16_t delta = (static_cast<int16_t>(endValue) - currentValue)*100;
	deltaValue = delta / time;
	if (deltaValue == 0)
		deltaValue = delta > 0 ? 1 : -1;
	fadeTime = time;
	previous = xpcc::Clock::now();
}

void ALWAYS_INLINE
xpcc::ui::Led::on(uint16_t time)
{
	fadeTo(time, 255);
}

void ALWAYS_INLINE
xpcc::ui::Led::off(uint16_t time)
{
	fadeTo(time, 0);
}

void
xpcc::ui::Led::update()
{
	// this should be called exactly once every 1 ms
	// but if the clock gets incremented by more than 1 ms, or the main loop is
	// busy, then we need to count these "missing" steps and apply them.

	if (fadeTime)
	{
		xpcc::Timestamp now = xpcc::Clock::now();
		uint8_t delta = (now - previous).getTime();

		if (delta)
		{
			previous = now;

			while (delta--)
			{
				startValue += deltaValue;
				currentValue = startValue/100;

				if (--fadeTime == 0) {
					currentValue = endValue;
					break;
				}
				if (currentValue == endValue) {
					deltaValue = 0;
				}
			}

			setValue(currentValue);
		}
	}
}
