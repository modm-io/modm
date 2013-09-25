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
#include <xpcc/processing/periodic_timer.hpp>

namespace xpcc
{

namespace ui
{

/**
 * LED Interface.
 *
 * This class supplies a basic interface to fade an LED to any
 * brightness level.
 * A lookup-table should be used to store the alpha corrected PWM
 * values, so that no calculation has to be done at run time.
 *
 * @author	Niklas Hauser
 * @ingroup led
 */
class Led
{
	/// This method is specific to the implementation and must be overwritten.
	virtual void
	setValue(uint8_t /*brightness*/)
	{
	}

public:
	Led();

	/// @param	brightness
	///		between 0 and length of lookup-table
	inline void
	setBrightness(uint8_t brightness);

	/// @return brightness of the LED
	ALWAYS_INLINE uint8_t
	getBrightness();

	/// @return `true` if LED is currently fading to another brightness,
	///			`false` if otherwise
	ALWAYS_INLINE bool
	isFading();

	/// Fade from the current brightness to a new brightness in the specified ms.
	/// Fading times of more than 25.5s are not possible, you must control
	/// fading externally in that case.
	void
	fadeTo(uint16_t time, uint8_t brightness);

	/**
	 * Mimmics the behaviour of normal lamps, which take a small amount
	 * of time until achiving full brightness.
	 * @param	time
	 * 		specify the fade up time in ms, 0 turn the LED on instantly
	 */
	ALWAYS_INLINE void
	on(uint16_t time=9);

	/**
	 * Mimmics the behaviour of normal lamps, which take a small amount
	 * of time until fully extinguishing.
	 * @param	time
	 * 		specify the fade up time in ms, 0 turn the LED off instantly
	 */
	ALWAYS_INLINE void
	off(uint16_t time=12);

	/// must be called at least every ms.
	inline void
	run();

protected:
	uint8_t currentValue;

	uint16_t startValue;
	uint8_t endValue;

	int16_t deltaValue;
	uint16_t fadeTime;

	xpcc::PeriodicTimer<> timer;
};

}

}

#include "led_impl.hpp"

#endif	// XPCC_UI_LED_HPP
