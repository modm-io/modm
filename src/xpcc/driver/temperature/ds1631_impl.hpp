// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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

#ifndef XPCC__DS1631_HPP
	#error	"Don't include this file directly, use 'ds1631.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename I2C>
xpcc::Ds1631<I2C>::Ds1631(uint8_t address) :
	i2c::Device<I2C>(address)
{
}

// ----------------------------------------------------------------------------
template <typename I2C>
void
xpcc::Ds1631<I2C>::configure(ds1631::Resolution resolution, bool continuousMode)
{
	if (this->i2c.start(this->deviceAddress | i2c::WRITE))
	{
		this->i2c.write(0xac);
		
		uint8_t config = resolution;
		if (!continuousMode) {
			config |= 0x01;
		}
		this->i2c.write(config);
	}
	this->i2c.stop();
}

template <typename I2C>
void
xpcc::Ds1631<I2C>::reset()
{
	if (this->i2c.start(this->deviceAddress | i2c::WRITE))
	{
		this->i2c.write(0x54);
	}
	this->i2c.stop();
}

// ----------------------------------------------------------------------------
template <typename I2C>
void
xpcc::Ds1631<I2C>::startConversion()
{
	if (this->i2c.start(this->deviceAddress | i2c::WRITE))
	{
		this->i2c.write(0x51);
	}
	this->i2c.stop();
}

template <typename I2C>
void
xpcc::Ds1631<I2C>::stopConversion()
{
	if (this->i2c.start(this->deviceAddress | i2c::WRITE))
	{
		this->i2c.write(0x22);
	}
	this->i2c.stop();
}

template <typename I2C>
bool
xpcc::Ds1631<I2C>::isConversionDone()
{
	if (this->i2c.start(this->deviceAddress | i2c::WRITE))
	{
		this->i2c.write(0xac);
		this->i2c.repeatedStart(this->deviceAddress | i2c::READ);
		
		uint8_t done = this->i2c.read(i2c::NACK) & 0x80;
		return (done == 0x80);
	}
	this->i2c.stop();
	
	return false;
}

// ----------------------------------------------------------------------------
template <typename I2C>
int16_t
xpcc::Ds1631<I2C>::readTemperature()
{
	uint16_t temperature = 0;
	if (this->i2c.start(this->deviceAddress | i2c::WRITE))
	{
		this->i2c.write(0xaa);
		if (this->i2c.repeatedStart(this->deviceAddress | i2c::READ))
		{
			temperature  = this->i2c.read(i2c::ACK) << 8;
			temperature |= this->i2c.read(i2c::NACK);
		}
	}
	this->i2c.stop();
	
	return temperature;
}
