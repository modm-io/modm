// coding: utf-8
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_I2C_WRITE_READ_ADAPTER_HPP
#define XPCC_I2C_WRITE_READ_ADAPTER_HPP

#include "i2c.hpp"

namespace xpcc
{

/**
 * This class is an implementation of xpcc::I2cTransaction which,
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
class I2cWriteReadAdapter : protected I2cTransaction
{
	uint8_t address;
	uint8_t readSize;
	uint8_t writeSize;
	uint8_t *readBuffer;
	const uint8_t *writeBuffer;
	volatile AdapterState adapterState;
	bool isReading;

public:
	I2cWriteReadAdapter(uint8_t address)
	:	address(address << 1), adapterState(AdapterState::Idle)
	{
	}

protected:
	/// @return `Busy` while an I2C operation is ongoing. Reinitialization
	///			is not permitted during this phase.
	AdapterState inline
	getAdapterState()
	{
		return adapterState;
	}

	/**
	 * Initializes the adapter with the required information for a write/read operation.
	 *
	 * @param[in] 	writeBuffer	buffer to be written to the slave
	 * @param		writeSize	number of bytes to be written, set to `0` to write nothing
	 * @param[out]	readBuffer	buffer to write the read bytes from the slave
	 * @param		readSize	number of bytes to be read, set to `0` to read nothing
     *
     * @return  `true` if adapter was not busy and accepted the information,
     *          `false` otherwise
	 */
	bool inline
	configureWriteRead(const uint8_t *writeBuffer, uint8_t writeSize, uint8_t *readBuffer, uint8_t readSize)
	{
		if (adapterState != AdapterState::Busy)
		{
			this->readBuffer = readBuffer;
			this->readSize = readBuffer ? readSize : 0;
			this->writeBuffer = writeBuffer;
			this->writeSize = writeBuffer ? writeSize : 0;
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
		if (adapterState == AdapterState::Busy)
			return false;
		adapterState = AdapterState::Busy;
		return true;
	}

	virtual void
	starting(Starting &starting)
	{
		starting.address = address;
		if (isReading) {
			starting.next = readSize ? Operation::Read : Operation::Stop;
		}
		else {
			starting.next = writeSize ? Operation::Write : Operation::Stop;
		}
		isReading = !isReading;
	}

	virtual void
	writing(Writing &writing)
	{
		writing.buffer = writeBuffer;
		writing.length = writeSize;
		writing.next = readSize ? OperationAfterWrite::Restart : OperationAfterWrite::Stop;
	}

	virtual void
	reading(Reading &reading)
	{
		reading.buffer = readBuffer;
		reading.length = readSize;
		reading.next = OperationAfterRead::Stop;
	}

	virtual void
	detaching(DetachCause cause)
	{
		isReading = false;
		adapterState = (cause == DetachCause::NormalStop) ? AdapterState::Idle : AdapterState::Error;
	}
	///@}
};

/**
 * This class is an implementation of xpcc::I2cTransaction which,
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
class I2cWriteAdapter : protected I2cTransaction
{
	uint8_t address;
	uint8_t size;
	const uint8_t *buffer;
	volatile AdapterState adapterState;

public:
	I2cWriteAdapter(uint8_t address)
	:	address(address << 1), adapterState(AdapterState::Idle)
	{
	}

protected:
	/// @return `Busy` while an I2C operation is ongoing. Reinitialization
	///			is not permitted during this phase.
	AdapterState inline
	getAdapterState()
	{
		return adapterState;
	}

	/**
	 * Initializes the adapter with the required information for a write operation
	 *
	 * The slave address is not configured, so use this method during normal operation to change buffers.
	 *
	 * @param[in] 	buffer	buffer to be written to the slave
	 * @param		size	number of bytes to be written, set to `0` to write nothing
     *
     * @return  `true` if adapter was not busy and accepted the information,
     *          `false` otherwise
	 */
	bool inline
	configureWrite(const uint8_t *buffer, uint8_t size)
	{
		if (adapterState != AdapterState::Busy)
		{
			this->buffer = buffer;
			this->size = buffer ? size : 0;
			return true;
		}
		return false;
	}

	/// @internal
	///@{
	virtual bool
	attaching()
	{
		if (adapterState == AdapterState::Busy)
			return false;
		adapterState = AdapterState::Busy;
		return true;
	}

	virtual void
	starting(Starting &starting)
	{
		starting.address = address;
		starting.next = size ? Operation::Write : Operation::Stop;
	}

	virtual void
	reading(Reading &reading)
	{
		reading.buffer = 0;
		reading.length = 0;
		reading.next = OperationAfterRead::Stop;
	}

	virtual void
	writing(Writing &writing)
	{
		writing.buffer = buffer;
		writing.length = size;
		writing.next = OperationAfterWrite::Stop;
	}

	virtual void
	detaching(DetachCause cause)
	{
		adapterState = (cause == DetachCause::NormalStop) ? AdapterState::Idle : AdapterState::Error;
	}
	///@}
};

/**
 * This class is an implementation of xpcc::I2cTransaction which,
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
class I2cReadAdapter : protected I2cTransaction
{
	uint8_t address;
	uint8_t size;
	uint8_t *buffer;
	volatile AdapterState adapterState;

public:
	I2cReadAdapter(uint8_t address)
	:	address(address << 1), adapterState(AdapterState::Idle)
	{
	}

protected:
	/// @return `Busy` while an I2C operation is ongoing. Reinitialization
	///			is not permitted during this phase.
	AdapterState inline
	getAdapterState()
	{
		return adapterState;
	}

	/**
	 * Initializes the adapter with the required information for a read operation
	 *
	 * The slave address is not configured, so use this method during normal operation to change buffers.
	 *
	 * @param[out] 	buffer	buffer to be read from the slave
	 * @param		size	number of bytes to be read, set to `0` to read nothing
     *
     * @return  `true` if adapter was not busy and accepted the information,
     *          `false` otherwise
	 */
	bool inline
	configureRead(uint8_t * buffer, uint8_t size)
	{
		if (adapterState != AdapterState::Busy)
		{
			this->buffer = buffer;
			this->size = buffer ? size : 0;
			return true;
		}
		return false;
	}

	///@{
	/// @internal
	virtual bool
	attaching()
	{
		if (adapterState == AdapterState::Busy)
			return false;
		adapterState = AdapterState::Busy;
		return true;
	}

	virtual void
	starting(Starting &starting)
	{
		starting.address = address;
		starting.next = size ? Operation::Read : Operation::Stop;
	}

	virtual void
	reading(Reading &reading)
	{
		reading.buffer = buffer;
		reading.length = size;
		reading.next = OperationAfterRead::Stop;
	}

	virtual void
	writing(Writing &writing)
	{
		writing.buffer = 0;
		writing.length = 0;
		writing.next = OperationAfterWrite::Stop;
	}

	virtual void
	detaching(DetachCause cause)
	{
		adapterState = (cause == DetachCause::NormalStop) ? AdapterState::Idle : AdapterState::Error;
	}
	///@}
};

} // namespace xpcc

#endif // XPCC_I2C_WRITE_READ_ADAPTER_HPP
