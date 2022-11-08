/*
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011, Martin Rosekeit
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, 2014-2017, 2021, Niklas Hauser
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
	/// @ingroup	modm_architecture
	/// @{

	/// Convert the argument into a C-String
	#define	MODM_STRINGIFY(s)		#s

	/// Concatenate two arguments
	#define	MODM_CONCAT(a,b)		a ## b

	/// Concatenate three arguments
	#define	MODM_CONCAT3(a,b,c)		a ## b ## c

	/// Concatenate four arguments
	#define	MODM_CONCAT4(a,b,c,d)		a ## b ## c ## d

	/// Concatenate five arguments
	#define	MODM_CONCAT5(a,b,c,d,e)		a ## b ## c ## d ## e

	/// Compute the size of a static (!) array.
	#define MODM_ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))

	/// Include a binary file into a specific section (usually `".rodata"`).
	/// Access via `extern "C" uint8_t {{name}}[]; extern "C" uint8_t {{name}}_length[];`.
	/// @see http://elm-chan.org/junk/32bit/binclude.html
	/// @warning The build systems cannot detect if the binary file changes and
	///          will therefore not recompile automatically!
	#define MODM_IMPORT_BINARY(name, file, section)

	/// Force inlining on functions if absolutely necessary (it rarely is).
	/// @warning Only use this if you really know what you're doing.
	#define modm_always_inline

	/// Specifies that a function should never be inlined.
	/// @note Please use `[[noinline]]` in a C++ context instead.
	#define modm_noinline

	/// Attached to a variable or a function this means that it is meant to be possibly unused.
	/// @note Please use `[[maybe_unused]]` in a C++ context instead.
	#define modm_unused

	/// Causes the declaration to be emitted as a weak symbol rather than a global.
	#define modm_weak

	/// Specifies a minimum alignment for the function, measured in bytes.
	#define modm_aligned(n)

	/// Specifies that a variable or structure field should have the smallest possible alignment.
	#define modm_packed

	/// Specifies that a variable (or function) lives in a particular section.
	#define modm_section(s)

	/// Marks a declaration as deprecated and displays a message.
	/// @note Please use `[[deprecated(msg)]]` in a C++ context instead.
	#define modm_deprecated(msg)

	/// Tells the compiler to not generate prologue/epilogue code for this function.
	#define modm_naked

	/// Tells the compiler that the function never returns.
	/// @note Please use `[[noreturn]]` in a C++ context instead.
	#define modm_noreturn

	/// Places a function in the fastest executable memory:
	/// instruction cache, core coupled memory or SRAM as fallback.
	#define modm_fastcode

	/// Places a variable in the fastest accessible memory:
	/// data cache, core coupled memory or SRAM as fallback.
	/// @note This memory location may not be DMA-able!
	#define modm_fastdata

	/// Places an array into the fastest accessible memory *with* DMA access:
	/// data cache, core coupled memory or SRAM as fallback.
	/// @note This memory location is DMA-able, but uninitialized!
	#define modm_faststack

	/// This branch is more likely to execute.
	/// @note Please use `[[likely]]` in a C++ context instead.
	#define modm_likely(x) (x)

	/// This branch is less likely to execute.
	/// @note Please use `[[unlikely]]` in a C++ context instead.
	#define modm_unlikely(x) (x)

	/// Mark a case statement as fallthrough.
	/// @note Please use `[[fallthrough]]` in a C++ context instead.
	#define modm_fallthrough

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

	#define MODM_ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))

	#define modm_always_inline		inline __attribute__((always_inline))
	#define modm_noinline			__attribute__((noinline))
	#define modm_used				__attribute__((used))
	#define modm_unused				__attribute__((unused))
	#define modm_aligned(n)			__attribute__((aligned(n)))
	#define modm_packed				__attribute__((packed))
	// see http://dbp-consulting.com/tutorials/StrictAliasing.html
	#define modm_may_alias			__attribute__((__may_alias__))
	#define modm_deprecated(msg)	__attribute__((deprecated(msg)))
	#define modm_likely(x)			__builtin_expect(!!(x), 1)
	#define modm_unlikely(x)		__builtin_expect(!!(x), 0)
	#define modm_section(s)			__attribute__((section(s)))
	#define modm_fallthrough		__attribute__((fallthrough))
	#define modm_noreturn			__attribute__((noreturn))
	#define modm_warn_unused_result	__attribute__((warn_unused_result))
	#define modm_naked				__attribute__((naked))

	#ifdef MODM_COMPILER_MINGW
	 	// FIXME: Windows Object Format PE does not support weak symbols
	 	//	- Bug reported for mingw32 : https://sourceware.org/bugzilla/show_bug.cgi?id=9687
	 	// 	- Investigate Boost libs how they solve this issue
	 	//	- __attribute__ ((weak, alias ("__aliasedFunction"))) seems to work on Windows
	#	define modm_weak
	#else
 	#	define modm_weak			__attribute__((weak))
	#endif

	#if defined(MODM_OS_HOSTED) || defined(MODM_CPU_AVR)
	#	define modm_fastcode
	#	define modm_ramcode
	#	define modm_fastdata
	#	define modm_faststack
	#else
	#	define modm_fastcode		modm_section(".fastcode") modm_noinline
	#	define modm_ramcode			modm_fastcode
	#	define modm_fastdata		modm_section(".fastdata")
	#	define modm_faststack		modm_section(".faststack")
	#endif

	#ifdef __cplusplus
	#	define modm_extern_c extern "C"
	#else
	#	define modm_extern_c
	#endif

	#define MODM_IMPORT_BINARY(name, file, section) \
		asm 										\
		(											\
			".section " #section "\n"				\
			".balign 4\n"							\
			".global " #name "\n"					\
			#name ":\n"								\
			".incbin \"" file "\"\n"				\
			".global " #name "_laddr\n"				\
			".set " #name "_laddr, . - " #name "\n"	\
			".balign 4\n"							\
			".section \".text\"\n"					\
		); 											\
		modm_extern_c uint8_t name ## _laddr[];		\
		modm_extern_c const size_t name ## _length = (size_t) name ## _laddr; \
		modm_extern_c uint8_t name[]


#endif	// !__DOXYGEN__

#endif	// MODM_UTILS_HPP
