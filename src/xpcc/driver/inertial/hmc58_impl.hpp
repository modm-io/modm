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
 *
 * $Id: hmc58_impl.hpp 738 2012-02-25 17:54:01Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__HMC58_HPP
#	error  "Don't include this file directly, use 'hmc58.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename TwiMaster >
xpcc::HMC58<TwiMaster>::HMC58(uint8_t address) :
	deviceAddress(address<<1)
{
}

template < typename TwiMaster >
bool
xpcc::HMC58<TwiMaster>::initialize(uint8_t dataOutputRate)
{
	newData = false;
	data[0] = hmc58::REGISTER_CONFIG_A;
	// configuration register A
	data[1] = hmc58::MEASUREMENT_AVERAGE_8_gc | dataOutputRate | hmc58::MEASUREMENT_MODE_NORMAL_gc;
	// configuration register B
	data[2] = 0x20; /* default gain of ~1Gs */
	// mode register
	data[3] = hmc58::OPERATION_MODE_CONTINUOUS_gc;
	
	return TwiMaster::startWrite(deviceAddress, data, 4);
}

template < typename TwiMaster >
bool
xpcc::HMC58<TwiMaster>::readMagnetometer()
{
	if (TwiMaster::start(deviceAddress)) {
		newData = false;
		data[0] = hmc58::REGISTER_DATA_X0;
		TwiMaster::attachDelegate(this);
		TwiMaster::writeRead(data, 1, 6);
		
		return true;
	}
	return false;
}

template < typename TwiMaster >
bool
xpcc::HMC58<TwiMaster>::isDataReady()
{
	return readRegister(hmc58::REGISTER_STATUS) & hmc58::STATUS_DATA_READY_bm;
}

template < typename TwiMaster >
bool
xpcc::HMC58<TwiMaster>::isNewDataAvailable()
{
	return newData;
}

template < typename TwiMaster >
uint8_t*
xpcc::HMC58<TwiMaster>::getData()
{
	newData = false;
	return data;
}

template < typename TwiMaster >
bool
xpcc::HMC58<TwiMaster>::setDataOutputRate(uint8_t dataOutputRate)
{
	uint8_t config = readRegister(hmc58::REGISTER_CONFIG_A);
	return writeRegister(hmc58::REGISTER_CONFIG_A, (config & ~hmc58::DATA_OUTPUT_RATE_gm) | dataOutputRate);
}

template < typename TwiMaster >
bool
xpcc::HMC58<TwiMaster>::setMeasurementMode(hmc58::MeasurementMode mode)
{
	uint8_t config = readRegister(hmc58::REGISTER_CONFIG_A);
	return writeRegister(hmc58::REGISTER_CONFIG_A, (config & hmc58::MEASUREMENT_MODE_gm) | mode);
}

template < typename TwiMaster >
bool
xpcc::HMC58<TwiMaster>::setGain(uint8_t gain)
{
	return writeRegister(hmc58::REGISTER_CONFIG_B, gain);
}

// ----------------------------------------------------------------------------
template < typename TwiMaster >
bool
xpcc::HMC58<TwiMaster>::writeRegister(hmc58::Register reg, uint8_t data)
{
	uint8_t buffer[2] = {reg, data};
	if (TwiMaster::startWrite(deviceAddress, buffer, 2))
	{
		while (TwiMaster::getBusyState() != xpcc::i2c::FREE)
			;
		return true;
	}
	return false;
}

template < typename TwiMaster >
uint8_t
xpcc::HMC58<TwiMaster>::readRegister(hmc58::Register reg)
{
	uint8_t buffer[1] = {reg};
	if (TwiMaster::startWriteRead(deviceAddress, buffer, 1, 1))
	{
		while (TwiMaster::getBusyState() != xpcc::i2c::FREE)
			;
		return buffer[0];
	}
	return 0;
}

// ----------------------------------------------------------------------------
template < typename TwiMaster >
void
xpcc::HMC58<TwiMaster>::twiCompletion(const uint8_t */*data*/, std::size_t /*index*/, bool /*reading*/)
{
	newData = true;
}
