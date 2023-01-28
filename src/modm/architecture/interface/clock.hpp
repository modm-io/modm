/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, 2015, Fabian Greif
 * Copyright (c) 2011-2019, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2015-2016, Sascha Schade
 * Copyright (c) 2019, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/utils.hpp>
#include <cstdint>
#include <chrono>

namespace modm
{

namespace chrono
{

/**
 * Returns a time_point in milliseconds.
 *
 * This clock is meant for measuring durations (time differences), not time
 * since reboot. You must not depend on this clock for timekeeping, especially
 * not over long periods of time, since:
 *
 * 1. the clock is NOT STEADY! The 32-bit millisecond counter will wrap around
 *    after ~49 days!
 * 2. the clock may have quite some drift and is not synchable to an external
 *    reference. You must therefore not rely on this clock for very long
 *    durations that need to be *accurate*.
 *
 * You must use an actual calendar implementation for proper time keeping!
 *
 * @ingroup modm_architecture_clock
 */
struct milli_clock
{
	using duration = std::chrono::duration<uint32_t, std::milli>;
	using rep = duration::rep;
	using period = duration::period;
	using time_point = std::chrono::time_point<milli_clock, duration>;

	static constexpr bool is_steady = false;

	static time_point
	now() noexcept;
};

/**
 * Returns a time_point in microseconds.
 *
 * This clock is meant for measuring durations (time differences), not time
 * since reboot. You must not depend on this clock for timekeeping, especially
 * not over long periods of time, since:
 *
 * 1. the clock is NOT STEADY! The 32-bit microsecond counter will wrap around
 *    after 71 minutes!
 * 2. the clock may have quite some drift and is not synchable to an external
 *    reference. You must therefore not rely on this clock for very long
 *    durations that need to be *accurate*.
 *
 * You must use an actual calendar implementation for proper time keeping!
 *
 * @ingroup modm_architecture_clock
 */
struct micro_clock
{
	using duration = std::chrono::duration<uint32_t, std::micro>;
	using rep = duration::rep;
	using period = duration::period;
	using time_point = std::chrono::time_point<micro_clock, duration>;

	static constexpr bool is_steady = false;

	static time_point
	now() noexcept;
};

} // namespace chrono

/// @ingroup modm_architecture_clock
using Clock = chrono::milli_clock;
/// @ingroup modm_architecture_clock
using PreciseClock = chrono::micro_clock;
using namespace ::std::chrono_literals;

}	// namespace modm

#if MODM_HAS_IOSTREAM
#include <modm/io/iostream.hpp>

namespace modm
{

/// @ingroup modm_architecture_clock
template<class C, class D>
modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::time_point<C, D>& m)
{
	s << m.time_since_epoch();
	return s;
}

/// @ingroup modm_architecture_clock
template<class T, class R>
modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::duration<T, R>& m)
{
	s << m.count();
	if constexpr (std::is_same_v<R, std::nano>)  s << "ns";
	if constexpr (std::is_same_v<R, std::micro>) s << "us";
	if constexpr (std::is_same_v<R, std::milli>) s << "ms";
	if constexpr (std::is_same_v<R, std::ratio<60>>) s << "min";
	if constexpr (std::is_same_v<R, std::ratio<3600>>) s << 'h';
	return s;
}

} // modm namespace
#endif
