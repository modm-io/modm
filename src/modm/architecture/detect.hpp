/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011-2012, 2014-2015, Niklas Hauser
 * Copyright (c) 2012, 2015, Sascha Schade
 * Copyright (c) 2015, Daniel Krebs
 * Copyright (c) 2016, Tarik TIRE
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifdef __DOXYGEN__

/// @ingroup modm_architecture
/// @{

/// Compiler description string
#define	MODM_COMPILER_STRING

/// Compiling with GCC
#define	MODM_COMPILER_GCC
/// Compiling with GCC on Windows
#define MODM_COMPILER_MINGW

/// Operating system description string
#define	MODM_OS_STRING

/// Compiling for a UNIX target
#define	MODM_OS_UNIX
/// Compiling for a Linux target
#define	MODM_OS_LINUX
/// Compiling for a macOS target
#define	MODM_OS_OSX
/// Compiling for a Windows 32-bit target
#define	MODM_OS_WIN32
/// Compiling for a Windows 64-bit target
#define	MODM_OS_WIN64
/// Compiling for a hosted (non-barebone) target
#define	MODM_OS_HOSTED
/// Compiling for a barebone target without OS
#define MODM_OS_NONE

/// CPU description string
#define	MODM_CPU_STRING

/// Compiling for x86-64 target
#define MODM_CPU_AMD64
/// Compiling for x86 target
#define MODM_CPU_I386

/// Compiling for AVR target
#define	MODM_CPU_AVR
/// Compiling for AVR ATmega target
#define	MODM_CPU_ATMEGA
/// Compiling for AVR ATtiny target
#define	MODM_CPU_ATTINY

/// Compiling for ARM target
#define	MODM_CPU_ARM
/// Compiling for ARM Cortex-M0 or Cortex-M0+ target
#define	MODM_CPU_CORTEX_M0
/// Compiling for ARM Cortex-M3 target
#define	MODM_CPU_CORTEX_M3
/// Compiling for ARM Cortex-M4 or Cortex-M7 target
#define	MODM_CPU_CORTEX_M4
/// Compiling for ARM Cortex-M33 target
#define MODM_CPU_CORTEX_M33
/// Compiling for ARM AArch64 target
#define MODM_CPU_AARCH64

/// Native memory alignment in bytes
#define MODM_ALIGNMENT

/// Compiling for a little endian memory layout
#define MODM_IS_LITTLE_ENDIAN
/// Compiling for a big endian memory layout
#define MODM_IS_BIG_ENDIAN

/// Size of pointer in bytes
#define	MODM_SIZEOF_POINTER

/// @}

#else // !__DOXYGEN__

/* ----------------------------------------------------------------------------
 * Determine compilation environment
 *
 * This relies on the predefined defines of the compiler.
 * To display the defines run:
 * $your-gcc-compiler -dM -E - < /dev/null | sort | less
 *
 * e.g.
 * $arm-none-eabi-g++ -dM -E - < /dev/null | sort | less
 *
 * Be aware that the defines change when you set options
 * for a specific compiler at compile time:
 *
 * $arm-none-eabi-g++ -dM -E -mcpu=cortex-m0 -mthumb -D__ARM_LPC1114_301__ -D__ARM_LPC11XX__ - < /dev/null
 */

#ifdef __clang__
#	define MODM_COMPILER_STRING	"Clang"
#	define MODM_COMPILER_CLANG	1
#elif defined __GNUC__
#	define MODM_COMPILER_GCC	1
#	ifdef __MINGW32__
#		define MODM_COMPILER_STRING	"MinGW / MinGW-w64"
#		define MODM_COMPILER_MINGW	1
#	else
#		define MODM_COMPILER_STRING	"Gnu GCC"
#	endif
#endif

#ifndef MODM_COMPILER_STRING
#	define MODM_COMPILER_STRING		"Unknown compiler"
#	warning	"Couldn't determine the used compiler! Please add it to <modm/architecture/detect.hpp>"
#endif

// ----------------------------------------------------------------------------
// Determine target operating system

#if defined linux || defined __linux__
#	define MODM_OS_HOSTED	1
#	define MODM_OS_LINUX	1
#	define MODM_OS_STRING 	"Linux"
#endif

#if (defined __MWERKS__ && defined __powerc && !defined macintosh) || defined __APPLE_CC__ || defined macosx
#	define MODM_OS_HOSTED	1
#	define MODM_OS_OSX		1
#	define MODM_OS_STRING	"MacOS X"
#endif

#ifdef __unix__
#	define MODM_OS_HOSTED	1
#	define MODM_OS_UNIX		1
#	ifndef MODM_OS_STRING
#		define MODM_OS_STRING	"Unix-like(generic)"
#	endif
#endif

#if defined _WIN32 || defined WIN32 || defined __NT__ || defined __WIN32__
#	define MODM_OS_HOSTED	1
#	define MODM_OS_WIN32	1
#	ifdef _WIN64
#		define MODM_OS_WIN64	1
#		define MODM_OS_STRING	"Win64"
#	else
#		ifndef MODM_OS_STRING
#			define MODM_OS_STRING	"Win32"
#		endif
#	endif
#endif

// If we did not detect any of Linux, OS X or Windows
#ifndef MODM_OS_HOSTED
#	define MODM_OS_NONE	1
#endif

// ----------------------------------------------------------------------------
// Determine CPU type

#ifdef __AVR__
#	define MODM_CPU_AVR		1
#	define MODM_ALIGNMENT	1
#	ifdef __AVR_XMEGA__
#		define MODM_CPU_ATXMEGA	1
#		define MODM_CPU_STRING	"Atmel XMEGA"
#	else
		// see http://www.nongnu.org/avr-libc/user-manual/using_tools.html
#		if __AVR_ARCH__ == 2 || __AVR_ARCH__ == 25 || __AVR_ARCH__ == 1
#			define MODM_CPU_ATTINY	1
#			define MODM_CPU_STRING	"Atmel tinyAVR"
#		else
#			define MODM_CPU_ATMEGA	1
#			define MODM_CPU_STRING	"Atmel megaAVR"
#		endif
#	endif
#endif

#if defined __X86__ || defined __i386__ || defined i386 || defined _M_IX86 || defined __386__ || defined __x86_64__ || defined _M_X64
#	if defined __x86_64__ || defined _M_X64
#		define MODM_CPU_AMD64 	1
#		define MODM_CPU_STRING	"AMD x86-64"
#		define MODM_ALIGNMENT	8
#	else
#		define MODM_CPU_I386	1
#		define MODM_CPU_STRING	"Intel 386+"
#		define MODM_ALIGNMENT	4
#	endif
#endif

#if defined __arm__ || defined __aarch64__
#	define MODM_CPU_ARM		1
#	define MODM_ALIGNMENT	4
#	if defined __ARM_ARCH_6SM__ || defined __ARM_ARCH_6M__
#		define MODM_CPU_CORTEX_M0	1
#		define MODM_CPU_STRING		"ARM Cortex-M0"
#	elif defined __ARM_ARCH_7M__
#		define MODM_CPU_CORTEX_M3	1
#		define MODM_CPU_STRING		"ARM Cortex-M3"
#	elif defined __ARM_ARCH_7EM__
#		define MODM_CPU_CORTEX_M4	1
#		define MODM_CPU_STRING		"ARM Cortex-M4"
#	elif defined __ARM_ARCH_8M_MAIN__
#		define MODM_CPU_CORTEX_M33	1
#		define MODM_CPU_STRING		"ARM Cortex-M33"
#	elif defined __ARM_ARCH_ISA_A64
#		define MODM_CPU_AARCH64		1
#		define MODM_CPU_STRING		"ARM AArch64"
#	endif
#endif

#ifndef MODM_CPU_STRING
#	error "Couldn't determine target CPU!"
#	define MODM_CPU_STRING	"Unknown"
#endif

// ----------------------------------------------------------------------------
// Provide some additional information

#ifdef __SIZEOF_POINTER__
#	define MODM_SIZEOF_POINTER	__SIZEOF_POINTER__
#else
#	error "MODM_SIZEOF_POINTER not defined! Please provide a suitable definition."
#endif

#define MODM_IS_LITTLE_ENDIAN	(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define MODM_IS_BIG_ENDIAN		(__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)

#define MODM_HAS_IOSTREAM		__has_include(<modm/io/iostream.hpp>)

#endif	// !__DOXYGEN__
