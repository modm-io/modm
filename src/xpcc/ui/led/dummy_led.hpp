// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_DUMMY_LED_HPP
#define XPCC_UI_DUMMY_LED_HPP

#include <stdint.h>
#include "led.hpp"

namespace xpcc
{

namespace ui
{

/**
 * Emtpy LED implementation.
 *
 * Use this as a drop-in replacement for when you do not drive an actual LED.
 * An example use would be the xpcc::ui::RgbLed class without a Blue Led.
 *
 * @author	Niklas Hauser
 * @ingroup led
 */
class DummyLed : public Led
{
	xpcc::Timeout<> timeout;

	virtual void
	setValue(uint8_t /*brightness*/)
	{
	}

public:
	DummyLed()
	:	Led(), timeout(0)
	{
	}

	inline void
	setBrightness(uint8_t brightness)
	{
		currentValue = brightness;
	}

	ALWAYS_INLINE bool
	isFading()
	{
		return !timeout.isExpired();
	}

	void
	fadeTo(uint16_t time, uint8_t brightness)
	{
		timeout.restart(time);
		currentValue = brightness;
	}

	inline void
	run()
	{
	}
};

}

}

#endif	// XPCC_UI_DUMMY_LED_HPP
