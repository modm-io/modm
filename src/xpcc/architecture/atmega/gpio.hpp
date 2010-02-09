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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__MEGA_GPIO_HPP
#define XPCC__MEGA_GPIO_HPP

#include <avr/io.h>
#include <xpcc/utils/macros.hpp>

namespace xpcc
{
	/**
	 * \ingroup	architecture
	 */
	namespace gpio
	{
		/**
		 * \ingroup	architecture
		 */
		typedef enum
		{
			INPUT,
			OUTPUT,
		} Mode;
		
		/**
		 * \ingroup	architecture
		 */
		typedef enum
		{
			NORMAL = 0,
			PULLUP = 1,
		} Configuration;
	}
}

/**
 * \ingroup	architecture
 * \brief	Create a input/output pin type
 */
#define	CREATE_IO_PIN(name, port, pin) \
	struct name { \
		name() { this->input() } \
		ALWAYS_INLINE static void output() { DDR ## port |= (1 << pin); } \
		ALWAYS_INLINE static void input() { DDR ## port &= ~(1 << pin); } \
		ALWAYS_INLINE static void set() { PORT ## port |= (1 << pin); } \
		ALWAYS_INLINE static void reset() { PORT ## port &= ~(1 << pin); } \
		ALWAYS_INLINE static bool get() { return (PIN ## port & (1 << pin)); } \
	}

/**
 * \ingroup	architecture
 * \brief	Create a output pin type
 */
#define	CREATE_OUTPUT_PIN(name, port, pin) \
	struct name { \
		name() { this->output(); } \
		ALWAYS_INLINE static void output() { DDR ## port |= (1 << pin); } \
		\
		ALWAYS_INLINE static void \
		set(bool status) { \
			if (status) { \
				set(); \
			} \
			else { \
				reset(); \
			} \
		} \
		\
		ALWAYS_INLINE static void set() { PORT ## port |= (1 << pin); } \
		ALWAYS_INLINE static void reset() { PORT ## port &= ~(1 << pin); } \
		ALWAYS_INLINE static void toggle() { PORT ## port ^= (1 << pin); } \
	}

/**
 * \ingroup	architecture
 * \brief	Create a input type
 */
#define CREATE_INPUT_PIN(name, port, pin) \
	struct name { \
		name() { this->input(); } \
		\
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL) { \
			input(); \
			if (config == ::xpcc::gpio::PULLUP) { \
				PORT ## port |= (1 << pin); \
			} \
			else { \
				PORT ## port &= ~(1 << pin); \
			} \
		} \
		\
		ALWAYS_INLINE static void input() { DDR ## port &= ~(1 << pin); } \
		ALWAYS_INLINE static bool get() { return (PIN ## port & (1 << pin)); } \
	}

#endif // XPCC__MEGA_GPIO_HPP
