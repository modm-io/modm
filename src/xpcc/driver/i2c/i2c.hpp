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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__I2C_HPP
#define XPCC__I2C_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/driver/interface.hpp>

namespace xpcc
{
	namespace i2c
	{
		static const bool ACK = true;
		static const bool NACK = false;
		
		static const uint8_t WRITE = 0x00;
		static const uint8_t READ = 0x01;
		
		/**
		 * \brief	Interface of a I2C master
		 * 
		 * This class definies the public interface for a synchronous master.
		 * This means that every transaction is blocking and have to be
		 * finished before the functions return.
		 * 
		 * \ingroup	interface
		 */
		class SynchronousMaster : public Interface
		{
			uint8_t
			read(bool ack);
			
			bool
			write(uint8_t data);
		};
		
		/**
		 * \brief	Interface for a I2C master with asynchronous transfers
		 * 
		 * A master with asynchronous transfers operates in the background
		 * until the operation is finished. This allows the use of DMA etc. and
		 * enables the program to do other things while the operation is
		 * going on.
		 * 
		 * \ingroup	interface
		 */
		class AsynchronousMaster : public Interface
		{
			void
			read(uint8_t *data, uint8_t bytes);
			
			bool
			write(uint8_t *data, uint8_t bytes);
			
			bool
			isFinished();
		};
	}
}

#endif // XPCC__I2C_HPP
