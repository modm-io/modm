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

#include <algorithm>
#include <concepts>
#include <cmath>
#include <limits>
#include <type_traits>

#include <modm/math/utils/arithmetic_traits.hpp>
#include <modm/math/utils/integer_traits.hpp>

namespace modm
{
/**
 * @brief	Saturation arithmetic building on 'Integer-Overflow-Builtins'
 * 			Implementation works with integer, unsigned integer and float or reference to them.
 * 			Operators work with the same types or Saturated types of them.
 * @see		https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html
 * @see		https://en.wikipedia.org/wiki/Saturation_arithmetic
 *
 * @author	Thomas Sommer
 *
 * @ingroup modm_math_saturated
 */

template<typename T>
requires std::integral<std::remove_reference_t<T>>
class Saturated
{
protected:
	using TP = std::remove_reference_t<T>;
	using TS = std::conditional_t<std::is_signed_v<T>, T, std::make_signed_t<modm::WideType<TP>>>;

	T value = 0;
private:
	static constexpr TP min = std::numeric_limits<TP>::min();
	static constexpr TP max = std::numeric_limits<TP>::max();

public:
	Saturated() = default;

	constexpr Saturated(const T& value) : value(value){};
	constexpr Saturated(const Saturated<T>& other) : value(other.value){};

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	constexpr Saturated(const U& v)
	{ value = std::clamp< modm::fits_any_t<TP, U> >(v, min, max); }

	template<typename U>
	requires std::floating_point<std::remove_reference_t<U>>
	constexpr Saturated(const U& v)
	{ value = std::clamp<float>(v, min, max); }

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	constexpr Saturated(const Saturated<U>& other)
	{ value = std::clamp< modm::fits_any_t<TP, U> >(other.value, min, max); }

	TP
	getValue() const
	{ return value; }

	// Implicitely serve underlying type so you can f.e. pass Saturated to std::abs()
	operator T&() { return value; }
	operator T() const { return value; }

	// comparison operators
	constexpr auto
	operator<=>(const Saturated<T>&) const = default;

	// operator=
	void
	operator=(const Saturated& other)
	{ value = other.value; }

	template<typename U>
	requires std::integral<std::remove_reference_t<U>>
	void
	operator=(const Saturated<U>& other)
	{ value = std::clamp< modm::fits_any_t<TP, U> >(other.value, min, max); }

	// Post: operator++, operator--
	Saturated&
	operator++()
	{
		if (value < max) value++;
		return *this;
	}

	Saturated&
	operator--()
	{
		if (value > min) value--;
		return *this;
	}

	// Pre: operator++(int), operator--(int)
	Saturated
	operator++(int)
	{
		Saturated tmp(*this);
		if (value < max) value++;
		return tmp;
	}

	Saturated
	operator--(int)
	{
		Saturated tmp(*this);
		if (value > min) value--;
		return tmp;
	}

	// operator+=, operator-=, operator*=
	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	Saturated&
	operator+=(const Saturated<U>& other)
	{
		if (__builtin_add_overflow(value, other.value, &value))
			value = max;

		return *this;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	Saturated&
	operator+=(const Saturated<U>& other)
	{
		if (other.value < 0) {
			if (__builtin_sub_overflow(value, -other.value, &value))
				value = min;
		} else {
			if (__builtin_add_overflow(value, other.value, &value))
				value = max;
		}

		return *this;
	}

	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	Saturated&
	operator-=(const Saturated<U>& other)
	{
		if (__builtin_sub_overflow(value, other.value, &value))
			value = min;

		return *this;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	Saturated&
	operator-=(const Saturated<U>& other)
	{
		if (other.value < 0) {
			if (__builtin_add_overflow(value, -other.value, &value))
				value = max;
		} else {
			if (__builtin_sub_overflow(value, other.value, &value))
				value = min;
		}

		return *this;
	}

	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	Saturated&
	operator*=(const Saturated<U>& other)
	{
		if (__builtin_mul_overflow(value, other.value, &value))
			value = max;

		return *this;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	Saturated&
	operator*=(const Saturated<U>& other)
	{
		if (other.value < 0) {
			if (__builtin_mul_overflow(value, -other.value, &value))
				value = max;
			value = -value;
		} else {
			if (__builtin_mul_overflow(value, other.value, &value))
				value = max;
		}

		return *this;
	}

	// OPTIMIZE By whatever reason, for operator*= the compiler doesn't implicitly construct Saturated types.
	// Overload plain types for now:
	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	Saturated&
	operator*=(const U& v)
	{
		if (__builtin_mul_overflow(value, v, &value))
			value = max;

		return *this;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	Saturated&
	operator*=(const U& v)
	{
		if (v < 0) {
			if (__builtin_mul_overflow(value, -v, &value))
				value = max;
			value = -value;
		} else {
			if (__builtin_mul_overflow(value, v, &value))
				value = max;
		}

		return *this;
	}

	// operator+, operator-, operator*
	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	TP
	operator+(const Saturated<U>& other)
	{
		Saturated<TP> tmp;

		if (__builtin_add_overflow(value, other.value, &tmp.value))
			tmp.value = max;

		return tmp.value;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	TP
	operator+(const Saturated<U>& other)
	{
		Saturated<TP> tmp;

		if (other.value < 0) {
			if (__builtin_sub_overflow(value, -other.value, &tmp.value))
				tmp.value = min;
		} else {
			if (__builtin_add_overflow(value, other.value, &tmp.value))
				tmp.value = max;
		}

		return tmp.value;
	}

	template<typename U>
	requires std::unsigned_integral<std::remove_reference_t<U>>
	TP
	operator-(const Saturated<U>& other)
	{
		Saturated<TP> tmp;

		if (__builtin_sub_overflow(value, other.value, &tmp.value))
			tmp.value = min;

		return tmp.value;
	}

	template<typename U>
	requires std::signed_integral<std::remove_reference_t<U>>
	TP
	operator-(const Saturated<U>& other)
	{
		Saturated<TP> tmp;

		if (other.value < 0) {
			if (__builtin_add_overflow(value, -other.value, &tmp.value))
				tmp.value = max;
		} else {
			if (__builtin_sub_overflow(value, other.value, &tmp.value))
				tmp.value = min;
		}

		return tmp.value;
	}

	TP
	operator*(const Saturated<T>& other)
	{
		Saturated<TP> tmp;

		if (__builtin_mul_overflow(value, other.value, &tmp.value))
			tmp.value = max;

		return tmp.value;
	}

	TS
	operator-()
 	{ return -TS(value); }

	void
	absolute()
	// Should be std::abs but that's troubelous for avr-gcc
	// @see: https://stackoverflow.com/questions/1374037/ambiguous-overload-call-to-absdouble
	{ value = abs(value); }

	template<typename>
	friend class Saturated;
};

}  // namespace modm