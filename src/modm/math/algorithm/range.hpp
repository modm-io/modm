/*
 * Copyright (c) 2015, Huu Nguyen
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the MIT License.
 * If a copy of the MPL was not distributed with this file, you can obtain
 * one at https://opensource.org/licenses/MIT.
 */
// Originally adapted from https://github.com/whoshuu/cpp_range
// ----------------------------------------------------------------------------

#pragma once
#include <cstddef>
#include <ranges>
#include <type_traits>

namespace modm
{

/**
 * Returns an iterable range [start, stop) of step != 0.
 *
 * The range is a random access view, so it can be used with the standard
 * range algorithms and adaptors. The number of elements is computed in
 * advance, so the range never overflows `T`.
 *
 * @warning Returns an empty range if
 *          (start >= stop and step >= 0) or (start <= stop and step <= 0)!
 *
 * @ingroup	modm_math_algorithm
 */
template <typename T>
constexpr auto
range(const T& start, const T& stop, const T& step)
{
	if constexpr (std::is_integral_v<T>)
	{
		// modular arithmetic avoids signed overflow for any start and step
		using U = std::make_unsigned_t<std::common_type_t<T, int>>;
		U distance{0}, stride{1};
		if (step > 0 and start < stop) { distance = U(stop) - U(start); stride = U(step); }
		if (step < 0 and start > stop) { distance = U(start) - U(stop); stride = U(0) - U(step); }
		const U count = distance / stride + (distance % stride != 0);
		return std::views::iota(U{0}, count) | std::views::transform(
				[start, step](U index) { return T(U(start) + index * U(step)); });
	}
	else
	{
		std::size_t count{0};
		if ((step > 0 and start < stop) or (step < 0 and start > stop))
		{
			const T steps = (stop - start) / step;
			count = std::size_t(steps);
			if (T(count) < steps) count++;
		}
		return std::views::iota(std::size_t{0}, count) | std::views::transform(
				[start, step](std::size_t index) { return T(start + T(index) * step); });
	}
}

/**
 * Returns an iterable range [start, stop) of step 1.
 * @warning Returns an empty range if start >= stop!
 *
 * @ingroup	modm_math_algorithm
 */
template <typename T>
constexpr auto
range(const T& start, const T& stop)
{
	return range(start, stop, T{1});
}

/**
 * Returns an iterable range [0, stop) of step 1.
 * @warning Returns an empty range if stop <= 0!
 *
 * @ingroup	modm_math_algorithm
 */
template <typename T>
constexpr auto
range(const T& stop)
{
	return range(T{0}, stop, T{1});
}

} // namespace modm
