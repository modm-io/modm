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

/// @ingroup modm_processing_resumable
/// @{

/// Declare start of resumable function with index.
/// @warning Use at start of the `resumable()` implementation!
#define RF_BEGIN(...)


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

#endif // MODM_RF_MACROS_FIBER_HPP
