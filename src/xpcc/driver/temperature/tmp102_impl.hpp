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
 * $Id: tmp102_impl.hpp 738 2012-02-25 17:54:01Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__TMP102_HPP
#	error  "Don't include this file directly, use 'tmp102.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename TwiMaster >
xpcc::TMP102<TwiMaster>::TMP102(uint8_t address) :
	deviceAddress(address<<1)
{
}

template < typename TwiMaster >
bool
xpcc::TMP102<TwiMaster>::initialize(tmp102::Config1 msb, tmp102::Config2 lsb)
{
	config = msb;
	uint8_t buffer[3] = {tmp102::REGISTER_CONFIGURATION, msb, lsb};
	if (TwiMaster::startWrite(deviceAddress, buffer, 3))
	{
		while (TwiMaster::getBusyState() != xpcc::i2c::FREE)
			;
		return true;
	}
	return false;
}

template < typename TwiMaster >
bool
xpcc::TMP102<TwiMaster>::startConversion()
{
	uint8_t buffer[2] = {tmp102::REGISTER_CONFIGURATION, config & tmp102::CONFIGURATION_ONE_SHOT_bm};
	return TwiMaster::startWrite(deviceAddress, buffer, 2);
}

template < typename TwiMaster >
bool
xpcc::TMP102<TwiMaster>::readTemperature()
{
	if (TwiMaster::start(deviceAddress)) {
		newData = false;
		data[0] = tmp102::REGISTER_TEMPERATURE;
		TwiMaster::attachDelegate(this);
		TwiMaster::writeRead(data, 1, 2);
		
		return true;
	}
	return false;
}

template < typename TwiMaster >
float
xpcc::TMP102<TwiMaster>::getTemperature()
{
	int16_t temp = static_cast<int16_t>((data[0]<<8)|data[1]);
	if (data[1] & tmp102::TEMPERATURE_EXTENDED_MODE_bm) {
		return (temp>>3)/16.f;
	}
	else {
		return (temp>>4)/16.f;
	}
}

template < typename TwiMaster >
bool
xpcc::TMP102<TwiMaster>::isNewDataAvailable()
{
	return newData;
}

template < typename TwiMaster >
uint8_t*
xpcc::TMP102<TwiMaster>::getData()
{
	newData = false;
	return data;
}

// ----------------------------------------------------------------------------
template < typename TwiMaster >
void
xpcc::TMP102<TwiMaster>::twiCompletion(const uint8_t */*data*/, std::size_t /*index*/, bool /*reading*/)
{
	newData = true;
}
