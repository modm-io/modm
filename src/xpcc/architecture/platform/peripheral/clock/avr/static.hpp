// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_AVR_CLOCK_HPP
#define XPCC_AVR_CLOCK_HPP

#include <stdint.h>
#include "../common/common_clock.hpp"

using namespace xpcc::clock;

namespace xpcc
{

namespace avr
{

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
