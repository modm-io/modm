// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__UARTHAL_5_HPP
#define XPCC_STM32__UARTHAL_5_HPP

#include <stdint.h>
#include "uart_hal_base.hpp"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Universal asynchronous receiver transmitter (UARTHAL5)
		 * 
		 * Not available on the low- and medium density devices.
		 * 
		 * Very basic implementation that exposes more hardware features than
		 * the regular Usart classes.
		 * 
		 * @ingroup		stm32
		 */
		class UartHal5 : public UartHalBase
		{
		public:

			/*
			 * Enables the clock, resets the hardware and sets the UE bit
			 */
			static void
			enable();

			/*
			 * Disables the hw module (by disabling its clock line)
			 */
			static void
			disable();

			/*
			 * Initialize Uart HAL Peripheral
			 *
			 * Enables clocks, the UART peripheral (but neither TX nor RX)
			 * Sets baudrate and parity.
			 */
			static void
			initialize(uint32_t baudrate, Parity parity = Parity::Disabled)
			{
				enable();
				// DIRTY HACK: disable and reenable uart to be able to set
				//             baud rate as well as parity
				UART5->CR1 &= ~USART_CR1_UE;	// Uart Disable
				setBaudrate(baudrate);
				setParity(parity);
				UART5->CR1 |=  USART_CR1_UE;	// Uart Reenable
			}

			enum Mapping
			{
				REMAP_PC12_PD2,		///< TX mapped to PC12, RX mapped to PD2
			};

			/**
			 * Configure the IO Pins for UartHal5
			 */
			static void
			configurePins(Mapping mapping);

		private:	// This methods can only be executed while UART is
					// disabled.
					// At the moment this is only the case in the initialize
					// method.
			/*
			 * Set Baudrate
			 *
			 * Remember to enable the peripheral before setting the baudrate
			 */
			static void
			setBaudrate(uint32_t baudrate);

			/*
			 * Disable Parity or Enable Odd/Even Parity
			 *
			 * This method assumes 8 databits + 1 parity bit
			 */
			static inline void
			setParity(const Parity parity)
			{
				uint32_t flags = UART5->CR1;
				flags &= ~(USART_CR1_PCE | USART_CR1_PS | USART_CR1_M);
				flags |= static_cast<uint32_t>(parity);
				if (parity != Parity::Disabled) {
					// Parity Bit counts as 9th bit -> enable 9 data bits
					flags |= USART_CR1_M;
				}
				UART5->CR1 = flags;
			}

		public:

			/*
			 * \brief	Write a single byte to the transmit register
			 *
			 * WARNING: this method does NOT do any sanity checks!!
			 * It is your responsibility to check if the register
			 * is empty!
			 */
			static inline void
			write(uint8_t data)
			{
			#if defined(STM32F3XX)
				UART5->TDR = data;
			#elif defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F4XX)
				UART5->DR = data;
			#else
				#error "This file is only for STM32F{1, 2, 3, 4}"
			#endif
			}

			/**
			 * \brief	Returns the value of the receive register
			 *
			 * WARNING: this method does NOT do any sanity checks!!
			 * It is your responsibility to check if the register
			 * contains something useful!
			 */
			static inline uint8_t
			read()
			{
			#if defined(STM32F3XX)
				return UART5->RDR;
			#elif defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F4XX)
				return UART5->DR;
			#else
				#error "This file is only for STM32F{1, 2, 3, 4}"
			#endif
			}



			/*
			 * Enable/Disable Transmitter
			 */
			static inline void
			setTransmitterEnable(const bool enable)
			{
				if (enable) {
					UART5->CR1 |=  USART_CR1_TE;
				} else {
					UART5->CR1 &= ~USART_CR1_TE;
				}
			}

			/*
			 * Enable/Disable Receiver
			 */
			static inline void
			setReceiverEnable(bool enable)
			{
				if (enable) {
					UART5->CR1 |=  USART_CR1_RE;
				} else {
					UART5->CR1 &= ~USART_CR1_RE;
				}
			}

			/*
			 * Returns true if data has been received
			 */
			static inline bool
			isReceiveRegisterNotEmpty()
			{
			#if defined(STM32F3XX)
				return UART5->ISR & USART_ISR_RXNE;
			#elif defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F4XX)
				return UART5->SR & USART_SR_RXNE;
			#else
				#error "This file is only for STM32F{1, 2, 3, 4}"
			#endif
			}

			/*
			 * Returns true if data can be written
			 */
			static inline bool
			isTransmitRegisterEmpty()
			{
			#if defined(STM32F3XX)
				return UART5->ISR & USART_ISR_TXE;
			#elif defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F4XX)
				return UART5->SR & USART_SR_TXE;
			#else
				#error "This file is only for STM32F{1, 2, 3, 4}"
			#endif
			}

			static void
			enableInterruptVector(bool enable, uint32_t priority);

			static inline void
			enableInterrupt(Interrupt interrupt)
			{
				UART5->CR1 |= interrupt;
			}

			static inline void
			disableInterrupt(Interrupt interrupt)
			{
				UART5->CR1 &= ~interrupt;
			}

			static inline InterruptFlag
			getInterruptFlags()
			{
			#if defined(STM32F3XX)		
				return static_cast<InterruptFlag>( UART5->ISR );
			#elif defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F4XX)
				return static_cast<InterruptFlag>( UART5->SR );
			#else
				#error "This file is only for STM32F{1, 2, 3, 4}"
			#endif			
			}

			static inline ErrorFlag
			getErrorFlags()
			{
			#if defined(STM32F3XX)		
				return static_cast<ErrorFlag>( UART5->ISR );
			#elif defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F4XX)
				return static_cast<ErrorFlag>( UART5->SR );
			#else
				#error "This file is only for STM32F{1, 2, 3, 4}"
			#endif			
			}

			static inline void
			resetInterruptFlags(InterruptFlag flags)
			{
			#if defined(STM32F3XX)					
				// Not all flags can be cleared by writing to this reg
				const uint32_t mask = USART_ICR_PECF  | USART_ICR_FECF   |
					USART_ICR_NCF   | USART_ICR_ORECF | USART_ICR_IDLECF |
					USART_ICR_TCCF  | USART_ICR_LBDCF | USART_ICR_CTSCF  |
					USART_ICR_RTOCF | USART_ICR_EOBCF | USART_ICR_CMCF   |
					USART_ICR_WUCF;
				// Flags are cleared by writing a one to the flag position.
				// Writing a zero is (hopefully) ignored.
				UART5->ICR = (flags & mask);
			#elif defined(STM32F10X) || defined(STM32F2XX) || defined(STM32F4XX)
				(void) flags;
				#warning resetInterruptFlags Not yet implemented
			#else
				#error "This file is only for STM32F{1, 2, 3, 4}"
			#endif			
			}
		};
	}
}

#endif // XPCC_STM32__UARTHAL_5_HPP