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
#include <xpcc/processing/coroutine.hpp>

namespace xpcc
{

/**
 * Base class of an I2C Device.
 *
 * This class provides functions for configuring the adapter and peripheral and starting transactions.
 * It also provides a basic `ping()` coroutine, which checks if the device responds to its address.
 *
 * You need to provide the number of required nesting levels for your driver here.
 * By default, 10 levels are chosen, which should be more than enough in any case.
 *
 * You can choose to use your own custom I2c Transaction Adapter, by providing a class subclassed from
 * an existing I2cAdapter or which implements the I2cTransaction interface manually.
 *
 * @tparam	I2cMaster		an I2cMaster conforming to the I2cMaster interface.
 * @tparam	NestingLevels	number of nesting levels required for your driver
 * @tparam	Adapter			an I2c Adapter conforming to the I2cTransaction interface.
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup i2c
 */
template < class I2cMaster, uint8_t NestingLevels = 10, class Adapter = I2cWriteReadAdapter >
class I2cDevice : protected xpcc::co::NestedCoroutine< NestingLevels >
{
public:
	///	@param	address	the slave address not yet shifted left (address < 128).
	I2cDevice(uint8_t address)
	:	adapter(address), configuration(nullptr)
	{
	}

	/// Sets a new address of the slave device.
	///	@param	address	the slave address not yet shifted left (address < 128).
	void
	setAddress(uint8_t address)
	{
		adapter.setAddress(address);
	}

	void inline
	attachConfigurationHandler(I2c::ConfigurationHandler handler)
	{
		configuration = handler;
	}

	/// @retval `true`	device responds to address
	/// @retval `false`	not device with address found
	xpcc::co::Result<bool>
	ping()
	{
		CO_BEGIN();

		CO_WAIT_UNTIL( adapter.configurePing() and startTransaction(&adapter) );

		CO_WAIT_WHILE( isTransactionRunning() );

		CO_END_RETURN( wasTransactionSuccessful() );
	}

protected:
	/// Configures the adapter with a write/read operation and starts the transaction.
	bool inline
	startWriteRead(const uint8_t *writeBuffer, std::size_t writeSize,
			uint8_t *readBuffer, std::size_t readSize)
	{
		return ( adapter.configureWriteRead(writeBuffer, writeSize, readBuffer, readSize) and
				startTransaction() );
	}

	/// Configures the adapter with a write operation and starts the transaction.
	bool inline
	startWrite(const uint8_t *buffer, std::size_t size)
	{
		return ( adapter.configureWrite(buffer, size) and
				startTransaction() );
	}

	/// Configures the adapter with a read operation and starts the transaction.
	bool inline
	startRead(uint8_t *buffer, std::size_t size)
	{
		return ( adapter.configureRead(buffer, size) and
				startTransaction() );
	}

	/// Starts the transaction with the declared transaction object.
	/// @param	transaction	pointer to transaction object, `nullptr` for own adapter.
	bool inline
	startTransaction(xpcc::I2cTransaction *transaction = nullptr)
	{
		return I2cMaster::start(transaction ? transaction : &adapter, configuration);
	}

protected:
	/// @returns `true` when adapter is busy.
	bool inline
	isTransactionRunning()
	{
		return adapter.isBusy();
	}

	/// @returns `true` when adapter did not return an error.
	bool inline
	wasTransactionSuccessful()
	{
		return (adapter.getState() != xpcc::I2c::AdapterState::Error);
	}

protected:
	Adapter adapter;
private:
	I2c::ConfigurationHandler configuration;
};

}	// namespace xpcc

#endif // XPCC_INTERFACE_I2C_DEVICE_HPP
