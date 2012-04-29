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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_I2C__READ_ADAPTER_HPP
#define XPCC_I2C__READ_ADAPTER_HPP

#include "delegate.hpp"
#include <xpcc/architecture.hpp>

namespace xpcc
{
	/**
	 * \brief	I2C Interface
	 * \ingroup	i2c
	 */
	namespace i2c
	{
		/**
		 * This class is an implementation of xpcc::i2c::Delegate which,
		 * when passed to an i2c driver, performs the sequence:
		 * start - address - read - stop.
		 *
		 * After initialization and performing operation this class remains
		 * holding references to passed buffers and may be reused again without
		 * reinitialization as long the passed buffers remain valid.
		 *
		 * While process is ongoing getState returns BUSY. Reinitialization
		 * is not permitted during this phase.
		 *
		 * Example:
		 * \code
		 * // instantiation of the delegate and creating of buffers somewhere
		 * // at a persistent place at least during operation is ongoing
		 * xpcc::i2c::ReadAdapter delegate;
		 * YourType buffer;
		 *
		 * // initialization of the delegate
		 * // (ReadAdapter does not need to be initializated each time it
		 * // is used if address, buffer and it's size don't change. However the content
		 * // of the buffer may change, but not during operation.)
		 * bool b = delegate.initialize(slaveAddress, buffer, sizeof(YourType))
		 *
		 * // try to start the transaction
		 * bool c = b && OneI2CMaster::start(&delegate);
		 *
		 * if c wait until delegate.getState != delegate.BUSY
		 *  check if delegate.getState is delegate.NO_ERROR to be shure transaction
		 *  has been completed successfully.
		 * else
		 * 	delegate has not been attached either because module was busy or
		 * 	delegate is already attached.
		 *
		 * \endcode
		 */
		class ReadAdapter : public Delegate
		{			
		private:
			uint8_t address;
			uint8_t size;
			const uint8_t *buffer;
			volatile adapter::State state;
			
		public:
			ReadAdapter()
			:	state(adapter::NO_ERROR)
			{
			}
			
			adapter::State
			getState()
			{
				return state;
			}
			
			bool
			initialize(uint8_t address, uint8_t * buffer, uint8_t size)
			{
				if (state != adapter::BUSY)
				{
					this->address = address;
					this->buffer = buffer;
					this->size = size;
					return true;
				}
				return false;
			}
			
			bool
			initialize(uint8_t * buffer, uint8_t size)
			{
				if (state != adapter::BUSY)
				{
					this->buffer = buffer;
					this->size = size;
					return true;
				}
				return false;
			}
			
			virtual bool
			attaching()
			{
				if (state == adapter::BUSY)
					return false;
				state = adapter::BUSY;
				return true;
			}
			
			virtual Starting
			started()
			{
				Starting s;
				s.address = address;
				s.next = size ? READ_OP : STOP_OP;
				return s;
			}
			
			virtual Reading
			reading()
			{
				Reading r;
				r.buffer = buffer;
				r.size = size;
				r.next = READ_STOP;
				return r;
			}
			
			virtual Writing
			writing()
			{
				Writing w;
				w.buffer = 0;
				w.size = 0;
				w.next = WRITE_STOP;
				return w;
			}
			
			virtual void
			stopped(DetachCause cause)
			{
				state = (cause == NORMAL_STOP) ? adapter::NO_ERROR : adapter::ERROR;
			}
		};
	}
}

#endif // XPCC_I2C__READ_ADAPTER_HPP
