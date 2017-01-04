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

#ifndef XPCC_MATH_TOLERANCE_HPP
#define XPCC_MATH_TOLERANCE_HPP

#include <stdint.h>
#include <modm/math.hpp>

namespace xpcc
{

/**
 * This class checks if values are within a certain tolerance.
 *
 * This can be used to guarantee the quality of certain parameters,
 * mostly baudrate or datarate.
 *
 * @ingroup	math
 */
class
Tolerance
{
public:
	///@{
	/// Common tolerances in percent
	static constexpr uint16_t Exact			=    0;
	static constexpr uint16_t ZeroPercent	=    0;
	static constexpr uint16_t HalfPercent	=    5;
	static constexpr uint16_t OnePercent	=   10;
	static constexpr uint16_t TwoPercent	=   20;
	static constexpr uint16_t FivePercent	=   50;
	static constexpr uint16_t TenPercent	=  100;
	static constexpr uint16_t TwentyPercent	=  200;
	static constexpr uint16_t DontCare		= 1000;
	///@}

	static constexpr int64_t
	absoluteError(uint32_t reference, uint32_t actual)
	{
		return (int64_t(reference) - actual);
	}

	static constexpr float
	relativeError(uint32_t reference, uint32_t actual)
	{
		return (1.f - float(actual) / reference);
	}

	static constexpr bool
	isErrorInTolerance(float error, uint16_t tolerance)
	{
		return (error == 0) or (((error > 0) ? error : -error) * 1000 <= tolerance);
	}

	static constexpr bool
	isValueInTolerance(uint32_t reference, uint32_t actual, uint16_t tolerance)
	{
		return isErrorInTolerance(relativeError(reference, actual), tolerance);
	}

	template< uint32_t reference, uint32_t actual, uint16_t tolerance >
	static void
	assertValueInTolerance()
	{
		static_assert(isValueInTolerance(reference, actual, tolerance),
				"The actual value exceeds the tolerance of reference!");
	}
};

} // namespace xpcc

#endif	// XPCC_MATH_TOLERANCE_HPP
