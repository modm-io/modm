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
 * // use a 16bits lookup table with 256 values.
 * xpcc::accessor::Flash<uint16_t> table(xpcc::ui::table16_256);
 * void
 * setOrange(uint8_t brightness)
 * {
 *     Timer4::setCompareValue(1, table[brightness]);
 * }
 * xpcc::ui::Led orange(setOrange);
 * // setup Timer4 etc...
 * @endcode
 *
 * This is just a very thin wrapper around xpcc::ui::Animation<uint8_t>.
 * If you need more advanced features, have a look there.
 *
 * @see Animation
 * @author	Niklas Hauser
 * @ingroup led
 */
class Led
{
public:
	Led();

	/// Requires a callback function pointer for value updates.
	Led(Animation<uint8_t>::Callback_t callback);

	/// @param	brightness
	///		between 0 and length of lookup-table (usually 255)
	void
	setBrightness(uint8_t brightness);

	/// @return brightness of the LED
	uint8_t
	getBrightness() const;

	/// @return `true` if LED is currently fading to another brightness,
	///			`false` if otherwise
	bool
	isFading() const;

	/// Fade from the current brightness to a new brightness in the specified ms.
	/// Fading times of more than ~32s are not possible. You must control
	/// fading externally in that case.
	void
	fadeTo(uint8_t brightness, uint16_t time);

	/**
	 * Mimmics the behaviour of normal lamps, which take a small amount
	 * of time until achieving full brightness.
	 * @param	time
	 * 		specify the fade up time in ms, `0` to turn the LED on instantly
	 */
	void
	on(uint16_t time=75);

	/**
	 * Mimmics the behaviour of normal lamps, which take a small amount
	 * of time until fully extinguished.
	 * @param	time
	 * 		specify the fade up time in ms, `0` to turn the LED off instantly
	 */
	void
	off(uint16_t time=120);

	/// Can be called at a interval of 1ms or less.
	/// If you do not need 1ms response time (e.g. for on(), off()),
	/// you may call this at intervals < 255ms.
	void
	update();

public:
	Animation<uint8_t> animation;

private:
	uint8_t brightness;
};

}	// namespace ui

}	// namespace xpcc

#include "led_impl.hpp"

#endif	// XPCC_UI_LED_HPP
