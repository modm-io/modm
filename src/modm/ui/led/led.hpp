/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_UI_LED_HPP
#define MODM_UI_LED_HPP

#include <stdint.h>
#include <modm/processing/timer.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/architecture/utils.hpp>
#include <modm/ui/animation.hpp>

namespace modm
{

namespace ui
{

/**
 * This class supplies a basic interface to fade an LED to any
 * brightness level.
 * A lookup-table should be used to store the alpha corrected PWM
 * values, so that no calculation has to be done at run time.
 *
 * Here is an example for the STM32F4:
 * @code
 * #include <modm/ui/led.hpp>
 * // use a lambda function without a closure, this is equivalent to a normal function pointer
 * modm::ui::Led orange([](uint8_t brightness)
 * {
 *     Timer4::setCompareValue(2, modm::ui::table22_16_256[brightness]);
 * });
 * // setup Timer4 etc...
 * @endcode
 *
 * This is just a very, very thin wrapper around modm::ui::Animation<uint8_t>.
 * If you need more advanced features, have a look there.
 *
 * @see modm_ui_animation
 * @author	Niklas Hauser
 * @ingroup modm_ui_led
 */
class Led
{
public:
	inline Led(): Led(nullptr) {}

	/// Requires a handler function pointer for value updates.
	inline Led(Animation<uint8_t>::Handler handler):
		animation(brightness, handler), brightness(0) {}

	/// @param	brightness
	///		between 0 and length of lookup-table (usually 255)
	inline void
	setBrightness(uint8_t brightness)
	{ animation.setValue(brightness); }

	/// @return brightness of the LED
	inline uint8_t
	getBrightness() const
	{ return animation.getValue(); }

	/// @return `true` if LED is currently fading to another brightness,
	///			`false` if otherwise
	inline bool
	isFading() const
	{ return animation.isAnimating(); }

	/// Fade from the current brightness to a new brightness in the specified ms.
	/// Fading times of more than ~32s are not possible. You must control
	/// fading externally in that case.
	inline void
	fadeTo(uint8_t brightness, uint16_t time)
	{ animation.animateTo(brightness, time); }

	/**
	 * Mimics the behavior of normal lamps, which take a small amount
	 * of time until achieving full brightness.
	 * @param	time
	 * 		specify the fade up time in ms, `0` to turn the LED on instantly
	 */
	inline void
	on(uint16_t time=75)
	{ fadeTo(255, time); }

	/**
	 * Mimics the behavior of normal lamps, which take a small amount
	 * of time until fully extinguished.
	 * @param	time
	 * 		specify the fade up time in ms, `0` to turn the LED off instantly
	 */
	inline void
	off(uint16_t time=120)
	{ fadeTo(0, time); }

	/// Can be called at a interval of 1ms or less.
	/// If you do not need 1ms response time (e.g. for on(), off()),
	/// you may call this at intervals < 255ms.
	inline void
	update()
	{ animation.update(); }

	/// This object can be cast to the underlying Animation object
	operator Animation<uint8_t>& ()
	{ return animation; }

private:
	Animation<uint8_t> animation;
	uint8_t brightness;
};

}	// namespace ui

}	// namespace modm

#endif	// MODM_UI_LED_HPP
