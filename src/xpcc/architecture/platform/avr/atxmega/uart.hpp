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
 * 
 * $hello please remove me$
 */
// ----------------------------------------------------------------------------
/**
 * \ingroup		atxmega
 * \defgroup	atxmega_uart	UART module
 */

#include <xpcc/architecture/utils.hpp>

namespace xpcc {
	namespace atxmega {
		/**
		 * \brief		Base class for the UART classes
		 * 
		 * Provides the common interface for all UARTs and alike. 
		 * This should be generalised for all platforms (at least
		 * atmega, atxmega and stm32)
		 * 
		 * \ingroup		atxmega_uart
		 * 
	 	 */
		class UartBase {
		public:
			/* **************************************************** *
			 * The five standard IO methods for all UARTs and alike *
			 * **************************************************** */

			/**
			 * \brief	Write a single byte
			 *
			 * \param	data	Single byte to write
			 */
			static void
			write(char data);

			/**
			 * \brief	Write a null-terminated string
			 *
			 * \param	*string	String to write
			 *
			 * The string must end with '\\0'.
			 */
			static void
			write(const char *string);

			/**
			 * \brief	Write a block of bytes
			 *
			 * \param	*buffer	Pointer to buffer with data to write
			 * \param	nbyte	Number of bytes to write
			 */
			static void
			write(const char *buffer, uint8_t nbyte);

			/**
			 * \brief	Read a single byte
			 *
			 * \param	&data	Byte read, if any
			 *
			 * \return	\n true if a byte was received, \n false otherwise
			 */
			static bool
			read(char& data);

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
			read(char *buffer, uint8_t nbyte);

		protected:
			/**
			 * \brief	Initialize UART module
			 *
			 * Set the output and input pins, the interrupts and enables transceiver and receiver.
			 * Does *not* set the baudrate. This should be done *before*.
			 */
			static void
			initialize();

			/**
			 * \brief	Set baud rate
			 *
			 * See Constructor.
			 *
			 * \param	baudrate	desired baud rate, must be constant at compile time
			 */
			ALWAYS_INLINE static void
			setBaudrate(uint32_t baudrate);

			/**
			 * \brief	Find the BSEL value depending on the BSCALE value and baudrate.
			 *
			 * \param	baudrate	Desired baudrate
			 * \param	bscale		The fractional divider.
			 *
			 * The formula is from table 21-1 on page 238 of the datasheet.
			 */
			ALWAYS_INLINE static uint16_t
			baudBselFromBaudscale(uint32_t const baudrate, int8_t const bscale)
			{
				uint16_t bsel = ((bscale) < 0) ?
					  (int)((((float)(F_CPU)/(16*(float)(baudrate)))-1)*(1<<-(bscale)))
					: (int)((float)(F_CPU)/((1<<(bscale))*16*(float)(baudrate)))-1  ;

				return bsel;
			}

			/**
			 * \brief	Find a suitable BSCALE value for a given baudrate by trial-and-error
			 *
			 * \param	baudrate	Desired baudrate.
			 */
			ALWAYS_INLINE static int8_t
			bScale(uint32_t const baudrate)
			{
				int8_t ret = 0;

				/*
				 * For 8N1 frames (1 start bit, 8 data bits, no parity, 1 stop bit)
				 * the minimum BSEL value is -6 and the maximum +6.
				 * (Datasheet, page 247, 21.09)
				 */

		//			uint16_t bsel_7 = baudBselFromBaudscale(baudrate, -7);
				uint16_t bsel_6 = baudBselFromBaudscale(baudrate, -6);
				uint16_t bsel_5 = baudBselFromBaudscale(baudrate, -5);
				uint16_t bsel_4 = baudBselFromBaudscale(baudrate, -4);
				uint16_t bsel_3 = baudBselFromBaudscale(baudrate, -3);
				uint16_t bsel_2 = baudBselFromBaudscale(baudrate, -2);
				uint16_t bsel_1 = baudBselFromBaudscale(baudrate, -1);
				uint16_t bsel_0 = baudBselFromBaudscale(baudrate,  0);
				uint16_t bsel01 = baudBselFromBaudscale(baudrate, +1);
				uint16_t bsel02 = baudBselFromBaudscale(baudrate, +2);
				uint16_t bsel03 = baudBselFromBaudscale(baudrate, +3);
				uint16_t bsel04 = baudBselFromBaudscale(baudrate, +4);
				uint16_t bsel05 = baudBselFromBaudscale(baudrate, +5);
		//			uint16_t bsel06 = baudBselFromBaudscale(baudrate, +6);

				if
		//			(bsel_7 < 4096) ret = -7;
		//			else if
						(bsel_6 < 4096) ret = -6;
				else if (bsel_5 < 4096) ret = -5;
				else if (bsel_4 < 4096) ret = -4;
				else if (bsel_3 < 4096) ret = -3;
				else if (bsel_2 < 4096) ret = -2;
				else if (bsel_1 < 4096) ret = -1;
				else if (bsel_0 < 4096) ret =  0;
				else if (bsel01 < 4096) ret =  1;
				else if (bsel02 < 4096) ret =  2;
				else if (bsel03 < 4096) ret =  3;
				else if (bsel04 < 4096) ret =  4;
				else if (bsel05 < 4096) ret =  5;
		//			else if (bsel06 < 4096) ret =  6;
		//			else                    ret =  7;
				else					ret =  6;

				return ret;
			}

			/**
			 * \brief	Convert the signed BSCALE value unsigned.
			 *
			 * \param	baudrate		Desired baud rate.
			 * \return	bscale_unsigned	Unsigned BSCALE value.
			 */
			ALWAYS_INLINE static uint8_t
			bScaleUnsigned(uint32_t baudrate)
			{
				int8_t bscale_signed = bScale(baudrate);

				uint8_t bscale_unsigned = 0;
				if (bscale_signed < 0)
				{
					bscale_unsigned = 16 + bscale_signed;
				}
				else
				{
					bscale_unsigned = bscale_signed;
				}
				return bscale_unsigned;
			}

			/**
			 * \brief	Select BSEL value.
			 *
			 * \param	baudrate	Desired baud rate.
			 */
			ALWAYS_INLINE static uint16_t
			bSel(uint32_t const baudrate)
			{
				uint16_t bsel = baudBselFromBaudscale(baudrate, bScale(baudrate));
				return bsel;
			}

		};
	} // atxmega namespace
} // xpcc namespace

#include "uart/uart_c0.hpp"
#include "uart/uart_c1.hpp"
#include "uart/uart_d0.hpp"
#include "uart/uart_d1.hpp"
#include "uart/uart_e0.hpp"
#include "uart/uart_e1.hpp"
#include "uart/uart_f0.hpp"
#include "uart/uart_f1.hpp"
