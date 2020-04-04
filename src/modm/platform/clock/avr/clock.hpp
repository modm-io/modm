/*
 * Copyright (c) 2013-2015, 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AVR_CLOCK_HPP
#define MODM_AVR_CLOCK_HPP

#include <stdint.h>


namespace modm
{

namespace platform
{

/// @ingroup modm_platform_clock
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

	static bool enable();
};

} // namespace platform

} // namespace modm

#endif	//  MODM_AVR_CLOCK_HPP
