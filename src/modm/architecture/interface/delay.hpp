/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include <chrono>

#ifdef __DOXYGEN__

namespace modm
{

/// @ingroup modm_architecture_delay
/// @{

/**
 * Spin for nanoseconds.
 *
 * @warning This function is implemented as best-effort and its resolution will
 *          be very coarse especially on platforms with very slow clocks.
 *
 * @warning The maximum delay is 1'000'000ns = 1 millisecond.
 */
void delay(std::chrono::nanoseconds ns);
void delay_ns(uint32_t ns);

/**
 * Is true if nanosecond delay is reasonably accurate.
 * On devices with low clock speed it may not be possible to provide this
 * function with <100ns or even <1000ns accuracy.
 */
#define MODM_DELAY_NS_IS_ACCURATE 0/1

/**
 * Spin for microseconds.
 *
 * @warning The maximum delay is 1'000'000us = 1 second. Use milliseconds for longer delays.
 *
 * @note In debug mode this function may raise "delay.us" if input validation
 *       fails.
 */
void delay(std::chrono::microseconds us);
void delay_us(uint32_t us);

/// Spin for milliseconds.
void delay(std::chrono::milliseconds ms);
void delay_ms(uint32_t ms);

/// @}

}

#else

// These functions may be implemented as header files, so we cannot rely on
// there being a link-able function and delegate this choice to the platform.
#include <modm/platform/core/delay_impl.hpp>

namespace modm
{

// Forward everything to specialized functions
template< class Rep >
void
delay(std::chrono::duration<Rep, std::nano> ns_)
{
    const auto ns{std::chrono::duration_cast<std::chrono::nanoseconds>(ns_)};
    delay_ns(ns.count());
}

template< class Rep >
void
delay(std::chrono::duration<Rep, std::micro> us_)
{
    const auto us{std::chrono::duration_cast<std::chrono::microseconds>(us_)};
    delay_us(us.count());
}

template< class Rep >
void
delay(std::chrono::duration<Rep, std::milli> ms_)
{
    const auto ms{std::chrono::duration_cast<std::chrono::milliseconds>(ms_)};
    delay_ms(ms.count());
}

// Everything else is cast to milliseconds
template<class Rep, class Period>
inline void delay(std::chrono::duration<Rep, Period> time)
{ delay(std::chrono::duration_cast<std::chrono::milliseconds>(time)); }

using namespace ::std::chrono_literals;

} // namespace modm

#endif

