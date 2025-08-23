/*
 * Copyright (c) 2014-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_I2C_DEVICE_HPP
#define MODM_INTERFACE_I2C_DEVICE_HPP

#include "i2c.hpp"
#include "i2c_master.hpp"
#include "i2c_transaction.hpp"
#include <modm/processing/fiber.hpp>

namespace modm
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
 * @ingroup modm_architecture_i2c_device
 */
template < class I2cMaster, uint8_t NestingLevels = 10, class Transaction = I2cWriteReadTransaction >
class I2cDevice : protected modm::NestedResumable< NestingLevels + 1 >
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
	bool
	ping()
	{
		modm::this_fiber::poll([&]{ return transaction.configurePing(); });
		return runTransaction();
	}

	/// Starts a write-read transaction and waits until finished.
	bool
	writeRead(const uint8_t *writeBuffer, std::size_t writeSize,
			  uint8_t *readBuffer, std::size_t readSize)
	{
		modm::this_fiber::poll([&]{ return transaction.configureWriteRead(writeBuffer, writeSize, readBuffer, readSize); });
		return runTransaction();
	}

	/// Starts a write transaction and waits until finished.
	bool
	write(const uint8_t *buffer, std::size_t size)
	{
		modm::this_fiber::poll([&]{ return transaction.configureWrite(buffer, size); });
		return runTransaction();
	}

	/// Starts a write transaction and waits until finished.
	bool
	read(uint8_t *buffer, std::size_t size)
	{
		modm::this_fiber::poll([&]{ return transaction.configureRead(buffer, size); });
		return runTransaction();
	}

protected:
	/// Starts the transaction with our own transaction object.
	bool inline
	startTransaction()
	{
		return startTransaction(&this->transaction);
	}

	/// Starts the transaction with a separate transaction object.
	bool inline
	startTransaction(modm::I2cTransaction *transaction)
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
		return (transaction.getState() != modm::I2c::TransactionState::Error);
	}

	/// Starts our own transaction and waits until finished.
	bool
	runTransaction()
	{
		modm::this_fiber::poll([&]{ return startTransaction(); });
		modm::this_fiber::poll([&]{ return isTransactionRunning(); });
		return wasTransactionSuccessful();
	}

protected:
	Transaction transaction;
private:
	I2c::ConfigurationHandler configuration;
};

}	// namespace modm

#endif // MODM_INTERFACE_I2C_DEVICE_HPP
