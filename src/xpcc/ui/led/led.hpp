// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_LED_HPP
#define XPCC_UI_LED_HPP

#include <stdint.h>
#include <xpcc/processing/timer.hpp>
#include <xpcc/architecture/driver/clock.hpp>
#include <xpcc/ui/animation.hpp>

namespace xpcc
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
 * #include <xpcc/ui/led.hpp>
 * // use a lambda function without a closure, this is equivalent to a normal function pointer
 * xpcc::ui::Led orange([](uint8_t brightness)
 * {
 *     Timer4::setCompareValue(2, xpcc::ui::table22_16_256[brightness]);
 * });
 * // setup Timer4 etc...
 * @endcode
 *
 * This is just a very, very thin wrapper around xpcc::ui::Animation<uint8_t>.
 * If you need more advanced features, have a look there.
 *
 * @see Animation
 * @author	Niklas Hauser
 * @ingroup led
 */
class Led
{
public:
	ALWAYS_INLINE Led(): Led(nullptr) {}

	/// Requires a handler function pointer for value updates.
	ALWAYS_INLINE Led(Animation<uint8_t>::Handler handler):
		animation(brightness, handler), brightness(0) {}

	/// @param	brightness
	///		between 0 and length of lookup-table (usually 255)
	ALWAYS_INLINE void
	setBrightness(uint8_t brightness)
	{ animation.setValue(brightness); }

	/// @return brightness of the LED
	ALWAYS_INLINE uint8_t
	getBrightness() const
	{ return animation.getValue(); }

	/// @return `true` if LED is currently fading to another brightness,
	///			`false` if otherwise
	ALWAYS_INLINE bool
	isFading() const
	{ return animation.isAnimating(); }

	/// Fade from the current brightness to a new brightness in the specified ms.
	/// Fading times of more than ~32s are not possible. You must control
	/// fading externally in that case.
	ALWAYS_INLINE void
	fadeTo(uint8_t brightness, uint16_t time)
	{ animation.animateTo(brightness, time); }

	/**
	 * Mimmics the behaviour of normal lamps, which take a small amount
	 * of time until achieving full brightness.
	 * @param	time
	 * 		specify the fade up time in ms, `0` to turn the LED on instantly
	 */
	ALWAYS_INLINE void
	on(uint16_t time=75)
	{ fadeTo(255, time); }

	/**
	 * Mimmics the behaviour of normal lamps, which take a small amount
	 * of time until fully extinguished.
	 * @param	time
	 * 		specify the fade up time in ms, `0` to turn the LED off instantly
	 */
	ALWAYS_INLINE void
	off(uint16_t time=120)
	{ fadeTo(0, time); }

	/// Can be called at a interval of 1ms or less.
	/// If you do not need 1ms response time (e.g. for on(), off()),
	/// you may call this at intervals < 255ms.
	ALWAYS_INLINE void
	update()
	{ animation.update(); }

public:
	Animation<uint8_t> animation;

private:
	uint8_t brightness;
};

}	// namespace ui

}	// namespace xpcc

#endif	// XPCC_UI_LED_HPP
