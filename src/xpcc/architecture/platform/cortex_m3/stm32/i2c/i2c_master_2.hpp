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

#ifndef XPCC_STM32__I2C_2_HPP
#define XPCC_STM32__I2C_2_HPP

#include <stdint.h>
#include <xpcc/driver/connectivity/i2c/master.hpp>


#include "../device.h"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		I2C2 Module.
		 * 
		 * @author		Georgi Grinshpun
		 * @author		Niklas Hauser
		 * @ingroup		stm32
		 */
		class I2cMaster2 : ::xpcc::i2c::Master
		{
		public:
			
//#if (defined DRIVER_CONNECTIVITY_I2C_DEBUG) && DRIVER_CONNECTIVITY_I2C_DEBUG
//			enum DebugEnum
//			{
//				IRQ_EV = 'i',
//				STARTBIT_SET = 's',
//				ADDRESS_SENT = 'a',
//				TRANSMITTER = 'T',
//				RECEIVER_NEXT_RESTART = 'r',
//				RECEIVER_NEXT_STOP = 'S',
//				RECEIVER_MANY_BYTES = 'm',
//				AFTER_WRITE_FINISHED_IRQ = 'f',
//				TXE_NO_BYTES_LEFT = 'l',
//				TXE_NEXT_WRITE = 'w',
//				TXE_NEXT_RESTART = 'R',
//				TXE_NEXT_STOP = 't',
//				TXE_BTF_NEXT_RESTART = 'b',
//				TXE_MORE_BYTES_LEFT = 'M',
//				RXNE_IRQ = 'Q',
//				RXNE_MANY_BYTES_LEFT = 'X',
//				RXNE_ONE_BYTE_LEFT = 'O',
//				RXNE_NO_BYTES_LEFT_NEXT_RESTART = 'B',
//				RXNE_NO_BYTES_LEFT_NEXT_STOP = 'K',
//				CALL_STARTING = 'Y',
//
//				IRQ_ER = 'E',
//			};
//#endif
			/*
			 YisiaTiMiMilti
			 YisiaTiMilRisiaSiQK
			 Y
			 
			 adapter.initialize(buffer, 2, data, 0);
			 CALL_STARTING
			 
			 IRQ_EV
			 STARTBIT_SET
			 
			 IRQ_EV
			 ADDRESS_SENT
			 TRANSMITTER
			 
			 IRQ_EV
			 TXE_MORE_BYTES_LEFT
			 
			 IRQ_EV
			 TXE_MORE_BYTES_LEFT
			 
			 IRQ_EV
			 TXE_NO_BYTES_LEFT
			 TXE_NEXT_STOP
			 
			 IRQ_EV
			 stopped
			 
			 
			 adapter.initialize(buffer, 1, buffer, 1);
			 CALL_STARTING
			 
			 IRQ_EV
			 STARTBIT_SET
			 
			 IRQ_EV
			 ADDRESS_SENT
			 TRANSMITTER
			 
			 IRQ_EV
			 TXE_MORE_BYTES_LEFT
			 
			 IRQ_EV
			 TXE_NO_BYTES_LEFT
			 TXE_NEXT_RESTART
			 
			 IRQ_EV
			 STARTBIT_SET
			 
			 IRQ_EV
			 ADDRESS_SENT
			 RECEIVER_NEXT_STOP
			 
			 IRQ_EV
			 RXNE_IRQ
			 RXNE_NO_BYTES_LEFT_NEXT_STOP
			 
			 
			 !! breaking !!
			 
			 
			 CALL_STARTING
			 
			 //*/
			

			enum Mapping
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				REMAP_PB10_PB11,	///< SCL mapped to PB10, SDA mapped to PB11
				REMAP_PF1_PF0,		///< SCL mapped to PF1, SDA mapped to PF0
				REMAP_PH4_PH5,		///< SCL mapped to PH4, SDA mapped to PH5
#else
				REMAP_PB10_PB11,						///< SCL mapped to PB10, SDA mapped to PB11
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
			 * @brief	Enables the peripheral clock for this module.
			 * You don't need to call this method at start, but you may.
			 */
			static void
			enable();

			/**
			 * Configure the IO Pins for this module
			 */
			static void
			configurePins(Mapping mapping);
			
			/**
			 * @brief	Initialize this module
			 * You must initialize pins before call to this method either by
			 * calling configurePins
			 * \param ccrPrescaler: I2CFrequency = STM32_APB1_FREQUENCY / (2 * ccrPrescaler)
			 */
			static void
			initialize(uint16_t ccrPrescaler);
			
			static void
			reset(bool error=false);
			
		public:
			/**
			 * You initialize an I2C transaction calling this method. Passed
			 * delegate will be attached to this module and has to manage
			 * the transaction until it is detached by calling stop.
			 * \return true if module is free and module is attached, false
			 * if module was busy. Be aware, that the delagete may get attached
			 * and detached before this method returns. In this case
			 * true is returned. That means each interconnection between
			 * passed delegate and the caller of this method must be
			 * performed before call.
			 * 
			 * \code
			 * ... initialization of delegate
			 * if (xpcc::stm32::I2c2::start(&delegate)){
			 *  // wait until delegate is finished
			 *  // don't interconnect with the delegate, only while a call to 
			 *  // one of it's methods
			 *  // after delegates' stop method has been called it is detached
			 *  // from module
			 * }
			 * else{
			 *  // delegate has not been attached because module was busy
			 * }
			 * 
			 * \endcode
			 */
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

#endif // XPCC_STM32__I2C_2_HPP