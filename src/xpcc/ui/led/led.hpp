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
#include "tables.hpp"
#include <xpcc/processing/periodic_timer.hpp>
#include <xpcc/architecture/driver/accessor/flash.hpp>

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
	uint16_t currentValue;

	float deltaValue;
	float startValue;
	uint16_t endValue;
	uint16_t fadeTime;

	xpcc::PeriodicTimer<> timer;
	std::size_t const tableSize;

	/// This method is specific to the implementation and must be overwritten.
	virtual void
	setValue(uint16_t /*brightness*/)
	{
	}

public:
	/// the constructor must not take any arguments.
	Led(std::size_t const tableSize=256)
	:	currentValue(0), deltaValue(0), startValue(0), endValue(0), fadeTime(0),
	 	timer(1), tableSize(tableSize)
	{
		setValue(0);
	}

	/// @param	brightness
	///		between 0 and length of lookup-table
	virtual void
	setBrightness(uint16_t brightness)
	{
		fadeTime = 0;
		if (brightness > tableSize-1) brightness = tableSize-1;
		currentValue = brightness;

		setValue(currentValue);
	}

	/// @return brightness of the LED
	virtual uint16_t
	getBrightness()
	{
		return currentValue;
	}

	/// @return `true` if LED is currently fading to another brightness,
	///			`false` if otherwise
	virtual bool
	isFading()
	{
		return static_cast<bool>(fadeTime);
	}

	/// Fade from the current brightness to a new brightness in the specified ms.
	virtual void
	fadeTo(uint16_t time, uint16_t brightness)
	{
		if (brightness == currentValue) return;
		if (brightness > tableSize-1) brightness = tableSize-1;
		if (!time) {
			currentValue = brightness;
			setValue(currentValue);
		}
		else {
			startValue = currentValue;
			endValue = brightness;
			deltaValue = (endValue - startValue) / static_cast<float>(time);
		}
		fadeTime = time;
	}

	/**
	 * Mimmics the behaviour of normal lamps, which take a small amount
	 * of time until achiving full brightness.
	 * @param	time
	 * 		specify the fade up time in ms, 0 turn the LED on instantly
	 */
	virtual void
	on(uint16_t time=7)
	{
		fadeTo(time, tableSize-1);
	}

	/**
	 * Mimmics the behaviour of normal lamps, which take a small amount
	 * of time until fully extinguishing.
	 * @param	time
	 * 		specify the fade up time in ms, 0 turn the LED off instantly
	 */
	virtual void
	off(uint16_t time=10)
	{
		fadeTo(time, 0);
	}

	/// must be called at least every ms.
	virtual void
	run()
	{
		if (timer.isExpired() && fadeTime)
		{
			startValue += deltaValue;
			currentValue = static_cast<uint16_t>(startValue);
			if (!--fadeTime) currentValue = endValue;

			setValue(currentValue);
		}
	}
};

}

}

#endif	// XPCC_UI_LED_HPP
