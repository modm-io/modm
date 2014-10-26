// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIPHERAL_I2C_TRANSACTION_HPP
#define XPCC_PERIPHERAL_I2C_TRANSACTION_HPP

#include "i2c.hpp"

namespace xpcc
{

/**
 * Abstract class for transactions.
 *
 * For true asynchronous operation, the communication driver should
 * inherit from this class, allowing multi-stage driver design and
 * performance gain by premature data evaluation.
 * This transaction object will stay attached to the `I2cMaster` during whole operation.
 *
 * @see I2cMaster
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup	i2c
 */
class I2cTransaction : public ::xpcc::I2c
{
public:
	struct Starting
	{
		/**
		 * Contains the information required to begin an I2C transfer.
		 *
		 * @param	address	the slave address excluding read/write bit
		 * @param	next	operation following the successful addressing of the slave
		 */
		Starting(uint8_t address, OperationAfterStart next)
		:	address(address), next(next)
		{
		}

		uint8_t address;
		OperationAfterStart next;
	};

	struct Writing
	{
		/**
		 * Contains the information required to begin a write operation.
		 *
		 * @param	buffer	buffer containing the data to write to the slave
		 * @param	length	number of bytes to be written
		 * @param	next	operation following the successful writing to the slave
		 */
		Writing(const uint8_t *buffer, std::size_t length, OperationAfterWrite next)
		:	buffer(buffer), length(buffer ? length : 0), next(next)
		{
		}

		const uint8_t *buffer;
		std::size_t length;
		OperationAfterWrite next;
	};

	struct Reading
	{
		/**
		 * Contains the information required to begin a read operation.
		 *
		 * @param	buffer	buffer to be filled with the bytes received from the slave
		 * @param	length	number of bytes to be read
		 * @param	next	operation following the successful reading from the slave
		 */
		Reading(uint8_t *buffer, std::size_t length, OperationAfterRead next)
		:	buffer(buffer), length(buffer ? length : 0), next(next)
		{
		}

		uint8_t *buffer;
		std::size_t length;
		OperationAfterRead next;
	};

public:
	/**
	 * This method is called when the I2cMaster is not currently
	 * in use by another transaction and can be attached.
	 *
	 * @return	`true` if the I2cMaster should attach this transaction,
	 * 			`false` if it should not attach it.
	 */
	virtual bool
	attaching() = 0;

	/**
	 * This method is called when the I2cMaster is ready to (re-)start an operation.
	 *
	 * @return	the `Starting` struct containing slave address and the next operation
	 */
	virtual Starting
	starting() = 0;

	/**
	 * This method is called before the I2cMaster begins a write operation.
	 *
	 * @return	the `Writing` struct containing the write buffer and size and next operation
	 */
	virtual Writing
	writing() = 0;

	/**
	 * This method is called before the I2cMaster begins a read operation.
	 *
	 * @return	the `Reading` struct containing the read buffer and size and next operation
	 */
	virtual Reading
	reading() = 0;

	/**
	 * This is called when all transmissions finished and transaction object is about to
	 * be detached. The I2cMaster will not be free until this method returns.
	 *
	 * @param	cause	specifies whether the detachment was expected (`NormalStop`),
	 * 					or a error occurred (`ErrorCondition`), which can, but does not need
	 * 					to be reacted upon.
	 */
	virtual void
	detaching(DetachCause cause) = 0;
};

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
class I2cWriteReadAdapter : public I2cTransaction
{
public:
	///	@param	address	the slave address not yet shifted left (address < 128).
	I2cWriteReadAdapter(uint8_t address)
	:	address(address << 1), readSize(0), writeSize(0),
		readBuffer(nullptr), writeBuffer(nullptr),
		state(AdapterState::Idle), isReading(false)
	{
	}

	/// @return `Busy` while an I2C operation is ongoing. Reinitialization
	///			is not permitted during this phase.
	AdapterState inline
	getState()
	{
		return state;
	}

	/// @return `true` while adapter is busy
	bool inline
	isBusy()
	{
		return (state == AdapterState::Busy);
	}

	/**
	 * Initializes the adapter to only send the address without payload.
	 *
	 * @return  `true` if adapter was not in use and accepted the information,
	 *          `false` otherwise
	 */
	bool ALWAYS_INLINE
	configurePing()
	{
		return configureWriteRead(nullptr, 0, nullptr, 0);
	}

	/**
	 * Initializes the adapter with the required information for a write/read operation.
	 *
	 * @param[in] 	writeBuffer	buffer to be written to the slave
	 * @param		writeSize	number of bytes to be written, set to `0` to write nothing
	 * @param[out]	readBuffer	buffer to write the read bytes from the slave
	 * @param		readSize	number of bytes to be read, set to `0` to read nothing
	 *
	 * @return  `true` if adapter was not in use and accepted the information,
	 *          `false` otherwise
	 */
	bool inline
	configureWriteRead(const uint8_t *writeBuffer, std::size_t writeSize,
			uint8_t *readBuffer, std::size_t readSize)
	{
		if (state != AdapterState::Busy)
		{
			this->readBuffer = readBuffer;
			this->readSize = readSize;
			this->writeBuffer = writeBuffer;
			this->writeSize = writeSize;
			isReading = (writeSize == 0);
			return true;
		}
		return false;
	}

	/**
	 * Initializes the adapter with the required information for a write operation.
	 *
	 * @param[in] 	buffer	buffer to be written to the slave
	 * @param		size	number of bytes to be written, set to `0` to write nothing
	 *
	 * @return  `true` if adapter was not in use and accepted the information,
	 *          `false` otherwise
	 */
	bool ALWAYS_INLINE
	configureWrite(const uint8_t *buffer, std::size_t size)
	{
		return configureWriteRead(buffer, size, nullptr, 0);
	}

	/**
	 * Initializes the adapter with the required information for a read operation.
	 *
	 * @param[out] 	buffer	buffer to be read from the slave
	 * @param		size	number of bytes to be read, set to `0` to read nothing
	 *
	 * @return  `true` if adapter was not in use and accepted the information,
	 *          `false` otherwise
	 */
	bool ALWAYS_INLINE
	configureRead(uint8_t *buffer, std::size_t size)
	{
		return configureWriteRead(nullptr, 0, buffer, size);
	}

protected:
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
		Starting starting(address, OperationAfterStart::Stop);
		if (isReading) {
			if (readSize) starting.next = OperationAfterStart::Read;
		}
		else {
			if (writeSize) starting.next = OperationAfterStart::Write;
		}
		isReading = !isReading;

		return starting;
	}

	virtual Writing
	writing()
	{
		return  Writing(writeBuffer, writeSize,
						readSize ? OperationAfterWrite::Restart : OperationAfterWrite::Stop);
	}

	virtual Reading
	reading()
	{
		return Reading(readBuffer, readSize, OperationAfterRead::Stop);
	}

	virtual void
	detaching(DetachCause cause)
	{
		isReading = false;
		state = (cause == DetachCause::NormalStop) ? AdapterState::Idle : AdapterState::Error;
	}

	uint8_t address;
	std::size_t readSize;
	std::size_t writeSize;
	uint8_t *readBuffer;
	const uint8_t *writeBuffer;
	volatile AdapterState state;
	bool isReading;
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
class I2cWriteAdapter : public I2cTransaction
{
public:
	///	@param	address	the slave address not yet shifted left (address < 128).
	I2cWriteAdapter(uint8_t address)
	:	address(address << 1), size(0), buffer(nullptr), state(AdapterState::Idle)
	{
	}

	/// @return `Busy` while an I2C operation is ongoing. Reinitialization
	///			is not permitted during this phase.
	AdapterState inline
	getState()
	{
		return state;
	}

	/// @return `true` while adapter is busy
	bool inline
	isBusy()
	{
		return (state == AdapterState::Busy);
	}

	/**
	 * Initializes the adapter to only send the address without payload.
	 *
	 * @return  `true` if adapter was not in use and accepted the information,
	 *          `false` otherwise
	 */
	bool ALWAYS_INLINE
	configurePing()
	{
		return configureWrite(nullptr, 0);
	}

	/**
	 * Initializes the adapter with the required information for a write operation.
	 *
	 * @param[in] 	buffer	buffer to be written to the slave
	 * @param		size	number of bytes to be written, set to `0` to write nothing
	 *
	 * @return  `true` if adapter was not in use and accepted the information,
	 *          `false` otherwise
	 */
	bool inline
	configureWrite(const uint8_t *buffer, std::size_t size)
	{
		if (state != AdapterState::Busy)
		{
			this->buffer = buffer;
			this->size = buffer ? size : 0;
			return true;
		}
		return false;
	}

protected:
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
		return Starting(address, size ? OperationAfterStart::Write : OperationAfterStart::Stop);
	}

	virtual Writing
	writing()
	{
		return Writing(buffer, size, OperationAfterWrite::Stop);
	}

	virtual Reading
	reading()
	{
		return Reading(nullptr, 0, OperationAfterRead::Stop);
	}

	virtual void
	detaching(DetachCause cause)
	{
		state = (cause == DetachCause::NormalStop) ? AdapterState::Idle : AdapterState::Error;
	}

	uint8_t address;
	std::size_t size;
	const uint8_t *buffer;
	volatile AdapterState state;
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
class I2cReadAdapter : public I2cTransaction
{
public:
	///	@param	address	the slave address not yet shifted left (address < 128).
	I2cReadAdapter(uint8_t address)
	:	address(address << 1), size(0), buffer(nullptr), state(AdapterState::Idle)
	{
	}

	/// @return `Busy` while an I2C operation is ongoing. Reinitialization
	///			is not permitted during this phase.
	AdapterState inline
	getState()
	{
		return state;
	}

	/// @return `true` while adapter is busy
	bool inline
	isBusy()
	{
		return (state == AdapterState::Busy);
	}

	/**
	 * Initializes the adapter to only send the address without payload.
	 *
	 * @return  `true` if adapter was not in use and accepted the information,
	 *          `false` otherwise
	 */
	bool ALWAYS_INLINE
	configurePing()
	{
		return configureRead(nullptr, 0);
	}

	/**
	 * Initializes the adapter with the required information for a read operation.
	 *
	 * @param[out] 	buffer	buffer to be read from the slave
	 * @param		size	number of bytes to be read, set to `0` to read nothing
	 *
	 * @return  `true` if adapter was not in use and accepted the information,
	 *          `false` otherwise
	 */
	bool inline
	configureRead(uint8_t * buffer, std::size_t size)
	{
		if (state != AdapterState::Busy)
		{
			this->buffer = buffer;
			this->size = size;
			return true;
		}
		return false;
	}

protected:
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
		return Starting(address, size ? OperationAfterStart::Read : OperationAfterStart::Stop);
	}

	virtual Writing
	writing()
	{
		return Writing(nullptr, 0, OperationAfterWrite::Stop);
	}

	virtual Reading
	reading()
	{
		return Reading(buffer, size, OperationAfterRead::Stop);
	}

	virtual void
	detaching(DetachCause cause)
	{
		state = (cause == DetachCause::NormalStop) ? AdapterState::Idle : AdapterState::Error;
	}

	uint8_t address;
	std::size_t size;
	uint8_t *buffer;
	volatile AdapterState state;
};

/**
 * This wrapper allows assigning a tag to an I2C transaction, to synchronize multiple
 * access to the I2C resource.
 *
 * After a transaction successfully started, the tag will be set to a non-zero value,
 * and will be reset to zero by this wrapper upon detaching.
 * If the transaction concluded without error, the tag's value will be copied into
 * the `success` integer, otherwise it will also be reset to zero.
 *
 * @author	Niklas Hauser
 * @ingroup	i2c
 */
template< class I2cAdapter >
class I2cTagAdapter : public I2cAdapter
{
	volatile uint8_t &tag;
	volatile uint8_t &success;

public:
	/**
	 * @param	address	the slave address not yet shifted left (address < 128).
	 * @param	tag		reference to a tag integer.
	 * @param	success	reference to a success integer.
	 */
	I2cTagAdapter(uint8_t address, volatile uint8_t &tag, volatile uint8_t &success)
	:	I2cAdapter(address), tag(tag), success(success)
	{
	}

protected:
	virtual void
	detaching(xpcc::I2c::DetachCause cause)
	{
		I2cAdapter::detaching(cause);
		// if no error occurred, copy the tag, else set success to zero
		success = (I2cAdapter::getState() != xpcc::I2c::AdapterState::Error) ? tag : 0;
		// delete the tag
		tag = 0;
	}
};

}	// namespace xpcc

#endif // XPCC_PERIPHERAL_I2C_TRANSACTION_HPP
