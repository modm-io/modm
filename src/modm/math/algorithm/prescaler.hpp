/*
 * Copyright (c) 2019, Niklas Hauser
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/math/units.hpp>

#include <algorithm>
#include <cstdint>
#include <ranges>

namespace modm
{

/**
 * Computes the best fitting prescaler from a list or range.
 *
 * @note For ranges the end is *inclusive*: [begin, end]!
 * @ingroup	modm_math_algorithm
 */
template<typename T>
class
GenericPrescaler
{
public:
	struct Result
	{
		constexpr Result(T input_frequency, T desired_frequency,
						 uint32_t index, uint32_t prescaler) :
			index{index}, prescaler{prescaler},
			frequency{input_frequency / prescaler},
			input_frequency{input_frequency},
			desired_frequency{desired_frequency},
			error{1.f - float(input_frequency) / float(prescaler * desired_frequency)}
		{}
		/// Zero-indexed prescaler result
		uint32_t index;
		/// Prescaler value
		uint32_t prescaler;
		/// Generated frequency
		T frequency;
		/// Input frequency
		T input_frequency;
		/// Desired Frequency
		T desired_frequency;
		/// Relative Frequency Error
		float error;
	};

public:
	/// From any forward range.
	/// @pre The container must be sorted from low to high numbers!
	static constexpr Result
	from_range(T input_frequency, T desired_frequency,
			   std::ranges::forward_range auto&& prescalers)
	{
		const double desired = double(input_frequency) / desired_frequency;
		const size_t prescaler_size = std::ranges::distance(
				std::ranges::begin(prescalers), std::ranges::end(prescalers));
		uint32_t prescaler_floor{*std::ranges::begin(prescalers)};
		uint32_t prescaler_ceiling{*std::ranges::begin(prescalers)};
		uint32_t ii_floor{0}, ii_ceiling{0};
		if (desired <= prescaler_floor or prescaler_size == 1)
			return {input_frequency, desired_frequency, 0, prescaler_floor};
		for (uint32_t prescaler : prescalers)
		{
			prescaler_ceiling = prescaler; ii_ceiling++;
			if (prescaler_floor <= desired and desired < prescaler_ceiling)
				break;
			prescaler_floor = prescaler; ii_floor++;
		}
		const T baud_lower = input_frequency / prescaler_ceiling;
		const T baud_upper = input_frequency / prescaler_floor;
		const double baud_middle = (baud_upper + baud_lower) / 2.0;

		if (desired_frequency <= baud_middle)
			return {input_frequency, desired_frequency, ii_ceiling-1, prescaler_ceiling};
		return {input_frequency, desired_frequency, ii_floor-1, prescaler_floor};
	}

	/// From a initializer list.
	/// @pre The list must be sorted from low to high numbers!
	static constexpr Result
	from_list(T input_frequency, T desired_frequency,
			  std::initializer_list<uint32_t> prescalers)
	{
		return from_range(input_frequency, desired_frequency, prescalers);
	}

	/// From any linear range via modifier function.
	/// @note the range end is *inclusive*: [begin, end].
	/// @pre begin must be smaller or equal than end!
	template< typename Function >
	static constexpr Result
	from_function(T input_frequency, T desired_frequency,
				  uint32_t begin, uint32_t end, Function prescaler_modifier)
	{
		struct linear_range_iterator
		{
			using iterator_category [[maybe_unused]] = std::forward_iterator_tag;
			using value_type [[maybe_unused]] = uint32_t;
			using difference_type [[maybe_unused]] = int32_t;
			using pointer [[maybe_unused]] = value_type*;
			using reference [[maybe_unused]] = value_type&;

			uint32_t state;
			Function *modifier;

			constexpr value_type operator*() const { return (*modifier)(state); }
			constexpr linear_range_iterator& operator++() { state++; return *this; }
			constexpr linear_range_iterator operator++(int) { auto old(*this); ++*this; return old; }
			constexpr bool operator==(const linear_range_iterator &o) const { return state == o.state; }
			constexpr difference_type operator-(const linear_range_iterator &o) const { return state - o.state; }
		};
		struct linear_range
		{
			uint32_t m_begin;
			uint32_t m_end;
			Function *modifier;

			constexpr linear_range_iterator begin() const { return {m_begin, modifier}; }
			constexpr linear_range_iterator end() const { return {m_end, modifier}; }
			constexpr size_t size() const { return m_end - m_begin; }
		};
		linear_range range{begin, end+1, &prescaler_modifier};
		return from_range(input_frequency, desired_frequency, range);
	}

	/// From any linear range.
	/// @note the range end is *inclusive*: [begin, end].
	/// @pre begin must be smaller or equal than end!
	static constexpr Result
	from_linear(T input_frequency, T desired_frequency, uint32_t begin, uint32_t end)
	{
		const double desired = double(input_frequency) / desired_frequency;
		const uint32_t prescaler_floor = std::max(uint32_t(desired), begin);
		const uint32_t prescaler_ceiling = std::min(uint32_t(desired) + 1, end);
		const T baud_lower = input_frequency / prescaler_ceiling;
		const T baud_upper = input_frequency / prescaler_floor;
		const double baud_middle = (baud_upper + baud_lower) / 2.0;
		if (desired_frequency <= baud_middle)
			return {input_frequency, desired_frequency, prescaler_ceiling - begin, prescaler_ceiling};
		return {input_frequency, desired_frequency, prescaler_floor - begin, prescaler_floor};
	}

	/// From any 2-logarithmic range.
	/// @note the range end is *inclusive*: [begin, end].
	/// @pre `begin` and `end` must be powers of two!
	/// @pre `begin` must be smaller or equal than `end`!
	static constexpr Result
	from_power(T input_frequency, T desired_frequency, uint32_t begin, uint32_t end)
	{
		const double desired = double(input_frequency) / desired_frequency;
		uint32_t ii{0}; uint32_t power{begin};
		for (; power < end; power *= 2, ii++)
			if (desired <= double(power * 4.0/3)) break;
		return {input_frequency, desired_frequency, ii, power};
	}
};

using Prescaler = GenericPrescaler<uint64_t>;

} // namespace modm
