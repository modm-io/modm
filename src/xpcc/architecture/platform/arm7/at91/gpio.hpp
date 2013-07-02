// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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

#ifndef XPCC_AT91__GPIO_HPP
#define XPCC_AT91__GPIO_HPP

#include <xpcc/architecture/peripheral/gpio.hpp>
#include "../at91.hpp"

namespace xpcc
{
	namespace gpio
	{
		/**
		 * \ingroup	at91
		 */
		enum Configuration
		{
			NORMAL,		///< standard operation (floating input, external signal needed!)
			PULLUP		///< enable the internal pull-up resistor
		};
	}
}

/**
 * \ingroup	at91
 * \brief	Create a input/output pin type
 * 
 * All I/O functions are reentrant and therefore may be used from within
 * an interrupt and the normal program. To achieve this every function has to
 * operate on read/write-only registers, so the Output Data Status
 * Register (ODSR) had to be disabled for write access.
 * 
 * Operations take between 1 and 6 cycles depending how much information is
 * already present in the registers (PIO base register and Pin mask may be loaded).
 * 
 * Example:
 * \code
 * GPIO__OUTPUT(Led1, A, 18);
 * GPIO__OUTPUT(Led2, A, 17);
 * ...
 * 
 * Led1::setOutput();
 * Led2::setOutput();
 * 
 * Led1::toggle();
 * Led2::set();
 * \endcode
 * 
 * This generates:
 * \verbatim
mvn     r3, #0xb00      ; Initialize registers
mov     r2, #0x40000
mov     r1, #0x20000

str     r2, [r3, #-255] ; Led1::setOutput();
str     r2, [r3, #-239]

str     r1, [r3, #-255] ; Led2::setOutput();
str     r1, [r3, #-239]

ldr     r0, [r3, #-199] ; Led1::toggle()
tst     r0, #0x40000
strne   r2, [r3, #-207]
streq   r2, [r3, #-203]

str     r1, [r3, #-207] ; Led2::set()
\endverbatim
 * 
 * Here you can see that the set() functions takes only one operation because
 * r1 and r3 are already initialized. If we call the same function out of context
 * it needs three operations:
 * 
 * \verbatim
mov     r2, #0x20000    ; Initialize registers
mvn     r3, #0xb00
str     r2, [r3, #-207] ; Led2::set()
\verbatim
 * 
 * \hideinitializer
 */
#define	GPIO__IO(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Mode mode, \
				  ::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL) \
		{ \
			if (mode == ::xpcc::gpio::INPUT) { \
				setInput(); \
				if (config == ::xpcc::gpio::PULLUP) { \
					CONCAT(AT91C_BASE_PIO, port)->PIO_PPUER = (1 << pin); \
				} \
				else { \
					CONCAT(AT91C_BASE_PIO, port)->PIO_PPUDR = (1 << pin); \
				} \
			} \
			else { \
				setOutput(); \
			} \
		} \
		ALWAYS_INLINE static void setOutput() { \
			/* PIO Enable and Output Enable */ \
			CONCAT(AT91C_BASE_PIO, port)->PIO_PER = (1 << pin); \
			CONCAT(AT91C_BASE_PIO, port)->PIO_OER = (1 << pin); } \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void setInput() { \
			CONCAT(AT91C_BASE_PIO, port)->PIO_PER = (1 << pin); \
			CONCAT(AT91C_BASE_PIO, port)->PIO_ODR = (1 << pin); } \
		ALWAYS_INLINE static void set()    { CONCAT(AT91C_BASE_PIO, port)->PIO_SODR = (1 << pin); } \
		ALWAYS_INLINE static void reset()  { CONCAT(AT91C_BASE_PIO, port)->PIO_CODR = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { \
			if (CONCAT(AT91C_BASE_PIO, port)->PIO_ODSR & (1 << pin)) { reset(); } else { set(); } } \
		ALWAYS_INLINE static bool read() { return (CONCAT(AT91C_BASE_PIO, port)->PIO_PDSR & (1 << pin)); } \
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
		ALWAYS_INLINE static void enableOpenDrainOutput()  { CONCAT(AT91C_BASE_PIO, port)->PIO_MDER = (1 << pin); } \
		ALWAYS_INLINE static void disableOpenDrainOutput() { CONCAT(AT91C_BASE_PIO, port)->PIO_MDDR = (1 << pin); } \
		\
		ALWAYS_INLINE static void enableInterrupt()  { CONCAT(AT91C_BASE_PIO, port)->PIO_IER = (1 << pin); } \
		ALWAYS_INLINE static void disableInterrupt() { CONCAT(AT91C_BASE_PIO, port)->PIO_IDR = (1 << pin); } \
		\
		ALWAYS_INLINE static void enablePeripheralA() { \
			CONCAT(AT91C_BASE_PIO, port)->PIO_PDR = (1 << pin); \
			CONCAT(AT91C_BASE_PIO, port)->PIO_ASR = (1 << pin); } \
		ALWAYS_INLINE static void enablePeripheralB() { \
			CONCAT(AT91C_BASE_PIO, port->PIO_PDR) = (1 << pin); \
			CONCAT(AT91C_BASE_PIO, port->PIO_BSR) = (1 << pin); } \
	}

/**
 * \brief	Create a output pin type
 * 
 * \hideinitializer
 * \ingroup	at91
 */
#define	GPIO__OUTPUT(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void setOutput() { \
			/* PIO Enable and Output Enable */ \
			CONCAT(AT91C_BASE_PIO, port)->PIO_PER = (1 << pin); \
			CONCAT(AT91C_BASE_PIO, port)->PIO_OER = (1 << pin); } \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void set()    { CONCAT(AT91C_BASE_PIO, port)->PIO_SODR = (1 << pin); } \
		ALWAYS_INLINE static void reset()  { CONCAT(AT91C_BASE_PIO, port)->PIO_CODR = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { \
			if (CONCAT(AT91C_BASE_PIO, port)->PIO_ODSR & (1 << pin)) { reset(); } else { set(); } } \
		ALWAYS_INLINE static void \
		set(bool status) { \
			if (status) { \
				set(); \
			} \
			else { \
				reset(); \
			} \
		} \
		ALWAYS_INLINE static void enableOpenDrainOutput()  { CONCAT(AT91C_BASE_PIO, port)->PIO_MDER = (1 << pin); } \
		ALWAYS_INLINE static void disableOpenDrainOutput() { CONCAT(AT91C_BASE_PIO, port)->PIO_MDDR = (1 << pin); } \
		\
		ALWAYS_INLINE static void enablePeripheralA() { \
			CONCAT(AT91C_BASE_PIO, port)->PIO_PDR = (1 << pin); \
			CONCAT(AT91C_BASE_PIO, port)->PIO_ASR = (1 << pin); } \
		ALWAYS_INLINE static void enablePeripheralB() { \
			CONCAT(AT91C_BASE_PIO, port)->PIO_PDR = (1 << pin); \
			CONCAT(AT91C_BASE_PIO, port)->PIO_BSR = (1 << pin); } \
	}

/**
 * \brief	Create a input type
 * 
 * \hideinitializer
 * \ingroup	at91
 */
#define GPIO__INPUT(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void \
		configure(::xpcc::gpio::Configuration config = ::xpcc::gpio::NORMAL) { \
			setInput(); \
			if (config == ::xpcc::gpio::PULLUP) { \
				CONCAT(AT91C_BASE_PIO, port)->PIO_PPUER = (1 << pin); \
			} \
			else { \
				CONCAT(AT91C_BASE_PIO, port)->PIO_PPUDR = (1 << pin); \
			} \
		} \
		ALWAYS_INLINE static void setInput() { \
			CONCAT(AT91C_BASE_PIO, port)->PIO_PER = (1 << pin); \
			CONCAT(AT91C_BASE_PIO, port)->PIO_ODR = (1 << pin); } \
		ALWAYS_INLINE static bool read() { return (CONCAT(AT91C_BASE_PIO, port)->PIO_PDSR & (1 << pin)); } \
		\
		ALWAYS_INLINE static void enableInterrupt()  { CONCAT(AT91C_BASE_PIO, port)->PIO_IER = (1 << pin); } \
		ALWAYS_INLINE static void disableInterrupt() { CONCAT(AT91C_BASE_PIO, port)->PIO_IDR = (1 << pin); } \
		\
		ALWAYS_INLINE static void enablePeripheralA() { \
			CONCAT(AT91C_BASE_PIO, port)->PIO_PDR = (1 << pin); \
			CONCAT(AT91C_BASE_PIO, port)->PIO_ASR = (1 << pin); } \
		ALWAYS_INLINE static void enablePeripheralB() { \
			CONCAT(AT91C_BASE_PIO, port)->PIO_PDR = (1 << pin); \
			CONCAT(AT91C_BASE_PIO, port)->PIO_BSR = (1 << pin); } \
	}

#endif // XPCC_AT91__GPIO_HPP
