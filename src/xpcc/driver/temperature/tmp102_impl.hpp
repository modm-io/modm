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

#ifndef XPCC__TMP102_HPP
#	error  "Don't include this file directly, use 'tmp102.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Tmp102<I2cMaster>::Tmp102(uint8_t* data, uint8_t address)
:	running(NOTHING_RUNNING), status(0), config(0), data(data)
{
	adapter.initialize(address << 1, buffer, 0, data, 0);
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::configure(tmp102::Config2 lsb, tmp102::Config1 msb)
{
	config = msb;
	buffer[0] = tmp102::REGISTER_CONFIGURATION;
	buffer[1] = msb;
	buffer[2] = lsb;
	adapter.initialize(buffer, 3, data, 0);
	
	return I2cMaster::startSync(&adapter);
}

template < typename I2cMaster >
void
xpcc::Tmp102<I2cMaster>::startConversion()
{
	status |= START_CONVERSION_PENDING;
}

template < typename I2cMaster >
void
xpcc::Tmp102<I2cMaster>::readTemperature()
{
	status |= READ_TEMPERATURE_PENDING;
}

template < typename I2cMaster >
float
xpcc::Tmp102<I2cMaster>::getTemperature()
{
	int16_t temp = static_cast<int16_t>((data[0]<<8)|data[1]);
	if (data[1] & tmp102::TEMPERATURE_EXTENDED_MODE) {
		return (temp>>3)/16.f;
	}
	else {
		return (temp>>4)/16.f;
	}
}

template < typename I2cMaster >
bool
xpcc::Tmp102<I2cMaster>::isNewDataAvailable()
{
	return status & NEW_TEMPERATURE_DATA;
}

template < typename I2cMaster >
uint8_t*
xpcc::Tmp102<I2cMaster>::getData()
{
	status &= ~NEW_TEMPERATURE_DATA;
	return data;
}

template < typename I2cMaster >
void
xpcc::Tmp102<I2cMaster>::update()
{
	if (running != NOTHING_RUNNING)
	{
		switch (adapter.getState())
		{
			case xpcc::i2c::adapter::NO_ERROR:
				if (running == READ_TEMPERATURE_RUNNING) {
					status |= NEW_TEMPERATURE_DATA;
				}
				
			case xpcc::i2c::adapter::ERROR:
				running = NOTHING_RUNNING;
				
			default:
				break;
		}
	}
	else  {
		if (status & START_CONVERSION_PENDING)
		{
			buffer[0] = tmp102::REGISTER_CONFIGURATION;
			buffer[1] = config & tmp102::CONFIGURATION_ONE_SHOT;
			adapter.initialize(buffer, 2, data, 0);
			
			if (I2cMaster::start(&adapter)) {
				status &= ~START_CONVERSION_PENDING;
				running = START_CONVERSION_RUNNING;
			}
		}
		else if (status & READ_TEMPERATURE_PENDING)
		{
			buffer[0] = tmp102::REGISTER_TEMPERATURE;
			adapter.initialize(buffer, 1, data, 2);
			
			if (I2cMaster::start(&adapter)) {
				status &= ~READ_TEMPERATURE_PENDING;
				running = READ_TEMPERATURE_RUNNING;
			}
		}
	}
}


