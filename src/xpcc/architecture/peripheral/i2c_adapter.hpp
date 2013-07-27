// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
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

#include "i2c.hpp"

namespace xpcc
{
	/**
	 * This class is an implementation of xpcc::I2cDelegate which,
	 * when passed to an i2c driver, performs the sequence:
	 * start - address - write - restart - address - read - stop.
	 *
	 * After initialization and performing operation this class keeps
	 * the references to passed buffers and may be reused again without
	 * reinitialization as long the passed buffers remain valid.
	 *
	 * @author	Georgi Grinshpun
	 * @author	Niklas Hauser
	 * @ingroup	i2c
	 */
	class I2cWriteReadAdapter : public I2cDelegate
	{
	private:
		uint8_t address;
		uint8_t readSize;
		uint8_t writeSize;
		uint8_t *readBuffer;
		const uint8_t *writeBuffer;
		volatile AdapterState state;
		bool isReading;

	public:
		I2cWriteReadAdapter()
		:	state(AdapterState::NoError)
		{
		}

		/// @return `Busy` while an I2C operation is ongoing. Reinitialization
		///			is not permitted during this phase.
		AdapterState
		getState()
		{
			return state;
		}

		/**
		 * @brief	Initializes the adapter with the slave address and the required information for a write/read operation
		 *
		 * Call this method during the drivers initialize method and give it the slave address,
		 * so you do not need to keep track of it externally.
		 *
		 * @param		address		the slave address excluding read/write bit
		 * @param[in] 	writeBuffer	buffer to be written to the slave
		 * @param		writeSize	number of bytes to be written, set to `0` to write nothing
		 * @param[out]	readBuffer	buffer to write the read bytes from the slave
		 * @param		readSize	number of bytes to be read, set to `0` to read nothing
		 */
		bool
		initialize(uint8_t address, const uint8_t * writeBuffer, uint8_t writeSize, uint8_t * readBuffer, uint8_t readSize)
		{
			if (state != AdapterState::Busy)
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

		/**
		 * @brief	Initializes the adapter with the required information for a write/read operation
		 *
		 * The slave address is not configured, so use this method during normal operation to change buffers.
		 *
		 * @param[in] 	writeBuffer	buffer to be written to the slave
		 * @param		writeSize	number of bytes to be written, set to `0` to write nothing
		 * @param[out]	readBuffer	buffer to write the read bytes from the slave
		 * @param		readSize	number of bytes to be read, set to `0` to read nothing
		 */
		bool
		initialize(const uint8_t * writeBuffer, uint8_t writeSize, uint8_t * readBuffer, uint8_t readSize)
		{
			if (state != AdapterState::Busy)
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

		///@{
		/// @internal
		virtual bool
		attaching()
		{
			if (state == AdapterState::Busy)
				return false;
			state = AdapterState::Busy;
			return true;
		}

		virtual Starting
		starting()
		{
			Starting s;
			s.address = address;
			if (isReading) {
				s.next = readSize ? Operation::Read : Operation::Stop;
			}
			else {
				s.next = writeSize ? Operation::Write : Operation::Stop;
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
			w.next = readSize ? OperationAfterWrite::Restart : OperationAfterWrite::Stop;
			return w;
		}

		virtual Reading
		reading()
		{
			Reading r;
			r.buffer = readBuffer;
			r.size = readSize;
			r.next = OperationAfterRead::Stop;
			return r;
		}

		virtual void
		stopped(DetachCause cause)
		{
			isReading = false;
			state = (cause == DetachCause::NormalStop) ? AdapterState::NoError : AdapterState::Error;
		}
		///@}
	};

	/**
	 * This class is an implementation of xpcc::I2cDelegate which,
	 * when passed to an i2c driver, performs the sequence:
	 * start - address - write - stop.
	 *
	 * After initialization and performing operation this class remains
	 * holding references to passed buffers and may be reused again without
	 * reinitialization as long the passed buffers remain valid.
	 *
	 * @author	Georgi Grinshpun
	 * @author	Niklas Hauser
	 * @ingroup	i2c
	 */
	class I2cWriteAdapter : public I2cDelegate
	{
	private:
		uint8_t address;
		uint8_t size;
		const uint8_t *buffer;
		volatile AdapterState state;

	public:
		I2cWriteAdapter()
		:	state(AdapterState::NoError)
		{
		}

		/// @return `Busy` while an I2C operation is ongoing. Reinitialization
		///			is not permitted during this phase.
		AdapterState
		getState()
		{
			return state;
		}

		/**
		 * @brief	Initializes the adapter with the slave address and the required information for a write operation
		 *
		 * Call this method during the drivers initialize method and give it the slave address,
		 * so you do not need to keep track of it externally.
		 *
		 * @param		address	the slave address excluding read/write bit
		 * @param[in] 	buffer	buffer to be written to the slave
		 * @param		size	number of bytes to be written, set to `0` to write nothing
		 */
		bool
		initialize(uint8_t address, const uint8_t * buffer, uint8_t size)
		{
			if (state != AdapterState::Busy)
			{
				this->address = address;
				this->buffer = buffer;
				this->size = size;
				return true;
			}
			return false;
		}

		/**
		 * @brief	Initializes the adapter with the required information for a write operation
		 *
		 * The slave address is not configured, so use this method during normal operation to change buffers.
		 *
		 * @param[in] 	buffer	buffer to be written to the slave
		 * @param		size	number of bytes to be written, set to `0` to write nothing
		 */
		bool
		initialize(const uint8_t * buffer, uint8_t size)
		{
			if (state != AdapterState::Busy)
			{
				this->buffer = buffer;
				this->size = size;
				return true;
			}
			return false;
		}

		/// @internal
		///@{
		virtual bool
		attaching()
		{
			if (state == AdapterState::Busy)
				return false;
			state = AdapterState::Busy;
			return true;
		}

		virtual Starting
		starting()
		{
			Starting s;
			s.address = address;
			s.next = size ? Operation::Write : Operation::Stop;
			return s;
		}

		virtual Reading
		reading()
		{
			Reading r;
			r.buffer = 0;
			r.size = 0;
			r.next = OperationAfterRead::Stop;
			return r;
		}

		virtual Writing
		writing()
		{
			Writing w;
			w.buffer = buffer;
			w.size = size;
			w.next = OperationAfterWrite::Stop;
			return w;
		}

		virtual void
		stopped(DetachCause cause)
		{
			state = (cause == DetachCause::NormalStop) ? AdapterState::NoError : AdapterState::Error;
		}
		///@}
	};

	/**
	 * This class is an implementation of xpcc::i2c::Delegate which,
	 * when passed to an i2c driver, performs the sequence:
	 * start - address - read - stop.
	 *
	 * After initialization and performing operation this class remains
	 * holding references to passed buffers and may be reused again without
	 * reinitialization as long the passed buffers remain valid.
	 *
	 * @author	Georgi Grinshpun
	 * @author	Niklas Hauser
	 * @ingroup	i2c
	 */
	class I2cReadAdapter : public I2cDelegate
	{
	private:
		uint8_t address;
		uint8_t size;
		const uint8_t *buffer;
		volatile AdapterState state;

	public:
		I2cReadAdapter()
		:	state(AdapterState::NoError)
		{
		}

		/// @return `Busy` while an I2C operation is ongoing. Reinitialization
		///			is not permitted during this phase.
		AdapterState
		getState()
		{
			return state;
		}

		/**
		 * @brief	Initializes the adapter with the slave address and the required information for a read operation
		 *
		 * Call this method during the drivers initialize method and give it the slave address,
		 * so you do not need to keep track of it externally.
		 *
		 * @param		address	the slave address excluding read/write bit
		 * @param[out] 	buffer	buffer to be read from the slave
		 * @param		size	number of bytes to be read, set to `0` to read nothing
		 */
		bool
		initialize(uint8_t address, uint8_t * buffer, uint8_t size)
		{
			if (state != AdapterState::Busy)
			{
				this->address = address;
				this->buffer = buffer;
				this->size = size;
				return true;
			}
			return false;
		}

		/**
		 * @brief	Initializes the adapter with the required information for a read operation
		 *
		 * The slave address is not configured, so use this method during normal operation to change buffers.
		 *
		 * @param[out] 	buffer	buffer to be read from the slave
		 * @param		size	number of bytes to be read, set to `0` to read nothing
		 */
		bool
		initialize(uint8_t * buffer, uint8_t size)
		{
			if (state != AdapterState::Busy)
			{
				this->buffer = buffer;
				this->size = size;
				return true;
			}
			return false;
		}

		///@{
		/// @internal
		virtual bool
		attaching()
		{
			if (state == AdapterState::Busy)
				return false;
			state = AdapterState::Busy;
			return true;
		}

		virtual Starting
		starting()
		{
			Starting s;
			s.address = address;
			s.next = size ? Operation::Read : Operation::Stop;
			return s;
		}

		virtual Reading
		reading()
		{
			Reading r;
			r.buffer = buffer;
			r.size = size;
			r.next = OperationAfterRead::Stop;
			return r;
		}

		virtual Writing
		writing()
		{
			Writing w;
			w.buffer = 0;
			w.size = 0;
			w.next = OperationAfterWrite::Stop;
			return w;
		}

		virtual void
		stopped(DetachCause cause)
		{
			state = (cause == DetachCause::NormalStop) ? AdapterState::NoError : AdapterState::Error;
		}
		///@}
	};
}

#endif // XPCC_I2C__WRITE_READ_ADAPTER_HPP
