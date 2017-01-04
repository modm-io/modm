/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011-2012, 2014-2015, Niklas Hauser
 * Copyright (c) 2012, 2015, Sascha Schade
 * Copyright (c) 2015, Daniel Krebs
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifdef __DOXYGEN__

/**
 * \brief	Compiler description
 * \ingroup	platform
 */
#define	XPCC_COMPILER_STRING

/**
 * \brief	Possible compilers
 * \ingroup	platform
 */
//@{
#define	XPCC_COMPILER_GCC
#define	XPCC_COMPILER_MSVC
//@}

/**
 * \brief	Operating system description
 * \ingroup	platform
 */
#define	XPCC_OS_STRING

/**
 * \brief	Possible operating systems
 * \ingroup	platform
 */
//@{
#define	XPCC_OS_UNIX
#define	XPCC_OS_LINUX
#define	XPCC_OS_OSX
#define	XPCC_OS_WIN32
#define	XPCC_OS_WIN64
//@}

/**
 * \brief	CPU description
 * \ingroup	platform
 */
#define	XPCC_CPU_STRING

//@{
/**
 * \brief	Possible CPU definitions
 * \ingroup	platform
 */
#define	XPCC_CPU_AVR
#define	XPCC_CPU_ATXMEGA
#define	XPCC_CPU_ATMEGA
#define	XPCC_CPU_ATTINY

#define	XPCC_CPU_ARM
#define	XPCC_CPU_ARM7TDMI
#define	XPCC_CPU_CORTEX_M0
#define	XPCC_CPU_CORTEX_M4
#define	XPCC_CPU_CORTEX_M4

#define	XPCC_OS_HOSTED
#define	XPCC_OS_HOSTED_64

//@{
/**
 * \brief	Memory descriptions
 * \ingroup	platform
 */
#define XPCC_ALIGNMENT

#define XPCC_ORDER_BIG_ENDIAN
#define XPCC_ORDER_LITTLE_ENDIAN
#define XPCC_ORDER_PDP_ENDIAN

#define XPCC_BYTE_ORDER

#define XPCC_IS_BIG_ENDIAN
#define XPCC_IS_LITTLE_ENDIAN

#define	XPCC_SIZEOF_POINTER
//@}

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
#	define XPCC_COMPILER_STRING	"Clang"
#	define XPCC_COMPILER_CLANG 1
#elif defined __GNUC__
#	define XPCC_COMPILER_GCC 1
#	if defined __MINGW32__
#		define XPCC_COMPILER_STRING	"MinGW / MinGW-w64"
#		define XPCC_COMPILER_MINGW 1
#	else
#		define XPCC_COMPILER_STRING	"Gnu GCC"
#	endif
#endif

#if defined _MSC_VER
#	define XPCC_COMPILER_STRING 	"Microsoft Visual C++"
#	define XPCC_COMPILER_MSVC 1
#endif

#if !defined XPCC_COMPILER_STRING
#	define XPCC_COMPILER_STRING 	"Unknown compiler"
#	warning	"Couldn't determine the used compiler! Please add it to <modm/architecture/detect.hpp>"
#endif

// ----------------------------------------------------------------------------
// Determine target operating system

#if defined linux || defined __linux__
#	define XPCC_OS_HOSTED 1
#	define XPCC_OS_LINUX 1
#	define XPCC_OS_STRING 			"Linux"
#endif

#if (defined __MWERKS__ && defined __powerc && !defined macintosh) || defined __APPLE_CC__ || defined macosx
#	define XPCC_OS_HOSTED 1
#	define XPCC_OS_OSX 1
#	define XPCC_OS_STRING			"MacOS X"
#endif

#if defined __unix__
#	define XPCC_OS_HOSTED 1
#	define XPCC_OS_UNIX 1
#	if !defined XPCC_OS_STRING
#		define XPCC_OS_STRING 		"Unix-like(generic)"
#	endif
#endif

#if defined _WIN32 || defined WIN32 || defined __NT__ || defined __WIN32__
#	define XPCC_OS_HOSTED 1
#	define XPCC_OS_WIN32 1
#	if defined _WIN64
#		define XPCC_OS_WIN64 1
#		define XPCC_OS_STRING		"Win64"
#	else
#		if !defined XPCC_OS_STRING
#			define XPCC_OS_STRING	"Win32"
#		endif
#	endif
#endif

#define XPCC_ORDER_BIG_ENDIAN 4321
#define XPCC_ORDER_LITTLE_ENDIAN 1234
#define XPCC_ORDER_PDP_ENDIAN 3412

// If we did not detect any of Linux, OS X or Windows
#if !defined(XPCC_OS_HOSTED)
#	define XPCC_OS_NONE	1
#endif

// ----------------------------------------------------------------------------
// Determine CPU type

#if defined __AVR__
#	define XPCC_CPU_AVR	1
#	define XPCC_ALIGNMENT	1
#	if defined __AVR_XMEGA__
#		define XPCC_CPU_ATXMEGA	1
#		define XPCC_CPU_STRING		"Atmel XMEGA"
#	else
		// see http://www.nongnu.org/avr-libc/user-manual/using_tools.html
#		if __AVR_ARCH__ == 2 || __AVR_ARCH__ == 25 || __AVR_ARCH__ == 1
#			define XPCC_CPU_ATTINY	1
#			define XPCC_CPU_STRING	"Atmel tinyAVR"
#		else
#			define XPCC_CPU_ATMEGA	1
#			define XPCC_CPU_STRING	"Atmel megaAVR"
#		endif
#	endif
#endif

#if defined __X86__ || defined __i386__ || defined i386 || defined _M_IX86 || defined __386__ || defined __x86_64__ || defined _M_X64
#	if defined __x86_64__ || defined _M_X64
#		define XPCC_CPU_AMD64 1
#		define XPCC_CPU_STRING		"AMD x86-64"
#		define XPCC_ALIGNMENT	8
#	else
#		define XPCC_CPU_I386 1
#		define XPCC_CPU_STRING		"Intel 386+"
#		define XPCC_ALIGNMENT	4
#	endif
#endif

#if defined __arm__ || defined __ARM_EABI__
#	define XPCC_CPU_ARM	1
#	define XPCC_ALIGNMENT	4
#	if defined __ARM_ARCH_4T__
#		define XPCC_CPU_ARM7TDMI	1
#		define XPCC_CPU_STRING		"ARM7TDMI"
#	elif defined __ARM_ARCH_6SM__ || defined __ARM_ARCH_6M__
#		define XPCC_CPU_CORTEX_M0	1
#		define XPCC_CPU_STRING		"Cortex-M0"
#	elif defined __ARM_ARCH_7M__
#		define XPCC_CPU_CORTEX_M3	1
#		define XPCC_CPU_STRING		"Cortex-M3"
#	elif defined __ARM_ARCH_7EM__
#		define XPCC_CPU_CORTEX_M4	1
#		define XPCC_CPU_STRING		"Cortex-M4"
#	elif defined __ARM_ARCH_6__
#		define XPCC_CPU_CORTEX_A6	1
#		define XPCC_CPU_STRING		"Cortex-A6"
#	elif defined __ARM_ARCH_7A__
#		define XPCC_CPU_CORTEX_A7	1
#		define XPCC_CPU_STRING		"Cortex-A7"
#	endif
#endif

#if !defined XPCC_CPU_STRING
#	error "Couldn't determine target CPU!"
#	define XPCC_CPU_STRING 		"Unknown"
#endif

// ----------------------------------------------------------------------------
// Provide some additional information

#if defined __SIZEOF_POINTER__
#	define XPCC_SIZEOF_POINTER			__SIZEOF_POINTER__
#else
#	if defined(XPCC_OS_HOSTED_64)
#		define XPCC_SIZEOF_POINTER		8
#	elif defined(XPCC_OS_HOSTED_32)
#		define XPCC_SIZEOF_POINTER		4
#	endif
#endif

#if !defined XPCC_SIZEOF_POINTER
#	error "XPCC_SIZEOF_POINTER not defined! Please provide a suitable definition."
#endif


#if defined __BYTE_ORDER__
#	define XPCC_BYTE_ORDER		__BYTE_ORDER__
#endif

#if !defined XPCC_BYTE_ORDER
#	error "XPCC_BYTE_ORDER not defined! Please provide a suitable definition."
#endif

#define XPCC_IS_BIG_ENDIAN		(XPCC_BYTE_ORDER == XPCC_ORDER_BIG_ENDIAN)
#define XPCC_IS_LITTLE_ENDIAN	(XPCC_BYTE_ORDER == XPCC_ORDER_LITTLE_ENDIAN)

#endif	// !__DOXYGEN__
