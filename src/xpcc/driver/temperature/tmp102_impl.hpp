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
 * $Id: tmp102_impl.hpp 734 2012-02-16 22:39:11Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__TMP102_HPP
	#error  "Don't include this file directly, use 'tmp102.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2C >
xpcc::Tmp102<I2C>::Tmp102(uint8_t address) :
	i2c::Device<I2C>(address)
{
}

template < typename I2C >
void
xpcc::Tmp102<I2C>::configure(uint8_t msb, uint8_t lsb)
{
	config = msb;
	writeRegister(REGISTER_CONFIGURATION, msb, lsb);
}

template < typename I2C >
void
xpcc::Tmp102<I2C>::startConversion()
{
	uint8_t buffer[2] = {REGISTER_CONFIGURATION, config & CONFIGURATION_ONE_SHOT_bm};
	this->i2c.write(this->deviceAddress, &buffer[0], 2);
}

template < typename I2C >
void
xpcc::Tmp102<I2C>::readTemperature()
{
	readRegister(REGISTER_TEMPERATURE, &data[0]);
}

template < typename I2C >
float
xpcc::Tmp102<I2C>::getTemperature()
{
	int16_t temp = static_cast<int16_t>((data[0]<<8)|data[1]);
	if (data[1] & TEMPERATURE_EXTENDED_MODE_bm) {
		return (temp>>3)/16.f;
	}
	else {
		return (temp>>4)/16.f;
	}
}

template < typename I2C >
uint8_t*
xpcc::Tmp102<I2C>::getData()
{
	return &data[0];
}

// ----------------------------------------------------------------------------
template < typename I2C >
void
xpcc::Tmp102<I2C>::writeRegister(Register reg, uint8_t msb, uint8_t lsb)
{
	uint8_t buffer[3] = {reg, msb, lsb};
	this->i2c.write(this->deviceAddress, &buffer[0], 3);
	
	while (this->i2c.isBusy())
		;
}

template < typename I2C >
void
xpcc::Tmp102<I2C>::readRegister(Register reg, uint8_t *buffer)
{
	buffer[0] = reg;
	this->i2c.writeRead(this->deviceAddress, &buffer[0], 1, 2);
	
	while (this->i2c.isBusy())
		;
}


