/*
 * Copyright (c) 2015, Huu Nguyen
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the MIT License.
 * If a copy of the MPL was not distributed with this file, you can obtain
 * one at https://opensource.org/licenses/MIT.
 */
// Adapted for constexpr from https://github.com/whoshuu/cpp_range
// ----------------------------------------------------------------------------

#pragma once
#include <iterator>
#include <type_traits>
// #include <modm/architecture/interface/assert.hpp> // TODO

namespace modm
{

/// @cond
namespace detail
{

template <typename T>
class Range
{
public:
	constexpr Range(const T& start, const T& stop, const T& step) :
		start_{start},
		stop_{validate(start,stop,step) ? stop : start},
		step_{validate(start,stop,step) ? step : 1}
	{}

	class iterator
	{
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef T value_type;
		typedef T& reference;
		typedef T* pointer;

		constexpr iterator(value_type value, T step, value_type boundary) :
			value_{value}, step_{step}, boundary_{boundary}, positive_step_(step_ > 0)
		{}
		constexpr iterator operator++() { value_ += step_; return *this; }
		constexpr reference operator*() { return value_; }
		constexpr pointer operator->() { return &value_; }
		constexpr bool operator==(const iterator& rhs)
		{
			if (positive_step_)
				return (value_ >= rhs.value_ and value_ > boundary_);
			return (value_ <= rhs.value_ and value_ < boundary_);
		}
		constexpr bool operator!=(const iterator& rhs)
		{
			if (positive_step_)
				return (value_ < rhs.value_ and value_ >= boundary_);
			return (value_ > rhs.value_ and value_ <= boundary_);
		}

	private:
		value_type value_;
		const T step_;
		const T boundary_;
		const bool positive_step_;
	};

	constexpr iterator begin() const { return iterator(start_, step_, start_); }
	constexpr iterator end()   const { return iterator(stop_,  step_, start_); }

private:
	constexpr bool
	validate(const T& start, const T& stop, const T& step)
	{
		bool step_ok = (step != 0);
		bool order_ok = not ((start > stop and step > 0) or (start < stop and step < 0));

		// TODO: Use std::is_constant_evaluated() to make modm_assert redirect to static_assert!
		// throw std::invalid_argument("Range step argument must not be zero");
		// modm_assert_debug(step_ok, "algorithm", "range", "step");
		// throw std::invalid_argument("Range arguments must result in termination");
		// modm_assert_debug(order_ok, "algorithm", "range", "termination");

		return step_ok and order_ok;
	}

	const T start_;
	const T stop_;
	const T step_;
};

} // namespace detail
/// @endcond

/**
 * Returns an iterable range [0, stop) of step 1.
 * @warning Returns an empty range if stop <= 0!
 *
 * @ingroup	modm_math_algorithm
 */
template <typename T>
constexpr detail::Range<T>
range(const T& stop)
{
	return detail::Range<T>(T{0}, stop, T{1});
}

/**
 * Returns an iterable range [start, stop) of step 1.
 * @warning Returns an empty range if start >= stop!
 *
 * @ingroup	modm_math_algorithm
 */
template <typename T>
constexpr detail::Range<T>
range(const T& start, const T& stop)
{
	return detail::Range<T>(start, stop, T{1});
}

/**
 * Returns an iterable range [start, stop) of step != 0.
 * @warning Returns an empty range if
 *          (start >= stop and step >= 0) or (start <= stop and step <= 0)!
 *
 * @ingroup	modm_math_algorithm
 */
template <typename T>
constexpr detail::Range<T>
range(const T& start, const T& stop, const T& step)
{
	return detail::Range<T>(start, stop, step);
}

} // namespace modm
