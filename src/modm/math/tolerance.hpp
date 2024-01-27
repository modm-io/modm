/*
 * Copyright (c) 2013-2015, 2018, 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/math/units.hpp>
#include <cmath>

namespace modm
{

/// @ingroup modm_math
template< typename T >
constexpr bool
isValueInTolerance(T reference, T actual, percent_t tolerance)
{
	return std::abs(1.0 - double(actual) / double(reference)) <= std::abs(double(tolerance));
}

} // namespace modm
