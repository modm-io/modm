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

#pragma once

#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <bit>

namespace modm
{

/**
 * Computes the best fitting prescaler and counter value from a list.
 *
 * @note For ranges the end is *inclusive*: [begin, end]!
 * @ingroup	modm_math_algorithm
 */
template<typename T>
class
GenericPrescalerCounter
{
public:
	struct Result
	{
		constexpr Result(T input_frequency, T desired_frequency,
						 uint32_t index, uint32_t prescaler, uint32_t counter) :
			index{index}, prescaler{prescaler}, counter{counter},
			frequency{input_frequency / (prescaler * counter)},
			input_frequency{input_frequency},
			desired_frequency{desired_frequency},
			error{1.f - float(input_frequency) / float(prescaler * counter * desired_frequency)}
		{}
		/// Zero-indexed prescaler result
		const uint32_t index;
		/// Prescaler value
		const uint32_t prescaler;
		/// Counter value
		const uint32_t counter;
		/// Generated frequency
		const T frequency;
		/// Input frequency
		const T input_frequency;
		/// Desired Frequency
		const T desired_frequency;
		/// Relative Frequency Error
		const float error;
	};

public:
	/// From any iterable container.
	/// @note container must have `begin()`, `end()` and `size()` function!
	template<class Iterator>
	static constexpr Result
	from_iterator(T input_frequency, T desired_frequency, uint32_t max_counter, Iterator prescalers)
	{
		const double desired = double(input_frequency) / desired_frequency;
		const auto [min, max] = std::minmax_element(prescalers.begin(), prescalers.end());
		// clamp to lowest frequency
		if (desired < (*min * 1)) {
			return {input_frequency, desired_frequency,
					uint32_t(std::distance(prescalers.begin(), min)), *min, 1};
		}
		// clamp to highest frequency
		if (desired > (*max * max_counter)) {
			return {input_frequency, desired_frequency,
					uint32_t(std::distance(prescalers.begin(), max)), *max, max_counter};
		}

		uint32_t out_prescaler{0}, out_index{0}, out_counter{0};
		double out_error{std::numeric_limits<double>::infinity()};
		auto fn_compare = [&](uint32_t idx, uint32_t pre, uint32_t cnt)
		{
			const double err = std::abs(desired - pre * cnt);
			if (err < out_error) {
				out_error = err;
				out_prescaler = pre;
				out_counter = cnt;
				out_index = idx;
			}
		};
		for (uint32_t idx{0}; const auto prescaler : prescalers)
		{
			if (max_counter * prescaler >= desired) {
				const auto cnt = std::max<uint32_t>(1, desired / prescaler);
				fn_compare(idx, prescaler, cnt);
				if (cnt + 1 <= max_counter) {
					fn_compare(idx, prescaler, cnt + 1);
				}
			}
			idx++;
		}
		return {input_frequency, desired_frequency, out_index, out_prescaler, out_counter};
	}

	/// From a initializer list.
	static constexpr Result
	from_list(T input_frequency, T desired_frequency, uint32_t max_counter,
			  std::initializer_list<uint32_t> prescalers)
	{
		return from_iterator(input_frequency, desired_frequency, max_counter, prescalers);
	}

	/// From any linear range via modifier function.
	/// @note the range end is *inclusive*: [begin, end].
	template< typename Function >
	static constexpr Result
	from_function(T input_frequency, T desired_frequency, uint32_t max_counter,
				  uint32_t begin, uint32_t end, Function prescaler_modifier)
	{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
		struct linear_range_iterator
		{
			using iterator_category [[maybe_unused]] = std::forward_iterator_tag;
			using value_type [[maybe_unused]] = uint32_t;
			using difference_type [[maybe_unused]] = int32_t;
			using pointer [[maybe_unused]] = value_type*;
			using reference [[maybe_unused]] = value_type&;

			value_type state;
			value_type value;
			Function modifier;
			bool reversed;

			constexpr value_type operator*() const { return modifier(value); }
			constexpr linear_range_iterator& operator++()
			{ state++; reversed ? value-- : value++; return *this; }
			constexpr linear_range_iterator& operator=(const linear_range_iterator&o)
			{ state = o.state; value = o.value; reversed = o.reversed; modifier = o.modifier; return *this; }
			constexpr bool operator==(const linear_range_iterator &o) const { return state == o.state; }
			constexpr bool operator!=(const linear_range_iterator &o) const { return state != o.state; }
			difference_type operator-(const linear_range_iterator &o) { return state - o.state; }
		};
#pragma GCC diagnostic pop
		struct linear_range
		{
			const uint32_t m_begin;
			const uint32_t m_end;
			const Function modifier;
			const bool reversed;

			constexpr linear_range_iterator begin() const
			{ return linear_range_iterator{m_begin, reversed ? m_end-1 : m_begin, modifier, reversed}; }
			constexpr linear_range_iterator end() const
			{ return linear_range_iterator{m_end, reversed ? m_begin : m_end-1, modifier, reversed}; }
			constexpr size_t size() const { return m_end - m_begin; }
		};
		const bool reversed = begin > end;
		if (reversed) std::swap(begin, end);
		linear_range range{begin, end+1, prescaler_modifier, reversed};
		return from_iterator(input_frequency, desired_frequency, max_counter, range);
	}

	/// From any linear range.
	/// @note the range end is *inclusive*: [begin, end].
	static constexpr Result
	from_range(T input_frequency, T desired_frequency, uint32_t max_counter,
			   uint32_t begin, uint32_t end)
	{
		return from_function(input_frequency, desired_frequency, max_counter,
							 begin, end, [](uint32_t ii) { return ii; });
	}

	/// From any 2-logarithmic range.
	/// @note the range end is *inclusive*: [begin, end].
	/// @param begin	must be a power-of-two!
	/// @param end		must be a power-of-two!
	static constexpr Result
	from_power(T input_frequency, T desired_frequency, uint32_t max_counter,
			   uint32_t begin, uint32_t end)
	{
		return from_function(input_frequency, desired_frequency, max_counter,
							 std::bit_width(begin - 1), std::bit_width(end - 1),
							 [](uint32_t ii) { return 1ul << ii; });
	}
};

using PrescalerCounter = GenericPrescalerCounter<uint64_t>;

} // namespace modm
