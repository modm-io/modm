/*
 * Copyright (c) 2014-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_I2C_TRANSACTION_HPP
#define MODM_INTERFACE_I2C_TRANSACTION_HPP

#include "i2c.hpp"

namespace modm
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
 * @ingroup	modm_architecture_i2c
 */
class I2cTransaction : public ::modm::I2c
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
	///	@param	address	the slave address not yet shifted left (address < 128).
	I2cTransaction(uint8_t address) :
		address(address << 1), state(TransactionState::Idle)
	{}

	///	@param	address	the slave address not yet shifted left (address < 128).
	void inline
	setAddress(uint8_t address)
	{
		this->address = address << 1;
	}

	/// @return `Busy` while an I2C operation is ongoing. Reinitialization
	///			is not permitted during this phase.
	TransactionState
	getState() const
	{
		return state;
	}

	/// @return `true` while adapter is busy
	bool
	isBusy() const
	{
		return (state == TransactionState::Busy);
	}

	/**
	 * Initializes the adapter to only send the address without payload.
	 *
	 * @retval `true` adapter configured successfully,
	 * @retval `false` adapter busy
	 */
	bool
	configurePing();

	/**
	 * Initializes the adapter with the required information for a write/read operation.
	 *
	 * @param[in] 	writeBuffer	buffer to be written to the slave
	 * @param		writeSize	number of bytes to be written, set to `0` to write nothing
	 * @param[out]	readBuffer	buffer to write the read bytes from the slave
	 * @param		readSize	number of bytes to be read, set to `0` to read nothing
	 *
	 * @retval `true` adapter configured successfully,
	 * @retval `false` adapter busy
	 */
	bool
	configureWriteRead(const uint8_t *writeBuffer, std::size_t writeSize,
							 uint8_t *readBuffer,  std::size_t readSize);

	/**
	 * Initializes the adapter with the required information for a write operation.
	 *
	 * @param[in] 	buffer	buffer to be written to the slave
	 * @param		size	number of bytes to be written, set to `0` to write nothing
	 *
	 * @retval `true` adapter configured successfully,
	 * @retval `false` adapter busy
	 */
	bool
	configureWrite(uint8_t *buffer, std::size_t size);

	/**
	 * Initializes the adapter with the required information for a read operation.
	 *
	 * @param[out] 	buffer	buffer to be read from the slave
	 * @param		size	number of bytes to be read, set to `0` to read nothing
	 *
	 * @retval `true` adapter configured successfully,
	 * @retval `false` adapter busy
	 */
	bool
	configureRead(uint8_t *buffer, std::size_t size);

public:
	/**
	 * This method is called when the I2cMaster is not currently
	 * in use by another transaction and can be attached.
	 *
	 * @return	`true` if the I2cMaster should attach this transaction,
	 * 			`false` if it should not attach it.
	 */
	virtual bool
	attaching()
	{
		if (state == TransactionState::Busy)
			return false;
		state = TransactionState::Busy;
		return true;
	}

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
	detaching(DetachCause cause)
	{
		state = (cause == DetachCause::NormalStop) ? TransactionState::Idle : TransactionState::Error;
	}

protected:
	uint8_t address;
	volatile TransactionState state;
};

/**
 * This class is an implementation of modm::I2cTransaction which,
 * when passed to an i2c driver, performs the sequence:
 * start - address - write - restart - address - read - stop.
 *
 * After initialization and performing operation this class keeps
 * the references to passed buffers and may be reused again without
 * reinitialization as long the passed buffers remain valid.
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup	modm_architecture_i2c
 */
class I2cWriteReadTransaction : public I2cTransaction
{
public:
	I2cWriteReadTransaction(uint8_t address) :
		I2cTransaction(address), readSize(0), writeSize(0),
		readBuffer(nullptr), writeBuffer(nullptr), isReading(false)
	{
	}

	bool
	configurePing()
	{
		return configureWriteRead(nullptr, 0, nullptr, 0);
	}

	bool inline
	configureWriteRead(const uint8_t *writeBuffer, std::size_t writeSize,
			uint8_t *readBuffer, std::size_t readSize)
	{
		if (state != TransactionState::Busy)
		{
			this->readBuffer = readBuffer;
			this->readSize = readSize;
			this->writeBuffer = writeBuffer;
			this->writeSize = writeSize;

			/* `isReading` a bit tricky to understand, so here is an explanation:
			 *
			 * There are three possibilities right after an I2C start:
			 * 1. Stop communication (used for ping),
			 * 2. Start Writing something,
			 * 3. Start Reading something.
			 *
			 * Number 1 is easy, we can simply check if both readSize and writeSize are zero.
			 *
			 * 2 and 3 seem easy too, simply check if writeSize or readSize is non-zero respectively. No?
			 * NO! We want to keep the writeSize and readSize persistent, so that you do not need to
			 * reconfigure the adapter if nothing changed.
			 *
			 * So assume the following sequence with both writeSize and readSize non-zero:
			 * start - write - restart - read - ...
			 *
			 * During the first `starting()`, we check `writeSize != 0`, and request next operation `Write`.
			 * Then, during the next restart we will check `writeSize != 0`, but because it has not changed,
			 * we will never be able to enter the reading phase.
			 * We cannot check `readSize != 0` first either, because we encounter the same problem.
			 *
			 * Ok, so we use `isReading` here to signal, whether we want to write or read during `starting()`.
			 */
			isReading = (writeSize == 0);
			// however, there is another problem with `isReading`, see description in `writing()`

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
	 * @return  `true` if adapter was not in use,
	 *          `false` otherwise
	 */
	bool
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
	 * @return  `true` if adapter was not in use,
	 *          `false` otherwise
	 */
	bool
	configureRead(uint8_t *buffer, std::size_t size)
	{
		return configureWriteRead(nullptr, 0, buffer, size);
	}

protected:
	Starting
	starting() override
	{
		Starting starting(address, OperationAfterStart::Stop);
		if (isReading) {
			if (readSize) starting.next = OperationAfterStart::Read;
		}
		else {
			if (writeSize) starting.next = OperationAfterStart::Write;
		}

		// This won't work here, see `writing()` for reason
		// isReading = !isReading;

		return starting;
	}

	Writing
	writing() override
	{
		/* So the `isReading` adventure continues here.
		 * The code shown in `starting()` does work, but only for the specific sequence of this adapter:
		 *     `start - write - restart - read - stop`
		 *
		 * The code starts to break once you have the sequence:
		 *     `start - write - write - ...`
		 * because during the second write `isReading` is suddenly true and we either Stop or start Reading.
		 *
		 * While the I2C standard prohibits any continued readings, as in
		 *     `start - read - read - ...`,
		 * the sequence:
		 *     `start - read - restart - read - ...`
		 * would lead to similar problems.
		 *
		 * When would these sequences ever arise, you ask?
		 * Well, when this Adapter is subclassed and `writing()` is overwritten so that
		 * a few bytes can be prepended to the actual payload.
		 *
		 * This would be implemented as such (prepending 2 address bytes to a block of data taken from I2cEeprom):
		 *
		 *     if (writeAddress) {
		 *         writeAddress = false;
		 *         return Writing(addressBuffer, 2, OperationAfterWrite::Write);
		 *     }
		 *     return I2cWriteReadTransaction::writing();
		 *
		 * This would call `starting()` twice, without modifying `isReading` until this method is called.
		 */
		isReading = (readSize != 0);
		return  Writing(writeBuffer, writeSize,
						isReading ? OperationAfterWrite::Restart : OperationAfterWrite::Stop);
	}

	Reading
	reading() override
	{
		return Reading(readBuffer, readSize, OperationAfterRead::Stop);
	}

protected:
	std::size_t readSize;
	std::size_t writeSize;
	uint8_t *readBuffer;
	const uint8_t *writeBuffer;
	bool isReading;
};

/**
 * This class is an implementation of modm::I2cTransaction which,
 * when passed to an i2c driver, performs the sequence:
 * start - address - write - stop.
 *
 * After initialization and performing operation this class remains
 * holding references to passed buffers and may be reused again without
 * reinitialization as long the passed buffers remain valid.
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup	modm_architecture_i2c
 */
class I2cWriteTransaction : public I2cTransaction
{
public:
	I2cWriteTransaction(uint8_t address)
	:	I2cTransaction(address), size(0), buffer(nullptr)
	{
	}

	bool
	configurePing()
	{
		return configureWrite(nullptr, 0);
	}

	bool
	configureWriteRead(const uint8_t *, std::size_t, uint8_t *, std::size_t)
	{
		return false;
	}

	bool inline
	configureWrite(const uint8_t *buffer, std::size_t size)
	{
		if (state != TransactionState::Busy)
		{
			this->buffer = buffer;
			this->size = buffer ? size : 0;
			return true;
		}
		return false;
	}

	bool
	configureRead(uint8_t *, std::size_t)
	{
		return false;
	}

protected:
	Starting
	starting() override
	{
		return Starting(address, size ? OperationAfterStart::Write : OperationAfterStart::Stop);
	}

	Writing
	writing() override
	{
		return Writing(buffer, size, OperationAfterWrite::Stop);
	}

	Reading
	reading() override
	{
		return Reading(nullptr, 0, OperationAfterRead::Stop);
	}

protected:
	std::size_t size;
	const uint8_t *buffer;
};

/**
 * This class is an implementation of modm::I2cTransaction which,
 * when passed to an i2c driver, performs the sequence:
 * start - address - read - stop.
 *
 * After initialization and performing operation this class remains
 * holding references to passed buffers and may be reused again without
 * reinitialization as long the passed buffers remain valid.
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup	modm_architecture_i2c
 */
class I2cReadTransaction : public I2cTransaction
{
public:
	I2cReadTransaction(uint8_t address)
	:	I2cTransaction(address), size(0), buffer(nullptr)
	{
	}

	bool
	configurePing()
	{
		return configureRead(nullptr, 0);
	}

	bool
	configureWriteRead(const uint8_t *, std::size_t, uint8_t *, std::size_t)
	{
		return false;
	}

	bool
	configureWrite(uint8_t *, std::size_t)
	{
		return false;
	}

	bool inline
	configureRead(uint8_t *buffer, std::size_t size)
	{
		if (state != TransactionState::Busy)
		{
			this->buffer = buffer;
			this->size = size;
			return true;
		}
		return false;
	}

protected:
	Starting
	starting() override
	{
		return Starting(address, size ? OperationAfterStart::Read : OperationAfterStart::Stop);
	}

	Writing
	writing() override
	{
		return Writing(nullptr, 0, OperationAfterWrite::Stop);
	}

	Reading
	reading() override
	{
		return Reading(buffer, size, OperationAfterRead::Stop);
	}

	std::size_t size;
	uint8_t *buffer;
};

}	// namespace modm

#endif // MODM_INTERFACE_I2C_TRANSACTION_HPP
