/*
 * Copyright (c) 2023, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_RF_MACROS_FIBER_HPP
#define MODM_RF_MACROS_FIBER_HPP

#include <modm/processing/fiber.hpp>
#include <mutex>

/// @ingroup modm_processing_resumable
/// @{

#ifdef __DOXYGEN__
/// Declare start of resumable function with index.
/// @warning Use at start of the `resumable()` implementation!
#define RF_BEGIN(index)

/**
 * Declare start of a nested resumable function.
 * This will immediately return if the nesting is too deep.
 *
 * @warning Use at start of the `resumable()` implementation!
 */
#define RF_BEGIN()
#endif


/**
 * End the resumable function and return a result.
 *
 * @warning	Use at end of the `resumable()` implementation only!
 * @hideinitializer
 */
#define RF_END_RETURN(...) \
	return __VA_ARGS__

/**
 * End the resumable function. You can use this to return `void`, or if the result does not matter.
 *
 * @warning	Use at end of the `resumable()` implementation only!
 * @hideinitializer
 */
#define RF_END() \
	return

/**
 * End the resumable function by calling another resumable function and returning its result.
 *
 * @warning	Use at end of the `resumable()` implementation only!
 * @hideinitializer
 */
#define RF_END_RETURN_CALL(...) \
	return __VA_ARGS__

/// Yield resumable function until next invocation.
/// @hideinitializer
#define RF_YIELD() \
	modm::this_fiber::yield()

/// Cause resumable function to wait until given child protothread completes.
/// @hideinitializer
#define RF_WAIT_THREAD(...) \
	RF_WAIT_WHILE((__VA_ARGS__).run())

/// Cause resumable function to wait **while** given `condition` is true.
/// @hideinitializer
#define RF_WAIT_WHILE(...) \
	while(__VA_ARGS__) { RF_YIELD(); }

/// Cause resumable function to wait **until** given `condition` is true.
/// @hideinitializer
#define RF_WAIT_UNTIL(...) \
	RF_WAIT_WHILE(!(__VA_ARGS__))

/// Calls a resumable function and returns its result.
/// @hideinitializer
#define RF_CALL(...) \
	__VA_ARGS__

/**
 * Calls a resumable function, busy-waits and returns its result.
 *
 * @hideinitializer
 */
#define RF_CALL_BLOCKING(...) \
	__VA_ARGS__

/// Exits a resumable function and returns another resumable function's result.
/// @hideinitializer
#define RF_RETURN_CALL(...) \
	return __VA_ARGS__

/// Stop and exit from resumable function with an optional result.
/// @hideinitializer
#define RF_RETURN(...) \
	return __VA_ARGS__

/// @}

#ifndef __DOXYGEN__

#define RF_BEGIN_0() \
	this->template checkRfType<true>(); \
	::std::lock_guard rfLockGuardState{this->rfState};

#define RF_BEGIN_1(index) \
	this->template checkRfType<false>(); \
	this->template checkRfFunctions<index>(); \
	::std::lock_guard rfLockGuardState{this->rfStateArray[index]};

#define MODM_RF_GET_MACRO(_0, _1, NAME, ...) NAME
#define RF_BEGIN(...) MODM_RF_GET_MACRO(_0 __VA_OPT__(,) __VA_ARGS__, RF_BEGIN_1, RF_BEGIN_0)(__VA_ARGS__)

#endif

#endif // MODM_RF_MACROS_FIBER_HPP
