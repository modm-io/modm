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

#ifndef XPCC__XMEGA_GPIO_HPP
#define XPCC__XMEGA_GPIO_HPP

#include <avr/io.h>
#include <xpcc/utils/misc.hpp>

namespace xpcc
{
	/**
	 * @ingroup	hal
	 * @headerfile	<xpcc/hal/peripheral/avr/xmega/gpio.hpp>
	 */
	namespace gpio
	{
		/**
		 * @ingroup	hal
		 */
		typedef enum
		{
			INPUT,
			OUTPUT,
		} Mode;
		
		/**
		 * @ingroup	hal
		 */
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

// some helper macros
#define	PORT(x)		(PORT ## x)

/**
 * @ingroup	hal
 * @brief	Create a input/output pin type
 */
#define	CREATE_IO_PIN(name, port, pin) \
	struct name \
	{ \
		inline name() { this->input() } \
		\
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Mode mode, \
				  ::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL, \
				  bool invert = false) \
		{ \
			if (mode == ::xpcc::gpio::INPUT) { \
				input(); \
			} \
			else { \
				output(); \
			} \
			PORT(port).PIN ## pin ## CTRL = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		\
		ALWAYS_INLINE static void output() { PORT(port).DIRSET = (1 << pin); } \
		ALWAYS_INLINE static void input() { PORT(port).DIRCLR = (1 << pin); } \
		ALWAYS_INLINE static void set() { PORT(port).OUTSET = (1 << pin); } \
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
		ALWAYS_INLINE static void reset() { PORT(port).OUTCLR = (1 << pin); } \
		ALWAYS_INLINE static bool get() { return (PORT(port).IN & (1 << pin)); } \
	}

/**
 * @ingroup	hal
 * @brief	Create a output pin type
 */
#define	CREATE_OUTPUT_PIN(name, port, pin) \
	struct name \
	{ \
		inline name() { this->output(); } \
		\
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL, \
				  bool invert = false) { \
			output(); \
			PORT(port).PIN ## pin ## CTRL = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		\
		ALWAYS_INLINE static void output() { PORT(port).DIRSET = (1 << pin); } \
		ALWAYS_INLINE static void set() { PORT(port).OUTSET = (1 << pin); } \
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
		ALWAYS_INLINE static void reset() { PORT(port).OUTCLR = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { PORT(port).OUTTGL = (1 << pin); } \
	}

/**
 * @ingroup	hal
 * @brief	Create a input pin type
 */
#define CREATE_INPUT_PIN(name, port, pin) \
	struct name \
	{ \
		inline name() { this->input(); } \
		\
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL, \
				  bool invert = false) { \
			input(); \
			PORT(port).PIN ## pin ## CTRL = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		\
		ALWAYS_INLINE static void input() { PORT(port).DIRCLR = (1 << pin); } \
		ALWAYS_INLINE static bool get() { return (PORT(port).IN & (1 << pin)); } \
	}


#endif // XPCC__XMEGA_GPIO_HPP
