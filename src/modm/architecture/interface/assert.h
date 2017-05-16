// coding: utf-8
/* Copyright (c) 2016-2017, Niklas Hauser
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ASSERT_H
#define XPCC_ASSERT_H

#if defined(__cplusplus) && !defined(XPCC_ASSERT_HPP)
#	error  "Don't include this file directly, use 'assert.hpp' instead!"
#endif

#include <stdint.h>
#include <stdbool.h>
#include <xpcc/architecture/utils.hpp>

#ifndef __DOXYGEN__

// FIXME: <xpcc/architecture/driver/flash.hpp> is not C compatible!
#ifdef XPCC__CPU_AVR
#	include <avr/pgmspace.h>
#	define xpcc_ifss(s) PSTR(s)
#else
#	define xpcc_ifss(s) ((const char *)(s))
#endif

#define xpcc_assert4(condition, module, location, failure) \
	({ \
		bool xpcc_assert_evaluated_condition = (bool) (condition); \
		if (!xpcc_assert_evaluated_condition) { \
			xpcc_assert_fail(xpcc_ifss(module "\0" location "\0" failure)); } \
		xpcc_assert_evaluated_condition; \
	})


#define xpcc_assert5(condition, module, location, failure, context) \
	({ \
		bool xpcc_assert_evaluated_condition = (bool) (condition); \
		if (!xpcc_assert_evaluated_condition) { \
			xpcc_assert_fail_context(xpcc_ifss(module "\0" location "\0" failure), (uintptr_t) context); } \
		xpcc_assert_evaluated_condition; \
	})

#define xpcc_assert_get_macro(_1,_2,_3,_4,_5,foo,...) foo
#define xpcc_assert(...) \
	xpcc_assert_get_macro(__VA_ARGS__, xpcc_assert5, xpcc_assert4)(__VA_ARGS__)

#ifdef XPCC_DEBUG_BUILD
#define xpcc_assert_debug(...) \
	xpcc_assert_get_macro(__VA_ARGS__, xpcc_assert5, xpcc_assert4)(__VA_ARGS__)
#else
#	define xpcc_assert_debug(condition, ...) \
	({ \
		bool xpcc_unused xpcc_assert_evaluated_condition = (bool) (condition); \
		xpcc_assert_evaluated_condition; \
	})
#endif

xpcc_extern_c void
xpcc_assert_fail_context(const char * identifier, uintptr_t context);

xpcc_extern_c void
xpcc_assert_fail(const char * identifier);

xpcc_extern_c void
xpcc_abandon(const char * module, const char * location, const char * failure, uintptr_t context);

#endif // __DOXYGEN__

#endif // XPCC_ASSERT_H
