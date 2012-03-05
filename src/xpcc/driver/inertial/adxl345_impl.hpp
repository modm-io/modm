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
 * $Id: adxl345_impl.hpp 738 2012-02-25 17:54:01Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ADXL345_HPP
#	error  "Don't include this file directly, use 'adxl345.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename TwiMaster >
xpcc::ADXL345<TwiMaster>::ADXL345(uint8_t address):
	deviceAddress(address << 1)
{
}

template < typename TwiMaster >
bool
xpcc::ADXL345<TwiMaster>::initialize(adxl345::Bandwidth bandwidth, bool streamMode)
{
	bool ok = writeRegister(adxl345::REGISTER_POWER_CTL, adxl345::POWER_MEASURE_bm);
	ok &= writeRegister(adxl345::REGISTER_DATA_FORMAT, adxl345::DATAFORMAT_FULL_RES_bm);
	ok &= writeRegister(adxl345::REGISTER_BW_RATE, bandwidth);
//	ok &= writeRegister(adxl345::REGISTER_INT_ENABLE, adxl345::INTERRUPT_DATA_READY_bm);
	if (streamMode) {
		ok &= writeRegister(adxl345::REGISTER_FIFO_CTL, adxl345::FIFO_CTL_MODE_STREAM_gc);
	}
	return ok;
}

template < typename TwiMaster >
bool
xpcc::ADXL345<TwiMaster>::readAccelerometer()
{
	if (TwiMaster::start(deviceAddress)) {
		newData = false;
		data[0] = adxl345::REGISTER_DATA_X0;
		TwiMaster::attachDelegate(this);
		TwiMaster::writeRead(data, 1, 6);
		
		return true;
	}
	return false;
}

template < typename TwiMaster >
bool
xpcc::ADXL345<TwiMaster>::isDataReady()
{
	return readRegister(adxl345::REGISTER_INT_SOURCE) & adxl345::INTERRUPT_DATA_READY_bm;
}

template < typename TwiMaster >
bool
xpcc::ADXL345<TwiMaster>::isNewDataAvailable()
{
	return newData;
}

template < typename TwiMaster >
uint8_t*
xpcc::ADXL345<TwiMaster>::getData()
{
	newData = false;
	return data;
}

// ----------------------------------------------------------------------------
template < typename TwiMaster >
bool
xpcc::ADXL345<TwiMaster>::writeRegister(adxl345::Register reg, uint8_t data)
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
xpcc::ADXL345<TwiMaster>::readRegister(adxl345::Register reg)
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
xpcc::ADXL345<TwiMaster>::twiCompletion(const uint8_t */*data*/, std::size_t /*index*/, bool /*reading*/)
{
	newData = true;
}

