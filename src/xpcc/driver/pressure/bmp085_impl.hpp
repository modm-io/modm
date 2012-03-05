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
 * $Id: bmp085_impl.hpp 738 2012-02-25 17:54:01Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__BMP085_HPP
#	error  "Don't include this file directly, use 'bmp085.hpp' instead!"
#endif
#include <xpcc/math/utils/operator.hpp>

// ----------------------------------------------------------------------------
template < typename TwiMaster >
xpcc::BMP085<TwiMaster>::BMP085(uint8_t address):
deviceAddress(address << 1)
{
}

template < typename TwiMaster >
bool
xpcc::BMP085<TwiMaster>::initialize(bmp085::Mode mode)
{
	if (TwiMaster::start(deviceAddress)) {
		newData = false;
		calTemperature = false;
		calPressure = false;
		config = mode;
		calibrationData[0] = bmp085::REGISTER_CAL_AC1;
		TwiMaster::attachDelegate(this);
		TwiMaster::writeRead(calibrationData, 1, 22);
		
		return true;
	}
	return false;
}

template < typename TwiMaster >
bool
xpcc::BMP085<TwiMaster>::startTemperatureMeasurement()
{
	uint8_t buffer[2] = {bmp085::REGISTER_CONTROL, bmp085::CONVERSION_TEMPERATURE_gc};
	return TwiMaster::startWrite(deviceAddress, buffer, 2);
}

template < typename TwiMaster >
bool
xpcc::BMP085<TwiMaster>::readTemperature()
{
	if (TwiMaster::start(deviceAddress)) {
		newData = false;
		calTemperature = false;
		data[0] = bmp085::REGISTER_CONVERSION_MSB;
		TwiMaster::attachDelegate(this);
		TwiMaster::writeRead(data, 1, 2);
		
		return true;
	}
	return false;
	
	
	return TwiMaster::startWriteRead(deviceAddress, data, 1, 2);
}

template < typename TwiMaster >
bool
xpcc::BMP085<TwiMaster>::startPressureMeasurement()
{
	uint8_t buffer[2] = {bmp085::REGISTER_CONTROL, bmp085::CONVERSION_PRESSURE_gc | config};
	return TwiMaster::startWrite(deviceAddress, buffer, 2);
}

template < typename TwiMaster >
bool
xpcc::BMP085<TwiMaster>::readPressure()
{
	if (TwiMaster::start(deviceAddress)) {
		newData = false;
		calPressure = false;
		data[2] = bmp085::REGISTER_CONVERSION_MSB;
		TwiMaster::attachDelegate(this);
		TwiMaster::writeRead(&data[2], 1, 3);
		
		return true;
	}
	return false;
}

template < typename TwiMaster >
bool
xpcc::BMP085<TwiMaster>::isNewDataAvailable()
{
	return newData;
}

template < typename TwiMaster >
uint8_t*
xpcc::BMP085<TwiMaster>::getData()
{
	newData = false;
	return data;
}

template < typename TwiMaster >
uint8_t*
xpcc::BMP085<TwiMaster>::getCalibrationData()
{
	return calibrationData;
}

template < typename TwiMaster >
int16_t*
xpcc::BMP085<TwiMaster>::getCalibratedTemperature()
{
	if (calTemperature) {
		int32_t x1, x2;
		uint16_t ut = static_cast<uint16_t>(data[0]<<8|data[1]);
		x1 = xpcc::math::mul(ut - ac6, ac5) >> 15;
		x2 = (static_cast<int32_t>(mc) << 11) / (x1 + md);
		b5 = x1 + x2;
		calibratedTemperature = static_cast<int16_t>((b5 + 8) >> 4);
	}
	
	calTemperature = false;
	return &calibratedTemperature;
}

template < typename TwiMaster >
int32_t*
xpcc::BMP085<TwiMaster>::getCalibratedPressure()
{
	if (calPressure) {
		int32_t x1, x2, x3, b3, p;
		int16_t b6;
		uint32_t b4, b7;
		uint8_t oss = ((config & bmp085::MODE_gm) >> 6);
		
		getCalibratedTemperature();
		
		uint32_t up = (static_cast<uint32_t>(data[2])<<16|data[3]<<8|data[4]) >> (8 - oss);
		b6 = static_cast<int16_t>(b5 - 4000);
		x1 = xpcc::math::mul(b2, xpcc::math::mul(b6, b6) >> 12) >> 11;
		x2 = xpcc::math::mul(ac2, b6) >> 11;
		x3 = x1 + x2;
		b3 = (xpcc::math::mul(ac1, 4) + x3) << oss;
		b3 = (b3 + 2) >> 2;
		x1 = xpcc::math::mul(ac3, b6) >> 13;
		x2 = xpcc::math::mul(b1, xpcc::math::mul(b6, b6) >> 12) >> 16;
		x3 = ((x1 + x2) + 2) >> 2;
		b4 = xpcc::math::mul(ac4, static_cast<uint16_t>(x3 + 32768)) >> 15;
		b7 = (up - b3) * (50000 >> oss);
		if (b7 < 0x80000000)
			p = (b7 << 1) / b4;
		else
			p = (b7 / b4) << 1;
		
		x1 = xpcc::math::mul(static_cast<uint16_t>(p >> 8), static_cast<uint16_t>(p >> 8));
		x1 = (x1 * 3038) >> 16;
		x2 = (-7357 * p) >> 16;
		calibratedPressure = p + ((x1 + x2 + 3791) >> 4);
	}
	
	calPressure = false;
	return &calibratedPressure;
}

// ----------------------------------------------------------------------------
template < typename TwiMaster >
void
xpcc::BMP085<TwiMaster>::twiCompletion(const uint8_t */*data*/, std::size_t index, bool /*reading*/)
{
	if (index == 22)
	{
		ac1 = static_cast<int16_t>(calibrationData[0]<<8|calibrationData[1]);
		ac2 = static_cast<int16_t>(calibrationData[2]<<8|calibrationData[3]);
		ac3 = static_cast<int16_t>(calibrationData[4]<<8|calibrationData[5]);
		ac4 = static_cast<uint16_t>(calibrationData[6]<<8|calibrationData[7]);
		ac5 = static_cast<uint16_t>(calibrationData[8]<<8|calibrationData[9]);
		ac6 = static_cast<uint16_t>(calibrationData[10]<<8|calibrationData[11]);
		
		b1 = static_cast<int16_t>(calibrationData[12]<<8|calibrationData[13]);
		b2 = static_cast<int16_t>(calibrationData[14]<<8|calibrationData[15]);
		
		mb = static_cast<int16_t>(calibrationData[16]<<8|calibrationData[17]);
		mc = static_cast<int16_t>(calibrationData[18]<<8|calibrationData[19]);
		md = static_cast<int16_t>(calibrationData[20]<<8|calibrationData[21]);
		return;
	}
	
	if (index == 2) calTemperature = true;
	if (index == 3) calPressure = true;
	newData = true;
}

