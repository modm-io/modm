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

#ifndef XPCC_STM32__USART_6_HPP
#define XPCC_STM32__USART_6_HPP

#include <stdint.h>
#include "uart_base.hpp"
#include "../device.h"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Universal synchronous/asynchronous receiver
		 * 				transmitter (USART6)
		 * 
		 * Simple unbuffered implementation.
		 * 
		 * @ingroup		stm32
		 */
		class Usart6 : public UartBase
		{
		public:
			Usart6(uint32_t baudrate)
			{
				setBaudrate(baudrate);
			}
			
			enum Mapping
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				REMAP_PC6_PC7,		///< TX mapped to PC6, RX mapped to PC7 (CK/PC8)
				REMAP_PG14_PG9,		///< TX/PG14, RX/PG9, CK/PD10, CTS/PG13,PG15, RTS/PG8,PG12
#else
				
#endif
			};
			
			/**
			 * Configure the IO Pins for Usart6
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
		 * Just add add a value for USART6_TX_BUFFER_SIZE and/or 
		 * USART6_RX_BUFFER_SIZE in the \c defines section. The size can
		 * be any value between 1 and 254.
		 * 
		 * Example:
		 * \code
		 * [defines]
		 * USART6_TX_BUFFER_SIZE = 20
		 * USART6_RX_BUFFER_SIZE = 30
		 * \endcode
		 * 
		 * \ingroup	stm32
		 */
		class BufferedUsart6 : public UartBase
		{
		public:
			BufferedUsart6(uint32_t baudrate)
			{
				setBaudrate(baudrate);
			}
			
			enum Mapping
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				REMAP_PC6_PC7,		///< TX mapped to PC6, RX mapped to PC7 (CK/PC8)
				REMAP_PG14_PG9,		///< TX/PG14, RX/PG9, CK/PD10, CTS/PG13,PG15, RTS/PG8,PG12
#else
				
#endif
			};
			
			/**
			 * Configure the IO Pins for Usart6
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
	}
}

#endif // XPCC_STM32__USART_6_HPP