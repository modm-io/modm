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
#include <cstdint>
#include <limits>
#include <modm/debug/logger.hpp>
#include <modm/math/utils/arithmetic_traits.hpp>
#include <type_traits>

#include <modm/math/utils/integer_traits.hpp>
using namespace std;

namespace modm
{
/**
 * @brief	Saturated arithmetics
 * @see		https://en.wikipedia.org/wiki/Saturation_arithmetic
 *
 * @author	Thomas Sommer
 *
 * @ingroup modm_math_saturated
 */

template<typename T>
requires integral<remove_reference_t<T>>
class Saturated
{
protected:
	T value;

	using TP = remove_reference_t<T>;
	using TS = conditional_t<is_signed_v<T>, T, make_signed_t<modm::WideType<TP>>>;

private:
	static constexpr TP min = numeric_limits<TP>::min();
	static constexpr TP max = numeric_limits<TP>::max();

public:
	// Constructors
	Saturated() = default;

	// ### Construct from T
	constexpr Saturated(const T& value) : value(value){};
	constexpr Saturated(const Saturated<T>& other) : value(other.value){};

	// ### Construct from U
	template<typename U>
	requires integral<remove_reference_t<U>>
	constexpr Saturated(const U& v)
	{
		value = clamp< modm::fits_any_t<TP, U> >(v, min, max);
	}

	template<typename U>
	requires floating_point<remove_reference_t<U>>
	constexpr Saturated(const U& v)
	{
		value = clamp<float>(v, min, max);
	}

	template<typename U>
	requires integral<remove_reference_t<U>>
	constexpr Saturated(const Saturated<U>& other)
	{
		value = clamp< modm::fits_any_t<TP, U> >(other.value, min, max);
	}

	// ### operator++, operator-- [Post]
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

	// operator++(int), operator--(int) [Pre]
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

	// ### operator- [Negate]
	TS
	operator-()
	{
		// Dump negation. Does not clamp.
		return -TS(value);
	}

	// ### operator=

	// Unrequired. generic version below is equivalent
	/* void
	operator=(const T& v) {
		value = clamp<T>(v, min, max);
	} */

	template<typename U>
	requires integral<remove_reference_t<U>>
	void
	operator=(const U& v)
	{
		value = clamp< modm::fits_any_t<TP, U> >(v, min, max);
	}

	void
	operator=(const Saturated& other)
	{
		value = other.value;
	}

	template<typename U>
	requires integral<remove_reference_t<U>>
	void
	operator=(const Saturated<U>& other)
	{
		value = clamp< modm::fits_any_t<TP, U> >(other.value, min, max);
	}

	// ### operator+=, operator-=, operator*=
	template<typename U>
	requires unsigned_integral<remove_reference_t<U>>
	Saturated&
	operator+=(const U& v)
	{
		if (__builtin_add_overflow(value, v, &value)) value = max;
		return *this;
	}

	template<typename U>
	requires signed_integral<remove_reference_t<U>>
	Saturated&
	operator+=(const U& v)
	{
		if (v < 0)
		{
			if (__builtin_sub_overflow(value, -v, &value)) value = min;
		} else
		{
			if (__builtin_add_overflow(value, v, &value)) value = max;
		}
		return *this;
	}

	template<typename U>
	requires unsigned_integral<remove_reference_t<U>>
	Saturated&
	operator-=(const U& v)
	{
		if (__builtin_sub_overflow(value, v, &value)) { value = min; }
		return *this;
	}

	template<typename U>
	requires signed_integral<remove_reference_t<U>>
	Saturated&
	operator-=(const U& v)
	{
		if (v < 0)
		{
			if (__builtin_add_overflow(value, -v, &value)) value = max;
		} else
		{
			if (__builtin_sub_overflow(value, v, &value)) value = min;
		}
		return *this;
	}

	template<typename U>
	requires unsigned_integral<remove_reference_t<U>>
	Saturated&
	operator*=(const U& v)
	{
		if (__builtin_mul_overflow(value, v, &value)) { value = max; }
		return *this;
	}

	template<typename U>
	requires signed_integral<remove_reference_t<U>>
	Saturated&
	operator*=(const U& v)
	{
		if (v < 0)
		{
			// FIXME unfinished business !?
			if (__builtin_mul_overflow(value, -v, &value)) value = max;
			value = -value;
		} else
		{
			if (__builtin_mul_overflow(value, v, &value)) value = max;
		}
		return *this;
	}

	// ### operator+, operator-, operator*
	template<typename U>
	requires unsigned_integral<remove_reference_t<U>>
	TP
	operator+(const U& v)
	{
		Saturated<TP> tmp;

		if (__builtin_add_overflow(value, v, &tmp.value)) tmp.value = max;

		return tmp.value;
	}

	template<typename U>
	requires signed_integral<remove_reference_t<U>>
	TP
	operator+(const U& v)
	{
		Saturated<TP> tmp;

		if (v < 0)
		{
			if (__builtin_sub_overflow(value, -v, &tmp.value)) tmp.value = min;
		} else
		{
			if (__builtin_add_overflow(value, v, &tmp.value)) tmp.value = max;
		}

		return tmp.value;
	}

	template<typename U>
	requires unsigned_integral<remove_reference_t<U>>
	TP
	operator-(const U& v)
	{
		Saturated<TP> tmp;

		if (__builtin_sub_overflow(value, v, &tmp.value)) tmp.value = min;

		return tmp.value;
	}

	template<typename U>
	requires signed_integral<remove_reference_t<U>>
	TP
	operator-(const U& v)
	{
		Saturated<TP> tmp;

		if (v < 0)
		{
			if (__builtin_add_overflow(value, -v, &tmp.value)) tmp.value = max;
		} else
		{
			if (__builtin_sub_overflow(value, v, &tmp.value)) tmp.value = min;
		}

		return tmp.value;
	}

	TP
	operator*(const T& v)
	{
		Saturated<TP> tmp;

		if (__builtin_mul_overflow(value, v, &tmp.value)) tmp.value = max;

		return tmp.value;
	}

	TP
	getValue() const
	{
		return value;
	}

	constexpr auto
	operator<=>(const Saturated<T>&) const = default;

	template<typename>
	friend class Saturated;
};
}  // namespace modm