/*
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ASSERT_HPP
#define MODM_ASSERT_HPP

#include <stdint.h>
#include <modm/architecture/utils.hpp>
#include <modm/utils/bit_constants.hpp>

/**
 * @ingroup		interface
 * @defgroup	assert	Assertions
 *
 * These functions allow you to assert a condition at runtime and define
 * failure handlers in your application that can decide what to do with this
 * assertion and provide custom functionality.
 *
 * Each assertion has the form `modm_assert(condition, module, location, failure)`,
 * where the condition is a boolean and rest are strings, so that a simple
 * string compare can be used to match for module, location or failure.
 * For example, the identifier `"can", "init", "timeout"` describes a timeout
 * failure in the CAN initializer function.
 * The assert `modm_assert_debug(condition, module, location, failure)` is only
 * available on debug builds and is removed from the code for a release build.
 *
 * The user can define one or multiple assertion handlers in any part of the
 * application using the `MODM_ASSERTION_HANDLER(function)` macro.
 * All assertion handlers will be executed when an assertion fails anywhere in
 * the code and get passed the identifier string.
 *
 * @note The order of assertion handler execution is undefined and must not be
 *       relied upon for any functionality!
 * @warning Assertion handlers may be executed in interrupt context!
 *
 * Depending on the information in the failure identifier, the assertion handler
 * returns `Abandonment::DontCare` if the failure is not of interest, or
 * `Abandonment::Ignore` for recoverable failures, or `Abandonment::Fail` for
 * failures that do not allow normal program continuation.
 * The program is aborted, if any assertion handler returns `Abandonment::Fail`,
 * all assertion handlers return `Abandonment::DontCare` or no assertion
 * handlers have been defined in the application.
 * Only if one or many assertion handlers return `Abandonment::Ignore` and the
 * remainder returns `Abandonment::DontCare`, only then is the assertion ignored.
 *
 * @note It is intended that the assertion handlers do not block (forever), so
 *       that all assertion handlers can get called.
 *
 * On program abandonment `modm_abandon(module, location, failure)` is called,
 * which exits the program silently by default.
 * Only on hosted an formatted error string is output by default.
 * It is therefore recommended to overwrite this function on embedded targets
 * for custom behavior like blinking an LED and printing to a serial connection.
 *
 * @warning The abandonment handler may also be executed in interrupt context!
 *
 * @see		driver
 * @author	Niklas Hauser
 */

namespace modm
{

/// Describes abandonment type of assertions.
/// @ingroup assert
enum class
Abandonment : uint8_t
{
	DontCare = Bit0,	///< Do not care about failure.
	Ignore = Bit1,		///< Safe to ignore this failure.
	Fail = Bit2			///< This failure is reason for abandonment.
};

/// Signature of the assertion handlers
/// @ingroup assert
using AssertionHandler = Abandonment (*)(const char * module,
										 const char * location,
										 const char * failure,
										 uintptr_t context);

} // namespace modm

#ifdef __DOXYGEN__

/**
 * This adds a function to the list of assertion handlers to execute on
 * assertion failure. Note that this macro does not give you any influence
 * over the order of handler execution on assertion failure.
 * Do not write assertion handlers that depend on any ordered execution!
 *
 * @warning On AVR targets the failure identifier resides in Flash memory!
 *
 * @param handler A function of signature `AssertionHandler`.
 *
 * @ingroup assert
 */
#define MODM_ASSERTION_HANDLER(handler)

/**
 * Assert a condition to be true with failure identifier.
 * This assert is always included in the source code.
 * @returns result of condition evaluation
 *
 * @note On AVR targets the failure identifier string is placed in Flash memory!
 *
 * @ingroup assert
 */
modm_extern_c bool
modm_assert(bool condition, const char * module, const char * location, const char * failure);

/**
 * Assert a condition to be true with failure identifier and context.
 * This assert is always included in the source code.
 * @returns result of condition evaluation
 *
 * @note On AVR targets the failure identifier string is placed in Flash memory!
 *
 * @ingroup assert
 */
modm_extern_c bool
modm_assert(bool condition, const char * module, const char * location, const char * failure, uintptr_t context);

/**
 * Assert a condition to be true with failure identifier.
 * This assert is only triggered in the source code on debug builds!
 * @returns result of condition evaluation
 *
 * @note On AVR targets the strings are placed in Flash memory!
 *
 * @ingroup assert
 */
modm_extern_c bool
modm_assert_debug(bool condition, const char * module, const char * location, const char * failure);

/**
 * Assert a condition to be true with failure identifier and context.
 * This assert is only triggered in the source code on debug builds!
 * @returns result of condition evaluation
 *
 * @note On AVR targets the strings are placed in Flash memory!
 *
 * @ingroup assert
 */
modm_extern_c bool
modm_assert_debug(bool condition, const char * module, const char * location, const char * failure, uintptr_t context);

/**
 * Overwriteable abandonment handler for all targets.
 *
 * You should overwrite this handler for custom failure behaviour like blinking
 * LEDs and outputting the failure string via a serial connection.
 *
 * @ingroup assert
 */
modm_extern_c void
modm_abandon(const char * module,
			 const char * location,
			 const char * failure,
			 uintptr_t context) modm_weak;

#else

#if defined MODM_CPU_ARM || defined MODM_CPU_AVR
#	define MODM_ASSERTION_LINKER_SECTION ".assertion"
#elif defined MODM_OS_OSX
#	define MODM_ASSERTION_LINKER_SECTION "__DATA,modm_assertion"
#elif defined MODM_OS_LINUX
#	define MODM_ASSERTION_LINKER_SECTION "modm_assertion"
#endif

#ifdef MODM_ASSERTION_LINKER_SECTION
#	define MODM_ASSERTION_HANDLER(handler) \
		__attribute__((section(MODM_ASSERTION_LINKER_SECTION), used)) \
		const modm::AssertionHandler \
		handler ## _assertion_handler_ptr = handler
#else
#	warning "MODM_ASSERTION_HANDLER(handler) ignored, due to missing linker section definition!"
#	define MODM_ASSERTION_HANDLER(handler)
#endif

#ifdef MODM_DEBUG_BUILD
#	define MODM_ASSERTION_HANDLER_DEBUG(handler) \
		MODM_ASSERTION_HANDLER(handler)
#else
#	define MODM_ASSERTION_HANDLER_DEBUG(handler)
#endif

#include "assert.h"

#endif // __DOXYGEN__

#endif // MODM_ASSERT_HPP
