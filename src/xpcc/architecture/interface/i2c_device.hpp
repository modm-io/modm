// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_INTERFACE_I2C_DEVICE_HPP
#define XPCC_INTERFACE_I2C_DEVICE_HPP

#include "i2c.hpp"
#include "i2c_master.hpp"
#include "i2c_transaction.hpp"
#include <xpcc/processing/resumable.hpp>

namespace xpcc
{

/**
 * Base class of an I2C Device.
 *
 * This class provides functions for configuring the adapter and peripheral and starting transactions.
 * It also provides a basic `ping()` resumable function, which checks if the device responds to its address.
 *
 * You need to provide the number of required nesting levels for your driver here.
 * By default, 10 levels are chosen, which should be more than enough in any case.
 *
 * You can choose to use your own custom I2c Transaction Adapter, by providing a class subclassed from
 * an existing I2cAdapter or which implements the I2cTransaction interface manually.
 *
 * @tparam	I2cMaster		an I2cMaster conforming to the I2cMaster interface.
 * @tparam	NestingLevels	number of nesting levels required for your driver
 * @tparam	Transaction		a class conforming to the I2cTransaction interface.
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup i2c
 */
template < class I2cMaster, uint8_t NestingLevels = 10, class Transaction = I2cWriteReadTransaction >
class I2cDevice : protected xpcc::co::NestedResumable< NestingLevels + 1 >
{
public:
	///	@param	address	the slave address not yet shifted left (address < 128).
	I2cDevice(uint8_t address)
	:	transaction(address), configuration(nullptr)
	{
	}

	/// Sets a new address of the slave device.
	///	@param	address	the slave address not yet shifted left (address < 128).
	void
	setAddress(uint8_t address)
	{
		transaction.setAddress(address);
	}

	/// Attaches a configuration handler, which is called before a transaction,
	/// whenever the configuration has to be changed.
	void inline
	attachConfigurationHandler(I2c::ConfigurationHandler handler)
	{
		configuration = handler;
	}

	/// @retval true	device responds to address
	/// @retval false	no device with address found
	xpcc::co::ResumableResult<bool>
	ping()
	{
		CO_BEGIN();

		CO_WAIT_UNTIL( transaction.configurePing() and startTransaction() );

		CO_WAIT_WHILE( isTransactionRunning() );

		CO_END_RETURN( wasTransactionSuccessful() );
	}

protected:
	/// Configures the transaction with a write/read operation and starts it.
	bool inline
	startWriteRead(const uint8_t *writeBuffer, std::size_t writeSize,
			uint8_t *readBuffer, std::size_t readSize)
	{
		return ( transaction.configureWriteRead(writeBuffer, writeSize, readBuffer, readSize) and
				startTransaction() );
	}

	/// Configures the transaction with a write operation and starts it.
	bool inline
	startWrite(const uint8_t *buffer, std::size_t size)
	{
		return ( transaction.configureWrite(buffer, size) and
				startTransaction() );
	}

	/// Configures the transaction with a read operation and starts it.
	bool inline
	startRead(uint8_t *buffer, std::size_t size)
	{
		return ( transaction.configureRead(buffer, size) and
				startTransaction() );
	}

protected:
	/// Starts the transaction with our own transaction object.
	bool inline
	startTransaction()
	{
		return startTransaction(&this->transaction);
	}

	/// Starts the transaction with a seperate transaction object.
	bool inline
	startTransaction(xpcc::I2cTransaction *transaction)
	{
		return I2cMaster::start(transaction, configuration);
	}

	/// @returns `true` when transaction is busy.
	bool inline
	isTransactionRunning()
	{
		return transaction.isBusy();
	}

	/// @returns `true` when transaction did not return an error.
	bool inline
	wasTransactionSuccessful()
	{
		return (transaction.getState() != xpcc::I2c::TransactionState::Error);
	}

	/// Starts our own transaction and waits until finished.
	xpcc::co::ResumableResult<bool>
	runTransaction()
	{
		CO_BEGIN();

		CO_WAIT_UNTIL( startTransaction() );

		CO_WAIT_WHILE( isTransactionRunning() );

		CO_END_RETURN( wasTransactionSuccessful() );
	}

protected:
	Transaction transaction;
private:
	I2c::ConfigurationHandler configuration;
};

}	// namespace xpcc

#endif // XPCC_INTERFACE_I2C_DEVICE_HPP
