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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__BMP085_HPP
#	error  "Don't include this file directly, use 'bmp085.hpp' instead!"
#endif
#include <xpcc/math/utils/operator.hpp>
#include <xpcc/math/utils/bit_operation.hpp>

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Bmp085<I2cMaster>::Bmp085(uint8_t* data, uint8_t address)
:	I2cWriteReadAdapter(address), running(NOTHING_RUNNING), status(0), calculation(0), data(data)
{
	configureWriteRead(buffer, 0, data, 0);
}

template < typename I2cMaster >
bool
xpcc::Bmp085<I2cMaster>::configure(bmp085::Mode mode)
{
	config = mode;
	buffer[0] = bmp085::REGISTER_CAL_AC1;
	configureWriteRead(buffer, 1, reinterpret_cast<uint8_t*>(&calibration), 22);
	status |= NEW_CALIBRATION_DATA;
	
	return I2cMaster::startBlocking(this);
}

template < typename I2cMaster >
void
xpcc::Bmp085<I2cMaster>::startTemperatureMeasurement()
{
	status |= START_TEMPERATURE_PENDING;
}

template < typename I2cMaster >
void
xpcc::Bmp085<I2cMaster>::readTemperature()
{
	status |= READ_TEMPERATURE_PENDING;
}

template < typename I2cMaster >
void
xpcc::Bmp085<I2cMaster>::startPressureMeasurement()
{
	status |= START_PRESSURE_PENDING;
}

template < typename I2cMaster >
void
xpcc::Bmp085<I2cMaster>::readPressure()
{
	status |= READ_PRESSURE_PENDING;
}

template < typename I2cMaster >
void
xpcc::Bmp085<I2cMaster>::startReadoutSequence()
{
	status |= READOUT_SEQUENCE;
}

template < typename I2cMaster >
bool
xpcc::Bmp085<I2cMaster>::isNewDataAvailable()
{
	return status & NEW_PRESSURE_DATA;
}

template < typename I2cMaster >
uint8_t*
xpcc::Bmp085<I2cMaster>::getData()
{
	status &= ~NEW_PRESSURE_DATA;
	return data;
}

template < typename I2cMaster >
uint16_t*
xpcc::Bmp085<I2cMaster>::getCalibrationData()
{
	return reinterpret_cast<uint16_t*>(calibration);
}

template < typename I2cMaster >
int16_t*
xpcc::Bmp085<I2cMaster>::getCalibratedTemperature()
{
	if (calculation & TEMPERATURE_NEEDS_UPDATE) {
		int32_t x1, x2;
		uint16_t ut = static_cast<uint16_t>(data[0]<<8|data[1]);
		x1 = xpcc::math::mul(ut - calibration.ac6, calibration.ac5) >> 15;
		x2 = (static_cast<int32_t>(calibration.mc) << 11) / (x1 + calibration.md);
		b5 = x1 + x2;
		calibratedTemperature = static_cast<int16_t>((b5 + 8) >> 4);
		calculation &= ~TEMPERATURE_NEEDS_UPDATE;
	}
	return &calibratedTemperature;
}

template < typename I2cMaster >
int32_t*
xpcc::Bmp085<I2cMaster>::getCalibratedPressure()
{
	if (calculation & PRESSURE_NEEDS_UPDATE) {
		int32_t x1, x2, x3, b3, p;
		int16_t b6;
		uint32_t b4, b7;
		uint8_t oss = ((config & bmp085::MODE) >> 6);
		
		getCalibratedTemperature();
		
		uint32_t up = (static_cast<uint32_t>(data[2])<<16|static_cast<uint16_t>(data[3])<<8|data[4]) >> (8 - oss);
		b6 = static_cast<int16_t>(b5 - 4000);
		x1 = xpcc::math::mul(calibration.b2, xpcc::math::mul(b6, b6) >> 12) >> 11;
		x2 = xpcc::math::mul(calibration.ac2, b6) >> 11;
		x3 = x1 + x2;
		b3 = (xpcc::math::mul(calibration.ac1, 4) + x3) << oss;
		b3 = (b3 + 2) >> 2;
		x1 = xpcc::math::mul(calibration.ac3, b6) >> 13;
		x2 = xpcc::math::mul(calibration.b1, xpcc::math::mul(b6, b6) >> 12) >> 16;
		x3 = ((x1 + x2) + 2) >> 2;
		b4 = xpcc::math::mul(calibration.ac4, static_cast<uint16_t>(x3 + 32768)) >> 15;
		b7 = (up - b3) * (50000 >> oss);
		if (b7 < 0x80000000)
			p = (b7 << 1) / b4;
		else
			p = (b7 / b4) << 1;
		
		x1 = xpcc::math::mul(static_cast<uint16_t>(p >> 8), static_cast<uint16_t>(p >> 8));
		x1 = (x1 * 3038) >> 16;
		x2 = (-7357 * p) >> 16;
		calibratedPressure = p + ((x1 + x2 + 3791) >> 4);
		calculation &= ~PRESSURE_NEEDS_UPDATE;
	}
	
	return &calibratedPressure;
}

template < typename I2cMaster >
void
xpcc::Bmp085<I2cMaster>::update()
{
	if (status & NEW_CALIBRATION_DATA) {
		uint16_t* element = reinterpret_cast<uint16_t*>(&calibration);
		element[0] = xpcc::swap(element[0]);
		element[1] = xpcc::swap(element[1]);
		element[2] = xpcc::swap(element[2]);
		element[3] = xpcc::swap(element[3]);
		element[4] = xpcc::swap(element[4]);
		element[5] = xpcc::swap(element[5]);
		
		element[6] = xpcc::swap(element[6]);
		element[7] = xpcc::swap(element[7]);
		
		element[8] = xpcc::swap(element[8]);
		element[9] = xpcc::swap(element[9]);
		element[10] = xpcc::swap(element[10]);
		status &= ~NEW_CALIBRATION_DATA;
	}
	
	if ((status & READOUT_SEQUENCE) && timeout.isExpired())
	{
		static uint8_t state(0);
		static const uint8_t conversionDelay[] = {5, 8, 14, 26};
		
		switch(state)
		{
			case 0:
				status &= ~READ_PRESSURE_PENDING;
				status |= START_TEMPERATURE_PENDING;
				++state;
				timeout.restart(5);
				break;
				
			case 1:
				status &= ~START_TEMPERATURE_PENDING;
				status |= READ_TEMPERATURE_PENDING;
				timeout.restart(1);
				++state;
				break;
				
			case 2:
				status &= ~READ_TEMPERATURE_PENDING;
				status |= START_PRESSURE_PENDING;
				++state;
			{
				uint8_t oss = ((config & bmp085::MODE) >> 6);
				timeout.restart(conversionDelay[oss]);
			}
				break;
				
			case 3:
				status &= ~START_PRESSURE_PENDING;
				status |= READ_PRESSURE_PENDING;
				
			default:
				status &= ~READOUT_SEQUENCE;
				state = 0;
				break;
		}
	}
	
	if (running != NOTHING_RUNNING)
	{
		switch (getAdapterState())
		{
			case xpcc::I2c::AdapterState::Idle:
				if (running == READ_TEMPERATURE_RUNNING) {
					status |= NEW_TEMPERATURE_DATA;
					calculation |= TEMPERATURE_NEEDS_UPDATE;
				}
				else if (running == READ_PRESSURE_RUNNING) {
					status |= NEW_PRESSURE_DATA;
					calculation |= PRESSURE_NEEDS_UPDATE;
				}
				
			case xpcc::I2c::AdapterState::Error:
				running = NOTHING_RUNNING;
				
			default:
				break;
		}
	}
	else {
		if (status & START_TEMPERATURE_PENDING)
		{
			buffer[0] = bmp085::REGISTER_CONTROL;
			buffer[1] = bmp085::CONVERSION_TEMPERATURE;
			configureWriteRead(buffer, 2, data, 0);
			
			if (I2cMaster::start(this)) {
				status &= ~START_TEMPERATURE_PENDING;
				running = START_TEMPERATURE_RUNNING;
			}
		}
		else if (status & READ_TEMPERATURE_PENDING)
		{
			buffer[0] = bmp085::REGISTER_CONVERSION_MSB;
			configureWriteRead(buffer, 1, data, 2);
			
			if (I2cMaster::start(this)) {
				status &= ~READ_TEMPERATURE_PENDING;
				running = READ_TEMPERATURE_RUNNING;
			}
		}
		else if (status & START_PRESSURE_PENDING)
		{
			buffer[0] = bmp085::REGISTER_CONTROL;
			buffer[1] = bmp085::CONVERSION_PRESSURE | config;
			configureWriteRead(buffer, 2, data, 0);
			
			if (I2cMaster::start(this)) {
				status &= ~START_PRESSURE_PENDING;
				running = START_PRESSURE_RUNNING;
			}
		}
		else if (status & READ_PRESSURE_PENDING)
		{
			buffer[0] = bmp085::REGISTER_CONVERSION_MSB;
			configureWriteRead(buffer, 1, data+2, 3);
			
			if (I2cMaster::start(this)) {
				status &= ~READ_PRESSURE_PENDING;
				running = READ_PRESSURE_RUNNING;
			}
		}
	}
}

