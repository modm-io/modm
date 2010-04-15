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
 * $Id: gpio.hpp 229 2010-03-18 02:30:56Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__PC_GPIO_HPP
#define XPCC__PC_GPIO_HPP

#include <xpcc/utils/macros.hpp>
#include <xpcc/architecture/general/gpio.hpp>

namespace xpcc
{
	namespace gpio
	{
		typedef enum
		{
			NORMAL = 0,
		} Configuration; // PINnCTRL
	};
};

/**
 * \brief	Create a input/output pin type
 * 
 * Dummy to compile µC-Code on PC.
 *
 * \ingroup	gpio
 */
#define	GPIO__IO(name, port, pin) \
	struct name \
	{ \
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Mode mode, \
				  ::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL, \
				  bool invert = false) \
		{ \
			(void) config; \
			(void) invert; \
		} \
		\
		ALWAYS_INLINE static void setOutput() { } \
		ALWAYS_INLINE static void setInput() { } \
		ALWAYS_INLINE static void high() { } \
		ALWAYS_INLINE static void low() { } \
		ALWAYS_INLINE static void toggle() { } \
		\
		ALWAYS_INLINE static void \
		set(bool status) { \
			(void) status; \
		} \
		\
		ALWAYS_INLINE static bool read() { return false; } \
		ALWAYS_INLINE static uint8_t getMask() { return 0;} \
	}

/**
 * \brief	Create a output pin type
 * 
 * \ingroup	gpio
 */
#define	GPIO__OUTPUT(name, port, pin) \
	struct name \
	{ \
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL, \
				  bool invert = false) { \
			(void) config; \
			(void) invert; \
		} \
		\
		ALWAYS_INLINE static void setOutput() { } \
		ALWAYS_INLINE static void high() { } \
		ALWAYS_INLINE static void low() { } \
		ALWAYS_INLINE static void toggle() { } \
		\
		ALWAYS_INLINE static void \
		set(bool status) { \
			(void) status; \
		} \
		ALWAYS_INLINE static uint8_t getMask() { return 0; } \
	}

/**
 * \brief	Create a input pin type
 * 
 * \ingroup	gpio
 */
#define GPIO__INPUT(name, port, pin) \
	struct name \
	{ \
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL, \
				  bool invert = false) { \
			(void) config; \
			(void) invert; \
		} \
		\
		ALWAYS_INLINE static void setInput() { } \
		ALWAYS_INLINE static bool read() { } \
		ALWAYS_INLINE static void \
		configureInputSense(::xpcc::gpio::InputSense inputSense){\
			(void) inputSense; \
		}\
		\
		ALWAYS_INLINE static void \
		configureInterrupt0(::xpcc::gpio::InterruptLevel0 interruptLevel) { \
			(void)interruptLevel; \
		} \
		\
		ALWAYS_INLINE static void \
		configureInterrupt1(::xpcc::gpio::InterruptLevel1 interruptLevel) { \
			(void)interruptLevel; \
		} \
		\
		ALWAYS_INLINE static uint8_t getMask() { return 0;} \
		ALWAYS_INLINE static uint16_t getInterrupt0Vector() { return 0;} \
		ALWAYS_INLINE static uint16_t getInterrupt1Vector() { return 0;} \
	}

#endif // XPCC__ATXMEGA_GPIO_HPP
