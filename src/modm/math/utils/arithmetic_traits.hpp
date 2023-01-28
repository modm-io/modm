/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2013, 2015, Sascha Schade
 * Copyright (c) 2015, Kevin Läufer
 * Copyright (c) 2018, Christopher Durand
 * Copyright (c) 2022, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ARITHMETIC_TRAITS_HPP
#define MODM_ARITHMETIC_TRAITS_HPP

#include <cstdint>
#include <limits>
#include <type_traits>
#include <cmath>

namespace modm
{

/// @cond
namespace detail
{
	// type trait to determine next possibly larger int type
	// char <= short <= int <= long <= long long
	template<typename T>
	struct NextInt
	{ using type = T; };

	// char is a distinct type, not identical with signed char or unsigned char
	template<>
	struct NextInt<char>
	{
		using type = std::conditional_t<
			std::is_signed_v<char>,
			short,
			unsigned short
		>;
	};

	template<>
	struct NextInt<signed char>
	{ using type = short; };

	template<>
	struct NextInt<unsigned char>
	{ using type = unsigned short; };

	template<>
	struct NextInt<short>
	{ using type = int; };

	template<>
	struct NextInt<unsigned short>
	{ using type = unsigned int; };

	template<>
	struct NextInt<int>
	{ using type = long; };

	template<>
	struct NextInt<unsigned int>
	{ using type = unsigned long; };

	template<>
	struct NextInt<long>
	{ using type = long long; };

	template<>
	struct NextInt<unsigned long>
	{ using type = unsigned long long; };

	// ------------------------------------------------------------------------

	// implement WideType<T>
	template<typename T>
	struct WideType
	{ using type = T; };

	template<>
	struct WideType<bool>
	{ using type = char; };

	template<>
	struct WideType<long long>
	{ using type = double; };

	template<>
	struct WideType<unsigned long long>
	{ using type = double; };

	template<typename T, typename = std::enable_if_t<
		std::is_integral_v<T> && !std::is_same_v<std::decay_t<T>, bool>
	> >
	using enable_if_int = T;

	template<typename T>
	struct WideType<enable_if_int<T>>
	{
		static constexpr bool isNextIntLarger =
			std::numeric_limits<typename NextInt<T>::type>::max() > std::numeric_limits<T>::max();

		using type = std::conditional_t<
			isNextIntLarger,
			typename NextInt<T>::type,
			typename WideType<typename NextInt<T>::type>::type
		>;
	};

	// ------------------------------------------------------------------------

	template<typename T>
	struct MakeSigned
	{
		using type = T;
	};

	template<typename T>
	struct MakeSigned<enable_if_int<T>>
	{
		using type = std::make_signed_t<T>;
	};

	template<typename T>
	struct MakeUnsigned
	{
		using type = T;
	};

	template<typename T>
	struct MakeUnsigned<enable_if_int<T>>
	{
		using type = std::make_unsigned_t<T>;
	};
}
/// @endcond

/// @ingroup modm_math_utils
/// @{
template<typename T>
using WideType = typename detail::WideType<T>::type;

template<typename T>
using SignedType = typename detail::MakeSigned<T>::type;

template<typename T>
using UnsignedType = typename detail::MakeUnsigned<T>::type;

/**
 * Arithmetic Traits
 *
 * Previously, some of the functionalities of <type_traits> and <limits> were
 * implemented here, when they were not yet available in the C++ standard
 * library. Only isInteger and decimalDigits are left, for all others please
 * use the C++ standard library in the future.
 *
 * @section provides	Values provided by these traits
 *
 * - `isInteger`		is this a integer type
 * - `decimalDigits`	count of digits to display this type in decimal
 *
 * @section usage	Usage
 * @code
 * if (modm::ArithmeticTraits<T>::isInteger)
 *     // ...
 *
 * constexpr unsigned char digits = modm::ArithmeticTraits<T>::decimalDigits;
 * @endcode
 */
template<typename T>
struct ArithmeticTraits
{
	static constexpr bool isInteger = std::is_integral_v<T>
		&& !std::is_same_v<std::decay_t<T>, bool>;

	static constexpr unsigned char decimalDigits =
		std::ceil(std::numeric_limits<T>::digits * log10(2)) + (std::is_signed_v<T> ? 1 : 0);
};
/// @}

} // namespace modm

#endif	// MODM_ARITHMETIC_TRAITS_HPP
