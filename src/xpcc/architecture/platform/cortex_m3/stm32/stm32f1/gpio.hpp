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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32F1__GPIO_HPP
#define XPCC_STM32F1__GPIO_HPP

#include <xpcc/architecture/driver/gpio.hpp>

#if defined LIBMAPLE
#include <libmaple/gpio.h>

typedef gpio_reg_map GPIO_TypeDef;

#else
#include <stm32f10x.h>

#define	GPIOA_BASE_ADDR GPIOA_BASE
#define	GPIOB_BASE_ADDR GPIOB_BASE
#define	GPIOC_BASE_ADDR GPIOC_BASE
#define	GPIOD_BASE_ADDR GPIOD_BASE
#define	GPIOE_BASE_ADDR GPIOE_BASE
#define	GPIOF_BASE_ADDR GPIOF_BASE
#define	GPIOG_BASE_ADDR GPIOG_BASE
#endif

#define	GPIO_REG(r)		reinterpret_cast<GPIO_TypeDef *>(r)

/*
 * Each port bit of the General Purpose IO (GPIO) Ports, can be
 * individually configured by software in several modes:
 * - Input floating
 * - Input pull-up
 * - Input-pull-down
 * - Analog
 * - Output open-drain
 * - Output push-pull
 * - Alternate function push-pull
 * - Alternate function open-drain
 */
namespace xpcc
{
	namespace stm32
	{
		/**
		 * \ingroup	stm32
		 */
		enum InputType
		{
			FLOATING = 0x4,
			PULLUP = 0x9,
			PULLDOWN = 0x8,
		};
		
		/**
		 * \ingroup	stm32
		 */
		enum OutputType
		{
			PUSH_PULL = 0x0,
			OPEN_DRAIN = 0x4,
		};
		
		/**
		 * \ingroup	stm32
		 */
		enum OutputSpeed
		{
			SPEED_2MHZ = 0x2,
			SPEED_10MHZ = 0x1,
			SPEED_50MHZ = 0x3,
		};
		
		/** \internal */
		template<unsigned int P, unsigned char N, bool = (N >= 8)>
		struct GpioMode {
			ALWAYS_INLINE static void setMode(uint32_t m) {
				uint32_t tmp = reinterpret_cast<GPIO_TypeDef*>(P)->CRH;
				tmp &= ~(0xf << ((N - 8) * 4));
				tmp |= (m << ((N - 8) * 4));
				reinterpret_cast<GPIO_TypeDef*>(P)->CRH = tmp;
			}
		};
		
		/** \internal */
		template<unsigned int P, unsigned char N>
		struct GpioMode<P, N, false> {
			ALWAYS_INLINE static void setMode(uint32_t m) {
				uint32_t tmp = reinterpret_cast<GPIO_TypeDef*>(P)->CRL;
				tmp &= ~(0xf << (N * 4));
				tmp |= m << (N * 4);
				reinterpret_cast<GPIO_TypeDef*>(P)->CRL = tmp;
			}
		};
	}
}

/**
 * \ingroup	stm32
 * \brief	Create a input/output pin type
 * 
 * \hideinitializer
 */
#define	GPIO__IO(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void \
		setOutput(::xpcc::stm32::OutputType type = ::xpcc::stm32::PUSH_PULL, \
				::xpcc::stm32::OutputSpeed speed = ::xpcc::stm32::SPEED_50MHZ) { \
			uint32_t config = 0x0 | type | speed; \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE_ADDR, pin>::setMode(config); \
		} \
		ALWAYS_INLINE static void \
		setAlternateFunction(::xpcc::stm32::OutputType type = ::xpcc::stm32::PUSH_PULL, \
				::xpcc::stm32::OutputSpeed speed = ::xpcc::stm32::SPEED_50MHZ) { \
			uint32_t config = 0x8 | type | speed; \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE_ADDR, pin>::setMode(config); \
		} \
		ALWAYS_INLINE static void \
		setInput(::xpcc::stm32::InputType type = ::xpcc::stm32::FLOATING) { \
			if (type == ::xpcc::stm32::PULLUP) { \
				GPIO_REG(GPIO ## port ## _BASE_ADDR)->BSRR = (1 << pin); \
			} else if (type == ::xpcc::stm32::PULLDOWN) { \
				GPIO_REG(GPIO ## port ## _BASE_ADDR)->BRR = (1 << pin); \
			} \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE_ADDR, pin>::setMode(type & 0xc); \
		} \
		ALWAYS_INLINE static void \
		setAnalogInput() { \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE_ADDR, pin>::setMode(0); \
		} \
		ALWAYS_INLINE static void set() { GPIO_REG(GPIO ## port ## _BASE_ADDR)->BSRR = (1 << pin); } \
		ALWAYS_INLINE static void reset() { GPIO_REG(GPIO ## port ## _BASE_ADDR)->BRR = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { \
			if (GPIO_REG(GPIO ## port ## _BASE_ADDR)->IDR & (1 << pin)) { reset(); } else { set(); } } \
		ALWAYS_INLINE static bool read() { return (GPIO_REG(GPIO ## port ## _BASE_ADDR)->IDR & (1 << pin)); } \
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
 * Examples:
 * \code
 * GPIO__OUTPUT(Led, C, 12);
 * 
 * Led::setOutput();
 * Led::setOutput(xpcc::stm32::PUSH_PULL);
 * Led::setOutput(xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_10MHZ);
 * Led::setOutput(xpcc::stm32::OPEN_DRAIN);
 * 
 * Led::setAlternateFunction(xpcc::stm32::PUSH_PULL); 
 * Led::setAlternateFunction(xpcc::stm32::OPEN_DRAIN);
 * 
 * Led::set();
 * Led::reset();
 * 
 * Led::toggle();
 * \endcode
 * 
 * \hideinitializer
 * \ingroup	stm32
 */
#define	GPIO__OUTPUT(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void \
		setOutput(::xpcc::stm32::OutputType type = ::xpcc::stm32::PUSH_PULL, \
				::xpcc::stm32::OutputSpeed speed = ::xpcc::stm32::SPEED_50MHZ) { \
			uint32_t config = 0x0 | type | speed; \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE_ADDR, pin>::setMode(config); \
		} \
		ALWAYS_INLINE static void \
		setAlternateFunction(::xpcc::stm32::OutputType type = ::xpcc::stm32::PUSH_PULL, \
				::xpcc::stm32::OutputSpeed speed = ::xpcc::stm32::SPEED_50MHZ) { \
			uint32_t config = 0x8 | type | speed; \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE_ADDR, pin>::setMode(config); \
		} \
		ALWAYS_INLINE static void set() { GPIO_REG(GPIO ## port ## _BASE_ADDR)->BSRR = (1 << pin); } \
		ALWAYS_INLINE static void reset() { GPIO_REG(GPIO ## port ## _BASE_ADDR)->BRR = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { \
			if (GPIO_REG(GPIO ## port ## _BASE_ADDR)->IDR & (1 << pin)) { reset(); } else { set(); } } \
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
 * Examples:
 * \code
 * GPIO__INPUT(Button, A, 0);
 * 
 * Button::setInput();
 * Button::setInput(xpcc::stm32::PULLUP);
 * Button::setInput(xpcc::stm32::PULLDOWN);
 * Button::setAnalogInput();
 * 
 * if (Button::read()) {
 *     ...
 * }
 * \endcode
 * 
 * \hideinitializer
 * \ingroup	stm32
 */
#define GPIO__INPUT(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void \
		setInput(::xpcc::stm32::InputType type = ::xpcc::stm32::FLOATING) { \
			if (type == ::xpcc::stm32::PULLUP) { \
				GPIO_REG(GPIO ## port ## _BASE_ADDR)->BSRR = (1 << pin); \
			} else if (type == ::xpcc::stm32::PULLDOWN) { \
				GPIO_REG(GPIO ## port ## _BASE_ADDR)->BRR = (1 << pin); \
			} \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE_ADDR, pin>::setMode(type & 0xc); \
		} \
		ALWAYS_INLINE static void \
		setAnalogInput() { \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE_ADDR, pin>::setMode(0); \
		} \
		ALWAYS_INLINE static bool read() { return (GPIO_REG(GPIO ## port ## _BASE_ADDR)->IDR & (1 << pin)); } \
	}

#endif // XPCC_STM32F1__GPIO_HPP
