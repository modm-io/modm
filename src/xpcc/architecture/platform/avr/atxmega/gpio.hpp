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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATXMEGA__GPIO_HPP
#define XPCC_ATXMEGA__GPIO_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/platform/avr.hpp>
#include <xpcc/architecture/driver/gpio.hpp>
#include <xpcc/math/utils/bit_operation.hpp>

namespace xpcc
{
	namespace atxmega
	{
		/**
		 * \ingroup	atxmega
		 */
		enum Configuration
		{
			NORMAL = PORT_OPC_TOTEM_gc,
			BUSKEEPER = PORT_OPC_BUSKEEPER_gc,
			PULLDOWN = PORT_OPC_PULLDOWN_gc,
			PULLUP = PORT_OPC_PULLUP_gc,
			WIREDDOR = PORT_OPC_WIREDOR_gc,
			WIREDAND = PORT_OPC_WIREDAND_gc,
			WIREDORPULL = PORT_OPC_WIREDORPULL_gc,
			WIREDANDPULL = PORT_OPC_WIREDANDPULL_gc,
			INVERTED = PORT_INVEN_bm
		}; // PINnCTRL
		
		/**
		 * \ingroup	atxmega
		 */
		enum InputSense
		{
			BOTHEDGES = PORT_ISC_BOTHEDGES_gc,
			RISING = PORT_ISC_RISING_gc,  
			FALLING = PORT_ISC_FALLING_gc, 
			LEVEL = PORT_ISC_LEVEL_gc,  
			INPUT_DISABLE = PORT_ISC_INPUT_DISABLE_gc,
		}; // PINnCTRL
		
		/**
		 * Be careful: There are only two Interrupts for a Port,
		 * so it is not possible to select different
		 * Interruptlevels for different Pins of one Port on the same Interrupt_x.
		 * Previous level settings will be overridden and be affected to all
		 * Pins on this Port for which interrupt_x has been enabled.
		 * 
		 * Switching level of interrupt_x off will not disable the interrupt, but
		 * clear the corresponding bit in the interrupt_x_mask.
		 * 
		 * \ingroup	atxmega
		 */
		enum InterruptLevel0
		{
			INT0LVL_OFF = PORT_INT0LVL_OFF_gc,
			INT0LVL_LO = PORT_INT0LVL_LO_gc,
			INT0LVL_MED = PORT_INT0LVL_MED_gc,
			INT0LVL_HI = PORT_INT0LVL_HI_gc,
		};
		
		/**
		 * \ingroup	atxmega
		 */
		enum InterruptLevel1
		{
			INT1LVL_OFF = PORT_INT1LVL_OFF_gc,
			INT1LVL_LO = PORT_INT1LVL_LO_gc,
			INT1LVL_MED = PORT_INT1LVL_MED_gc,
			INT1LVL_HI = PORT_INT1LVL_HI_gc,
		};
	}
}

/**
 * \brief	Create a input/output pin type
 * 
 * \hideinitializer
 * \ingroup	atxmega
 */
#define	GPIO__IO(name, port, pin) \
	struct name \
	{ \
		ALWAYS_INLINE static void setInput() { CONCAT3(PORT, port, _DIRCLR) = (1 << pin); } \
		ALWAYS_INLINE static void \
		setInput(::xpcc::atxmega::Configuration config, \
				  bool invert = false) { \
			setInput(); \
			CONCAT5(PORT, port, _PIN, pin, CTRL) = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		\
		ALWAYS_INLINE static void setOutput() { CONCAT3(PORT, port, _DIRSET) = (1 << pin); } \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void \
		setOutput(::xpcc::atxmega::Configuration config, \
				  bool invert = false) { \
			setOutput(); \
			CONCAT5(PORT, port, _PIN, pin, CTRL) = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		ALWAYS_INLINE static void set()    { CONCAT3(PORT, port, _OUTSET) = (1 << pin); } \
		ALWAYS_INLINE static void reset()  { CONCAT3(PORT, port, _OUTCLR) = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { CONCAT3(PORT, port, _OUTTGL) = (1 << pin); } \
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
		ALWAYS_INLINE static bool read() { return (CONCAT3(PORT, port, _IN) & (1 << pin)); } \
		ALWAYS_INLINE static PORT_t& getPort() { return CONCAT(PORT, port);} \
		ALWAYS_INLINE static uint8_t getMask() { return (1 << pin);} \
		ALWAYS_INLINE static uint8_t getEventChannelMuxInput() { \
			return (reinterpret_cast<uint16_t>(&CONCAT(PORT, port)) - 0x0600)/4 + 0x50 + pin;} \
	}

/**
 * \brief	Create a output pin type
 * 
 * \hideinitializer
 * \ingroup	atxmega
 */
#define	GPIO__OUTPUT(name, port, pin) \
	struct name \
	{ \
		ALWAYS_INLINE static void \
		setOutput(::xpcc::atxmega::Configuration config, \
				  bool invert = false) { \
			setOutput(); \
			CONCAT5(PORT, port, _PIN, pin, CTRL) = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		\
		ALWAYS_INLINE static void setOutput() { CONCAT3(PORT, port, _DIRSET) = (1 << pin); } \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void set()    { CONCAT3(PORT, port, _OUTSET) = (1 << pin); } \
		ALWAYS_INLINE static void reset()  { CONCAT3(PORT, port, _OUTCLR) = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { CONCAT3(PORT, port, _OUTTGL) = (1 << pin); } \
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
		ALWAYS_INLINE static PORT_t& getPort() { return CONCAT(PORT, port);} \
		ALWAYS_INLINE static uint8_t getMask() { return (1 << pin);} \
		ALWAYS_INLINE static uint8_t getEventChannelMuxInput() { \
			return (reinterpret_cast<uint16_t>(&CONCAT(PORT, port)) - 0x0600)/4 + 0x50 + pin;} \
	}

/**
 * \brief	Create a input pin type
 * 
 * \hideinitializer
 * \ingroup	atxmega
 */
#define GPIO__INPUT(name, port, pin) \
	struct name \
	{ \
		ALWAYS_INLINE static void \
		setInput(::xpcc::atxmega::Configuration config, \
				  bool invert = false) { \
			setInput(); \
			CONCAT5(PORT, port, _PIN, pin, CTRL) = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		\
		ALWAYS_INLINE static void setInput() { CONCAT3(PORT, port, _DIRCLR) = (1 << pin); } \
		ALWAYS_INLINE static bool read() { return (CONCAT3(PORT, port, _IN) & (1 << pin)); } \
		ALWAYS_INLINE static void \
		configureInputSense(::xpcc::atxmega::InputSense inputSense){\
			CONCAT5(PORT, port, _PIN, pin, CTRL) = (CONCAT5(PORT, port, _PIN, pin, CTRL) & ~PORT_ISC_gm) | inputSense; \
		}\
		\
		ALWAYS_INLINE static void \
		configureInterrupt0(::xpcc::atxmega::InterruptLevel interruptLevel) { \
			if (interruptLevel){\
				CONCAT3(PORT, port, _INT0MASK) |= getMask();\
				CONCAT3(PORT, port, _INTCTRL) = (CONCAT3(PORT, port, _INTCTRL) & ~PORT_INT0LVL_gm) | (interruptLevel & PORT_INT0LVL_gm);\
			}\
			else{\
				CONCAT3(PORT, port, _INT0MASK) &= ~getMask();\
			}\
		} \
		\
		ALWAYS_INLINE static void \
		configureInterrupt1(::xpcc::atxmega::InterruptLevel interruptLevel) { \
			if (interruptLevel){\
				CONCAT3(PORT, port, _INT1MASK) |= getMask();\
				CONCAT3(PORT, port, _INTCTRL) = (CONCAT3(PORT, port, _INTCTRL) & ~PORT_INT1LVL_gm) | ((interruptLevel << 2) & PORT_INT1LVL_gm);\
			}\
			else{\
				CONCAT3(PORT, port, _INT1MASK) &= ~getMask();\
			}\
		} \
		\
		ALWAYS_INLINE static PORT_t& getPort() { return CONCAT(PORT, port);} \
		ALWAYS_INLINE static uint8_t getMask() { return (1 << pin);} \
		ALWAYS_INLINE static uint8_t getEventChannelMuxInput() { \
			return (reinterpret_cast<uint16_t>(&CONCAT(PORT, port)) - 0x0600)/4 + 0x50 + pin;} \
		ALWAYS_INLINE static uint16_t getInterrupt0Vector() { return CONCAT3(PORT, port, _INT0_vect_num);} \
		ALWAYS_INLINE static uint16_t getInterrupt1Vector() { return CONCAT3(PORT, port, _INT1_vect_num);} \
	}

/**
 * \brief	Create a lower nibble P0..3
 * 
 * \hideinitializer
 * \ingroup	atxmega
 */
#define GPIO__NIBBLE_LOW(name, port) \
	struct name { \
		ALWAYS_INLINE static void setOutput() { \
			CONCAT3(PORT, port, _DIRSET) = 0x0f; \
		} \
		ALWAYS_INLINE static void setInput() { \
			CONCAT3(PORT, port, _DIRCLR) = 0x0f; \
		} \
		ALWAYS_INLINE static uint8_t read() { \
			return CONCAT3(PORT, port, _IN) & 0x0f; \
		} \
		ALWAYS_INLINE static void write(uint8_t data) { \
			CONCAT3(PORT, port, _OUTSET) =   data & 0x0f; \
			CONCAT3(PORT, port, _OUTCLR) = ~(data & 0x0f); \
		} \
	}

/**
 * \brief	Create a lower nibble P4..7
 * 
 * \hideinitializer
 * \ingroup	atxmega
 */
#define GPIO__NIBBLE_HIGH(name, port) \
	struct name { \
		ALWAYS_INLINE static void setOutput() { \
			CONCAT3(PORT, port, _DIRSET) = 0xf0; \
		} \
		ALWAYS_INLINE static void setInput() { \
			CONCAT3(PORT, port, _DIRCLR) = 0xf0; \
		} \
		ALWAYS_INLINE static uint8_t read() { \
			return (CONCAT3(PORT, port, _IN) >> 4); \
		} \
		ALWAYS_INLINE static void write(uint8_t data) { \
			data = ::xpcc::swap(data); \
			CONCAT3(PORT, port, _OUTSET) =   data & 0xf0; \
			CONCAT3(PORT, port, _OUTCLR) = ~(data & 0xf0); \
		} \
	}

/**
 * \brief	Use a full 8-Bit port
 * 
 * \see		xpcc::gpio::Port()
 * 
 * \hideinitializer
 * \ingroup	atxmega
 */
#define GPIO__PORT(name, port) \
	struct name { \
		ALWAYS_INLINE static void setOutput() { \
			CONCAT3(PORT, port, _DIRSET) = 0xff; \
		} \
		ALWAYS_INLINE static void setInput() { \
			CONCAT3(PORT, port, _DIRCLR) = 0xff; \
		} \
		ALWAYS_INLINE static void setInput(::xpcc::atxmega::Configuration config, \
				  bool invert = false) { \
			CONCAT3(PORT, port, _DIRCLR) = 0xff; \
			PORTCFG.MPCMASK = 0xff; \
			CONCAT3(PORT, port, _PIN0CTRL) = config | ((invert) ? PORT_INVEN_bm : 0); \
		} \
		ALWAYS_INLINE static uint8_t read() { \
			return CONCAT3(PORT, port, _IN); \
		} \
		ALWAYS_INLINE static void write(uint8_t data) { \
			CONCAT3(PORT, port, _OUT) = data; \
		} \
	}

#endif	// XPCC_ATXMEGA__GPIO_HPP
