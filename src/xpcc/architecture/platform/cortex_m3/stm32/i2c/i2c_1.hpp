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

#ifndef XPCC_STM32__I2C_1_HPP
#define XPCC_STM32__I2C_1_HPP

#include <stdint.h>
#include <xpcc/driver/connectivity/i2c/master.hpp>

#include "../device.h"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		I2C1 Module.
		 * 
		 * @author		Georgi
		 * @ingroup		stm32
		 */
		class I2c1 : ::xpcc::i2c::Master
		{
		public:
			enum Mapping
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				REMAP_PB6_PB7,		///< SCL mapped to PB6, SDA mapped to PB7
				REMAP_PB8_PB9,		///< SCL mapped to PB8, SDA mapped to PB9
#else
				REMAP_PB6_PB7 = 0,						///< SCL mapped to PB6, SDA mapped to PB7
				REMAP_PB8_PB9 = AFIO_MAPR_I2C1_REMAP,	///< SCL mapped to PB8, SDA mapped to PB9
#endif
			};
			
			/**
			 * Configure the IO Pins for I2C1
			 */
			static void
			configurePins(Mapping mapping);
			
			/**
			 * @brief	Initialize I2C module
			 */
			static void
			initialize();
			
		public:
			static bool
			start(uint8_t slaveAddress);

			static void
			restart(uint8_t slaveAddress);

			static void
			stop();

			static void
			read(uint8_t *data, std::size_t size, xpcc::i2c::ReadParameter param = xpcc::i2c::READ_STOP);

			static void
			write(const uint8_t *data, std::size_t size);
			
		public:
			static xpcc::i2c::BusyState
			getBusyState();

			static xpcc::i2c::BusState
			getBusState();
		};
	}
}

#endif // XPCC_STM32__I2C_1_HPP