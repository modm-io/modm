/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2011, Martin Rosekeit
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012, 2014-2016, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_UTILS_HPP
#define XPCC_UTILS_HPP

#include "detect.hpp"
#include "legacy_macros.hpp"

#ifdef __DOXYGEN__
	/// @ingroup	platform
	/// @{

	/// Convert the argument into a C-String
	#define	XPCC_STRINGIFY(s)		#s

	/// Concatenate two arguments
	#define	XPCC_CONCAT(a,b)		a ## b

	/// Concatenate three arguments
	#define	XPCC_CONCAT3(a,b,c)		a ## b ## c

	/// Concatenate three arguments
	#define	XPCC_CONCAT4(a,b,c,d)		a ## b ## c ## d

	/// Concatenate three arguments
	#define	XPCC_CONCAT5(a,b,c,d,e)		a ## b ## c ## d ## e

	/// Compute the size of a static (!) array.
	#define XPCC_ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))

	/**
	 * Force inlining on functions if needed. Compiling with -Os  does not
	 * always inline them when declared only `inline`
	 */
	#define xpcc_always_inline

	/// Attached to a variable or a function this means that it is meant to be possibly unused.
	#define xpcc_unused

	/// Causes the declaration to be emitted as a weak symbol rather than a global.
	#define xpcc_weak

	/// Specifies a minimum alignment for the function, measured in bytes.
	#define xpcc_aligned(n)

	/// Specifies that a variable or structure field should have the smallest possible alignment.
	#define xpcc_packed

	/// Specifies that a variable (or function) lives in a particular section.
	#define xpcc_section(s)

	/// Specifies that a function is placed in fastest executable memory.
	/// @note This is not always SRAM, since Flash accelerators can be faster.
	#define xpcc_fastcode

	/// Specifies that a variable is placed in fastest accessible memory.
	#define xpcc_fastdata

	/// This branch is more likely to execute.
	#define xpcc_likely(x) (x)

	/// This branch is less likely to execute.
	#define xpcc_unlikely(x) (x)

	/// @}

#else // !__DOXYGEN__

	#define	XPCC_STRINGIFY(s)	XPCC_STRINGIFYx(s)
	#define	XPCC_STRINGIFYx(s)	XPCC_STRINGIFYxx(s)
	#define	XPCC_STRINGIFYxx(s)	#s

	#define	XPCC_CONCAT(a,b)	XPCC_CONCATx(a,b)
	#define	XPCC_CONCATx(a,b)	XPCC_CONCATxx(a,b)
	#define	XPCC_CONCATxx(a,b)	a ## b

	#define	XPCC_CONCAT3(a,b,c)		XPCC_CONCAT3x(a,b,c)
	#define	XPCC_CONCAT3x(a,b,c)	XPCC_CONCAT3xx(a,b,c)
	#define	XPCC_CONCAT3xx(a,b,c)	a ## b ## c

	#define	XPCC_CONCAT4(a,b,c,d)	XPCC_CONCAT4x(a,b,c,d)
	#define	XPCC_CONCAT4x(a,b,c,d)	XPCC_CONCAT4xx(a,b,c,d)
	#define	XPCC_CONCAT4xx(a,b,c,d)	a ## b ## c ## d

	#define	XPCC_CONCAT5(a,b,c,d,e)		XPCC_CONCAT5x(a,b,c,d,e)
	#define	XPCC_CONCAT5x(a,b,c,d,e)	XPCC_CONCAT5xx(a,b,c,d,e)
	#define	XPCC_CONCAT5xx(a,b,c,d,e)	a ## b ## c ## d ## e


	#define xpcc_always_inline		inline __attribute__((always_inline))
	#define xpcc_unused				__attribute__((unused))
	#define xpcc_aligned(n)			__attribute__((aligned(n)))
	#define xpcc_packed				__attribute__((packed))
	// see http://dbp-consulting.com/tutorials/StrictAliasing.html
	#define xpcc_may_alias			__attribute__((__may_alias__))
	#define xpcc_deprecated(msg)	__attribute__((deprecated(msg)))
	#define xpcc_likely(x)			__builtin_expect(!!(x), 1)
	#define xpcc_unlikely(x)		__builtin_expect(!!(x), 0)
	#define xpcc_section(s)			__attribute__((section(s)))

	#ifdef XPCC_COMPILER_MINGW
	 	// FIXME: Windows Object Format PE does not support weak symbols
	 	//	- Bug reported for mingw32 : https://sourceware.org/bugzilla/show_bug.cgi?id=9687
	 	// 	- Investigate Boost libs how they solve this issue
	 	//	- __attribute__ ((weak, alias ("__aliasedFunction"))) seems to work on Windows
	#	define xpcc_weak
	#else
 	#	define xpcc_weak			__attribute__((weak))
	#endif

	#ifdef XPCC_OS_HOSTED
	#	define xpcc_fastcode
	#	define xpcc_fastdata
	#else
	#	define xpcc_fastcode		xpcc_section(".fastcode")
	#	define xpcc_fastdata		xpcc_section(".fastdata")
	#endif

	#ifdef __cplusplus
	#	define xpcc_extern_c extern "C"
	#else
	#	define xpcc_extern_c
	#endif

	#define XPCC_ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))

#endif	// !__DOXYGEN__

#endif	// XPCC_UTILS_HPP
