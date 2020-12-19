/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2015-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_PLATFORM_FLASH_READER_HPP
#define	MODM_PLATFORM_FLASH_READER_HPP

/// @cond
#include <modm/architecture/utils.hpp>
#include <cstring>

// A simple implementation for all targets that don't have a
// strict separation between Flash and RAM like the AVRs does.

// 'const' implies internal linkage in C++ if not explicit declared
// as 'extern'.
#define	FLASH_STORAGE(var)				extern const var; const var
#define	EXTERN_FLASH_STORAGE(var)		extern const var

#define	FLASH_STORAGE_STRING(s)			extern const char s[]; const char s[]
#define	EXTERN_FLASH_STORAGE_STRING(s)	extern const char s[]

#define	IFSS(s)	((const char *)(s))

namespace modm
{

template<typename T, std::size_t size>
struct FlashReader
{
	modm_always_inline
	static T
	read(const T* p)
	{
		return *p;
	}
};

}	// namespace modm

/// @endcond
#endif	// MODM_PLATFORM_FLASH_READER_HPP
