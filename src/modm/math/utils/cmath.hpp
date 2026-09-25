/*
 * Copyright (c) 2026, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <cmath>
#include <concepts>
#include <limits>
#include <modm/architecture/detect.hpp>

namespace modm::detail
{

template<std::floating_point T>
consteval T
consteval_ceil(T value)
{
	static_assert(std::numeric_limits<T>::is_iec559);

	// handle NaN
	if (value != value) return value;

	constexpr auto mantissaDigits = std::numeric_limits<T>::digits;
	static_assert(std::numeric_limits<long long>::digits >= mantissaDigits);

	// calculate magnitude limit outside which all values must be integral
	constexpr auto limit = T{1ll << (mantissaDigits - 1)};
	if (value >= limit || value <= -limit) {
		return value;
	}

	const auto truncated = static_cast<T>(static_cast<long long>(value));
	if (value <= T{0}) {
		// preserve sign of zero, e.g. ceil(-0.5f) = -0.0f
		if (truncated == 0) {
			return value * 0;
		}

		return truncated;
	} else {
		return (value > truncated) ? (truncated + T{1}) : truncated;
	}
}

template<std::floating_point T>
consteval T
consteval_floor(T value)
{
	// handle NaN
	if (value != value) return value;

	constexpr auto mantissaDigits = std::numeric_limits<T>::digits;
	static_assert(std::numeric_limits<long long>::digits >= mantissaDigits);

	// calculate magnitude limit outside which all values must be integral
	constexpr auto limit = T{1ll << (mantissaDigits - 1)};
	if (value >= limit || value <= -limit) {
		return value;
	}

	// preserve sign of zero, floor(-0.0f) = -0.0f
	if (value == T{0}) {
		return value;
	}

	const auto truncated = static_cast<T>(static_cast<long long>(value));
	if (value >= T{0}) {
		return truncated;
	} else {
		return (value < truncated) ? (truncated - T{1}) : truncated;
	}
}

template <std::floating_point T>
consteval T
consteval_round(T value)
{
	// handle NaN
	if (value != value) return value;

	constexpr auto mantissaDigits = std::numeric_limits<T>::digits;
	static_assert(std::numeric_limits<long long>::digits >= mantissaDigits);

	// calculate magnitude limit outside which all values must be integral
	constexpr auto limit = T{1ll << (mantissaDigits - 1)};
	if (value >= limit || value <= -limit) {
		return value;
	}

	const auto truncated = static_cast<T>(static_cast<long long>(value));
    const auto fraction = value - truncated;

	if (std::signbit(value)) {
		if (fraction <= T{-0.5}) return truncated - T{1};
		if (truncated == 0) return value * T{0};
		return truncated;
	} else {
		if (fraction >= T{0.5}) return truncated + T{1};
		return truncated;
	}
}

} // namespace modm::detail

namespace modm
{

template<std::floating_point T>
constexpr T
ceil(T value)
{
#ifdef MODM_COMPILER_CLANG
    if consteval {
        return detail::consteval_ceil(value);
    } else {
        return std::ceil(value);
    }
#else
    return std::ceil(value);
#endif
}

template<std::floating_point T>
constexpr T
floor(T value)
{
#ifdef MODM_COMPILER_CLANG
    if consteval {
        return detail::consteval_floor(value);
    } else {
        return std::floor(value);
    }
#else
    return std::floor(value);
#endif
}

template<std::floating_point T>
constexpr T
round(T value)
{
#ifdef MODM_COMPILER_CLANG
    if consteval {
        return detail::consteval_round(value);
    } else {
        return std::round(value);
    }
#else
    return std::round(value);
#endif
}

} // namespace modm
