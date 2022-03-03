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

#include <limits>
#include <type_traits>

#include <cmath>
namespace modm
{
/// @ingroup modm_math_utils
/// @{

/// Trait the smallest unsigned type that fits n Bits
/// @author		Thomas Sommer
template <int Bits>
struct uint_t
{
	using least = std::conditional_t<
		(Bits <= 8), uint8_t, std::conditional_t<
		(Bits <= 16), uint16_t, std::conditional_t<
		(Bits <= 32), uint32_t, std::enable_if_t<
		(Bits <= 64), uint64_t>>>>;
};

template<int Bits>
using least_uint = typename modm::uint_t<Bits>::least;

/// @cond
namespace detail
{
	template<typename T>
	constexpr int most_digits() {
		return std::numeric_limits<T>::digits;
	}

	template<typename T, typename... Ts>
	constexpr std::enable_if_t<sizeof...(Ts), int>
	most_digits() {
		return std::max(std::numeric_limits<T>::digits, most_digits<Ts...>());
	}
}
/// @endcond

/// Trait the smallest integral - signed or unsigned - fitting any Ts
/// @author	Thomas Sommer
template <typename ... Ts>
struct fits_any {
	static constexpr int most_dig = detail::most_digits<Ts...>();

	using type = std::conditional_t<
		std::conjunction_v<std::is_unsigned<Ts>...>,
		typename uint_t<most_dig>::least,
		// An odd most_dig means: type with most digits is signed integral
		std::make_signed_t<typename uint_t<most_dig % 2 ? most_dig : most_dig * 2>::least>
	>;
};

template <typename ... Ts>
using fits_any_t = typename fits_any<Ts...>::type;

/// @}
}
