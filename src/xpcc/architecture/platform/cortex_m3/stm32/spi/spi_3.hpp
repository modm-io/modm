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

#ifndef XPCC_STM32__SPI_3_HPP
#define XPCC_STM32__SPI_3_HPP

#include <stdint.h>
#include "../device.h"
#include <xpcc/driver/connectivity/spi/spi_master.hpp>

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Serial peripheral interface (SPI3)
		 * 
		 * Simple unbuffered implementation.
		 * 
		 * STM32F10x:
		 * - SPI1 (APB2) at 72MHz
		 * - SPI2 and SPI3 (APB1) at 36MHz.
		 * 
		 * STM32F2/4xx:
		 * - TODO
		 * 
		 * @ingroup		stm32
		 */
		class SpiMaster3 : public xpcc::SpiMaster
		{
		public:
			enum Mode
			{
				MODE_0 = 0,								///< SCK normal, sample on rising edge
				MODE_1 = SPI_CR1_CPHA,					///< SCK normal, sample on falling edge
				MODE_2 = SPI_CR1_CPOL,					///< SCK inverted, sample on falling edge
				MODE_3 = SPI_CR1_CPOL | SPI_CR1_CPHA,	///< SCK inverted, sample on rising edge
			};
			
			enum Prescaler
			{
				PRESCALER_2 = 0,
				PRESCALER_4 = SPI_CR1_BR_0,
				PRESCALER_8 = SPI_CR1_BR_1,
				PRESCALER_16 = SPI_CR1_BR_1 | SPI_CR1_BR_0,
				PRESCALER_32 = SPI_CR1_BR_2,
				PRESCALER_64 = SPI_CR1_BR_2 | SPI_CR1_BR_0,
				PRESCALER_128 = SPI_CR1_BR_2 | SPI_CR1_BR_1,
				PRESCALER_256 = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,
			};
			
			enum Mapping
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				REMAP_PB3_PB4_PB5,		///< SCK/PB3, MISO/PB4, MOSI/PB5, NSS/PA15
				REMAP_PC10_PC11_PC12,	///< SCK/PC10, MISO/PC11, MOSI/PC12, NSS/PA4
#else
				REMAP_PB3_PB4_PB5 = 0,							///< SCK/PB3, MISO/PB4, MOSI/PB5, NSS/PA15
#	if defined(STM32F10X_CL) 
				REMAP_PC10_PC11_PC12 = AFIO_MAPR_SPI3_REMAP,	///< SCK/PC10, MISO/PC11, MOSI/PC12, NSS/PA4
#	endif
#endif
			};
			
			/**
			 * Configure the IO Pins for SPI3
			 * 
			 * \warning	NSS is not configured and has to be handled
			 * 			by the user!
			 */
			static void
			configurePins(Mapping mapping);
			
		public:
			/**
			 * @brief	Initialize SPI module
			 */
			static void
			initialize(Mode mode = MODE_0, Prescaler prescaler = PRESCALER_64);
			
			static uint8_t
			write(uint8_t data);
			
			static bool
			setBuffer(uint16_t length,
					  uint8_t* transmit=0, uint8_t* receive=0,
					  bool transmitIncr=true, bool receiveIncr=true);
			
			static bool
			transfer(bool send=true, bool receive=false, bool wait=true);
			
			static bool
			isFinished();
		};
	}
}

#endif // XPCC_STM32__SPI_3_HPP