/*
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LEGACY_MACROS_HPP
#define MODM_LEGACY_MACROS_HPP

#include "detect.hpp"

#ifndef __DOXYGEN__

#ifndef MODM_DISABLE_LEGACY_MACROS

#	define STRINGIFY(s)			MODM_STRINGIFY(s)
#	define CONCAT(a,b)			MODM_CONCAT(a,b)
#	define CONCAT3(a,b,c)		MODM_CONCAT3(a,b,c)
#	define CONCAT4(a,b,c,d)		MODM_CONCAT4(a,b,c,d)
#	define CONCAT5(a,b,c,d,e)	MODM_CONCAT5(a,b,c,d,e)

#	define ALWAYS_INLINE  		modm_always_inline
#	define ATTRIBUTE_UNUSED		modm_unused
#	define ATTRIBUTE_WEAK		modm_weak
#	define ATTRIBUTE_ALIGNED(n)	modm_aligned(n)
#	define ATTRIBUTE_PACKED		modm_packed
#	define ATTRIBUTE_FASTCODE	modm_fastcode
#	define ATTRIBUTE_FASTDATA	modm_fastdata
#	define ATTRIBUTE_MAY_ALIAS	modm_may_alias
#	define likely(x)			modm_likely(x)
#	define unlikely(x)			modm_unlikely(x)

#	ifdef MODM_OS_HOSTED
#		define 	MAIN_FUNCTION		int main( int argc, char* argv[] )
#		define	MAIN_FUNCTION_NAKED	int main( int,      char**       )
#	else
#		define	MAIN_FUNCTION	int main(void)
#		define	MAIN_FUNCTION_NAKED MAIN_FUNCTION
#	endif

#	define ENUM_CLASS_FLAG(name) \
		inline name operator|(name a, name b) \
		{return static_cast<name>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));} \
		inline uint32_t operator&(name a, name b) \
		{return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b));} \
		inline uint32_t operator&(uint32_t a, name b) \
		{return ((a) & static_cast<uint32_t>(b));} \
		inline uint32_t operator&(name a, uint32_t b) \
		{return (static_cast<uint32_t>(a) & (b));}

#endif

#define MODM_ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))


#endif	// !__DOXYGEN__

#endif	// MODM_LEGACY_MACROS_HPP
