/*
 * Copyright (c) 2013-2015, 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MATH_TOLERANCE_HPP
#define MODM_MATH_TOLERANCE_HPP

#include <stdint.h>
#include <modm/math/units.hpp>

namespace modm
{

/**
 * This class checks if values are within a certain tolerance.
 *
 * This can be used to guarantee the quality of certain parameters,
 * mostly baudrate or datarate.
 *
 * @ingroup	modm_math
 */
class
Tolerance
{
public:
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
	isErrorInTolerance(float error, percent_t tolerance)
	{
		return (error == 0) or (((error > 0) ? error : -error) <= pct2f(tolerance));
	}

	static constexpr bool
	isValueInTolerance(uint32_t reference, uint32_t actual, percent_t tolerance)
	{
		return isErrorInTolerance(relativeError(reference, actual), tolerance);
	}

	template< uint32_t reference, uint32_t actual, percent_t tolerance >
	static void
	assertValueInTolerance()
	{
		static_assert(isValueInTolerance(reference, actual, tolerance),
				"The actual value exceeds the tolerance of reference!");
	}
};

} // namespace modm

#endif	// MODM_MATH_TOLERANCE_HPP
