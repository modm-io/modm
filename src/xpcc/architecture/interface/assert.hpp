// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ASSERT_HPP
#define XPCC_ASSERT_HPP

#include <stdint.h>
#include <xpcc/architecture/utils.hpp>
#include <xpcc/utils/bit_constants.hpp>
#include <xpcc/architecture/driver/accessor/flash.hpp>

/**
 * @ingroup		interface
 * @defgroup	assert	Assertions
 *
 * These functions allow you to assert a condition at runtime and define
 * failure handlers in your application that can decide what to do with this
 * assertion and provide custom functionality.
 *
 * Each assertion has the form `xpcc_assert(condition, module, location, failure)`,
 * where the condition is a boolean and rest are strings, so that a simple
 * string compare can be used to match for module, location or failure.
 * For example, the identifier `"can", "init", "timeout"` describes a timeout
 * failure in the CAN initializer function.
 * The assert `xpcc_assert_debug(condition, module, location, failure)` is only
 * available on debug builds and is removed from the code for a release build.
 *
 * The user can define one or multiple assertion handlers in any part of the
 * application using the `XPCC_ASSERTION_HANDLER(function)` macro.
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
 * On program abandonment `xpcc_abandon(module, location, failure)` is called,
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

namespace xpcc
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

} // namespace xpcc

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
#define XPCC_ASSERTION_HANDLER(handler)

/**
 * Assert a condition to be true with failure identifier.
 * This assert is always included in the source code.
 *
 * @note On AVR targets the failure identifier string is placed in Flash memory!
 *
 * @ingroup assert
 */
#define xpcc_assert(condition, module, location, failure)

/**
 * Assert a condition to be true with failure identifier and context.
 * This assert is always included in the source code.
 *
 * @note On AVR targets the failure identifier string is placed in Flash memory!
 *
 * @ingroup assert
 */
#define xpcc_assert(condition, module, location, failure, context)

/**
 * Assert a condition to be true with failure identifier.
 * This assert is only included in the source code on debug builds!
 *
 * @note On AVR targets the strings are placed in Flash memory!
 *
 * @ingroup assert
 */
#define xpcc_assert_debug(condition, module, location, failure)

/**
 * Assert a condition to be true with failure identifier and context.
 * This assert is only included in the source code on debug builds!
 *
 * @note On AVR targets the strings are placed in Flash memory!
 *
 * @ingroup assert
 */
#define xpcc_assert_debug(condition, module, location, failure, context)

/**
 * Overwriteable abandonment handler for all targets.
 *
 * You should overwrite this handler for custom failure behaviour like blinking
 * LEDs and outputting the failure string via a serial connection.
 *
 * @ingroup assert
 */
extern "C" void
xpcc_abandon(const char * module,
			 const char * location,
			 const char * failure,
			 uintptr_t context) xpcc_weak;

#else

#if defined XPCC__CPU_ARM || defined XPCC__CPU_AVR
#	define XPCC_ASSERTION_LINKER_SECTION ".assertion"
#elif defined XPCC__OS_OSX
#	define XPCC_ASSERTION_LINKER_SECTION "__DATA,xpcc_assertion"
#elif defined XPCC__OS_LINUX
#	define XPCC_ASSERTION_LINKER_SECTION "xpcc_assertion"
#endif

#ifdef XPCC_ASSERTION_LINKER_SECTION
#	define XPCC_ASSERTION_HANDLER(handler) \
		__attribute__((section(XPCC_ASSERTION_LINKER_SECTION), used)) \
		const xpcc::AssertionHandler \
		handler ## _assertion_handler_ptr = handler
#else
#	define XPCC_ASSERTION_HANDLER(handler)
#endif

#define xpcc_assert4(condition, module, location, failure) \
	xpcc_assert5(condition, module, location, failure, 0)

#define xpcc_assert5(condition, module, location, failure, context) \
	if ((bool) (condition)) {} else { \
		xpcc_assert_fail(INLINE_FLASH_STORAGE_STRING(module "\0" location "\0" failure), (uintptr_t) context); }

#define xpcc_assert_get_macro(_1,_2,_3,_4,_5,foo,...) foo
#define xpcc_assert(...) \
	xpcc_assert_get_macro(__VA_ARGS__, xpcc_assert5, xpcc_assert4)(__VA_ARGS__)

#ifndef NDEBUG
#define xpcc_assert_debug(...) \
	xpcc_assert_get_macro(__VA_ARGS__, xpcc_assert5, xpcc_assert4)(__VA_ARGS__)
#	define XPCC_ASSERTION_HANDLER_DEBUG(handler) \
		XPCC_ASSERTION_HANDLER(handler)
#else
#	define xpcc_assert_debug(...)
#	define XPCC_ASSERTION_HANDLER_DEBUG(handler)
#endif

extern "C" {

void xpcc_assert_fail(const char * identifier, uintptr_t context);

void xpcc_abandon(const char * module, const char * location, const char * failure, uintptr_t context);

}

#endif // __DOXYGEN__

#endif // XPCC_ASSERT_HPP
