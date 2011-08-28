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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__HMC58_HPP
	#error  "Don't include this file directly, use 'hmc58.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2C >
xpcc::Hmc58<I2C>::Hmc58(uint8_t address) :
	xpcc::i2c::Device<I2C>(address<<1)
{
}

template < typename I2C >
void
xpcc::Hmc58<I2C>::initialize(uint8_t dataOutputRate)
{
	data[0] = REGISTER_CONFIG_A;
	// configuration register A
	data[1] = MEASUREMENT_AVERAGE_8_gc | dataOutputRate | MEASUREMENT_MODE_NORMAL_gc;
	// configuration register B
	data[2] = 0x20; /* default gain of ~1Gs */
	// mode register
	data[3] = OPERATION_MODE_CONTINUOUS_gc;

	this->i2c.write(this->deviceAddress, &data[0], 4);
}

template < typename I2C >
void
xpcc::Hmc58<I2C>::readCompass()
{
	data[0] = REGISTER_DATA_X0;
	this->i2c.writeRead(this->deviceAddress, &data[0], 1, 6);
	
	newData = true;
}

template < typename I2C >
bool
xpcc::Hmc58<I2C>::isDataReady()
{
	if (this->i2c.isBusy()) return false;
	return readRegister(REGISTER_STATUS) & STATUS_DATA_READY_bm;
}

template < typename I2C >
bool
xpcc::Hmc58<I2C>::isNewDataAvailable()
{
	return newData;
}

template < typename I2C >
uint8_t*
xpcc::Hmc58<I2C>::getData()
{
	newData = false;
	return &data[0];
}

template < typename I2C >
void
xpcc::Hmc58<I2C>::setDataOutputRate(uint8_t dataOutputRate)
{
	uint8_t config = readRegister(REGISTER_CONFIG_A);
	writeRegister(REGISTER_CONFIG_A, (config & ~DATA_OUTPUT_RATE_gm) | dataOutputRate);
}

template < typename I2C >
void
xpcc::Hmc58<I2C>::setMeasurementMode(MeasurementMode mode)
{
	uint8_t config = readRegister(REGISTER_CONFIG_A);
	writeRegister(REGISTER_CONFIG_A, (config & MEASUREMENT_MODE_gm) | mode);
}

template < typename I2C >
void
xpcc::Hmc58<I2C>::setGain(uint8_t gain)
{
	writeRegister(REGISTER_CONFIG_B, gain);
}

// ----------------------------------------------------------------------------
template < typename I2C >
void
xpcc::Hmc58<I2C>::writeRegister(Register reg, uint8_t data)
{
	uint8_t buffer[2] = {reg, data};
	this->i2c.write(this->deviceAddress, &buffer[0], 2);
}

template < typename I2C >
uint8_t
xpcc::Hmc58<I2C>::readRegister(Register reg)
{
	uint8_t buffer[1] = {reg};
	this->i2c.writeRead(this->deviceAddress, &buffer[0], 1, 1);
	
	while (this->i2c.isBusy())
		;
	return buffer[0];
}


