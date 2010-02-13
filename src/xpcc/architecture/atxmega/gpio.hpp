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

#ifndef XPCC__ATXMEGA_GPIO_HPP
#define XPCC__ATXMEGA_GPIO_HPP

#include <avr/io.h>

#include <xpcc/utils/macros.hpp>
#include <xpcc/architecture/general/gpio.hpp>

namespace xpcc
{
	namespace gpio
	{
		typedef enum
		{
			NORMAL = PORT_OPC_TOTEM_gc,
			BUSKEEPER = PORT_OPC_BUSKEEPER_gc,
			PULLDOWN = PORT_OPC_PULLDOWN_gc,
			PULLUP = PORT_OPC_PULLUP_gc,
			WIREDDOR = PORT_OPC_WIREDOR_gc,
			WIREDAND = PORT_OPC_WIREDAND_gc,
			WIREDORPULL = PORT_OPC_WIREDORPULL_gc,
			WIREDANDPULL = PORT_OPC_WIREDANDPULL_gc,
		} Configuration;
	}
}

/**
 * \brief	Create a input/output pin type
 * 
 * \ingroup	gpio
 */
#define	CREATE_IO_PIN(name, port, pin) \
	struct name \
	{ \
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Mode mode, \
				  ::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL, \
				  bool invert = false) \
		{ \
			if (mode == ::xpcc::gpio::INPUT) { \
				setInput(); \
			} \
			else { \
				setOutput(); \
			} \
			PORT ## port ## _PIN ## pin ## CTRL = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		\
		ALWAYS_INLINE static void setOutput() { PORT ## port ## _DIRSET = (1 << pin); } \
		ALWAYS_INLINE static void setInput() { PORT ## port ## _DIRCLR = (1 << pin); } \
		ALWAYS_INLINE static void set() { PORT ## port ## _OUTSET = (1 << pin); } \
		ALWAYS_INLINE static void reset() { PORT ## port ## _OUTCLR = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { PORT ## port ## _OUTTGL = (1 << pin); } \
		\
		ALWAYS_INLINE static void \
		write(bool status) { \
			if (status) { \
				set(); \
			} \
			else { \
				reset(); \
			} \
		} \
		\
		ALWAYS_INLINE static bool read() { return (PORT ## port ## _IN & (1 << pin)); } \
	}

/**
 * \brief	Create a output pin type
 * 
 * \ingroup	gpio
 */
#define	CREATE_OUTPUT_PIN(name, port, pin) \
	struct name \
	{ \
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL, \
				  bool invert = false) { \
			setOutput(); \
			PORT ## port ## _PIN ## pin ## CTRL = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		\
		ALWAYS_INLINE static void setOutput() { PORT ## port ## _DIRSET = (1 << pin); } \
		ALWAYS_INLINE static void set() { PORT ## port ## _OUTSET = (1 << pin); } \
		ALWAYS_INLINE static void reset() { PORT ## port ## _OUTCLR = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { PORT ## port ## _OUTTGL = (1 << pin); } \
		\
		ALWAYS_INLINE static void \
		write(bool status) { \
			if (status) { \
				set(); \
			} \
			else { \
				reset(); \
			} \
		} \
	}

/**
 * \brief	Create a input pin type
 * 
 * \ingroup	gpio
 */
#define CREATE_INPUT_PIN(name, port, pin) \
	struct name \
	{ \
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL, \
				  bool invert = false) { \
			setInput(); \
			PORT ## port ## _PIN ## pin ## CTRL = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		\
		ALWAYS_INLINE static void setInput() { PORT ## port ## _DIRCLR = (1 << pin); } \
		ALWAYS_INLINE static bool read() { return (PORT ## port ## _IN & (1 << pin)); } \
	}

// FIXME
//#define CREATE_LOW_NIBBLE(name, port)
//#define CREATE_HIGH_NIBBLE(name, port)

#endif // XPCC__ATXMEGA_GPIO_HPP
