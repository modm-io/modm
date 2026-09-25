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

#include "consteval_cmath_test.hpp"
#include <modm/math/utils/cmath.hpp>

namespace {

template<std::floating_point T>
constexpr bool
is_negative_zero(T value)
{
	if (value != T{0}) return false;
	return std::signbit(value);
}

static_assert(!is_negative_zero(0.1f));
static_assert(!is_negative_zero(0.0f));
static_assert(is_negative_zero(-0.0f));
static_assert(!is_negative_zero(0.1));
static_assert(!is_negative_zero(0.0));
static_assert(is_negative_zero(-0.0));

// 2^60 + 2^40
template<std::floating_point T>
constexpr T LargeNumber = T{1152922604118474752.0l};

} // namespace

void
ConstevalCmathTest::testCeilFloat()
{
	using modm::detail::consteval_ceil;

	// Standard rounding
	static_assert(consteval_ceil(1.1f) == 2.0f);
	static_assert(consteval_ceil(1.9f) == 2.0f);
	static_assert(consteval_ceil(-1.1f) == -1.0f);
	static_assert(consteval_ceil(-1.9f) == -1.0f);

	// Integers should remain unchanged
	static_assert(consteval_ceil(2.0f) == 2.0f);
	static_assert(consteval_ceil(-2.0f) == -2.0f);

	// Sign of zero (0.0 == -0.0 is true, sign cannot be checked with ==)
	static_assert(consteval_ceil(0.0f) == 0.0f);
	static_assert(!is_negative_zero(consteval_ceil(0.0f)));
	static_assert(is_negative_zero(consteval_ceil(-0.0f)));
	static_assert(is_negative_zero(consteval_ceil(-0.5f)));

	// Mantissa Limit (2^23)
	static_assert(consteval_ceil(8388607.5f) == 8388608.0f); // 2^23 - 0.5
	static_assert(consteval_ceil(8388608.0f) == 8388608.0f); // 2^23
	static_assert(consteval_ceil(8388609.0f) == 8388609.0f); // 2^23 + 1.0

	// Large numbers
	static_assert(consteval_ceil(LargeNumber<float>) == LargeNumber<float>);
	static_assert(consteval_ceil(-LargeNumber<float>) == -LargeNumber<float>);

	// Infinity
	static_assert(consteval_ceil(std::numeric_limits<float>::infinity()) ==
				std::numeric_limits<float>::infinity());

	static_assert(consteval_ceil(-std::numeric_limits<float>::infinity()) ==
				-std::numeric_limits<float>::infinity());

	// NaN
	static_assert(std::isnan(consteval_ceil(std::numeric_limits<float>::quiet_NaN())));
	static_assert(std::isnan(consteval_ceil(std::numeric_limits<float>::signaling_NaN())));

	// dummy, test is compile time only
	TEST_ASSERT_TRUE(true);
}

void
ConstevalCmathTest::testCeilDouble()
{
	using modm::detail::consteval_ceil;

	// Standard rounding
	static_assert(consteval_ceil(1.1) == 2.0);
	static_assert(consteval_ceil(1.9) == 2.0);
	static_assert(consteval_ceil(-1.1) == -1.0);
	static_assert(consteval_ceil(-1.9) == -1.0);

	// Integers should remain unchanged
	static_assert(consteval_ceil(2.0) == 2.0);
	static_assert(consteval_ceil(-2.0) == -2.0);

	// Sign of zero (0.0 == -0.0 is true, sign cannot be checked with ==)
	static_assert(consteval_ceil(0.0) == 0.0);
	static_assert(!is_negative_zero(consteval_ceil(0.0)));
	static_assert(is_negative_zero(consteval_ceil(-0.0)));
	static_assert(is_negative_zero(consteval_ceil(-0.5)));

	// Mantissa Limit (2^52)
	static_assert(consteval_ceil(4503599627370495.5) == 4503599627370496.0); // 2^52 - 0.5
	static_assert(consteval_ceil(4503599627370496.0) == 4503599627370496.0); // 2^52
	static_assert(consteval_ceil(4503599627370497.0) == 4503599627370497.0); // 2^52 + 1.0

	// Large numbers (2^60 + 2^40)
	static_assert(consteval_ceil(LargeNumber<double>) == LargeNumber<double>);
	static_assert(consteval_ceil(-LargeNumber<double>) == -LargeNumber<double>);

	// Infinity
	static_assert(consteval_ceil(std::numeric_limits<double>::infinity()) ==
				std::numeric_limits<double>::infinity());

	static_assert(consteval_ceil(-std::numeric_limits<double>::infinity()) ==
				-std::numeric_limits<double>::infinity());

	// NaN
	static_assert(std::isnan(consteval_ceil(std::numeric_limits<double>::quiet_NaN())));
	static_assert(std::isnan(consteval_ceil(std::numeric_limits<double>::signaling_NaN())));

	// dummy, test is compile time only
	TEST_ASSERT_TRUE(true);
}

void
ConstevalCmathTest::testFloorFloat()
{
	using modm::detail::consteval_floor;

	// Standard rounding
	static_assert(consteval_floor(1.1f) == 1.0f);
	static_assert(consteval_floor(1.9f) == 1.0f);
	static_assert(consteval_floor(-1.1f) == -2.0f);
	static_assert(consteval_floor(-1.9f) == -2.0f);

	// Integers should remain unchanged
	static_assert(consteval_floor(2.0f) == 2.0f);
	static_assert(consteval_floor(-2.0f) == -2.0f);

	// Sign of zero (0.0 == -0.0 is true, sign cannot be checked with ==)
	static_assert(consteval_floor(0.0f) == 0.0f);
	static_assert(consteval_floor(-0.0f) == -0.0f);
	static_assert(!is_negative_zero(consteval_floor(0.0f)));
	static_assert(is_negative_zero(consteval_floor(-0.0f)));

	// Mantissa Limit (2^23)
	static_assert(consteval_floor(8388607.5f) == 8388607.0f); // 2^23 - 0.5
	static_assert(consteval_floor(8388608.0f) == 8388608.0f); // 2^23
	static_assert(consteval_floor(8388609.0f) == 8388609.0f); // 2^23 + 1.0

	// Large numbers
	static_assert(consteval_floor(LargeNumber<float>) == LargeNumber<float>);
	static_assert(consteval_floor(-LargeNumber<float>) == -LargeNumber<float>);

	// Infinity
	static_assert(consteval_floor(std::numeric_limits<float>::infinity()) ==
								  std::numeric_limits<float>::infinity());

	static_assert(consteval_floor(-std::numeric_limits<float>::infinity()) ==
								  -std::numeric_limits<float>::infinity());

	// NaN
	static_assert(std::isnan(consteval_floor(std::numeric_limits<float>::quiet_NaN())));
	static_assert(std::isnan(consteval_floor(std::numeric_limits<float>::signaling_NaN())));

	// dummy, test is compile time only
	TEST_ASSERT_TRUE(true);
}

void
ConstevalCmathTest::testFloorDouble()
{
	using modm::detail::consteval_floor;

	// Standard rounding
	static_assert(consteval_floor(1.1) == 1.0);
	static_assert(consteval_floor(1.9) == 1.0);
	static_assert(consteval_floor(-1.1) == -2.0);
	static_assert(consteval_floor(-1.9) == -2.0);

	// Integers should remain unchanged
	static_assert(consteval_floor(2.0) == 2.0);
	static_assert(consteval_floor(-2.0) == -2.0);

	// Sign of zero (0.0 == -0.0 is true, sign cannot be checked with ==)
	static_assert(consteval_floor(0.0) == 0.0);
	static_assert(consteval_floor(-0.0) == -0.0);
	static_assert(!is_negative_zero(consteval_floor(0.0)));
	static_assert(is_negative_zero(consteval_floor(-0.0)));

	// Mantissa Limit (2^52)
	static_assert(consteval_floor(4503599627370495.5) == 4503599627370495.0); // 2^52 - 0.5
	static_assert(consteval_floor(4503599627370496.0) == 4503599627370496.0); // 2^52
	static_assert(consteval_floor(4503599627370497.0) == 4503599627370497.0); // 2^52 + 1.0

	// Large numbers
	static_assert(consteval_floor(LargeNumber<double>) == LargeNumber<double>);
	static_assert(consteval_floor(-LargeNumber<double>) == -LargeNumber<double>);

	// Infinity
	static_assert(consteval_floor(std::numeric_limits<double>::infinity()) ==
								  std::numeric_limits<double>::infinity());

	static_assert(consteval_floor(-std::numeric_limits<double>::infinity()) ==
								  -std::numeric_limits<double>::infinity());

	// NaN
	static_assert(std::isnan(consteval_floor(std::numeric_limits<double>::quiet_NaN())));
	static_assert(std::isnan(consteval_floor(std::numeric_limits<double>::signaling_NaN())));

	// dummy, test is compile time only
	TEST_ASSERT_TRUE(true);
}

void
ConstevalCmathTest::testRoundFloat()
{
	using modm::detail::consteval_round;

	// Standard rounding
	static_assert(consteval_round(1.1f) == 1.0f);
	static_assert(consteval_round(1.4f) == 1.0f);
	static_assert(consteval_round(1.5f) == 2.0f);
	static_assert(consteval_round(1.9f) == 2.0f);
	static_assert(consteval_round(-1.1f) == -1.0f);
	static_assert(consteval_round(-1.4f) == -1.0f);
	static_assert(consteval_round(-1.5f) == -2.0f);
	static_assert(consteval_round(-1.9f) == -2.0f);

	// Integers should remain unchanged
	static_assert(consteval_round(2.0f) == 2.0f);
	static_assert(consteval_round(-2.0f) == -2.0f);

	// Sign of zero (0.0 == -0.0 is true, sign cannot be checked with ==)
	static_assert(consteval_round(0.0f) == 0.0f);
	static_assert(consteval_round(-0.0f) == -0.0f);
	static_assert(consteval_round(-0.4f) == -0.0f);
	static_assert(!is_negative_zero(consteval_round(0.0f)));
	static_assert(is_negative_zero(consteval_round(-0.0f)));
	static_assert(is_negative_zero(consteval_round(-0.4f)));

	// Mantissa Limit (2^23)
	static_assert(consteval_round(8388607.5f) == 8388608.0f); // 2^23 - 0.5
	static_assert(consteval_round(8388608.0f) == 8388608.0f); // 2^23
	static_assert(consteval_round(8388609.0f) == 8388609.0f); // 2^23 + 1.0

	// Large numbers
	static_assert(consteval_round(LargeNumber<float>) == LargeNumber<float>);
	static_assert(consteval_round(-LargeNumber<float>) == -LargeNumber<float>);

	// Infinity
	static_assert(consteval_round(std::numeric_limits<float>::infinity()) ==
								  std::numeric_limits<float>::infinity());

	static_assert(consteval_round(-std::numeric_limits<float>::infinity()) ==
								  -std::numeric_limits<float>::infinity());

	// NaN
	static_assert(std::isnan(consteval_round(std::numeric_limits<float>::quiet_NaN())));
	static_assert(std::isnan(consteval_round(std::numeric_limits<float>::signaling_NaN())));

	// dummy, test is compile time only
	TEST_ASSERT_TRUE(true);
}

void
ConstevalCmathTest::testRoundDouble()
{
	using modm::detail::consteval_round;

	// Standard rounding
	static_assert(consteval_round(1.1) == 1.0);
	static_assert(consteval_round(1.4) == 1.0);
	static_assert(consteval_round(1.5) == 2.0);
	static_assert(consteval_round(1.9) == 2.0);
	static_assert(consteval_round(-1.1) == -1.0);
	static_assert(consteval_round(-1.4) == -1.0);
	static_assert(consteval_round(-1.5) == -2.0);
	static_assert(consteval_round(-1.9) == -2.0);

	// Integers should remain unchanged
	static_assert(consteval_round(2.0) == 2.0);
	static_assert(consteval_round(-2.0) == -2.0);

	// Sign of zero (0.0 == -0.0 is true, sign cannot be checked with ==)
	static_assert(consteval_round(0.0) == 0.0);
	static_assert(consteval_round(-0.0) == -0.0);
	static_assert(!is_negative_zero(consteval_round(0.0)));
	static_assert(is_negative_zero(consteval_round(-0.0)));

	// Mantissa Limit (2^52)
	static_assert(consteval_round(4503599627370495.5) == 4503599627370496.0); // 2^52 - 0.5
	static_assert(consteval_round(4503599627370496.0) == 4503599627370496.0); // 2^52
	static_assert(consteval_round(4503599627370497.0) == 4503599627370497.0); // 2^52 + 1.0

	// Large numbers
	static_assert(consteval_round(LargeNumber<double>) == LargeNumber<double>);
	static_assert(consteval_round(-LargeNumber<double>) == -LargeNumber<double>);

	// Infinity
	static_assert(consteval_round(std::numeric_limits<double>::infinity()) ==
								  std::numeric_limits<double>::infinity());

	static_assert(consteval_round(-std::numeric_limits<double>::infinity()) ==
								  -std::numeric_limits<double>::infinity());

	// NaN
	static_assert(std::isnan(consteval_round(std::numeric_limits<double>::quiet_NaN())));
	static_assert(std::isnan(consteval_round(std::numeric_limits<double>::signaling_NaN())));

	// dummy, test is compile time only
	TEST_ASSERT_TRUE(true);
}
