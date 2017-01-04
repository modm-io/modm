/*
 * Copyright (c) 2012-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_BMA180_HPP
#	error  "Don't include this file directly, use 'bma180.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Bma180<I2cMaster>::Bma180(uint8_t* data, uint8_t address)
:	I2cWriteReadTransaction(address), status(0), data(data)
{
	configureWriteRead(buffer, 0, data, 0);
}

template < typename I2cMaster >
bool
xpcc::Bma180<I2cMaster>::configure(bma180::Range range, bma180::Bandwidth bandwidth, bma180::ModeConfig mode, bool interrupt)
{
	// enable image writing
	bool ok;
	ok = writeRegister(bma180::REGISTER_CTRL0, bma180::EE_W);
	// set range and sample skipping
	ok &= writeMaskedRegister(bma180::REGISTER_OFFSET_LSB1, bma180::RANGE | bma180::SAMPLE_SKIPPING, bma180::SAMPLE_SKIPPING | range);
	// set bandwidth
	ok &= writeRegister(bma180::REGISTER_BW_TCS, bandwidth);
	// set mode configuration
	ok &= writeMaskedRegister(bma180::REGISTER_TRF_Z, bma180::MODE_CONFIG, mode);
	// set interrupt
	if (interrupt) ok &= writeRegister(bma180::REGISTER_CTRL3, bma180::NEW_DATA_INT);
	
	return ok;
}

template < typename I2cMaster >
void
xpcc::Bma180<I2cMaster>::readAccelerometer()
{
	status |= READ_ACCELEROMETER_PENDING;
}

template < typename I2cMaster >
bool
xpcc::Bma180<I2cMaster>::isNewDataAvailable()
{
	return status & NEW_ACCELEROMETER_DATA;
}

template < typename I2cMaster >
uint8_t*
xpcc::Bma180<I2cMaster>::getData()
{
	status &= ~NEW_ACCELEROMETER_DATA;
	return data;
}

template < typename I2cMaster >
void
xpcc::Bma180<I2cMaster>::update()
{
	if (status & READ_ACCELEROMETER_RUNNING &&
		getAdapterState() == xpcc::I2c::AdapterState::Idle) {
		status &= ~READ_ACCELEROMETER_RUNNING;
		status |= NEW_ACCELEROMETER_DATA;
	}
	else if (status & READ_ACCELEROMETER_PENDING)
	{
		buffer[0] = bma180::REGISTER_DATA_X0;
		configureWriteRead(buffer, 1, data, 7);
		
		if (I2cMaster::start(this)) {
			status &= ~READ_ACCELEROMETER_PENDING;
			status |= READ_ACCELEROMETER_RUNNING;
		}
	}
}

template < typename I2cMaster >
bool
xpcc::Bma180<I2cMaster>::reset()
{
	return writeRegister(bma180::REGISTER_RESET, bma180::RESET);
}

template < typename I2cMaster >
bool
xpcc::Bma180<I2cMaster>::writeMaskedRegister(bma180::Register reg, uint8_t mask, uint8_t value)
{
	readRegister(reg);
	return writeRegister(reg, (buffer[0] & ~mask) | value);
}

// MARK: - private
template < typename I2cMaster >
bool
xpcc::Bma180<I2cMaster>::writeRegister(bma180::Register reg, uint8_t value)
{
	while (getAdapterState() == xpcc::I2c::AdapterState::Busy)
		;
	buffer[0] = reg;
	buffer[1] = value;
	configureWriteRead(buffer, 2, data, 0);
	
	return I2cMaster::startBlocking(this);
}

template < typename I2cMaster >
uint8_t
xpcc::Bma180<I2cMaster>::readRegister(bma180::Register reg)
{
	while (getAdapterState() == xpcc::I2c::AdapterState::Busy)
		;
	buffer[0] = reg;
	configureWriteRead(buffer, 1, buffer, 1);
	
	while (!I2cMaster::startBlocking(this))
		;
	return buffer[0];
}


