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


/// Compiler description
/// @ingroup modm_architecture
#define	MODM_COMPILER_STRING

/// Possible compilers
/// @ingroup modm_architecture
/// @{
#define	MODM_COMPILER_GCC
#define	MODM_COMPILER_MSVC
/// @}

/// Operating system description
/// @ingroup modm_architecture
#define	MODM_OS_STRING

/// Possible operating systems
/// @ingroup modm_architecture
/// @{
#define	MODM_OS_UNIX
#define	MODM_OS_LINUX
#define	MODM_OS_OSX
#define	MODM_OS_WIN32
#define	MODM_OS_WIN64
#define	MODM_OS_HOSTED
#define	MODM_OS_HOSTED_64
/// @}

/// CPU description
/// @ingroup modm_architecture
#define	MODM_CPU_STRING

/// Possible CPU definitions
/// @ingroup modm_architecture
/// @{
#define	MODM_CPU_AVR
#define	MODM_CPU_ATXMEGA
#define	MODM_CPU_ATMEGA
#define	MODM_CPU_ATTINY

#define	MODM_CPU_ARM
#define	MODM_CPU_ARM7TDMI
#define	MODM_CPU_CORTEX_M0
#define	MODM_CPU_CORTEX_M3
#define	MODM_CPU_CORTEX_M4
/// @}

///	Memory descriptions
/// @ingroup modm_architecture
/// @{
#define MODM_ALIGNMENT

#define MODM_ORDER_BIG_ENDIAN
#define MODM_ORDER_LITTLE_ENDIAN
#define MODM_ORDER_PDP_ENDIAN

#define MODM_BYTE_ORDER

#define MODM_IS_BIG_ENDIAN
#define MODM_IS_LITTLE_ENDIAN

#define	MODM_SIZEOF_POINTER
/// @}

#else //!__DOXYGEN__

// ----------------------------------------------------------------------------
// Determine compilation environment

/**
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

#if defined __clang__
#	define MODM_COMPILER_STRING	"Clang"
#	define MODM_COMPILER_CLANG 1
#elif defined __GNUC__
#	define MODM_COMPILER_GCC 1
#	if defined __MINGW32__
#		define MODM_COMPILER_STRING	"MinGW / MinGW-w64"
#		define MODM_COMPILER_MINGW 1
#	else
#		define MODM_COMPILER_STRING	"Gnu GCC"
#	endif
#endif

#if defined _MSC_VER
#	define MODM_COMPILER_STRING 	"Microsoft Visual C++"
#	define MODM_COMPILER_MSVC 1
#endif

#if !defined MODM_COMPILER_STRING
#	define MODM_COMPILER_STRING 	"Unknown compiler"
#	warning	"Couldn't determine the used compiler! Please add it to <modm/architecture/detect.hpp>"
#endif

// ----------------------------------------------------------------------------
// Determine target operating system

#if defined linux || defined __linux__
#	define MODM_OS_HOSTED 1
#	define MODM_OS_LINUX 1
#	define MODM_OS_STRING 			"Linux"
#endif

#if (defined __MWERKS__ && defined __powerc && !defined macintosh) || defined __APPLE_CC__ || defined macosx
#	define MODM_OS_HOSTED 1
#	define MODM_OS_OSX 1
#	define MODM_OS_STRING			"MacOS X"
#endif

#if defined __unix__
#	define MODM_OS_HOSTED 1
#	define MODM_OS_UNIX 1
#	if !defined MODM_OS_STRING
#		define MODM_OS_STRING 		"Unix-like(generic)"
#	endif
#endif

#if defined _WIN32 || defined WIN32 || defined __NT__ || defined __WIN32__
#	define MODM_OS_HOSTED 1
#	define MODM_OS_WIN32 1
#	if defined _WIN64
#		define MODM_OS_WIN64 1
#		define MODM_OS_STRING		"Win64"
#	else
#		if !defined MODM_OS_STRING
#			define MODM_OS_STRING	"Win32"
#		endif
#	endif
#endif

#define MODM_ORDER_BIG_ENDIAN 4321
#define MODM_ORDER_LITTLE_ENDIAN 1234
#define MODM_ORDER_PDP_ENDIAN 3412

// If we did not detect any of Linux, OS X or Windows
#if !defined(MODM_OS_HOSTED)
#	define MODM_OS_NONE	1
#endif

// ----------------------------------------------------------------------------
// Determine CPU type

#if defined __AVR__
#	define MODM_CPU_AVR	1
#	define MODM_ALIGNMENT	1
#	if defined __AVR_XMEGA__
#		define MODM_CPU_ATXMEGA	1
#		define MODM_CPU_STRING		"Atmel XMEGA"
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
#		define MODM_CPU_AMD64 1
#		define MODM_CPU_STRING		"AMD x86-64"
#		define MODM_ALIGNMENT	8
#	else
#		define MODM_CPU_I386 1
#		define MODM_CPU_STRING		"Intel 386+"
#		define MODM_ALIGNMENT	4
#	endif
#endif

#if defined __arm__ || defined __aarch64__
#	define MODM_CPU_ARM	1
#	define MODM_ALIGNMENT	4
#	if defined __ARM_ARCH_4T__
#		define MODM_CPU_ARM7TDMI	1
#		define MODM_CPU_STRING		"ARM7TDMI"
#	elif defined __ARM_ARCH_6SM__ || defined __ARM_ARCH_6M__
#		define MODM_CPU_CORTEX_M0	1
#		define MODM_CPU_STRING		"ARM Cortex-M0"
#	elif defined __ARM_ARCH_7M__
#		define MODM_CPU_CORTEX_M3	1
#		define MODM_CPU_STRING		"ARM Cortex-M3"
#	elif defined __ARM_ARCH_7EM__
#		define MODM_CPU_CORTEX_M4	1
#		define MODM_CPU_STRING		"ARM Cortex-M4"
#	elif defined __ARM_ARCH_6__
#		define MODM_CPU_CORTEX_A6	1
#		define MODM_CPU_STRING		"ARM Cortex-A6"
#	elif defined __ARM_ARCH_7A__
#		define MODM_CPU_CORTEX_A7	1
#		define MODM_CPU_STRING		"ARM Cortex-A7"
#	elif defined __ARM_ARCH_ISA_A64
#		define MODM_CPU_AARCH64	1
#		define MODM_CPU_STRING		"ARM AArch64"
#	endif
#endif

#if !defined MODM_CPU_STRING
#	error "Couldn't determine target CPU!"
#	define MODM_CPU_STRING 		"Unknown"
#endif

// ----------------------------------------------------------------------------
// Provide some additional information

#if defined __SIZEOF_POINTER__
#	define MODM_SIZEOF_POINTER			__SIZEOF_POINTER__
#else
#	if defined(MODM_OS_HOSTED_64)
#		define MODM_SIZEOF_POINTER		8
#	elif defined(MODM_OS_HOSTED_32)
#		define MODM_SIZEOF_POINTER		4
#	endif
#endif

#if !defined MODM_SIZEOF_POINTER
#	error "MODM_SIZEOF_POINTER not defined! Please provide a suitable definition."
#endif


#if defined __BYTE_ORDER__
#	define MODM_BYTE_ORDER		__BYTE_ORDER__
#endif

#if !defined MODM_BYTE_ORDER
#	error "MODM_BYTE_ORDER not defined! Please provide a suitable definition."
#endif

#define MODM_IS_BIG_ENDIAN		(MODM_BYTE_ORDER == MODM_ORDER_BIG_ENDIAN)
#define MODM_IS_LITTLE_ENDIAN	(MODM_BYTE_ORDER == MODM_ORDER_LITTLE_ENDIAN)

#define MODM_HAS_IOSTREAM __has_include(<modm/io/iostream.hpp>)

#endif	// !__DOXYGEN__
