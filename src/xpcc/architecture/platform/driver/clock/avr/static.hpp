/*
 * Copyright (c) 2013-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_AVR_CLOCK_HPP
#define XPCC_AVR_CLOCK_HPP

#include <stdint.h>
#include "../generic/common_clock.hpp"

using namespace xpcc::clock;

namespace xpcc
{

namespace avr
{

/// @ingroup avr
class
SystemClock
{
public:
	// Frequencies
	static constexpr uint32_t Frequency = F_CPU;

	static constexpr uint32_t Timer = Frequency;

	static constexpr uint32_t Dac = Frequency;

	static constexpr uint32_t Can = Frequency;

	static constexpr uint32_t I2c = Frequency;

	static constexpr uint32_t Spi = Frequency;

	static constexpr uint32_t UsartSpi = Frequency;

	static constexpr uint32_t Uart = Frequency;

	static constexpr uint32_t Adc = Frequency;

	static StartupError
	enable()
	{
		// do nothing
		return StartupError::None;
	}

};

} // namespace avr

} // namespace xpcc

#endif	//  XPCC_AVR_CLOCK_HPP
