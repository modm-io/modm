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

#ifndef XPCC_I2C__WRITE_READ_ADAPTER_HPP
#define XPCC_I2C__WRITE_READ_ADAPTER_HPP

#include "delegate.hpp"

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
		 * start - address - write - restart - address - read - stop.
		 *
		 * After initialization and performing operation this class remains
		 * holding references to passed buffers and may be reused again without
		 * reinitialization as long the passed buffers remain valid.
		 *
		 * While process is ongoing getState returns BUSY. Reinitialization
		 * is not permitted during this phase.
		 *
		 * Code example is provided by WriteAdapter
		 *
		 * \author Georgi Grinshpun
		 */
		class WriteReadAdapter : public Delegate
		{
		private:
			uint8_t address;
			uint8_t readSize;
			uint8_t writeSize;
			uint8_t *readBuffer;
			const uint8_t *writeBuffer;
			volatile adapter::State state;
			bool isReading;
			
		public:
			WriteReadAdapter()
			:	state(adapter::NO_ERROR)
			{
			}

			adapter::State
			getState()
			{
				return state;
			}

			bool
			initialize(uint8_t address, const uint8_t * writeBuffer, uint8_t writeSize, uint8_t * readBuffer, uint8_t readSize)
			{
				if (state != adapter::BUSY)
				{
					this->address = address;
					this->readBuffer = readBuffer;
					this->readSize = readSize;
					this->writeBuffer = writeBuffer;
					this->writeSize = writeSize;
					isReading = writeSize ? false : true;
					return true;
				}
				return false;
			}
			
			bool
			initialize(const uint8_t * writeBuffer, uint8_t writeSize, uint8_t * readBuffer, uint8_t readSize)
			{
				if (state != adapter::BUSY)
				{
					this->readBuffer = readBuffer;
					this->readSize = readSize;
					this->writeBuffer = writeBuffer;
					this->writeSize = writeSize;
					isReading = writeSize ? false : true;
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
			starting()
			{
				Starting s;
				s.address = address;
				if (isReading) {
					s.next = readSize ? READ_OP : STOP_OP;
				}
				else {
					s.next = writeSize ? WRITE_OP : STOP_OP;
				}
				isReading = !isReading;
				return s;
			}
			
			virtual Writing
			writing()
			{
				Writing w;
				w.buffer = writeBuffer;
				w.size = writeSize;
				w.next = readSize ? WRITE_RESTART : WRITE_STOP;
				return w;
			}

			virtual Reading
			reading()
			{
				Reading r;
				r.buffer = readBuffer;
				r.size = readSize;
				r.next = READ_STOP;
				return r;
			}

			virtual void
			stopped(DetachCause cause)
			{
				isReading = false;
				state = (cause == NORMAL_STOP) ? adapter::NO_ERROR : adapter::ERROR;
			}
		};
	}
}

#endif // XPCC_I2C__WRITE_READ_ADAPTER_HPP
