/*
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "arithmetic_traits_test.hpp"

#include <modm/math/utils/arithmetic_traits.hpp>
#include <stdint.h>
#include <cstdint>
#include <climits>

template<typename T>
constexpr T Min_v = std::numeric_limits<T>::min();

template<typename T>
constexpr T Max_v = std::numeric_limits<T>::max();

void
ArithmeticTraitsTest::testMinMax()
{
	// test signed int types
	// assuming ints are two's complement and (u)int*_t types exist
	static_assert(Min_v<int8_t>  == -128);
	static_assert(Min_v<int16_t> == -32768LL);
	static_assert(Min_v<int32_t> == (-2147483648LL));
	static_assert(Min_v<int64_t> == (-9223372036854775807LL - 1LL));

	static_assert(Max_v<int8_t>  == 127);
	static_assert(Max_v<int16_t> == 32767LL);
	static_assert(Max_v<int32_t> == 2147483647LL);
	static_assert(Max_v<int64_t> == 9223372036854775807LL);

	static_assert(Min_v<int8_t>  == INT8_MIN);
	static_assert(Min_v<int16_t> == INT16_MIN);
	static_assert(Min_v<int32_t> == INT32_MIN);
	static_assert(Min_v<int64_t> == INT64_MIN);

	static_assert(Max_v<int8_t>  == INT8_MAX);
	static_assert(Max_v<int16_t> == INT16_MAX);
	static_assert(Max_v<int32_t> == INT32_MAX);
	static_assert(Max_v<int64_t> == INT64_MAX);

	// test unsigned int types
	static_assert(Min_v<uint8_t>  == 0);
	static_assert(Min_v<uint16_t> == 0);
	static_assert(Min_v<uint32_t> == 0);
	static_assert(Min_v<uint64_t> == 0);

	static_assert(Max_v<uint8_t>  == 255);
	static_assert(Max_v<uint16_t> == 65535ULL);
	static_assert(Max_v<uint32_t> == 4294967295ULL);
	static_assert(Max_v<uint64_t> == 18446744073709551615ULL);

	static_assert(Max_v<uint8_t>  == UINT8_MAX);
	static_assert(Max_v<uint16_t> == UINT16_MAX);
	static_assert(Max_v<uint32_t> == UINT32_MAX);
	static_assert(Max_v<uint64_t> == UINT64_MAX);

	// test bool
	static_assert(Min_v<bool> == false);
	static_assert(Max_v<bool> == true);

	// test char
	static_assert(Min_v<char> == CHAR_MIN);
	static_assert(Max_v<char> == CHAR_MAX);
	static_assert(Min_v<signed char> == SCHAR_MIN);
	static_assert(Max_v<signed char> == SCHAR_MAX);
	static_assert(Max_v<unsigned char> == UCHAR_MAX);

	// dummy, test is compile time only
	TEST_ASSERT_TRUE(true);
}

namespace
{

template<size_t N>
constexpr size_t
len([[maybe_unused]] const char (&arr) [N])
{
	return (N == 0) ? 0 : N - 1;
}

}

template<typename T>
constexpr size_t Digits_v = modm::ArithmeticTraits<T>::decimalDigits;

void
ArithmeticTraitsTest::testDigits()
{
	static_assert(Digits_v<int8_t>  == len("-127"));
	static_assert(Digits_v<int16_t> == len("-32768"));
	static_assert(Digits_v<int32_t> == len("-2147483648"));
	static_assert(Digits_v<int64_t> == len("-9223372036854775808"));

	static_assert(Digits_v<uint8_t>  == len("255"));
	static_assert(Digits_v<uint16_t> == len("65535"));
	static_assert(Digits_v<uint32_t> == len("4294967295"));
	static_assert(Digits_v<uint64_t> == len("18446744073709551615"));

	// dummy, test is compile time only
	TEST_ASSERT_TRUE(true);
}

template<typename T>
using Wide_t = modm::WideType<T>;

// check if U and V represent compatible types
template<typename U, typename V>
constexpr bool Same_v =
	(Min_v<U> == Min_v<V>) &&
	(Max_v<U> == Max_v<V>) &&
	sizeof(U) == sizeof(V) &&
	std::is_assignable_v<U&, V> &&
	std::is_assignable_v<V&, U>;

void
ArithmeticTraitsTest::testWideType()
{
	static_assert(Same_v<Wide_t<int8_t>,	int16_t>);
	static_assert(Same_v<Wide_t<int16_t>,	int32_t>);
	static_assert(Same_v<Wide_t<int32_t>,	int64_t>);
	static_assert(Same_v<Wide_t<int64_t>,	double>);

	static_assert(Same_v<Wide_t<uint8_t>,	uint16_t>);
	static_assert(Same_v<Wide_t<uint16_t>, 	uint32_t>);
	static_assert(Same_v<Wide_t<uint32_t>, 	uint64_t>);
	static_assert(Same_v<Wide_t<uint64_t>, 	double>);

	static_assert(Same_v<Wide_t<intmax_t>,	double>);
	static_assert(Same_v<Wide_t<uintmax_t>, double>);

	static_assert(Same_v<Wide_t<float>,			float>);
	static_assert(Same_v<Wide_t<double>,		double>);
	static_assert(Same_v<Wide_t<long double>,	long double>);

	// dummy, test is compile time only
	TEST_ASSERT_TRUE(true);
}

template<typename T>
using Signed_t = modm::SignedType<T>;

template<typename T>
using Unsigned_t = modm::UnsignedType<T>;

void
ArithmeticTraitsTest::testSignedUnsigned()
{
	// test int types
	static_assert(Same_v<Signed_t<int8_t>,	int8_t>);
	static_assert(Same_v<Signed_t<int16_t>,	int16_t>);
	static_assert(Same_v<Signed_t<int32_t>,	int32_t>);
	static_assert(Same_v<Signed_t<int64_t>,	int64_t>);

	static_assert(Same_v<Signed_t<short>,		short>);
	static_assert(Same_v<Signed_t<int>,			int>);
	static_assert(Same_v<Signed_t<long>,		long>);
	static_assert(Same_v<Signed_t<long long>,	long long>);

	static_assert(Same_v<Signed_t<uint8_t>,		int8_t>);
	static_assert(Same_v<Signed_t<uint16_t>,	int16_t>);
	static_assert(Same_v<Signed_t<uint32_t>,	int32_t>);
	static_assert(Same_v<Signed_t<uint64_t>,	int64_t>);

	static_assert(Same_v<Signed_t<unsigned short>,		short>);
	static_assert(Same_v<Signed_t<unsigned int>,		int>);
	static_assert(Same_v<Signed_t<unsigned long>,		long>);
	static_assert(Same_v<Signed_t<unsigned long long>,	long long>);

	static_assert(Same_v<Unsigned_t<int8_t>,	uint8_t>);
	static_assert(Same_v<Unsigned_t<int16_t>,	uint16_t>);
	static_assert(Same_v<Unsigned_t<int32_t>,	uint32_t>);
	static_assert(Same_v<Unsigned_t<int64_t>,	uint64_t>);

	static_assert(Same_v<Unsigned_t<short>,		unsigned short>);
	static_assert(Same_v<Unsigned_t<int>,		unsigned int>);
	static_assert(Same_v<Unsigned_t<long>,		unsigned long>);
	static_assert(Same_v<Unsigned_t<long long>,	unsigned long long>);

	static_assert(Same_v<Unsigned_t<uint8_t>,	uint8_t>);
	static_assert(Same_v<Unsigned_t<uint16_t>,	uint16_t>);
	static_assert(Same_v<Unsigned_t<uint32_t>,	uint32_t>);
	static_assert(Same_v<Unsigned_t<uint64_t>,	uint64_t>);

	static_assert(Same_v<Unsigned_t<unsigned short>,		unsigned short>);
	static_assert(Same_v<Unsigned_t<unsigned int>,			unsigned int>);
	static_assert(Same_v<Unsigned_t<unsigned long>,			unsigned long>);
	static_assert(Same_v<Unsigned_t<unsigned long long>,	unsigned long long>);

	// test float types
	static_assert(Same_v<Signed_t<float>,		float>);
	static_assert(Same_v<Signed_t<double>,		double>);
	static_assert(Same_v<Signed_t<long double>,	long double>);

	static_assert(Same_v<Unsigned_t<float>,			float>);
	static_assert(Same_v<Unsigned_t<double>,		double>);
	static_assert(Same_v<Unsigned_t<long double>,	long double>);

	// dummy, test is compile time only
	TEST_ASSERT_TRUE(true);
}
