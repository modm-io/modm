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
 *
 * $Id: constants.hpp 735 2012-02-18 22:04:33Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_I2C__CONSTANTS_HPP
#define XPCC_I2C__CONSTANTS_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * \brief	I2C Interface
	 * \ingroup	i2c
	 */
	namespace i2c
	{
		/**
		 * \brief	Address modifier for writing. To be used by I2C Drivers.
		 * 
		 * \ingroup	i2c
		 */
		static const uint8_t WRITE = 0x00;
		
		/**
		 * \brief	Address modifier for reading. To be used by I2C Drivers.
		 * 
		 * \ingroup	i2c
		 */
		static const uint8_t READ = 0x01;
		
		/**
		 * \brief	While reading/writing you have to decide if after operation 
		 * 			bus goes into BUS_STOPPED, BUS_HOLD or BUS_WRITE state.
		 * 
		 * \ingroup	i2c
		 */
		enum OperationParameter
		{
			READ_STOP = 0,		//!< After Reading busState results BUS_STOPPED
			READ_RESTART = 1,	//!< After Reading busState results BUS_HOLD
			WRITE_STOP = 2,		//!< After Writing busState results BUS_STOPPED
			WRITE_RESTART = 3,	//!< After Writing busState results BUS_WRITE
		};
		
		/**
		 * \brief	If an error occurs these states give further information.
		 *			After an error a stop condition is generated, forcing a
		 *			release of bus control.
		 *			Regaining control can be attemped by calling start.
		 *
		 * \ingroup	i2c
		 */
		enum ErrorState
		{
			NO_ERROR,			//!< No Error ocurred
			ADDRESS_NACK,		//!< Slave address was transmitted and NACK received
			DATA_NACK,			//!< Data was transmitted and NACK received
			ARBITRATION_LOST,	//!< Arbitration was lost during writing or reading
			BUS_ERROR,			//!< Misplaced Start or Stop condition
			UNKNOWN_ERROR		//!< Unknown error condition
		};
		
		/**
		 * \brief	Status of bus
		 * \ingroup	i2c	
		 */
		enum BusState
		{
			BUS_RESET = 0,		//!< Last operation was not successful, give up bus control (call stop). No restart possible.
			BUS_STANDBY = 1,	//!< Last operation (Start or Restart) was successful, ready for writing or reading. Note: this state does not indicate a present device, only the bus is ready to be used and reserved for the owner!
			BUS_WRITE = 2,		//!< Last operation (write) was successful, ready for writing, restart or stop
			BUS_HOLD = 3,		//!< Last operation (read) was successful, ready for restart only.
			BUS_STOPPED = 4,	//!< Last operation (read) was successful, give up bus control (call stop). No restart possible.
		};
		
		/**
		 * \brief	Status of a asynchronous operation
		 * \ingroup	i2c
		 */
		enum BusyState
		{
			UNKNOWN = 0,	//!< Bus has been reset and is in no known state
			FREE = 1,		//!< Bus is not used, call start for gaining control
			OCCUPIED = 2,	//!< Asynchronous operation is finished, check BusState for the result
			BUSY = 3,		//!< Asynchronous operation is ongoing
		};
	}
}

#endif // XPCC_I2C__CONSTANTS_HPP
