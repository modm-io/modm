// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LPC23_24xx__GPIO_HPP
#define XPCC_LPC23_24xx__GPIO_HPP

#define	GPIO__IO(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Mode mode, \
				  ::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL) \
		{ \
			if (mode == ::xpcc::gpio::INPUT) { \
				setInput(); \
			} \
			else { \
				setOutput(); \
			} \
		} \
		ALWAYS_INLINE static void setOutput() { \
			CONCAT3(FIO, port, DIR) |= (1 << pin); } \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void setInput() { CONCAT3(FIO, port, DIR) &= ~(1 << pin); } \
		ALWAYS_INLINE static void set()      { CONCAT3(FIO, port, SET)  =  (1 << pin); } \
		ALWAYS_INLINE static void reset()    { CONCAT3(FIO, port, CLR)  =  (1 << pin); } \
		ALWAYS_INLINE static void toggle() { \
			if (CONCAT3(FIO, port, SET) & (1 << pin)) { reset(); } else { set(); } } \
		ALWAYS_INLINE static bool read() { return (CONCAT3(FIO, port, PIN) & (1 << pin)); } \
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
	}

/**
 * \brief	Create a output pin type
 *
 * \hideinitializer
 * \ingroup	lpc2000
 */
#define	GPIO__OUTPUT(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void setOutput() { \
			CONCAT3(FIO, port, DIR) |= (1 << pin); } \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void set()   { CONCAT3(FIO, port, SET) = (1 << pin); } \
		ALWAYS_INLINE static void reset() { CONCAT3(FIO, port, CLR) = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { \
			if (CONCAT3(FIO, port, SET) & (1 << pin)) { reset(); } else { set(); } } \
		ALWAYS_INLINE static void \
		set(bool status) { \
			if (status) { \
				set(); \
			} \
			else { \
				reset(); \
			} \
		} \
	}

/**
 * \brief	Create a input type
 *
 * \hideinitializer
 * \ingroup	lpc2000
 */
#define GPIO__INPUT(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL) { \
			setInput(); \
		} \
		ALWAYS_INLINE static void setInput() { CONCAT3(FIO, port, DIR) &= ~(1 << pin); } \
		ALWAYS_INLINE static bool read() { return (CONCAT3(FIO, port, PIN) & (1 << pin)); } \
	}

#endif // XPCC_LPC23_24xx__GPIO_HPP
