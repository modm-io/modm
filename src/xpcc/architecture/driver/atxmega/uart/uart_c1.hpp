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


#ifndef XPCC__XMEGA_UART_C1_HPP
#define XPCC__XMEGA_UART_C1_HPP

#include <stdint.h>

namespace xpcc
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
		static inline void
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
		
		static void
		write(char data);
		
		/**
		 * \brief	Write a string
		 * 
		 * The string musst end with '\\0'.
		 */
		static void
		write(const char *string);
		
		static bool
		read(char& c);
		
	protected:
		static void
		setBaudrateRegister(uint16_t ubrr);
	};
	
	// ------------------------------------------------------------------------
	/**
	 * \brief	Buffered Uart
	 * 
	 * This implementation uses a ringbuffer.
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
		static inline void
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
		
		static void
		write(char data);
		
		/**
		 * \brief	Write a string
		 * 
		 * The string musst end with '\\0'.
		 */
		static void
		write(const char *string);
		
		static bool
		read(char& c);
		
		/**
		 * \brief	Read a block of bytes
		 * 
		 * \param	*buffer	Pointer to a buffer big enough to storage \a n bytes
		 * \param	n	Number of bytes to be read
		 * 
		 * \return	Number of bytes which could be read, maximal \a n
		 */
		static uint8_t
		read(char *buffer, uint8_t n);
		
	protected:
		static void
		setBaudrateRegister(uint16_t ubrr, bool doubleSpeed);
	};
	
	// ------------------------------------------------------------------------
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

#endif // XPCC__XMEGA_UART_C1_HPP