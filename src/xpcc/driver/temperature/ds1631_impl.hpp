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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__DS1631_HPP
	#error	"Don't include this file directly, use 'ds1631.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename I2cMaster>
xpcc::Ds1631<I2cMaster>::Ds1631(uint8_t deviceAddress)
:	status(0), config(0), data(data)
{
	adapter.initialize(address << 1, buffer, 0, data, 0);
}

// ----------------------------------------------------------------------------
template <typename I2cMaster>
void
xpcc::Ds1631<I2cMaster>::configure(ds1631::Resolution resolution, bool continuousMode)
{
	buffer[0] = 0xac;
	buffer[1] = resolution | (continuousMode ? 0 : 0x01);
	adapter.initialize(buffer, 3, data, 0);
	
	return I2cMaster::startSync(&adapter);
}

template <typename I2cMaster>
void
xpcc::Ds1631<I2cMaster>::reset()
{
	buffer[0] = 0x54;
	adapter.initialize(buffer, 1, data, 0);
	
	return I2cMaster::start(&adapter);
}

// ----------------------------------------------------------------------------
template <typename I2cMaster>
void
xpcc::Ds1631<I2cMaster>::startConversion()
{
	buffer[0] = 0x51;
	adapter.initialize(buffer, 1, data, 0);
	
	return I2cMaster::start(&adapter);
}

template <typename I2cMaster>
void
xpcc::Ds1631<I2cMaster>::stopConversion()
{
	buffer[0] = 0x22;
	adapter.initialize(buffer, 1, data, 0);
	
	return I2cMaster::start(&adapter);
}

template <typename I2cMaster>
bool
xpcc::Ds1631<I2cMaster>::isConversionDone()
{
	buffer[0] = 0xac;
	adapter.initialize(buffer, 1, buffer, 1);

	if (I2cMaster::startSync(&adapter))
		return (buffer[0] & 0x80);
	
	return false;
}

// ----------------------------------------------------------------------------
template <typename I2cMaster>
int16_t
xpcc::Ds1631<I2cMaster>::readTemperature()
{
	buffer[0] = 0xaa;
	adapter.initialize(buffer, 1, data, 2);
	
	return I2cMaster::start(&adapter);
}
