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

#ifndef XPCC__UC3B_GPIO_HPP
#define XPCC__UC3B_GPIO_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/driver/gpio.hpp>
#include <xpcc/architecture/platform/avr32.hpp>

#include <xpcc/math/utils/bit_operation.hpp>

namespace xpcc
{
	namespace avr32
	{
		enum Configuration
		{
			NORMAL = 0,
			PULLUP = 1,
		};
	}
}

/**
 * \brief	Pin Macros without padding a padding 0. Ugly but \c #if is not possible in \c #define
 */
#define AVR32_PIN_PA1 AVR32_PIN_PA01
#define AVR32_PIN_PA2 AVR32_PIN_PA02
#define AVR32_PIN_PA3 AVR32_PIN_PA03
#define AVR32_PIN_PA4 AVR32_PIN_PA04
#define AVR32_PIN_PA5 AVR32_PIN_PA05
#define AVR32_PIN_PA6 AVR32_PIN_PA06
#define AVR32_PIN_PA7 AVR32_PIN_PA07
#define AVR32_PIN_PA8 AVR32_PIN_PA08
#define AVR32_PIN_PA9 AVR32_PIN_PA09

#define AVR32_PIN_PB1 AVR32_PIN_PB01
#define AVR32_PIN_PB2 AVR32_PIN_PB02
#define AVR32_PIN_PB3 AVR32_PIN_PB03
#define AVR32_PIN_PB4 AVR32_PIN_PB04
#define AVR32_PIN_PB5 AVR32_PIN_PB05
#define AVR32_PIN_PB6 AVR32_PIN_PB06
#define AVR32_PIN_PB7 AVR32_PIN_PB07
#define AVR32_PIN_PB8 AVR32_PIN_PB08
#define AVR32_PIN_PB9 AVR32_PIN_PB09


/**
 * \brief	Create a input/output pin type
 * 
 * \hideinitializer
 * \ingroup	atmega
 */
#if 1 == 0
#define	GPIO__IO(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void setInput() { CONCAT(DDR, port) &= ~(1 << pin); } \
		ALWAYS_INLINE static void \
		setInput(::xpcc::atmega::Configuration config) \
		{ \
			setInput(); \
			if (config == ::xpcc::atmega::PULLUP) { \
				CONCAT(PORT, port) |= (1 << pin); \
			} \
			else { \
				CONCAT(PORT, port) &= ~(1 << pin); \
			} \
		} \
		ALWAYS_INLINE static void setOutput() { CONCAT(DDR, port) |= (1 << pin); } \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void set()    { CONCAT(PORT, port) |=  (1 << pin); } \
		ALWAYS_INLINE static void reset()  { CONCAT(PORT, port) &= ~(1 << pin); } \
		ALWAYS_INLINE static void toggle() { CONCAT(PORT, port) ^=  (1 << pin); } \
		ALWAYS_INLINE static bool read()   { return (CONCAT(PIN, port) & (1 << pin)); } \
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
#endif

/**
 * \brief	Create a output pin type
 * 
 * \hideinitializer
 * \ingroup	uc3b
 */

#define	GPIO__OUTPUT(name, g_port, gpio_pin) \
	struct name { \
		ALWAYS_INLINE static void setOutput() { \
			volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[CONCAT3(AVR32_PIN_P, g_port, gpio_pin) >> 5]; \
			gpio_port->oders = 1 << (gpio_pin & 0x1F); /* The GPIO output driver is enabled for that pin. */ \
			gpio_port->gpers = 1 << (gpio_pin & 0x1F); /* The GPIO module controls that pin. */ \
		} \
		ALWAYS_INLINE static void \
		setOutput(bool status) { \
			set(status); \
			setOutput(); \
		} \
		ALWAYS_INLINE static void \
		set()     { \
			volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[CONCAT3(AVR32_PIN_P, g_port, gpio_pin) >> 5]; \
			gpio_port->ovrs  = 1 << (gpio_pin & 0x1F); /* Value to be driven on the I/O line: 1. */ \
		} \
		ALWAYS_INLINE static void \
		reset()   { \
			volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[CONCAT3(AVR32_PIN_P, g_port, gpio_pin) >> 5]; \
			gpio_port->ovrc  = 1 << (gpio_pin & 0x1F); /* Value to be driven on the I/O line: 0. */ \
		} \
		ALWAYS_INLINE static void \
		toggle()  { \
			volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[CONCAT3(AVR32_PIN_P, g_port, gpio_pin) >> 5]; \
			gpio_port->ovrt  = 1 << (gpio_pin & 0x1F); /* Toggle the I/O line. */ \
		} \
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

#if 1 == 0
#define	GPIO__OUTPUT(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void setOutput() { CONCAT(DDR, port) |= (1 << pin); } \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void set()    { CONCAT(PORT, port) |=  (1 << pin); } \
		ALWAYS_INLINE static void reset()  { CONCAT(PORT, port) &= ~(1 << pin); } \
		ALWAYS_INLINE static void toggle() { CONCAT(PORT, port) ^=  (1 << pin); } \
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
#endif

/**
 * \brief	Create a input type
 * 
 * read: PVR
 * config:
 *
 * \hideinitializer
 * \ingroup	atmega
 */
#define GPIO__INPUT(name, g_port, gpio_pin) \
	struct name { \
		ALWAYS_INLINE static void \
		setInput(::xpcc::avr32::Configuration config) { \
			setInput(); \
			volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[CONCAT3(AVR32_PIN_P, g_port, gpio_pin) >> 5]; \
			if (config == ::xpcc::avr32::PULLUP) { \
				gpio_port->puers = 1 << (gpio_pin & 0x1F); /* The pull-up is enabled for that pin. */ \
			} \
			else { \
				gpio_port->puerc = 1 << (gpio_pin & 0x1F); /* The pull-up is disabled for that pin. */ \
			} \
		} \
		ALWAYS_INLINE static void \
		setInput() { \
			volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[CONCAT3(AVR32_PIN_P, g_port, gpio_pin) >> 5]; \
			gpio_port->oderc = 1 << (gpio_pin & 0x1F); /* The GPIO output driver is disabled for that pin. */ \
			gpio_port->gpers = 1 << (gpio_pin & 0x1F); /* The GPIO module controls that pin. */ \
		} \
		ALWAYS_INLINE static bool \
		read() { \
			volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[CONCAT3(AVR32_PIN_P, g_port, gpio_pin) >> 5]; \
			return (gpio_port->pvr & (1 << (gpio_pin & 0x1f))); \
		} \
	}

/**
 * \brief	Connect the lower four bits to a nibble (P0..3)
 * 
 * \see		xpcc::gpio::Nibble()
 * 
 * \hideinitializer
 * \ingroup	atmega
 */
#if 1 == 0
#define GPIO__NIBBLE_LOW(name, port) \
	struct name { \
		ALWAYS_INLINE static void setOutput() { \
			CONCAT(DDR, port) |= 0x0f; \
		} \
		ALWAYS_INLINE static void setInput() { \
			CONCAT(DDR, port) &= ~0x0f; \
		} \
		ALWAYS_INLINE static uint8_t read() { \
			return (CONCAT(PIN, port) & 0x0f); \
		} \
		ALWAYS_INLINE static void write(uint8_t data) { \
			CONCAT(PORT, port) = (data & 0x0f) | (CONCAT(PORT, port) & 0xf0); \
		} \
	}
#endif

/**
 * \brief	Connect the higher four bits to a nibble (P4..7)
 * 
 * \see		xpcc::gpio::Nibble()
 * 
 * \hideinitializer
 * \ingroup	atmega
 */
#if 1 == 0
#define GPIO__NIBBLE_HIGH(name, port) \
	struct name { \
		ALWAYS_INLINE static void setOutput() { \
			CONCAT(DDR, port) |= 0xf0; \
		} \
		ALWAYS_INLINE static void setInput() { \
			CONCAT(DDR, port) &= ~0xf0; \
		} \
		ALWAYS_INLINE static uint8_t read() { \
			uint8_t data = CONCAT(PIN, port); \
			return (data >> 4); \
		} \
		ALWAYS_INLINE static void write(uint8_t data) { \
			data = ::xpcc::swap(data); \
			CONCAT(PORT, port) = (data & 0xf0) | (CONCAT(PORT, port) & 0x0f); \
		} \
	}
#endif

/**
 * \brief	Use a full 8-Bit port
 * 
 * \see		xpcc::gpio::Port()
 * 
 * \hideinitializer
 * \ingroup	atmega
 */
#if 1 == 0
#define GPIO__PORT(name, port) \
	struct name { \
		ALWAYS_INLINE static void setOutput() { \
			CONCAT(DDR, port) = 0xff; \
		} \
		ALWAYS_INLINE static void setInput() { \
			CONCAT(DDR, port) = 0; \
		} \
		ALWAYS_INLINE static uint8_t read() { \
			return CONCAT(PIN, port); \
		} \
		ALWAYS_INLINE static void write(uint8_t data) { \
			CONCAT(PORT, port) = data; \
		} \
	}
#endif

#endif	// XPCC__UC3B_GPIO_HPP
