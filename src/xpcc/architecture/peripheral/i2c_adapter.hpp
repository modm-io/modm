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
class I2cWriteReadAdapter : public I2cTransaction
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
	:	state(AdapterState::Idle)
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

private:
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
		state = (cause == DetachCause::NormalStop) ? AdapterState::Idle : AdapterState::Error;
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
class I2cWriteAdapter : public I2cTransaction
{
private:
	uint8_t address;
	uint8_t size;
	const uint8_t *buffer;
	volatile AdapterState state;

public:
	I2cWriteAdapter()
	:	state(AdapterState::Idle)
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

private:
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
		state = (cause == DetachCause::NormalStop) ? AdapterState::Idle : AdapterState::Error;
	}
	///@}
};

/**
 * This class is an implementation of xpcc::I2cDelegate which,
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
class I2cReadAdapter : public I2cTransaction
{
private:
	uint8_t address;
	uint8_t size;
	uint8_t *buffer;
	volatile AdapterState state;

public:
	I2cReadAdapter()
	:	state(AdapterState::Idle)
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

private:
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
		state = (cause == DetachCause::NormalStop) ? AdapterState::Idle : AdapterState::Error;
	}
	///@}
};

} // namespace xpcc

#endif // XPCC_I2C_WRITE_READ_ADAPTER_HPP
