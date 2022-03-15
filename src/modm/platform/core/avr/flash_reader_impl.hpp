/*
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

// Already documented in :architecture:accessor
/// @cond
#include <cstddef>
#include <avr/pgmspace.h>

#define	FLASH_STORAGE(var)				extern const var PROGMEM; const var
#define	EXTERN_FLASH_STORAGE(var)		extern const var PROGMEM

#define	FLASH_STORAGE_STRING(s)			extern const char s[] PROGMEM; const char s[]
#define	EXTERN_FLASH_STORAGE_STRING(s)	extern const char s[] PROGMEM

#define	IFSS(s)	(modm::accessor::Flash<char>(PSTR(s)))

namespace modm
{

template<typename T, std::size_t size>
struct FlashReader
{
	static T
	read(const void* p)
	{
		T retval;
		memcpy_P(reinterpret_cast<void *>(&retval), p, size);
		return retval;
	}
};

template<typename T>
struct FlashReader<T, 1>
{
	static T
	read(const void* p)
	{
		union {
			uint8_t in;
			T out;
		} convert;

		convert.in = pgm_read_byte(p);
		return convert.out;
	}
};

template<typename T>
struct FlashReader<T, 2>
{
	static T
	read(const void* p)
	{
		union {
			uint16_t in;
			T out;
		} convert;

		convert.in = pgm_read_word(p);
		return convert.out;
	}
};

template<typename T>
struct FlashReader<T, 4>
{
	static T
	read(const void* p)
	{
		// we have to work with this union here because the return
		// type of pgm_read_dword() is always uint32_t and the compiler
		// refuses to accept a simple cast to T if T is no build-in type.
		union {
			uint32_t in;
			T out;
		} convert;

		convert.in = pgm_read_dword(p);
		return convert.out;
	}
};

}	// namespace modm

/// @endcond

#endif	// MODM_PLATFORM_FLASH_READER_HPP
