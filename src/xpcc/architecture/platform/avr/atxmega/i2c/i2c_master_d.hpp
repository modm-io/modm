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

#ifndef XPCC_ATXMEGA__I2C_MASTER_D_HPP
#define XPCC_ATXMEGA__I2C_MASTER_D_HPP

#include <stdint.h>
#include <xpcc/driver/connectivity/i2c/master.hpp>

#if defined(TWID) || defined(__DOXYGEN__)

namespace xpcc
{
	namespace atxmega
	{
		/**
		 * \brief	Asynchronous I2C master module for Port D
		 *
		 * Interrupts must be enabled.
		 * 
		 * \author Niklas Hauser
		 * \ingroup	atxmega_i2c
		 * \ingroup	i2c
		 */
		class I2cMasterD : ::xpcc::i2c::Master
		{
		public:
			enum ErrorState
			{
				NO_ERROR,			//!< No Error occurred
				DATA_NACK,			//!< Data was transmitted and NACK received
				ARBITRATION_LOST,	//!< Arbitration was lost during writing or reading
				BUS_ERROR,			//!< Misplaced Start or Stop condition
				UNKNOWN_ERROR		//!< Unknown error condition
			};
			
			static void
			initialize(uint8_t baud, bool pullUpResistors=false);
			
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
			 * if (xpcc::stm32::I2cD::start(&delegate)){
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

#endif	// TWID
#endif	// XPCC_ATXMEGA__I2C_MASTER_D_HPP