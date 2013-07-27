// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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

#ifndef XPCC__BMA180_HPP
#	error  "Don't include this file directly, use 'bma180.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Bma180<I2cMaster>::Bma180(uint8_t* data, uint8_t address)
:	status(0), data(data)
{
	adapter.initialize(address << 1, buffer, 0, data, 0);
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
	ok &= writeMaskedRegister(bma180::REGISTER_TCO_Z, bma180::MODE_CONFIG, mode);
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
		adapter.getState() == xpcc::I2c::AdapterState::NoError) {
		status &= ~READ_ACCELEROMETER_RUNNING;
		status |= NEW_ACCELEROMETER_DATA;
	}
	else if (status & READ_ACCELEROMETER_PENDING)
	{
		buffer[0] = bma180::REGISTER_DATA_X0;
		adapter.initialize(buffer, 1, data, 7);
		
		if (I2cMaster::start(&adapter)) {
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
	while (adapter.getState() == xpcc::I2c::AdapterState::Busy)
		;
	buffer[0] = reg;
	buffer[1] = value;
	adapter.initialize(buffer, 2, data, 0);
	
	return I2cMaster::startSync(&adapter);
}

template < typename I2cMaster >
uint8_t
xpcc::Bma180<I2cMaster>::readRegister(bma180::Register reg)
{
	while (adapter.getState() == xpcc::I2c::AdapterState::Busy)
		;
	buffer[0] = reg;
	adapter.initialize(buffer, 1, buffer, 1);
	
	while (!I2cMaster::startSync(&adapter))
		;
	return buffer[0];
}


