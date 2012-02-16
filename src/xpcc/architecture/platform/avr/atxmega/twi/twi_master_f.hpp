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

#ifndef XPCC_ATXMEGA__TWI_MASTER_F_HPP
#define XPCC_ATXMEGA__TWI_MASTER_F_HPP

#include <stdint.h>
#include <xpcc/driver/connectivity/i2c/master.hpp>

#if defined(TWIF) || defined(__DOXYGEN__)

namespace xpcc
{
	namespace atxmega
	{
		/**
		 * \brief	Synchronous TWI master module for Port F
		 *
		 * Interrupts must be enabled.
		 * 
		 * \warning	You must only use the AsynchronousTwiMaster or the
		 * 			SynchronousTwiMaster class, never both of them!
		 * 
		 * \author Niklas Hauser
		 * \ingroup	atxmega_twi
		 * \ingroup	i2c
		 */
		class AsynchronousTwiMasterF : public xpcc::i2c::Master
		{
		public:
			static void
			initialize(uint8_t baud, bool pullUpResistors=false);
			
			static bool
			start(uint8_t slaveAddress, uint8_t operation=xpcc::i2c::WRITE);
			
			static void
			restart(uint8_t slaveAddress, uint8_t operation=xpcc::i2c::WRITE);
			
			static void
			stop();
			
			static void
			read(uint8_t *data, std::size_t size, xpcc::i2c::OperationParameter param = xpcc::i2c::READ_STOP);
			
			static void
			write(uint8_t *data, std::size_t size, xpcc::i2c::OperationParameter param = xpcc::i2c::READ_STOP);
			
			static void
			writeRead(uint8_t *data, std::size_t writeSize, std::size_t readSize, xpcc::i2c::OperationParameter param = xpcc::i2c::READ_STOP);
			
			static void
			attachDelegate(xpcc::i2c::Delegate *object);
			
			static xpcc::i2c::ErrorState
			getErrorState();
			
			static xpcc::i2c::BusyState
			getBusyState();
			
			static xpcc::i2c::BusState
			getBusState();
			
			ALWAYS_INLINE
			static bool
			startWrite(uint8_t slaveAddress, uint8_t *data, std::size_t size, xpcc::i2c::OperationParameter param = xpcc::i2c::WRITE_STOP)
			{
				if (start(slaveAddress)) {
					writeRead(data, size, 0, (param == xpcc::i2c::WRITE_RESTART), false);
					return true;
				}
				return false;
			}
			
			ALWAYS_INLINE
			static bool
			startRead(uint8_t slaveAddress, uint8_t *data, std::size_t size, xpcc::i2c::OperationParameter param = xpcc::i2c::READ_STOP)
			{
				if (start(slaveAddress, xpcc::i2c::READ)) {
					writeRead(data, 0, size, false, (param == xpcc::i2c::READ_RESTART));
					return true;
				}
				return false;
			}
			
			ALWAYS_INLINE
			static bool
			startWriteRead(uint8_t slaveAddress, uint8_t *data, std::size_t writeSize, std::size_t readSize, xpcc::i2c::OperationParameter param = xpcc::i2c::READ_STOP)
			{
				if (start(slaveAddress)) {
					writeRead(data, writeSize, readSize, false, (param == xpcc::i2c::READ_RESTART));
					return true;
				}
				return false;
			}
			
		private:
			static void
			writeRead(uint8_t *data, std::size_t writeSize, std::size_t readSize, bool restartW, bool restartR);
		};
	}
}

#endif	// TWIF
#endif	// XPCC_ATXMEGA__TWI_MASTER_F_HPP