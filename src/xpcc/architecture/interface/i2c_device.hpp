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
 * This class provides functions for configuring the peripheral.
 *
 * @author	Georgi Grinshpun
 * @author	Niklas Hauser
 * @ingroup i2c
 */
template < class I2cMaster, uint8_t Levels = 10, class Adapter = I2cWriteReadAdapter >
class I2cDevice : protected xpcc::co::NestedCoroutine< Levels >
{
public:
	I2cDevice(uint8_t address)
	:	adapter(address), configuration(nullptr)
	{
	}

	void inline
	attachConfigurationHandler(I2c::ConfigurationHandler handler)
	{
		configuration = handler;
	}

	xpcc::co::Result<bool>
	ping()
	{
		CO_BEGIN();

		CO_WAIT_UNTIL( adapter.configurePing() and startTransaction(&adapter) );

		CO_WAIT_WHILE( isTransactionRunning() );

		CO_END_RETURN( wasTransactionSuccessful() );
	}

protected:
	bool inline
	startWriteRead(const uint8_t *writeBuffer, std::size_t writeSize,
			uint8_t *readBuffer, std::size_t readSize)
	{
		return ( adapter.configureWriteRead(writeBuffer, writeSize, readBuffer, readSize) and
				startTransaction(&adapter) );
	}

	bool inline
	startWrite(const uint8_t *buffer, std::size_t size)
	{
		return ( adapter.configureWrite(buffer, size) and
				startTransaction(&adapter) );
	}

	bool inline
	startRead(uint8_t *buffer, std::size_t size)
	{
		return ( adapter.configureRead(buffer, size) and
				startTransaction(&adapter) );
	}

	bool inline
	startTransaction(xpcc::I2cTransaction *transaction)
	{
		return I2cMaster::start(transaction, configuration);
	}

protected:
	bool inline
	isTransactionRunning()
	{
		return adapter.isBusy();
	}

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
