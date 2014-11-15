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
:	Led(nullptr)
{
}

xpcc::ui::Led::Led(Animation<uint8_t>::Callback_t callback)
:	animation(brightness, callback), brightness(0)
{
}

void ALWAYS_INLINE
xpcc::ui::Led::setBrightness(uint8_t brightness)
{
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

void ALWAYS_INLINE
xpcc::ui::Led::fadeTo(uint8_t brightness, uint16_t time)
{
	animation.animateTo(brightness, time);
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

void ALWAYS_INLINE
xpcc::ui::Led::update()
{
	animation.update();
}
