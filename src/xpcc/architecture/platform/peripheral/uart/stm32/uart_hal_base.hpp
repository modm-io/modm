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

#ifndef XPCC_STM32__UART_HAL_BASE_HPP
#define XPCC_STM32__UART_HAL_BASE_HPP

#include <stdint.h>
#include "uart_base.hpp"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Base class for the UART Hal classes
		 * 
		 * Provides some common enum that do not depend on the specific UART.
		 * 
		 * @internal
		 * @ingroup		stm32
		 */
		class UartHalBase : public UartBase /* for calculateBaudrateSettings */
		{
		public:
			enum Interrupt : uint32_t
			{
			#if defined(STM32F3XX)
				INTERRUPT_CHARACTER_MATCH = USART_CR1_CMIE,		/**< A USART interrupt is generated when match character is received. */
			#endif
				INTERRUPT_TX_EMPTY		  = USART_CR1_TXEIE,	/**< Call interrupt when transmit register is empty (i.e. the byte 
																 *   has been transfered to the shift out register */
				INTERRUPT_TX_COMPLETE	  = USART_CR1_TCIE,		/**< Called when the byte was completely transmitted */
				INTERRUPT_RX_NOT_EMPTY	  = USART_CR1_RXNEIE,	/**< Call interrupt when char received (RXNE) or overrun occurred (ORE) */
				INTERRUPT_PARITY_ERROR	  = USART_CR1_PEIE,		/**< Call interrupt when a parity error occurred. */
			};

			enum InterruptFlag : uint32_t
			{
			#if defined(STM32F3XX)
				FLAG_CHARACTER_MATCH	= USART_ISR_CMF,
				FLAG_TX_EMPTY			= USART_ISR_TXE,
				FLAG_TX_COMPLETE		= USART_ISR_TC,
				FLAG_RX_NOT_EMPTY		= USART_ISR_RXNE,
				FLAG_PARITY_ERROR		= USART_ISR_PE,
				FLAG_OVERRUN_ERROR		= USART_ISR_ORE,
			#elif defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F4XX)
				FLAG_TX_EMPTY			= USART_SR_TXE,			/**< Set if the transmit data register is empty. */
				FLAG_TX_COMPLETE		= USART_SR_TC,			/**< Set if the transmission is complete. */
				FLAG_RX_NOT_EMPTY		= USART_SR_RXNE,		/**< Set if the receive data register is not empty. */
				FLAG_PARITY_ERROR		= USART_SR_PE,			/**< Set if a parity error was detected. */
				FLAG_FRAMING_ERROR		= USART_SR_FE,			/**< Set if a framing error was detected. */
				FLAG_OVERRUN_ERROR		= USART_SR_ORE,			/**< Set if receive register was not cleared. */
			#else
				#error "This file is only for STM32F{1, 2, 3, 4}"
			#endif
			};

			enum class Parity : uint32_t
			{
				Disabled 	= 0,
				Even 		= USART_CR1_PCE,
				Odd  		= USART_CR1_PCE | USART_CR1_PS,
			};

			enum ErrorFlag : uint32_t
			{
			#if defined(STM32F3XX)
				ERROR_OVERRUN 	= USART_ISR_ORE,		/**< Set if a de-synchronization, excessive noise or a break character is detected. */
				ERROR_FRAMING 	= USART_ISR_FE,
				ERROR_PARITY 	= USART_ISR_PE,
			#elif defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F4XX)
				ERROR_OVERRUN 	= USART_SR_ORE,			/**< Set if a de-synchronization, excessive noise or a break character is detected. */
				ERROR_FRAMING 	= USART_SR_FE,
				ERROR_PARITY 	= USART_SR_PE,				
			#else
				#error "This file is only for STM32F{1, 2, 3, 4}"
			#endif
			};
		};
	}
}

#endif // XPCC_STM32__UART_HAL_BASE_HPP
