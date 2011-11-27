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

#ifndef XPCC_STM32__USART_3_HPP
#define XPCC_STM32__USART_3_HPP

#include <stdint.h>
#include "uart_base.hpp"
#include "../device.h"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Universal synchronous/asynchronous receiver
		 * 				transmitter (USART3)
		 * 
		 * Simple unbuffered implementation.
		 * 
		 * @ingroup		stm32
		 */
		class Usart3 : public UartBase
		{
		public:
			Usart3(uint32_t baudrate)
			{
				setBaudrate(baudrate);
			}
			
			enum Mapping
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				REMAP_PB10_PB11,	///< TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14
				REMAP_PC10_PC11,	///< TX/PC10, RX/PC11, CK/PC12, CTS/PB13, RTS/PB14
				REMAP_PD8_PD9,		///< TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12
#else
				REMAP_PB10_PB11 = AFIO_MAPR_USART3_REMAP_NOREMAP,		///< TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14
				REMAP_PC10_PC11 = AFIO_MAPR_USART3_REMAP_PARTIALREMAP,	///< TX/PC10, RX/PC11, CK/PC12, CTS/PB13, RTS/PB14
				REMAP_PD8_PD9 = AFIO_MAPR_USART3_REMAP_FULLREMAP,		///< TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12
#endif
			};
			
			/**
			 * Configure the IO Pins for I2C3
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
			write(char data);
			
			/**
			 * \brief	Write a string
			 * 
			 * The string musst end with \c '\\0'.
			 */
			static void
			write(const char *string);
			
			/**
			 * \brief	Read a single byte
			 */
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
		};
	}
}

#endif // XPCC_STM32__USART_3_HPP