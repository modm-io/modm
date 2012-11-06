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

#ifndef XPCC_INTERFACE__UART_HPP
#define XPCC_INTERFACE__UART_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * \brief		Buffered UART
	 *
	 * \ingroup		interface
	 */
	class Uart : public Interface
	{
	public:
		/**
		 * \brief	Sets the baudrate at compile time.
		 */
		static void
		setBaudrate(uint32_t baudrate);
		
		/**
		 * \brief	Write a single byte and wait for completion.
		 */
		static void
		writeBlocking(uint8_t data);
		
		/**
		 * \brief	Write a block of bytes and wait for completion.
		 */
		static void
		writeBlocking(const uint8_t *data, std::size_t lenth);
		
		// asynchronous
		/**
		 * \brief	Read a single byte
		 *
		 * \return	`true` if a byte has been read
		 */
		static bool
		read(uint8_t &c);
	};

	/**
	 * \brief		Buffered UART
	 *
	 * \ingroup		interface
	 */
	class BufferedUart : public Uart
	{
	public:
		/**
		 * \brief	Pushes a single byte into the buffer
		 *
		 * \return	`false` if buffer is full
		 */
		static bool
		write(uint8_t data);
		
		/**
		 * \brief	Pushes a block of bytes into the buffer
		 *
		 * \param	*buffer	Pointer to a buffer big enough to store \a n bytes
		 * \param	n		Number of bytes to be written
		 *
		 * \return	the number of bytes pushed into the buffer, maximal \a n
		 */
		static std::size_t
		write(const uint8_t *data, std::size_t length);
		
		/**
		 * \brief	Read a block of bytes
		 *
		 * \param	*buffer	Pointer to a buffer big enough to store \a n bytes
		 * \param	n		Number of bytes to be read
		 *
		 * \return	Number of bytes which could be read, maximal \a n
		 */
		static std::size_t
		read(uint8_t *buffer, std::size_t length);
		
		/**
		 * \return	`true` if the buffer is empty and the last byte has been sent
		 */
		static bool
		isWriteFinished();
	};
}

#endif // XPCC_INTERFACE__UART_HPP
