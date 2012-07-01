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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__USART_1_HPP
#define XPCC_STM32__USART_1_HPP

#include <stdint.h>
#include "uart_base.hpp"
#include "../device.h"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Universal synchronous/asynchronous receiver
		 * 				transmitter (USART1)
		 * 
		 * Simple unbuffered implementation.
		 * 
		 * @ingroup		stm32
		 */
		class Usart1 : public UartBase
		{
		public:
			Usart1(uint32_t baudrate)
			{
				setBaudrate(baudrate);
			}
			
			enum Mapping
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				REMAP_PA9_PA10,		///< TX mapped to PA9, RX mapped to PA10 (CTS/PA11, RTS/PA12, CK/PA8)
				REMAP_PB6_PB7,		///< TX mapped to PB6, RX mapped to PB7
#else
				REMAP_PA9_PA10 = 0,						///< TX mapped to PA9, RX mapped to PA10
				REMAP_PB6_PB7 = AFIO_MAPR_USART1_REMAP,	///< TX mapped to PB6, RX mapped to PB7
#endif
			};
			
			/**
			 * Configure the IO Pins for Usart1
			 */
			static void
			configurePins(Mapping mapping);
			
			/**
			 * \brief	Set baudrate
			 * \param	baudrate	desired baud rate
			 */
			static void
			setBaudrate(uint32_t baudrate);
			
			/**
			 * \brief	Send a single byte
			 */
			static void
			write(uint8_t data);
			
			/**
			 * \brief	Write a block of bytes
			 *
			 * \param	*buffer	Pointer to a buffer
			 * \param	n	Number of bytes to be read
			 */
			static void
			write(const uint8_t *buffer, uint8_t n);
			
			/**
			 * \brief	Read a single byte
			 */
			static bool
			read(uint8_t& c);
			
			/**
			 * \brief	Read a block of bytes
			 * 
			 * \param	*buffer	Pointer to a buffer big enough to storage \a n bytes
			 * \param	n	Number of bytes to be read
			 * 
			 * \return	Number of bytes which could be read, maximal \a n
			 */
			static uint8_t
			read(uint8_t *buffer, uint8_t n);
		};
		
		
		// --------------------------------------------------------------------
		/**
		 * \brief	Buffered Usart
		 * 
		 * This implementation uses a ringbuffer. The size of the ringbuffer
		 * can be changed in the \c project.cfg file.
		 * 
		 * Just add add a value for USART1_TX_BUFFER_SIZE and/or 
		 * USART1_RX_BUFFER_SIZE in the \c defines section. The size can
		 * be any value between 1 and 254.
		 * 
		 * Example:
		 * \code
		 * [defines]
		 * USART1_TX_BUFFER_SIZE = 20
		 * USART1_RX_BUFFER_SIZE = 30
		 * \endcode
		 * 
		 * \ingroup	stm32
		 */
		class BufferedUsart1 : public UartBase
		{
		public:
			/**
			 * Set baudrate.
			 * 
			 * \param	baudrate	Desired baud rate (e.g. 115200)
			 * \param	interruptPriority
			 * 			Interrupt vector priority (0=highest to 15=lowest)
			 * \param	blocking
			 * 			The write-function waits until a free slot is available
			 * 			in the send buffer.
			 */
			BufferedUsart1(uint32_t baudrate,
					uint32_t interruptPriority, bool blocking = true)
			{
				setBaudrate(baudrate, interruptPriority, blocking);
			}
			
			enum Mapping
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				REMAP_PA9_PA10,		///< TX mapped to PA9, RX mapped to PA10 (CTS/PA11, RTS/PA12, CK/PA8)
				REMAP_PB6_PB7,		///< TX mapped to PB6, RX mapped to PB7
#else
				REMAP_PA9_PA10 = 0,						///< TX mapped to PA9, RX mapped to PA10
				REMAP_PB6_PB7 = AFIO_MAPR_USART1_REMAP,	///< TX mapped to PB6, RX mapped to PB7
#endif
			};
			
			/**
			 * Configure the IO Pins for Usart1
			 */
			static void
			configurePins(Mapping mapping);
			
			/**
			 * Set baudrate.
			 * 
			 * \param	baudrate	Desired baud rate (e.g. 115200)
			 * \param	interruptPriority
			 * 			Interrupt vector priority (0=highest to 15=lowest)
			 * \param	blocking
			 * 			The write-function waits until a free slot is available
			 * 			in the send buffer.
			 */
			static void
			setBaudrate(uint32_t baudrate, uint32_t interruptPriority,
					bool blocking = true);
			
			/**
			 * \brief	Send a single byte
			 */
			static void
			write(uint8_t data);
			
			/**
			 * \brief	Write a block of bytes
			 *
			 * \param	*buffer	Pointer to a buffer
			 * \param	n	Number of bytes to be read
			 */
			static void
			write(const uint8_t *buffer, uint8_t n);
			
			/**
			 * \brief	Read a single byte
			 */
			static bool
			read(uint8_t& c);
			
			/**
			 * \brief	Read a block of bytes
			 * 
			 * \param	*buffer	Pointer to a buffer big enough to storage \a n bytes
			 * \param	n	Number of bytes to be read
			 * 
			 * \return	Number of bytes which could be read, maximal \a n
			 */
			static uint8_t
			read(uint8_t *buffer, uint8_t n);
			
			/**
			 * \brief	Empty the receive FIFO queue and USART buffer.
			 *
			 * \return	the size of the deleted FIFO queue.
			 */
			static uint8_t
			flushReceiveBuffer();
			
//			static uint8_t
//			flushTransmitBuffer();
		};

		// --------------------------------------------------------------------
		/**
		 * \brief	BufferedFlow Usart
		 * 
		 * Buffered UARAT with Hardware Flow Control. 
		 * 
		 * This implementation uses a ringbuffer. The size of the ringbuffer
		 * can be changed in the \c project.cfg file.
		 * 
		 * Just add add a value for USART1_TX_BUFFER_SIZE and/or 
		 * USART1_RX_BUFFER_SIZE in the \c defines section. The size can
		 * be any value between 1 and 254.
		 * 
		 * Example:
		 * \code
		 * [defines]
		 * USART1_TX_BUFFER_SIZE = 20
		 * USART1_RX_BUFFER_SIZE = 30
		 * \endcode
		 * 
		 * \ingroup	stm32
		 */
		class BufferedFlowUsart1 : public UartBase
		{
		public:
			/**
			 * Set baudrate.
			 * 
			 * \param	baudrate	Desired baud rate (e.g. 115200)
			 * \param	interruptPriority
			 * 			Interrupt vector priority (0=highest to 15=lowest)
			 * \param	blocking
			 * 			The write-function waits until a free slot is available
			 * 			in the send buffer.
			 */
			BufferedFlowUsart1(uint32_t baudrate,
					uint32_t interruptPriority, bool blocking = true)
			{
				setBaudrate(baudrate, interruptPriority, blocking);
			}
			
			enum Mapping
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				REMAP_PA9_PA10,		///< TX mapped to PA9, RX mapped to PA10 (CTS/PA11, RTS/PA12, CK/PA8)
				REMAP_PB6_PB7,		///< TX mapped to PB6, RX mapped to PB7
#else
				REMAP_PA9_PA10 = 0,						///< TX mapped to PA9, RX mapped to PA10
				REMAP_PB6_PB7 = AFIO_MAPR_USART1_REMAP,	///< TX mapped to PB6, RX mapped to PB7
#endif
			};
			
			/**
			 * Configure the IO Pins for Usart1
			 */
			static void
			configurePins(Mapping mapping);
			
			/**
			 * Set baudrate.
			 * 
			 * \param	baudrate	Desired baud rate (e.g. 115200)
			 * \param	interruptPriority
			 * 			Interrupt vector priority (0=highest to 15=lowest)
			 * \param	blocking
			 * 			The write-function waits until a free slot is available
			 * 			in the send buffer.
			 */
			static void
			setBaudrate(uint32_t baudrate, uint32_t interruptPriority,
					bool blocking = true);
			
			/**
			 * \brief	Send a single byte
			 */
			static void
			write(uint8_t data);
			
			/**
			 * \brief	Write a block of bytes
			 *
			 * \param	*buffer	Pointer to a buffer
			 * \param	n	Number of bytes to be read
			 */
			static void
			write(const uint8_t *buffer, uint8_t n);
			
			/**
			 * \brief	Read a single byte
			 */
			static bool
			read(uint8_t& c);
			
			/**
			 * \brief	Read a block of bytes
			 * 
			 * \param	*buffer	Pointer to a buffer big enough to storage \a n bytes
			 * \param	n	Number of bytes to be read
			 * 
			 * \return	Number of bytes which could be read, maximal \a n
			 */
			static uint8_t
			read(uint8_t *buffer, uint8_t n);
			
			/**
			 * \brief	Empty the receive FIFO queue and USART buffer.
			 *
			 * \return	the size of the deleted FIFO queue.
			 */
			static uint8_t
			flushReceiveBuffer();
			
//			static uint8_t
//			flushTransmitBuffer();
		};

		
		
#if defined(STM32F2XX) || defined(STM32F4XX)
		// --------------------------------------------------------------------
		/**
		 * \brief		USART1 in SPI master mode
		 * 
		 * FIXME currently not working!
		 * 
		 * \ingroup		stm32
		 */
		class UsartSpi1 : public UartBase
		{
		private:
			// TX mapped to PA9, RX mapped to PA10 (CTS/PA11, RTS/PA12, CK/PA8)
			// TX mapped to PB6, RX mapped to PB7
			GPIO__OUTPUT(TxdA9, A, 9);
			GPIO__INPUT(RxdA10, A, 10);
			GPIO__OUTPUT(CkA8, A, 8);
			
			GPIO__OUTPUT(TxdB6, B, 6);
			GPIO__INPUT(RxdB7, B, 7);
			
		public:
			enum Mode
			{
				MODE_0 = 0,
				MODE_1 = USART_CR2_CPHA,
				MODE_2 = USART_CR2_CPOL,
				MODE_3 = USART_CR2_CPOL | USART_CR2_CPHA,
			};
			
			enum MappingTx
			{
				REMAP_PA9,
				REMAP_PB6,
			};
			
			enum MappingRx
			{
				REMAP_PA10,
				REMAP_PB7,
			};
			
			enum MappingCk
			{
				REMAP_PA8,
			};
			
			ALWAYS_INLINE void
			configureTxPin(MappingTx mapping)
			{
				switch (mapping) {
					case REMAP_PA9:  TxdA9::setAlternateFunction(AF_USART1, xpcc::stm32::PUSH_PULL); break;
					case REMAP_PB6:  TxdB6::setAlternateFunction(AF_USART1, xpcc::stm32::PUSH_PULL); break;
				}
			}
			
			ALWAYS_INLINE void
			configureRxPin(MappingRx mapping)
			{
				switch (mapping) {
					case REMAP_PA10: RxdA10::setAlternateFunction(AF_USART1); break;
					case REMAP_PB7:  RxdB7::setAlternateFunction(AF_USART1); break;
				}
			}
			
			ALWAYS_INLINE void
			configureCkPin(MappingCk mapping)
			{
				switch (mapping) {
					case REMAP_PA8:  CkA8::setAlternateFunction(AF_USART1, xpcc::stm32::PUSH_PULL); break;
				}
			}
			
			/**
			 * Constructor
			 */
			UsartSpi1(uint32_t bitrate, Mode mode = MODE_0);
			
			/**
			 * Transfer byte.
			 */
			static uint8_t
			write(uint8_t data);
		};
#endif
	}
}

#endif // XPCC_STM32__USART_1_HPP