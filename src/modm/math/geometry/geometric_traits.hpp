/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GEOMETRIC_TRAITS_HPP
#define MODM_GEOMETRIC_TRAITS_HPP

#include <cmath>
#include <stdint.h>
#include <type_traits>
#include <modm/architecture/utils.hpp>

namespace modm
{
	/// @cond
	namespace detail
	{
		/// Floating point type for geometric calculations: `float` for integers
		template<typename T>
		using GeometricFloatType = std::conditional_t<std::is_floating_point_v<T>, T, float>;

		/**
		 * Signed type for products of coordinates.
		 *
		 * 8-bit and 16-bit integers are widened, larger integers are not,
		 * since 64-bit operations are very, very slow on an AVR.
		 */
		template<typename T, bool = std::is_integral_v<T>>
		struct GeometricWideType
		{ using type = T; };

		template<typename T>
		struct GeometricWideType<T, true>
		{
			using type = std::conditional_t<sizeof(T) == 1, int16_t,
						 std::conditional_t<sizeof(T) == 2, int32_t, std::make_signed_t<T>>>;
		};

		/// Rounds when converting to an integer
		template<typename T, typename F>
		constexpr T
		geometricRound(F value)
		{
			if constexpr (std::is_integral_v<T>) return T(std::round(value));
			else return T(value);
		}
	}
	/// @endcond

	/**
	 * \brief	Traits for all geometric classes
	 *
	 * \ingroup	modm_math_geometry
	 * \author	Fabian Greif
	 */
	template <typename T>
	struct [[deprecated("Use the WideType and FloatType of the geometric classes instead")]]
	GeometricTraits // DEPRECATED: 2027q4
	{
		static constexpr bool isValidType = std::is_arithmetic_v<T>;

		using FloatType = detail::GeometricFloatType<T>;
		using WideType = typename detail::GeometricWideType<T>::type;

		/**
		 * \brief	Round if converting from a floating point base to
		 * 			a integer base.
		 */
		static constexpr T
		round(FloatType value)
		{
			return detail::geometricRound<T>(value);
		}
	};
}

#endif // MODM_GEOMETRIC_TRAITS_HPP
