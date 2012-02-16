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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ITG3200_HPP
#	error  "Don't include this file directly, use 'itg3200.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename TwiMaster >
xpcc::Itg3200<TwiMaster>::Itg3200(uint8_t address):
deviceAddress(address << 1)
{
}

template < typename TwiMaster >
bool
xpcc::Itg3200<TwiMaster>::initialize(uint8_t divider, Filter filter, bool interrupt)
{
	newData = false;
	newGyro = false;
	data[0] = REGISTER_SMPLRT_DIV;
	// sample rate register
	data[1] = divider;
	// scale and filter register
	data[2] = SCALE_FS_2000_gc | filter;
	// interrupt register
	data[3] = interrupt ? INTERRUPT_RAW_RDY_EN_bm : 0;
	
	return TwiMaster::startWrite(deviceAddress, data, 4);
}

template < typename TwiMaster >
bool
xpcc::Itg3200<TwiMaster>::readGyroscope()
{
	if (TwiMaster::start(deviceAddress)) {
		newGyro = false;
		data[2] = REGISTER_DATA_X0;
		TwiMaster::attachDelegate(this);
		TwiMaster::writeRead(&data[2], 1, 6);
		
		return true;
	}
	return false;
}

template < typename TwiMaster >
bool
xpcc::Itg3200<TwiMaster>::readTemperatureGyroscope()
{
	if (TwiMaster::start(deviceAddress)) {
		newGyro = false;
		data[0] = REGISTER_DATA_T0;
		TwiMaster::attachDelegate(this);
		TwiMaster::writeRead(data, 1, 8);
		
		return true;
	}
	return false;
}

template < typename TwiMaster >
bool
xpcc::Itg3200<TwiMaster>::isDataReady()
{
	return readRegister(REGISTER_STATUS) & STATUS_RAW_RDY_bm;
}

template < typename TwiMaster >
bool
xpcc::Itg3200<TwiMaster>::isNewDataAvailable()
{
	return newData;
}

template < typename TwiMaster >
bool
xpcc::Itg3200<TwiMaster>::isNewGyroDataAvailable()
{
	return newGyro;
}

template < typename TwiMaster >
uint8_t*
xpcc::Itg3200<TwiMaster>::getGyroData()
{
	newGyro = false;
	return &data[2];
}

template < typename TwiMaster >
uint8_t*
xpcc::Itg3200<TwiMaster>::getData()
{
	newData = false;
	newGyro = false;
	return data;
}

template < typename TwiMaster >
bool
xpcc::Itg3200<TwiMaster>::setSampleRateDivider(uint8_t divider)
{
	return writeRegister(REGISTER_SMPLRT_DIV, divider);
}

template < typename TwiMaster >
bool
xpcc::Itg3200<TwiMaster>::setInterrupts(Interrupt interrupts)
{
	return writeRegister(REGISTER_INTERRUPT, interrupts);
}

// ----------------------------------------------------------------------------
template < typename TwiMaster >
bool
xpcc::Itg3200<TwiMaster>::writeRegister(Register reg, uint8_t data)
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
xpcc::Itg3200<TwiMaster>::readRegister(Register reg)
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
bool
xpcc::Itg3200<TwiMaster>::twiCompletion(const uint8_t */*data*/, std::size_t index, bool /*reading*/)
{
	if (index >= 6) newGyro = true;
	if (index == 8) newData = true;
	return false;
}

