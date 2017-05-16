// coding: utf-8
/* Copyright (c) 2016-2017, Niklas Hauser
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ASSERT_H
#define MODM_ASSERT_H

#if defined(__cplusplus) && !defined(MODM_ASSERT_HPP)
#	error  "Don't include this file directly, use 'assert.hpp' instead!"
#endif

#include <stdint.h>
#include <stdbool.h>
#include <modm/architecture/utils.hpp>

#ifndef __DOXYGEN__

// FIXME: <modm/architecture/driver/flash.hpp> is not C compatible!
#ifdef MODM_CPU_AVR
#	include <avr/pgmspace.h>
#	define modm_ifss(s) PSTR(s)
#else
#	define modm_ifss(s) ((const char *)(s))
#endif

#define modm_assert4(condition, module, location, failure) \
	({ \
		bool modm_assert_evaluated_condition = (bool) (condition); \
		if (!modm_assert_evaluated_condition) { \
			modm_assert_fail(modm_ifss(module "\0" location "\0" failure)); } \
		modm_assert_evaluated_condition; \
	})


#define modm_assert5(condition, module, location, failure, context) \
	({ \
		bool modm_assert_evaluated_condition = (bool) (condition); \
		if (!modm_assert_evaluated_condition) { \
			modm_assert_fail_context(modm_ifss(module "\0" location "\0" failure), (uintptr_t) context); } \
		modm_assert_evaluated_condition; \
	})

#define modm_assert_get_macro(_1,_2,_3,_4,_5,foo,...) foo
#define modm_assert(...) \
	modm_assert_get_macro(__VA_ARGS__, modm_assert5, modm_assert4)(__VA_ARGS__)

#ifdef MODM_DEBUG_BUILD
#define modm_assert_debug(...) \
	modm_assert_get_macro(__VA_ARGS__, modm_assert5, modm_assert4)(__VA_ARGS__)
#else
#	define modm_assert_debug(condition, ...) \
	({ \
		bool modm_unused modm_assert_evaluated_condition = (bool) (condition); \
		modm_assert_evaluated_condition; \
	})
#endif

modm_extern_c void
modm_assert_fail_context(const char * identifier, uintptr_t context);

modm_extern_c void
modm_assert_fail(const char * identifier);

modm_extern_c void
modm_abandon(const char * module, const char * location, const char * failure, uintptr_t context);

#endif // __DOXYGEN__

#endif // MODM_ASSERT_H
