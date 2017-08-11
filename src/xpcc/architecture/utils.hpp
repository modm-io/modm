// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__UTILS_HPP
#define XPCC__UTILS_HPP

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

	#define	XPCC_STRINGIFY(s)	XPCC_STRINGIFY_(s)
	#define	XPCC_STRINGIFY_(s)	XPCC_STRINGIFY__(s)
	#define	XPCC_STRINGIFY__(s)	#s

	#define	XPCC_CONCAT(a,b)	XPCC_CONCAT_(a,b)
	#define	XPCC_CONCAT_(a,b)	XPCC_CONCAT__(a,b)
	#define	XPCC_CONCAT__(a,b)	a ## b

	#define	XPCC_CONCAT3(a,b,c)		XPCC_CONCAT3_(a,b,c)
	#define	XPCC_CONCAT3_(a,b,c)	XPCC_CONCAT3__(a,b,c)
	#define	XPCC_CONCAT3__(a,b,c)	a ## b ## c

	#define	XPCC_CONCAT4(a,b,c,d)	XPCC_CONCAT4_(a,b,c,d)
	#define	XPCC_CONCAT4_(a,b,c,d)	XPCC_CONCAT4__(a,b,c,d)
	#define	XPCC_CONCAT4__(a,b,c,d)	a ## b ## c ## d

	#define	XPCC_CONCAT5(a,b,c,d,e)		XPCC_CONCAT5_(a,b,c,d,e)
	#define	XPCC_CONCAT5_(a,b,c,d,e)	XPCC_CONCAT5__(a,b,c,d,e)
	#define	XPCC_CONCAT5__(a,b,c,d,e)	a ## b ## c ## d ## e


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

	#ifdef XPCC__COMPILER_MINGW
	 	// FIXME: Windows Object Format PE does not support weak symbols
	 	//	- Bug reported for mingw32 : https://sourceware.org/bugzilla/show_bug.cgi?id=9687
	 	// 	- Investigate Boost libs how they solve this issue
	 	//	- __attribute__ ((weak, alias ("__aliasedFunction"))) seems to work on Windows
	#	define xpcc_weak
	#else
 	#	define xpcc_weak			__attribute__((weak))
	#endif

	#ifdef XPCC__OS_HOSTED
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

	#ifdef __has_cpp_attribute
		#if __has_cpp_attribute(fallthrough)
		#	define XPCC_FALLTHROUGH [[fallthrough]]
		#else
		#	define XPCC_FALLTHROUGH
		#endif
	#else
	#	define XPCC_FALLTHROUGH
	#endif // __has_cpp_attribute

#endif	// !__DOXYGEN__

#endif	// XPCC__UTILS_HPP
