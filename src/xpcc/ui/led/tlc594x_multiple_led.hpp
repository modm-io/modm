// coding: utf-8
/* Copyright (c) 2012, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_TLC594X_MULTIPLE_LED_HPP
#define XPCC_UI_TLC594X_MULTIPLE_LED_HPP

#include <stdint.h>
#include "led.hpp"
#include "tables.hpp"
#include <xpcc/architecture/driver/accessor.hpp>
#include <xpcc/driver/pwm/tlc594x.hpp>

namespace xpcc
{

namespace ui
{

/**
 * LED Implementation for the TLC594X for multiple channels.
 *
 * @author	Niklas Hauser
 * @ingroup led
 */
template< typename PwmController >
class TLC594XMultipleLed : public Led
{
	const uint8_t *channels;
	uint8_t channelSize;
	xpcc::accessor::Flash<uint16_t> table;

	virtual void
	setValue(uint8_t brightness)
	{
		for (uint_fast8_t i = 0; i < channelSize; ++i)
		{
			PwmController::setChannel(channels[i], table[brightness]);
		}
	}

public:
	TLC594XMultipleLed(const uint8_t* channels, uint8_t const channelSize, const uint16_t* table=table12_256)
	:	Led(), channels(channels), channelSize(channelSize), table(table)
	{
	}
};

} // namespace ui

} // namespace xpcc

#endif	// XPCC_UI_TLC594X_MULTIPLE_LED_HPP
