/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/algorithm/prescaler.hpp>

#include "prescaler_test.hpp"
using namespace modm::literals;

void
PrescalerTest::testList()
{
	// one prescaler
	{
		const auto result = modm::Prescaler::from_list(
				1_MHz, 1_MHz, {1});
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 1ul);
		TEST_ASSERT_EQUALS(result.frequency, 1'000'000ul);
	}
	// two prescaler 1
	{
		const auto result = modm::Prescaler::from_list(
				1_MHz, 1_MHz, {1, 2});
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 1ul);
		TEST_ASSERT_EQUALS(result.frequency, 1'000'000ul);
	}
	// two prescaler 2
	{
		const auto result = modm::Prescaler::from_list(
				1_MHz, 0.5_MHz, {1, 2});
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 2ul);
		TEST_ASSERT_EQUALS(result.frequency, 500'000ul);
	}
	// exact matches
	{
		const auto result = modm::Prescaler::from_list(
				1_MHz, 0.1_MHz, {1,2,9,10});
		TEST_ASSERT_EQUALS(result.index, 3ul);
		TEST_ASSERT_EQUALS(result.prescaler, 10ul);
		TEST_ASSERT_EQUALS(result.frequency, 100'000ul);
	}
	{
		const auto result = modm::Prescaler::from_list(
				1_MHz, 0.5_MHz, {1,2,9,10});
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 2ul);
		TEST_ASSERT_EQUALS(result.frequency, 500'000ul);
	}
	// inexact matches within prescaler range
	{
		const auto result = modm::Prescaler::from_list(
				1_MHz, 0.2_MHz, {1,2,9,10});
		TEST_ASSERT_EQUALS(result.index, 2ul);
		TEST_ASSERT_EQUALS(result.prescaler, 9ul);
		TEST_ASSERT_EQUALS(result.frequency, 111'111ul);
	}
	{
		const auto result = modm::Prescaler::from_list(
				1_MHz, 0.7_MHz, {1,2,9,10});
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 2ul);
		TEST_ASSERT_EQUALS(result.frequency, 500'000ul);
	}
	// inexact matches on error border within prescaler range
	{
		const auto result = modm::Prescaler::from_list(
				1_MHz, 0.75_MHz, {1,2,9,10});
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 2ul);
		TEST_ASSERT_EQUALS(result.frequency, 500'000ul);
	}
	// clamping below prescaler range
	{
		const auto result = modm::Prescaler::from_list(
				1_MHz, 2_MHz, {1,2,9,10});
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 1ul);
		TEST_ASSERT_EQUALS(result.frequency, 1'000'000ul);
	}
	// clamping above prescaler range
	{
		const auto result = modm::Prescaler::from_list(
				1_MHz, 0.05_MHz, {1,2,9,10});
		TEST_ASSERT_EQUALS(result.index, 3ul);
		TEST_ASSERT_EQUALS(result.prescaler, 10ul);
		TEST_ASSERT_EQUALS(result.frequency, 100'000ul);
	}

	// constexpr with static_assert
	{
		constexpr auto result = modm::Prescaler::from_list(
				1_MHz, 0.1_MHz, {1,2,9,10});
		static_assert(result.index == 3ul);
		static_assert(result.prescaler == 10ul);
		static_assert(result.frequency == 100'000ul);
	}
}


void
PrescalerTest::testFunction()
{
	// identity with one prescaler
	{
		const auto result = modm::Prescaler::from_function(
				1_MHz, 0.25_MHz, 1, 1, [](uint32_t ii) { return ii; });
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 1ul);
		TEST_ASSERT_EQUALS(result.frequency, 1'000'000ul);
	}
	// identity with two prescalers
	{
		const auto result = modm::Prescaler::from_function(
				1_MHz, 0.25_MHz, 1, 2, [](uint32_t ii) { return ii; });
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 2ul);
		TEST_ASSERT_EQUALS(result.frequency, 500'000ul);
	}
	// identity
	{
		const auto result = modm::Prescaler::from_function(
				1_MHz, 0.25_MHz, 1, 10, [](uint32_t ii) { return ii; });
		TEST_ASSERT_EQUALS(result.index, 3ul);
		TEST_ASSERT_EQUALS(result.prescaler, 4ul);
		TEST_ASSERT_EQUALS(result.frequency, 250'000ul);
	}
	// multiply
	{
		const auto result = modm::Prescaler::from_function(
				1_MHz, 0.25_MHz, 1, 10, [](uint32_t ii) { return ii * 2; });
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 4ul);
		TEST_ASSERT_EQUALS(result.frequency, 250'000ul);
	}
	// square
	{
		const auto result = modm::Prescaler::from_function(
				1_MHz, 0.1_MHz, 1, 10, [](uint32_t ii) { return ii * ii; });
		TEST_ASSERT_EQUALS(result.index, 2ul);
		TEST_ASSERT_EQUALS(result.prescaler, 9ul);
		TEST_ASSERT_EQUALS(result.frequency, 111'111ul);
	}
	{
		const auto result = modm::Prescaler::from_function(
				1_MHz, 0.01_MHz, 1, 10, [](uint32_t ii) { return ii * ii; });
		TEST_ASSERT_EQUALS(result.index, 9ul);
		TEST_ASSERT_EQUALS(result.prescaler, 100ul);
		TEST_ASSERT_EQUALS(result.frequency, 10'000ul);
	}

	// constexpr with static_assert
	{
		constexpr auto result = modm::Prescaler::from_function(
				1_MHz, 0.167_MHz, 1, 10, [](uint32_t ii) { return ii * 2; });
		static_assert(result.index == 2ul);
		static_assert(result.prescaler == 6ul);
		static_assert(result.frequency == 166'666ul);
	}
}

void
PrescalerTest::testRange()
{
	// one prescaler
	{
		const auto result = modm::Prescaler::from_range(
				1_MHz, 0.25_MHz, 1, 1);
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 1ul);
		TEST_ASSERT_EQUALS(result.frequency, 1'000'000ul);
	}
	// two prescalers
	{
		const auto result = modm::Prescaler::from_range(
				1_MHz, 0.25_MHz, 1, 2);
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 2ul);
		TEST_ASSERT_EQUALS(result.frequency, 500'000ul);
	}
	// clamp lower range
	{
		const auto result = modm::Prescaler::from_range(
				1_MHz, 2_MHz, 1, 10);
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 1ul);
		TEST_ASSERT_EQUALS(result.frequency, 1'000'000ul);
	}
	// clamp upper range
	{
		const auto result = modm::Prescaler::from_range(
				1_MHz, 0.05_MHz, 1, 10);
		TEST_ASSERT_EQUALS(result.index, 9ul);
		TEST_ASSERT_EQUALS(result.prescaler, 10ul);
		TEST_ASSERT_EQUALS(result.frequency, 100'000ul);
	}
	// decide on error border
	{
		const auto result = modm::Prescaler::from_range(
				1_MHz, 0.75_MHz, 1, 10);
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 2ul);
		TEST_ASSERT_EQUALS(result.frequency, 500'000ul);
	}
}

void
PrescalerTest::testPower()
{
	// one prescaler
	{
		const auto result = modm::Prescaler::from_power(
				1_MHz, 0.25_MHz, 1, 1);
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 1ul);
		TEST_ASSERT_EQUALS(result.frequency, 1'000'000ul);
	}
	// two prescalers
	{
		const auto result = modm::Prescaler::from_power(
				1_MHz, 0.25_MHz, 1, 2);
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 2ul);
		TEST_ASSERT_EQUALS(result.frequency, 500'000ul);
	}
	// middle of range
	{
		const auto result = modm::Prescaler::from_power(
				1_MHz, 0.05_MHz, 1, 128);
		TEST_ASSERT_EQUALS(result.index, 4ul);
		TEST_ASSERT_EQUALS(result.prescaler, 16ul);
		TEST_ASSERT_EQUALS(result.frequency, 62'500ul);
	}
	// decide on error border
	{
		const auto result = modm::Prescaler::from_power(
				1_MHz, 0.75_MHz, 1, 128);
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 1ul);
		TEST_ASSERT_EQUALS(result.frequency, 1'000'000ul);
	}
	{
		const auto result = modm::Prescaler::from_power(
				1_MHz, 0.74_MHz, 1, 128);
		TEST_ASSERT_EQUALS(result.index, 1ul);
		TEST_ASSERT_EQUALS(result.prescaler, 2ul);
		TEST_ASSERT_EQUALS(result.frequency, 500'000ul);
	}
	// clamp lower range
	{
		const auto result = modm::Prescaler::from_power(
				1_MHz, 2_MHz, 1, 16);
		TEST_ASSERT_EQUALS(result.index, 0ul);
		TEST_ASSERT_EQUALS(result.prescaler, 1ul);
		TEST_ASSERT_EQUALS(result.frequency, 1'000'000ul);
	}
	// clamp upper range
	{
		const auto result = modm::Prescaler::from_power(
				1_MHz, 0.05_MHz, 1, 8);
		TEST_ASSERT_EQUALS(result.index, 3ul);
		TEST_ASSERT_EQUALS(result.prescaler, 8ul);
		TEST_ASSERT_EQUALS(result.frequency, 125'000ul);
	}
}
