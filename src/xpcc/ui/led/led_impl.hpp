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
:	brightnessAnimation(currentBrightness), currentBrightness(0)
{
}

void inline
xpcc::ui::Led::setBrightness(uint8_t brightness)
{
	brightnessAnimation.setValue(brightness);
	setValue(currentBrightness);
}

uint8_t ALWAYS_INLINE
xpcc::ui::Led::getBrightness() const
{
	return brightnessAnimation.getValue();
}

bool ALWAYS_INLINE
xpcc::ui::Led::isFading() const
{
	return brightnessAnimation.isAnimating();
}

void inline
xpcc::ui::Led::fadeTo(uint16_t time, uint8_t brightness)
{
	if (!brightnessAnimation.animateTo(time, brightness))
	{
		setValue(brightness);
	}
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

void inline
xpcc::ui::Led::update()
{
	if (brightnessAnimation.update())
	{
		setValue(currentBrightness);
	}
}
