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
	timer(1)
{
}

void
xpcc::ui::Led::setBrightness(uint8_t brightness)
{
	fadeTime = 0;
	currentValue = brightness;

	setValue(currentValue);
}

uint8_t
xpcc::ui::Led::getBrightness()
{
	return currentValue;
}

bool
xpcc::ui::Led::isFading()
{
	return static_cast<bool>(fadeTime != 0);
}

void
xpcc::ui::Led::fadeTo(uint16_t time, uint8_t brightness)
{
	if (brightness == currentValue)
		return;

	if (time == 0) {
		currentValue = brightness;
		setValue(currentValue);
	}
	else {
		startValue = static_cast<uint16_t>(currentValue)*100;
		endValue = brightness;
		int16_t delta = (static_cast<int16_t>(endValue) - currentValue)*100;
		deltaValue = delta / static_cast<int16_t>(time);
		if (deltaValue == 0) deltaValue = delta > 0 ? 1 : -1;
	}
	fadeTime = time;
}

void
xpcc::ui::Led::on(uint16_t time)
{
	fadeTo(time, 255);
}

void
xpcc::ui::Led::off(uint16_t time)
{
	fadeTo(time, 0);
}

void
xpcc::ui::Led::run()
{
	if (timer.isExpired() && fadeTime)
	{
		startValue += deltaValue;
		currentValue = startValue/100;
		if (--fadeTime == 0) currentValue = endValue;
		if (currentValue == endValue) deltaValue = 0;

		setValue(currentValue);
	}
}
