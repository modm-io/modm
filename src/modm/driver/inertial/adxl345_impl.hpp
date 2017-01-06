/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ADXL345_HPP
#	error  "Don't include this file directly, use 'adxl345.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
modm::Adxl345<I2cMaster>::Adxl345(uint8_t* data, uint8_t address)
:	I2cWriteReadTransaction(address), status(0), data(data)
{
	configureWriteRead(buffer, 0, data, 0);
}

template < typename I2cMaster >
bool
modm::Adxl345<I2cMaster>::configure(adxl345::Bandwidth bandwidth, bool streamMode, bool enableInterrupt)
{
	bool ok = writeRegister(adxl345::REGISTER_POWER_CTL, adxl345::POWER_MEASURE);
	ok &= writeRegister(adxl345::REGISTER_DATA_FORMAT, adxl345::DATAFORMAT_FULL_RES);
	ok &= writeRegister(adxl345::REGISTER_BW_RATE, bandwidth);
	if (enableInterrupt) ok &= writeRegister(adxl345::REGISTER_INT_ENABLE, adxl345::INTERRUPT_DATA_READY);
	if (streamMode) ok &= writeRegister(adxl345::REGISTER_FIFO_CTL, adxl345::FIFO_CTL_MODE_STREAM);
	return ok;
}

template < typename I2cMaster >
void
modm::Adxl345<I2cMaster>::readAccelerometer()
{
	status |= READ_ACCELEROMETER_PENDING;
}

template < typename I2cMaster >
bool
modm::Adxl345<I2cMaster>::isDataReady()
{
	return readRegister(adxl345::REGISTER_INT_SOURCE) & adxl345::INTERRUPT_DATA_READY;
}

template < typename I2cMaster >
bool
modm::Adxl345<I2cMaster>::isNewDataAvailable()
{
	return status & NEW_ACCELEROMETER_DATA;
}

template < typename I2cMaster >
uint8_t*
modm::Adxl345<I2cMaster>::getData()
{
	status &= ~NEW_ACCELEROMETER_DATA;
	return data;
}

template < typename I2cMaster >
void
modm::Adxl345<I2cMaster>::update()
{
	if (status & READ_ACCELEROMETER_RUNNING &&
		getAdapterState() == modm::I2c::AdapterState::Idle) {
		status &= ~READ_ACCELEROMETER_RUNNING;
		status |= NEW_ACCELEROMETER_DATA;
	}
	else if (status & READ_ACCELEROMETER_PENDING) 
	{
		buffer[0] = adxl345::REGISTER_DATA_X0;
		configureWriteRead(buffer, 1, data, 6);
		
		if (I2cMaster::start(this)) {
			status &= ~READ_ACCELEROMETER_PENDING;
			status |= READ_ACCELEROMETER_RUNNING;
		}
	}
}

// ----------------------------------------------------------------------------
template < typename I2cMaster >
bool
modm::Adxl345<I2cMaster>::writeRegister(adxl345::Register reg, uint8_t value)
{
	while (getAdapterState() == modm::I2c::AdapterState::Busy)
		;
	buffer[0] = reg;
	buffer[1] = value;
	configureWriteRead(buffer, 2, data, 0);
	
	return I2cMaster::startBlocking(this);
}

template < typename I2cMaster >
uint8_t
modm::Adxl345<I2cMaster>::readRegister(adxl345::Register reg)
{
	while (getAdapterState() == modm::I2c::AdapterState::Busy)
		;
	buffer[0] = reg;
	configureWriteRead(buffer, 1, buffer, 1);
	
	while (!I2cMaster::startBlocking(this))
		;
	return buffer[0];
}

