/*
 * Copyright (c) 2021-2023, Thomas Sommer
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
#include <tuple>
#include <bit>

#include "arithmetic_traits.hpp"

namespace modm
{
/// @ingroup modm_math_utils
/// @{
using builtin_int_t = std::tuple<int8_t, int16_t, int32_t, int64_t>;
using builtin_uint_t = std::tuple<uint8_t, uint16_t, uint32_t, uint64_t>;

/// Trait the smallest unsigned integral fitting n [D]igits
/// @author		Thomas Sommer
template<int D>
requires(0 < D and D <= 64)
using least_uint = std::tuple_element_t<int(std::log2(std::bit_ceil(std::max<unsigned int>(D, 8))>>3)), builtin_uint_t>;

/// @cond
namespace detail
{
	template<typename... Ts>
	consteval int max_digits() {
		return std::max({std::numeric_limits<Ts>::digits...});
	}

	template <typename ... Ts>
	struct fits_any {
		static constexpr int max_digits = max_digits<Ts...>();
		static constexpr bool is_any_signed = std::disjunction_v<std::is_signed<Ts>...>;

		using type = std::conditional_t<is_any_signed,
			::modm::WideType<std::make_signed_t<least_uint<max_digits>>>,
			least_uint<max_digits>
		>;
	};
}
/// @endcond

/// Trait the smallest integral - signed or unsigned - fitting any Ts
/// @author	Thomas Sommer
template <typename ... Ts>
using fits_any = typename detail::fits_any<Ts...>::type;
} // namespace modm