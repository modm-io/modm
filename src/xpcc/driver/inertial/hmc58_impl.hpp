// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__HMC58_HPP
#	error  "Don't include this file directly, use 'hmc58.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Hmc58<I2cMaster>::Hmc58(uint8_t* data, uint8_t address)
:	I2cWriteReadAdapter(address), status(0), data(data)
{
	configureWriteRead(buffer, 0, data, 0);
}

template < typename I2cMaster >
bool
xpcc::Hmc58<I2cMaster>::configure(uint8_t dataOutputRate)
{
	buffer[0] = hmc58::REGISTER_CONFIG_A;
	// configuration register A
	buffer[1] = hmc58::MEASUREMENT_AVERAGE_8 | dataOutputRate | hmc58::MEASUREMENT_MODE_NORMAL;
	// configuration register B
	buffer[2] = 0x20; /* default gain of ~1Gs */
	// mode register
	buffer[3] = hmc58::OPERATION_MODE_CONTINUOUS;
	configureWriteRead(buffer, 4, data, 0);
	
	return I2cMaster::startBlocking(this);
}

template < typename I2cMaster >
void
xpcc::Hmc58<I2cMaster>::readMagnetometer()
{
	status |= READ_MAGNETOMETER_PENDING;
}

template < typename I2cMaster >
bool
xpcc::Hmc58<I2cMaster>::isDataReady()
{
	return readRegister(hmc58::REGISTER_STATUS) & hmc58::STATUS_DATA_READY;
}

template < typename I2cMaster >
bool
xpcc::Hmc58<I2cMaster>::isNewDataAvailable()
{
	return status & NEW_MAGNETOMETER_DATA;
}

template < typename I2cMaster >
uint8_t*
xpcc::Hmc58<I2cMaster>::getData()
{
	status &= ~NEW_MAGNETOMETER_DATA;
	return data;
}

template < typename I2cMaster >
bool
xpcc::Hmc58<I2cMaster>::setDataOutputRate(uint8_t dataOutputRate)
{
	uint8_t config = readRegister(hmc58::REGISTER_CONFIG_A);
	return writeRegister(hmc58::REGISTER_CONFIG_A, (config & ~hmc58::DATA_OUTPUT_RATE_gm) | dataOutputRate);
}

template < typename I2cMaster >
bool
xpcc::Hmc58<I2cMaster>::setMeasurementMode(hmc58::MeasurementMode mode)
{
	uint8_t config = readRegister(hmc58::REGISTER_CONFIG_A);
	return writeRegister(hmc58::REGISTER_CONFIG_A, (config & hmc58::MEASUREMENT_MODE_gm) | mode);
}

template < typename I2cMaster >
bool
xpcc::Hmc58<I2cMaster>::setGain(uint8_t gain)
{
	return writeRegister(hmc58::REGISTER_CONFIG_B, gain);
}

template < typename I2cMaster >
void
xpcc::Hmc58<I2cMaster>::update()
{
	if (status & READ_MAGNETOMETER_RUNNING &&
		getAdapterState() == xpcc::I2c::AdapterState::Idle) {
		status &= ~READ_MAGNETOMETER_RUNNING;
		status |= NEW_MAGNETOMETER_DATA;
	}
	else if (status & READ_MAGNETOMETER_PENDING) 
	{
		buffer[0] = hmc58::REGISTER_DATA_X0;
		configureWriteRead(buffer, 1, data, 6);
		
		if (I2cMaster::start(this)) {
			status &= ~READ_MAGNETOMETER_PENDING;
			status |= READ_MAGNETOMETER_RUNNING;
		}
	}
}

// MARK: - private
template < typename I2cMaster >
bool
xpcc::Hmc58<I2cMaster>::writeRegister(hmc58::Register reg, uint8_t value)
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
xpcc::Hmc58<I2cMaster>::readRegister(hmc58::Register reg)
{
	while (getAdapterState() == xpcc::I2c::AdapterState::Busy)
		;
	buffer[0] = reg;
	configureWriteRead(buffer, 1, buffer, 1);
	
	while (!I2cMaster::startBlocking(this))
		;
	return buffer[0];
}