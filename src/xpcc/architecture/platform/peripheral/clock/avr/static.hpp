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

namespace xpcc
{

namespace avr
{

class
SystemClock
{
public:
	// Frequencies
	static constexpr int Frequency = F_CPU;

	static constexpr int Timer = Frequency;

	static constexpr int Dac = Frequency;

	static constexpr int Can = Frequency;

	static constexpr int I2c = Frequency;

	static constexpr int Spi = Frequency;

	static constexpr int UsartSpi = Frequency;

	static constexpr int Uart = Frequency;

	static constexpr int Adc = Frequency;

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
