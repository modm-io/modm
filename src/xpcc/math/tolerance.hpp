// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_MATH_TOLERANCE_HPP
#define XPCC_MATH_TOLERANCE_HPP

#include <stdint.h>
#include <xpcc/math.hpp>

namespace xpcc
{

/**
 * This class checks if values are within a certain tolerance.
 *
 * This can be used to guarantee the quality of certain parameters,
 * mostly baudrate or datarate.
 */
class
Tolerance
{
public:
	/// Common tolerances in percent
	///@{
	static constexpr uint16_t Exact			=    0;
	static constexpr uint16_t HalfPercent	=    5;
	static constexpr uint16_t OnePercent	=   10;
	static constexpr uint16_t TwoPercent	=   20;
	static constexpr uint16_t FivePercent	=   50;
	static constexpr uint16_t DontCare		= 1000;
	///@}

	static constexpr int64_t
	absoluteError(uint32_t reference, uint32_t actual)
	{
		return (static_cast<int64_t>(reference) - actual);
	}

	static constexpr float
	relativeError(uint32_t reference, uint32_t actual)
	{
		return static_cast<float>(absoluteError(reference, actual)) / reference;
	}

	static constexpr bool
	isErrorInTolerance(float error, uint16_t tolerance)
	{
		return (error == 0) || (std::abs(error)*1000 <= tolerance);
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
