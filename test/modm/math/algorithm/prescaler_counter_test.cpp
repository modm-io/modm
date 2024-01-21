/*
 * Copyright (c) 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/algorithm/prescaler.hpp>
#include <modm/math/algorithm/prescaler_counter.hpp>

#include "prescaler_counter_test.hpp"
using namespace modm::literals;

using Prescaler = modm::GenericPrescalerCounter<double>;

void
PrescalerCounterTest::testList()
{
	// one prescaler
	{
		const auto result = Prescaler::from_list(
				32_kHz, 1_Hz, 1ul << 12, {8});
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 8ul);
		TEST_ASSERT_EQUALS(result.counter, 4000ul);
		TEST_ASSERT_EQUALS_FLOAT(result.frequency, 1.0);
		TEST_ASSERT_EQUALS_FLOAT(result.error, 0.0);
	}
	// two prescaler 1
	{
		const auto result = Prescaler::from_list(
				32_kHz, 1_Hz, 1ul << 12, {4, 8});
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 8ul);
		TEST_ASSERT_EQUALS(result.counter, 4000ul);
		TEST_ASSERT_EQUALS_FLOAT(result.frequency, 1.0);
		TEST_ASSERT_EQUALS_FLOAT(result.error, 0.0);
	}
	// two prescaler 2
	{
		const auto result = Prescaler::from_list(
				32_kHz, 2_Hz, 1ul << 12, {9, 11});
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 9ul);
		TEST_ASSERT_EQUALS(result.counter, 1778ul);
		TEST_ASSERT_EQUALS_FLOAT(result.frequency, 1.99975f);
		TEST_ASSERT_EQUALS_FLOAT(result.error, 0.000125);
	}
	// exact matches
	{
		const auto result = Prescaler::from_list(
				40_kHz, 4_Hz, 1ul << 16, {7,8,9,10,11,12});
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 8ul);
		TEST_ASSERT_EQUALS(result.counter, 1250ul);
		TEST_ASSERT_EQUALS(result.frequency, 4);
		TEST_ASSERT_EQUALS(result.error, 0);
	}
	{
		const auto result = Prescaler::from_list(
				32_kHz, 0.25, 1ul << 10, {128,64,32});
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 128ul);
		TEST_ASSERT_EQUALS(result.counter, 1000ul);
		TEST_ASSERT_EQUALS(result.frequency, 0.25);
		TEST_ASSERT_EQUALS(result.error, 0);
	}
	// inexact matches within prescaler range
	{
		const auto result = Prescaler::from_list(
				32_kHz, 1.902515, 1ul << 12, {3,5,7,11});
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 5ul);
		TEST_ASSERT_EQUALS(result.counter, 3364ul);
		TEST_ASSERT_EQUALS_FLOAT(result.frequency, 1.9025f);
		TEST_ASSERT_EQUALS_FLOAT(result.error, 0.00001);
	}
	{
		const auto result = Prescaler::from_list(
				2_kHz, 1.093948, 1ul << 8, {11,13,17});
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 11ul);
		TEST_ASSERT_EQUALS(result.counter, 166ul);
		TEST_ASSERT_EQUALS_FLOAT(result.frequency, 1.095290f);
		TEST_ASSERT_EQUALS_FLOAT(result.error, -0.00123);
	}
	{
		const auto result = Prescaler::from_list(
				2_kHz, 1.25, 1ul << 6, {23,29,31});
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 29ul);
		TEST_ASSERT_EQUALS(result.counter, 55ul);
		TEST_ASSERT_EQUALS_FLOAT(result.frequency, 1.253918);
		TEST_ASSERT_EQUALS_FLOAT(result.error, -0.003135);
	}
	// inexact matches on error border within prescaler range
	{
		const auto result = Prescaler::from_list(
				1_kHz, 0.8138020833, 1ul << 6, {1024}); // ±20%
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 1024ul);
		TEST_ASSERT_EQUALS(result.counter, 1ul);
		TEST_ASSERT_EQUALS_FLOAT(result.frequency, 0.9765625);
		TEST_ASSERT_EQUALS_FLOAT(result.error, -0.2);
	}
	// clamping below prescaler range
	{
		const auto result = Prescaler::from_list(
				1_kHz, 10_kHz, 1ul << 10, {9,1,10,2});
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 1ul);
		TEST_ASSERT_EQUALS(result.counter, 1ul);
		TEST_ASSERT_EQUALS_FLOAT(result.frequency, 1000);
		TEST_ASSERT_EQUALS_FLOAT(result.error, 0.9);
	}
	// clamping above prescaler range
	{
		const auto result = Prescaler::from_list(
				10_kHz, 1_Hz, 1ul << 10, {1,4,3,2});
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 4ul);
		TEST_ASSERT_EQUALS(result.counter, 1024ul);
		TEST_ASSERT_EQUALS_FLOAT(result.frequency, 2.44140625);
		TEST_ASSERT_EQUALS_FLOAT(result.error, -1.44141);
	}
	// const with static_assert
	{
		constexpr auto result = Prescaler::from_list(
				10_kHz, 1_Hz, 1ul << 10, {51,7,11});
		static_assert(result.index == 2ul);
		static_assert(result.prescaler == 11ul);
		static_assert(result.counter == 909);
		static_assert(result.frequency - 1.0001 < 0.01);
		static_assert(result.error - -0.00001 < 0.01);
	}
}

void
PrescalerCounterTest::testFunction()
{
	// implemented via iterator
	{
		const auto result = Prescaler::from_function(
				32_kHz, 2_Hz, 1ul << 12, 9, 11, [](uint32_t ii) { return 2*ii; });
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 20ul);
		TEST_ASSERT_EQUALS(result.counter, 800ul);
		TEST_ASSERT_EQUALS(result.frequency, 2);
		TEST_ASSERT_EQUALS(result.error, 0);
	}
	{
		const auto result = Prescaler::from_function(
				32_kHz, 2_Hz, 1ul << 12, 11, 9, [](uint32_t ii) { return 2*ii; });
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 20ul);
		TEST_ASSERT_EQUALS(result.counter, 800ul);
		TEST_ASSERT_EQUALS(result.frequency, 2);
		TEST_ASSERT_EQUALS(result.error, 0);
	}
}

void
PrescalerCounterTest::testLinear()
{
	// implemented via iterator
	{
		const auto result = Prescaler::from_linear(
				40_kHz, 4_Hz, 1ul << 10, 8, 12);
		TEST_ASSERT_EQUALS(result.index, 2ul);
		TEST_ASSERT_EQUALS(result.prescaler, 10ul);
		TEST_ASSERT_EQUALS(result.counter, 1000ul);
		TEST_ASSERT_EQUALS(result.frequency, 4);
		TEST_ASSERT_EQUALS(result.error, 0);
	}
	{
		const auto result = Prescaler::from_linear(
				40_kHz, 4_Hz, 1ul << 10, 12, 8);
		TEST_ASSERT_EQUALS(result.index, 2ul);
		TEST_ASSERT_EQUALS(result.prescaler, 10ul);
		TEST_ASSERT_EQUALS(result.counter, 1000ul);
		TEST_ASSERT_EQUALS(result.frequency, 4);
		TEST_ASSERT_EQUALS(result.error, 0);
	}
}

void
PrescalerCounterTest::testPower()
{
	// implemented via iterator
	{
		const auto result = Prescaler::from_power(
				40_kHz, 4_Hz, 1ul << 10, 8, 32);
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 16ul);
		TEST_ASSERT_EQUALS(result.counter, 625ul);
		TEST_ASSERT_EQUALS(result.frequency, 4);
		TEST_ASSERT_EQUALS(result.error, 0);
	}
	{
		const auto result = Prescaler::from_power(
				40_kHz, 4_Hz, 1ul << 10, 32, 8);
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 16ul);
		TEST_ASSERT_EQUALS(result.counter, 625ul);
		TEST_ASSERT_EQUALS(result.frequency, 4);
		TEST_ASSERT_EQUALS(result.error, 0);
	}
}
