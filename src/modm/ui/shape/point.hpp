/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/math/geometry/vector.hpp>

namespace modm::shape
{

class Point : public Vector<int16_t, 2>
{
public:
	constexpr Point() = default;

	// Allow arbitray arithmetic Types for Point-construction
	// This suspresses "narrowing conversion" compiler warnings
	template<typename T, typename U>
	requires std::is_arithmetic_v<T> and std::is_arithmetic_v<U>
	constexpr Point(T x, U y) : Vector<int16_t, 2>(x, y){};

	template<typename T>
	requires std::is_arithmetic_v<T>
	constexpr Point(Vector<T, 2> vector) : Vector<int16_t, 2>(vector){};

	modm_always_inline
	void swap() {
		int16_t temp = x;
		x = y;
		y = temp;
	}

	modm_always_inline
	constexpr Point swapped() const {
		return {y, x};
	}
};

using Size = Point;

}  // namespace modm::shape