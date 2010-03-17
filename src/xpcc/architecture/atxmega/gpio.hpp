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
		} Configuration; // PINnCTRL
		
		typedef enum
		{
			BOTHEDGES = PORT_ISC_BOTHEDGES_gc,
			RISING = PORT_ISC_RISING_gc,  
			FALLING = PORT_ISC_FALLING_gc, 
			LEVEL = PORT_ISC_LEVEL_gc,  
			INPUT_DISABLE = PORT_ISC_INPUT_DISABLE_gc,
		} InputSense; // PINnCTRL
		
		/**
		 * 
		 * Be Carefull: There are only two Interrupts for a Port,
		 * so it is not possible to select different
		 * Interruptlevels for different Pins of one Port on the same Interrupt_x.
		 * Previous level settings will be overridden and be affected to all
		 * Pins on this Port for which interrupt_x has been enabled.
		 * 
		 * Switching level of interrupt_x off will not disable the interrupt, but
		 * clear the corresponding bit in the interrupt_x_mask.
		 * 
		 */
		typedef enum
		{
			INT0LVL_OFF = PORT_INT0LVL_OFF_gc,
			INT0LVL_LO = PORT_INT0LVL_LO_gc,
			INT0LVL_MED = PORT_INT0LVL_MED_gc,
			INT0LVL_HI = PORT_INT0LVL_HI_gc,
		} InterruptLevel0;
		
		typedef enum
		{
			INT1LVL_OFF = PORT_INT1LVL_OFF_gc,
			INT1LVL_LO = PORT_INT1LVL_LO_gc,
			INT1LVL_MED = PORT_INT1LVL_MED_gc,
			INT1LVL_HI = PORT_INT1LVL_HI_gc,
		} InterruptLevel1;
	}
}

/**
 * \brief	Create a input/output pin type
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
		ALWAYS_INLINE static void high() { PORT ## port ## _OUTSET = (1 << pin); } \
		ALWAYS_INLINE static void low() { PORT ## port ## _OUTCLR = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { PORT ## port ## _OUTTGL = (1 << pin); } \
		\
		ALWAYS_INLINE static void \
		write(bool status) { \
			if (status) { \
				high(); \
			} \
			else { \
				low(); \
			} \
		} \
		\
		ALWAYS_INLINE static bool read() { return (PORT ## port ## _IN & (1 << pin)); } \
		ALWAYS_INLINE static PORT_t& getPort() { return PORT ## port;} \
		ALWAYS_INLINE static uint8_t getMask() { return (1 << pin);} \
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
			setOutput(); \
			PORT ## port ## _PIN ## pin ## CTRL = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		\
		ALWAYS_INLINE static void setOutput() { PORT ## port ## _DIRSET = (1 << pin); } \
		ALWAYS_INLINE static void high() { PORT ## port ## _OUTSET = (1 << pin); } \
		ALWAYS_INLINE static void low() { PORT ## port ## _OUTCLR = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { PORT ## port ## _OUTTGL = (1 << pin); } \
		\
		ALWAYS_INLINE static void \
		write(bool status) { \
			if (status) { \
				high(); \
			} \
			else { \
				low(); \
			} \
		} \
		ALWAYS_INLINE static PORT_t& getPort() { return PORT ## port;} \
		ALWAYS_INLINE static uint8_t getMask() { return (1 << pin);} \
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
			setInput(); \
			PORT ## port ## _PIN ## pin ## CTRL = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		\
		ALWAYS_INLINE static void setInput() { PORT ## port ## _DIRCLR = (1 << pin); } \
		ALWAYS_INLINE static bool read() { return (PORT ## port ## _IN & (1 << pin)); } \
		ALWAYS_INLINE static void \
		configureInputSense(::xpcc::gpio::InputSense inputSense){\
			PORT ## port ## _PIN ## pin ## CTRL = (PORT ## port ## _PIN ## pin ## CTRL & ~PORT_ISC_gm) | inputSense; \
		}\
		\
		ALWAYS_INLINE static void \
		configureInterrupt0(::xpcc::gpio::InterruptLevel0 interruptLevel) { \
			if (interruptLevel){\
				PORT ## port ## _INT0MASK |= getMask();\
				PORT ## port ## _INTCTRL = (PORT ## port ## _INTCTRL & ~PORT_INT0LVL_gm) | (interruptLevel & PORT_INT0LVL_gm);\
			}\
			else{\
				PORT ## port ## _INT0MASK &= ~getMask();\
			}\
		} \
		\
		ALWAYS_INLINE static void \
		configureInterrupt1(::xpcc::gpio::InterruptLevel1 interruptLevel) { \
			if (interruptLevel){\
				PORT ## port ## _INT1MASK |= getMask();\
				PORT ## port ## _INTCTRL = (PORT ## port ## _INTCTRL & ~PORT_INT1LVL_gm) | (interruptLevel & PORT_INT1LVL_gm);\
			}\
			else{\
				PORT ## port ## _INT1MASK &= ~getMask();\
			}\
		} \
		\
		ALWAYS_INLINE static PORT_t& getPort() { return PORT ## port;} \
		ALWAYS_INLINE static uint8_t getMask() { return (1 << pin);} \
		ALWAYS_INLINE static uint16_t getInterrupt0Vector() { return PORT ## port ## _INT0_vect_num;} \
		ALWAYS_INLINE static uint16_t getInterrupt1Vector() { return PORT ## port ## _INT1_vect_num;} \
	}

// FIXME
//#define GPIO__NIBBLE_LOW(name, port)
//#define GPIO__NIBBLE_HIGH(name, port)

#endif // XPCC__ATXMEGA_GPIO_HPP
