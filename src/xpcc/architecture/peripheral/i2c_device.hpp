// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIPHERAL_I2C_DEVICE_HPP
#define XPCC_PERIPHERAL_I2C_DEVICE_HPP

#include "i2c.hpp"
#include "i2c_master.hpp"
#include "i2c_transaction.hpp"

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
template < class I2cMaster >
class I2cDevice
{
	I2c::Configuration_t *configuration;

public:
	I2cDevice()
	:	configuration(nullptr)
	{
	}

	void inline
	attachPeripheralConfiguration(I2c::Configuration_t *configuration)
	{
		this->configuration = configuration;
	}

protected:
	bool inline
	startTransaction(xpcc::I2cTransaction *transaction)
	{
		return I2cMaster::start(transaction, this->configuration);
	}

	bool inline
	startBlockingTransaction(xpcc::I2cTransaction *transaction)
	{
		return I2cMaster::startBlocking(transaction, this->configuration);
	}
};

}	// namespace xpcc

#endif // XPCC_PERIPHERAL_I2C_DEVICE_HPP
