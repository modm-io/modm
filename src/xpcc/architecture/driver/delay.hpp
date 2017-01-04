/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Dave Webb
 * Copyright (c) 2012, 2014, Sascha Schade
 * Copyright (c) 2012, 2014-2016, Niklas Hauser
 * Copyright (c) 2014, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_DELAY_HPP
#define	XPCC_DELAY_HPP

#include <stdint.h>

#ifdef __DOXYGEN__

namespace xpcc
{
	/**
	 * \brief	Delay ns nanoseconds
	 * \ingroup	architecture
	 */
	void
	delayNanoseconds(uint16_t ns);

	/**
	 * \brief	Delay us microseconds
	 * \ingroup	architecture
	 */
	void
	delayMicroseconds(uint16_t us);

	/**
	 * \brief	Delay ms milliseconds
	 * \ingroup	architecture
	 */
	void
	delayMilliseconds(uint16_t ms);
}

#else // !__DOXYGEN__

#include <modm/architecture/detect.hpp>
#include <modm/architecture/utils.hpp>

#if defined(XPCC_CPU_AVR)

	#include <util/delay.h>

	namespace xpcc
	{
		void xpcc_always_inline
		delayNanoseconds(uint16_t /*ns*/)
		{
			_delay_us(1);
		}

		xpcc_always_inline void
		delayMicroseconds(uint16_t us)
		{
			while(us--) _delay_us(1);
		}

		xpcc_always_inline void
		delayMilliseconds(uint16_t ms)
		{
			while(ms--) _delay_ms(1);
		}
	}

#elif defined(XPCC_OS_UNIX) || defined(XPCC_OS_OSX)

	#include <unistd.h>

	namespace xpcc
	{
		void xpcc_always_inline
		delayNanoseconds(uint16_t /*ns*/)
		{
			usleep(1);
		}

		xpcc_always_inline void
		delayMicroseconds(uint16_t us)
		{
			usleep(us);
		}

		xpcc_always_inline void
		delayMilliseconds(uint16_t ms)
		{
			usleep(uint32_t(ms)*1000);
		}
	}

#elif defined(XPCC_OS_WIN32)

	namespace xpcc
	{
		/*inline void
		delayMicroseconds(int us)
		{
			int ms = us / 1000;
			if (ms <= 0) {
				ms = 1;
			}
			Sleep(ms);
		}

		inline void
		delay_ms(int ms)
		{
			Sleep(ms);
		}*/

		inline void
		delayNanoseconds(int)
		{
			// TODO
		}

		inline void
		delayMicroseconds(int)
		{
			// TODO
		}

		inline void
		delayMilliseconds(int)
		{
			// TODO
		}
	}

#elif defined(XPCC_CPU_ARM)

	extern "C" void _delay_ns(uint32_t ns);
	extern "C" void _delay_us(uint32_t us);
	extern "C" void _delay_ms(uint32_t ms);

	namespace xpcc
	{
		/// @warning    There is little to no timing guarantee with this method!
		xpcc_always_inline void
		delayNanoseconds(uint16_t ns)
		{
			::_delay_ns(ns);
		}

		xpcc_always_inline void
		delayMicroseconds(uint16_t us)
		{
			::_delay_us(us);
		}

		/// @warning    this method is _not_ guaranteed to work with inputs over 9000ms
		///             since "It's Over 9000"! (meaning 32bit arithmetics).
		xpcc_always_inline void
		delayMilliseconds(uint16_t ms)
		{
			::_delay_ms(ms);
		}
	}
#else
	#error "Unknown architecture, please add some specific delay functions!"
#endif

#endif	// !__DOXYGEN__
#endif	// XPCC_DELAY_HPP
