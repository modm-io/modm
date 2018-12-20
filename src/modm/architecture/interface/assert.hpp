/*
 * Copyright (c) 2016-2018, Niklas Hauser
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
#include <modm/math/utils/bit_constants.hpp>
#include <modm/architecture/interface/accessor.hpp>


namespace modm
{

/// Describes abandonment type of assertions.
/// @ingroup modm_architecture_assert
enum class
Abandonment : uint8_t
{
	DontCare = Bit0,	///< Do not care about failure.
	Ignore = Bit1,		///< Safe to ignore this failure.
	Fail = Bit2			///< This failure is reason for abandonment.
};

/// Signature of the assertion handlers
/// @ingroup modm_architecture_assert
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
 * @ingroup modm_architecture_assert
 */
#define MODM_ASSERTION_HANDLER(handler)

/**
 * Assert a condition to be true with failure identifier.
 * This assert is always included in the source code.
 * @returns result of condition evaluation
 *
 * @note On AVR targets the failure identifier string is placed in Flash memory!
 *
 * @ingroup modm_architecture_assert
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
 * @ingroup modm_architecture_assert
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
 * @ingroup modm_architecture_assert
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
 * @ingroup modm_architecture_assert
 */
modm_extern_c bool
modm_assert_debug(bool condition, const char * module, const char * location, const char * failure, uintptr_t context);

/**
 * Overwriteable abandonment handler for all targets.
 *
 * You should overwrite this handler for custom failure behaviour like blinking
 * LEDs and outputting the failure string via a serial connection.
 *
 * @ingroup modm_architecture_assert
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
