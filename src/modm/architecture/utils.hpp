/*
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011, Martin Rosekeit
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, 2014-2017, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2015, Sascha Schade
 * Copyright (c) 2016, Tarik TIRE
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_UTILS_HPP
#define MODM_UTILS_HPP

#include "detect.hpp"

#ifdef __DOXYGEN__
	/// @ingroup	platform
	/// @{

	/// Convert the argument into a C-String
	#define	MODM_STRINGIFY(s)		#s

	/// Concatenate two arguments
	#define	MODM_CONCAT(a,b)		a ## b

	/// Concatenate three arguments
	#define	MODM_CONCAT3(a,b,c)		a ## b ## c

	/// Concatenate three arguments
	#define	MODM_CONCAT4(a,b,c,d)		a ## b ## c ## d

	/// Concatenate three arguments
	#define	MODM_CONCAT5(a,b,c,d,e)		a ## b ## c ## d ## e

	/// Compute the size of a static (!) array.
	#define MODM_ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))

	/**
	 * Force inlining on functions if needed. Compiling with -Os  does not
	 * always inline them when declared only `inline`
	 */
	#define modm_always_inline

	/// Attached to a variable or a function this means that it is meant to be possibly unused.
	#define modm_unused

	/// Causes the declaration to be emitted as a weak symbol rather than a global.
	#define modm_weak

	/// Specifies a minimum alignment for the function, measured in bytes.
	#define modm_aligned(n)

	/// Specifies that a variable or structure field should have the smallest possible alignment.
	#define modm_packed

	/// Specifies that a variable (or function) lives in a particular section.
	#define modm_section(s)

	/// Specifies that a function is placed in fastest executable memory.
	/// @note This is not always SRAM, since Flash accelerators can be faster.
	#define modm_fastcode

	/// Specifies that a variable is placed in fastest accessible memory.
	#define modm_fastdata

	/// This branch is more likely to execute.
	#define modm_likely(x) (x)

	/// This branch is less likely to execute.
	#define modm_unlikely(x) (x)

	/// @}

#else // !__DOXYGEN__

	#define	MODM_STRINGIFY(s)	MODM_STRINGIFYx(s)
	#define	MODM_STRINGIFYx(s)	MODM_STRINGIFYxx(s)
	#define	MODM_STRINGIFYxx(s)	#s

	#define	MODM_CONCAT(a,b)	MODM_CONCATx(a,b)
	#define	MODM_CONCATx(a,b)	MODM_CONCATxx(a,b)
	#define	MODM_CONCATxx(a,b)	a ## b

	#define	MODM_CONCAT3(a,b,c)		MODM_CONCAT3x(a,b,c)
	#define	MODM_CONCAT3x(a,b,c)	MODM_CONCAT3xx(a,b,c)
	#define	MODM_CONCAT3xx(a,b,c)	a ## b ## c

	#define	MODM_CONCAT4(a,b,c,d)	MODM_CONCAT4x(a,b,c,d)
	#define	MODM_CONCAT4x(a,b,c,d)	MODM_CONCAT4xx(a,b,c,d)
	#define	MODM_CONCAT4xx(a,b,c,d)	a ## b ## c ## d

	#define	MODM_CONCAT5(a,b,c,d,e)		MODM_CONCAT5x(a,b,c,d,e)
	#define	MODM_CONCAT5x(a,b,c,d,e)	MODM_CONCAT5xx(a,b,c,d,e)
	#define	MODM_CONCAT5xx(a,b,c,d,e)	a ## b ## c ## d ## e


	#define modm_always_inline		inline __attribute__((always_inline))
	#define modm_unused				__attribute__((unused))
	#define modm_aligned(n)			__attribute__((aligned(n)))
	#define modm_packed				__attribute__((packed))
	// see http://dbp-consulting.com/tutorials/StrictAliasing.html
	#define modm_may_alias			__attribute__((__may_alias__))
	#define modm_deprecated(msg)	__attribute__((deprecated(msg)))
	#define modm_likely(x)			__builtin_expect(!!(x), 1)
	#define modm_unlikely(x)		__builtin_expect(!!(x), 0)
	#define modm_section(s)			__attribute__((section(s)))

	#ifdef MODM_COMPILER_MINGW
	 	// FIXME: Windows Object Format PE does not support weak symbols
	 	//	- Bug reported for mingw32 : https://sourceware.org/bugzilla/show_bug.cgi?id=9687
	 	// 	- Investigate Boost libs how they solve this issue
	 	//	- __attribute__ ((weak, alias ("__aliasedFunction"))) seems to work on Windows
	#	define modm_weak
	#else
 	#	define modm_weak			__attribute__((weak))
	#endif

	#ifdef MODM_OS_HOSTED
	#	define modm_fastcode
	#	define modm_fastdata
	#else
	#	define modm_fastcode		modm_section(".fastcode")
	#	define modm_fastdata		modm_section(".fastdata")
	#endif

	#ifdef __cplusplus
	#	define modm_extern_c extern "C"
	#else
	#	define modm_extern_c
	#endif

	#define MODM_ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))

	#ifdef __has_cpp_attribute
		#if __has_cpp_attribute(fallthrough)
		#	define MODM_FALLTHROUGH [[fallthrough]]
		#else
		#	define MODM_FALLTHROUGH
		#endif
	#else
	#	define MODM_FALLTHROUGH
	#endif // _has_cpp_attribute


#endif	// !__DOXYGEN__

#endif	// MODM_UTILS_HPP
