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
:	animation(brightness), brightness(0)
{
}

void inline
xpcc::ui::Led::setBrightness(uint8_t brightness)
{
	setValue(currentBrightness);
	animation.setValue(brightness);
}

uint8_t ALWAYS_INLINE
xpcc::ui::Led::getBrightness() const
{
	return animation.getValue();
}

bool ALWAYS_INLINE
xpcc::ui::Led::isFading() const
{
	return animation.isAnimating();
}

void inline
xpcc::ui::Led::fadeTo(uint8_t brightness, uint16_t time)
{
	brightnessAnimation.animateTo(brightness, time);
}

void ALWAYS_INLINE
xpcc::ui::Led::on(uint16_t time)
{
	fadeTo(255, time);
}

void ALWAYS_INLINE
xpcc::ui::Led::off(uint16_t time)
{
	fadeTo(0, time);
}

void inline
xpcc::ui::Led::update()
{
	if (brightnessAnimation.update())
	{
		setValue(currentBrightness);
	}
}
