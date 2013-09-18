// coding: utf-8
/* Copyright (c) 2012, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_TLC594X_LED_HPP
#define XPCC_UI_TLC594X_LED_HPP

#include <stdint.h>
#include "led.hpp"
#include <xpcc/driver/pwm/tlc594x.hpp>


namespace xpcc
{

namespace ui
{

/**
 * LED Implementation for the TLC594X.
 *
 * @author	Niklas Hauser
 * @ingroup led
 */
template< typename PwmController >
class TLC594XLed : virtual public Led
{
	const uint8_t channel;
	xpcc::accessor::Flash<uint16_t> table;

	virtual void
	setValue(uint16_t brightness)
	{
		PwmController::setChannel(channel, table[brightness]);
	}

public:
	TLC594XLed(const uint8_t channel, const uint16_t* table=led::table12_256, std::size_t const tableSize=256)
	:	Led(tableSize), table(table), currentValue(0)
	{
	}
};

}

}

#endif	// XPCC_UI_TLC594X_LED_HPP
