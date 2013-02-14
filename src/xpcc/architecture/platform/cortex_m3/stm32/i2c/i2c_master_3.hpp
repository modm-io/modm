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

#ifndef XPCC_STM32__I2C_3_HPP
#define XPCC_STM32__I2C_3_HPP

#include <stdint.h>
#include <xpcc/driver/connectivity/i2c/master.hpp>


#include "../device.h"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		I2C3 Module.
		 * 
		 * @author		Georgi Grinshpun
		 * @author		Niklas Hauser
		 * @ingroup		stm32
		 */
		class I2cMaster3 : ::xpcc::i2c::Master
		{
		public:
			enum Mapping
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				REMAP_PA8_PC9,		///< SCL mapped to PA8, SDA mapped to PC9
				REMAP_PH7_PH8,		///< SCL mapped to PH7, SDA mapped to PH8
#else
				
#endif
			};
			
			enum ErrorState
			{
				NO_ERROR,			//!< No Error occurred
				DATA_NACK,			//!< Data was transmitted and NACK received
				ARBITRATION_LOST,	//!< Arbitration was lost during writing or reading
				BUS_ERROR,			//!< Misplaced Start or Stop condition
				UNKNOWN_ERROR		//!< Unknown error condition
			};
			

			/**
			 * Configure the IO Pins for this module
			 */
			static void
			configurePins(Mapping mapping);
			
			/**
			 * @brief	Initialize this with a prescaler
			 *
			 * You must initialize pins before call to this method either by
			 * calling configurePins
			 
			 * \param ccrPrescaler: I2CFrequency = STM32_APB1_FREQUENCY / (2 * ccrPrescaler)
			 */
			static void
			initialize(uint16_t ccrPrescaler);
			
			static void
			reset(bool error=false);
			
		public:
			static bool
			start(xpcc::i2c::Delegate *delegate);
			
			static bool
			startSync(xpcc::i2c::Delegate *delegate);
			
			/**
			 * This method returns the actual error state if it is called
			 * from the delegates stop method.
			 */
			static uint8_t
			getErrorState();
		};
	}
}

#endif // XPCC_STM32__I2C_3_HPP