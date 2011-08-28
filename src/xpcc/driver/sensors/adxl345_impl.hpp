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

#ifndef XPCC__ADXL345_HPP
	#error  "Don't include this file directly, use 'adxl345.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2C >
xpcc::Adxl345<I2C>::Adxl345(uint8_t address) :
	xpcc::i2c::Device<I2C>(address<<1)
{
}

template < typename I2C >
void
xpcc::Adxl345<I2C>::initialize(Bandwidth bandwidth)
{
	writeRegister(REGISTER_POWER_CTL, POWER_MEASURE_bm);
	writeRegister(REGISTER_DATA_FORMAT, DATAFORMAT_FULL_RES_bm);
	writeRegister(REGISTER_BW_RATE, bandwidth);
	writeRegister(REGISTER_FIFO_CTL, FIFO_CTL_MODE_STREAM_gc);
}

template < typename I2C >
void
xpcc::Adxl345<I2C>::readAcceleration()
{
	data[0] = REGISTER_DATA_X0;
	this->i2c.writeRead(this->deviceAddress, &data[0], 1, 6);
	
	newData = true;
}

template < typename I2C >
void
xpcc::Adxl345<I2C>::readAccelerationAverage()
{
	newData = false;
	uint8_t bufferData[6];
	int16_t *buffer = reinterpret_cast<int16_t*>(&bufferData[0]);
	int16_t *result = reinterpret_cast<int16_t*>(&data[0]);
	
	for (uint_fast8_t i=0; i < 32; ++i) {	
		bufferData[0] = REGISTER_DATA_X0;
		this->i2c.writeRead(this->deviceAddress, &bufferData[0], 1, 6);
		
		while (this->i2c.isBusy())
			;
		
		result[0] += buffer[0];
		result[1] += buffer[1];
		result[2] += buffer[2];
	}
	
	result[0] >>= 5;
	result[1] >>= 5;
	result[2] >>= 5;
	
	newData = true;
}

template < typename I2C >
bool
xpcc::Adxl345<I2C>::isDataReady()
{
	if (this->i2c.isBusy()) return false;
	return readRegister(REGISTER_INT_SOURCE) & INTERRUPT_DATA_READY_bm;
}

template < typename I2C >
bool
xpcc::Adxl345<I2C>::isNewDataAvailable()
{
	return newData;
}

template < typename I2C >
uint8_t*
xpcc::Adxl345<I2C>::getData()
{
	newData = false;
	return &data[0];
}

// ----------------------------------------------------------------------------
template < typename I2C >
void
xpcc::Adxl345<I2C>::writeRegister(Register reg, uint8_t data)
{
	uint8_t buffer[2] = {reg, data};
	this->i2c.write(this->deviceAddress, &buffer[0], 2);
}

template < typename I2C >
uint8_t
xpcc::Adxl345<I2C>::readRegister(Register reg)
{
	uint8_t buffer[1] = {reg};
	this->i2c.writeRead(this->deviceAddress, &buffer[0], 1, 1);
	
	while (this->i2c.isBusy())
		;
	return buffer[0];
}


