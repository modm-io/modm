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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATXMEGA__UART_C1_HPP
#define XPCC_ATXMEGA__UART_C1_HPP

#include <stdint.h>
#include <xpcc/architecture/utils.hpp>

namespace xpcc
{
	namespace atxmega
	{
		/**
		 * \brief		UARTC1
		 * \ingroup		atxmega_uart
		 */
		class UartC1
		{
		public:
			UartC1(uint32_t baudrate)
			{
				this->setBaudrate(baudrate);
			}
			
			/**
			 * \brief	Set baud rate
			 *
			 * If this function is called with a constant value as parameter,
			 * all the calculation is done by the compiler, so no 32-bit
			 * arithmetic is needed at run-time!
			 *
			 * \param	baudrate	desired baud rate
			 * \param	u2x			enabled double speed mode
			 */
			static ALWAYS_INLINE void
			setBaudrate(uint32_t baudrate, bool u2x = false)
			{
				uint16_t ubrr;
				if (u2x) {
					ubrr  = (F_CPU / (baudrate * 8l)) - 1;
					ubrr |= 0x8000;
				}
				else {
					ubrr = (F_CPU / (baudrate * 16l)) - 1;
				}
				setBaudrateRegister(ubrr);
			}
			
			/* **************************************************** *
			 * The four standard IO methods for all UARTs and alike *
			 * **************************************************** */
			
			/**
			 * \brief	Write a single byte
			 *
			 * \param	data	Single byte to write
			 */
			static void
			write(uint8_t data);
			
			/**
			 * \brief	Write a block of bytes
			 *
			 * \param	*buffer	Pointer to buffer with data to write
			 * \param	nbyte	Number of bytes to write
			 */
			static void
			write(const uint8_t *buffer, uint8_t nbyte);
			
			/**
			 * \brief	Read a single byte
			 *
			 * \param	&char	Byte read, if any
			 *
			 * \return	\n true if a byte was received, \n false otherwise
			 */
			static bool
			read(uint8_t& data);
			
			/**
			 * \brief	Read a block of bytes
			 *
			 * This is non-blocking. If no data is available this function waits
			 * about 1.5 times of a frame length. 
			 * This is only correct at a baud rate of 115200.  
			 * 
			 * \param	*buffer	Pointer to a buffer big enough to store \a n bytes
			 * \param	nbyte	Number of bytes to be read
			 * 
			 * \return	Number of bytes which could be read, maximum \a n
			 */
			static uint8_t
			read(uint8_t *buffer, uint8_t nbyte);
			
		protected:
			static void
			setBaudrateRegister(uint16_t ubrr);
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief	Buffered Uart
		 * 
		 * This implementation uses a ringbuffer. The size of the ringbuffer
		 * can be changed in the \c project.cfg file.
		 * 
		 * Just add add a value for UARTC1_TX_BUFFER_SIZE and/or 
		 * UARTC1_RX_BUFFER_SIZE in the \c defines section. The size can
		 * be any value between 1 and 254.
		 * 
		 * Example:
		 * \code
		 * [defines]
		 * UARTC1_TX_BUFFER_SIZE = 20
		 * UARTC1_RX_BUFFER_SIZE = 30
		 * \endcode
		 * 
		 * \ingroup	atxmega_uart
		 */
		class BufferedUartC1
		{
		public:
			BufferedUartC1(uint32_t baudrate)
			{
				this->setBaudrate(baudrate);
			}
			
			/**
			 * \brief	Set baud rate
			 *
			 * If this function is called with a constant value as parameter,
			 * all the calculation is done by the compiler, so no 32-bit
			 * arithmetic is needed at run-time!
			 *
			 * \param	baudrate	desired baud rate
			 * \param	doubleSpeed	enabled double speed mode
			 */
			static ALWAYS_INLINE void
			setBaudrate(uint32_t baudrate, bool doubleSpeed = false)
			{
				uint16_t ubrr;
				if (doubleSpeed) {
					ubrr  = (F_CPU / (baudrate * 8l)) - 1;
				}
				else {
					ubrr = (F_CPU / (baudrate * 16l)) - 1;
				}
				setBaudrateRegister(ubrr, doubleSpeed);
			}
			
			static inline void
			setFractionalBaudrate(uint16_t bsel, int8_t bscale)
			{
				uint16_t ubrr = (bsel & 0x0fff) | ((bscale & 0x0f) << 12);
				setBaudrateRegister(ubrr, false);
			}
			
			/**
			 * \brief	Write a single byte
			 *
			 * \param	data	Byte to write
			 */
			static void
			write(uint8_t data);
			
			/**
			 * \brief	Write a block of bytes
			 *
			 * \param	*buffer	Pointer to buffer with data to write
			 * \param	nbyte	Number of bytes to write
			 */
			static void
			write(const uint8_t *buffer, uint8_t nbyte);
			
			/**
			 * \brief	Read a single byte
			 *
			 * \param	&char	Byte read, if any
			 *
			 * \return	\n true if a byte was received, \n false otherwise
			 */
			static bool
			read(uint8_t& data);
			
			/**
			 * \brief	Read a block of bytes
			 * 
			 * \param	*buffer	Pointer to a buffer big enough to store \a n bytes
			 * \param	nbyte	Number of bytes to be read
			 * 
			 * \return	Number of bytes which could be read, maximum \a n
			 */
			static uint8_t
			read(uint8_t *buffer, uint8_t nbyte);
			
			/**
			 * \brief	Check whether any errors occurred during receiving
			 *			Be aware that these indicate an error that occurred
			 *			somewhere since resetting the Error Flags (with 
			 *			resetErrorFlags()), so you cannot tell which byte had
			 *			the error.
			 *
			 * \return	\c 0 if no errors occurred, otherwise a value that
			 *			corresponds to the Error Flags in the status register.
			 */
			static uint8_t
			readErrorFlags();

			static void
			resetErrorFlags();

			/**
			 * \brief	Empty the receive FIFO queue and UART buffer.
			 *
			 * \return	the size of the deleted FIFO queue.
			 */
			static uint8_t
			flushReceiveBuffer();
			
//			static uint8_t
//			flushTransmitBuffer();
			
		protected:
			static void
			setBaudrateRegister(uint16_t ubrr, bool doubleSpeed);
		};
		
		// --------------------------------------------------------------------
		/**
		 * \brief		UARTC1 in SPI master mode
		 * 
		 * \ingroup		atxmega_spi
		 * \ingroup		atxmega_uart
		 */
		class UartSpiC1
		{
		public:
			UartSpiC1()
			{
				this->initialize();
			}
			
			static void
			initialize();
			
			static uint8_t
			write(uint8_t data);
		};
	}
}

#endif // XPCC_ATXMEGA__UART_C1_HPP