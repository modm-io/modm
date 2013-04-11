// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
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

#ifndef XPCC__HMC6343_HPP
#	error  "Don't include this file directly, use 'hmc6343.hpp' instead!"
#endif
#include <xpcc/math/utils/operator.hpp>
#include <xpcc/math/utils/bit_operation.hpp>

// ----------------------------------------------------------------------------
template < typename I2cMaster >
xpcc::Hmc6343<I2cMaster>::Hmc6343(uint8_t* data, uint8_t address)
:	timeout(500), running(RESET_PROCESSOR_RUNNING), status(0), pending(0), data(data)
{
	adapter.initialize(address << 1, buffer, 0, data, 0);
}

// MARK: - configuration

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::configure(hmc6343::MeasurementRate measurementRate)
{
	return writeRegister(hmc6343::EEPROM_OPERATION_MODE_2, measurementRate);
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::setOrientation(hmc6343::Orientation orientation)
{
	return writeCommand(static_cast<hmc6343::Command>(orientation));
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::setDeviationAngle(int16_t angle)
{
	bool ok = writeRegister(hmc6343::EEPROM_DEVIATION_ANGLE_LSB, angle & 0xff);
	if (ok) {
		ok &= writeRegister(hmc6343::EEPROM_DEVIATION_ANGLE_MSB, angle >> 8);
	}
	return ok;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::setVariationAngle(int16_t angle)
{
	bool ok = writeRegister(hmc6343::EEPROM_VARIATION_ANGLE_LSB, angle & 0xff);
	if (ok) {
		ok &= writeRegister(hmc6343::EEPROM_VARIATION_ANGLE_MSB, angle >> 8);
	}
	return ok;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::setIIRFilter(uint8_t filter)
{
	// only 0 to 15 (0x0f) values allowed
	return writeRegister(hmc6343::EEPROM_FILTER_LSB, filter & 0x0f);
}

// MARK: - modes

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::enterRunMode()
{
	return writeCommand(hmc6343::COMMAND_ENTER_RUN_MODE);
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::enterStandByMode()
{
	return writeCommand(hmc6343::COMMAND_ENTER_STANDBY_MODE);
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::enterSleepMode()
{
	return writeCommand(hmc6343::COMMAND_ENTER_SLEEP_MODE);
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::exitSleepMode()
{
	if (writeCommand(hmc6343::COMMAND_EXIT_SLEEP_MODE))
	{
		// response delay time 20ms
		timeout.restart(20);
//		running = EXIT_SLEEP_MODE_RUNNING;
		return true;
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::enterUserCalibrationMode()
{
	return writeCommand(hmc6343::COMMAND_ENTER_USER_CALIBRATION);
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::exitUserCalibrationMode()
{
	if (writeCommand(hmc6343::COMMAND_EXIT_USER_CALIBRATION_MODE))
	{
		// response delay time 50ms
		timeout.restart(50);
//		running = EXIT_USER_CALIBRATION_MODE_RUNNING;
		return true;
	}
	return false;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::resetProcessor()
{
	if (writeCommand(hmc6343::COMMAND_RESET_PROCESSOR))
	{
		// response delay time 500ms
		timeout.restart(500);
//		running = RESET_PROCESSOR_RUNNING;
		return true;
	}
	return false;
}

// MARK: data requests

template < typename I2cMaster >
void
xpcc::Hmc6343<I2cMaster>::readAcceleration()
{
	pending |= START_ACCELERATION_PENDING;
}

template < typename I2cMaster >
void
xpcc::Hmc6343<I2cMaster>::readMagnetometer()
{
	pending |= START_MAGNETOMETER_PENDING;
}

template < typename I2cMaster >
void
xpcc::Hmc6343<I2cMaster>::readAttitude()
{
	pending |= START_ATTITUDE_PENDING;
}

template < typename I2cMaster >
void
xpcc::Hmc6343<I2cMaster>::readTemperature()
{
	pending |= START_TEMPERATURE_PENDING;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isNewAccelerationDataAvailable()
{
	return status & NEW_ACCELERATION_DATA;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isNewMagnetometerDataAvailable()
{
	return status & NEW_MAGNETOMETER_DATA;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isNewAttitudeDataAvailable()
{
	return status & NEW_ATTITUDE_DATA;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::isNewTemperatureDataAvailable()
{
	return status & NEW_TEMPERATURE_DATA;
}

// MARK: - data access

template < typename I2cMaster >
uint8_t*
xpcc::Hmc6343<I2cMaster>::getAcceleration()
{
	status &= ~NEW_ACCELERATION_DATA;
	return data;
}

template < typename I2cMaster >
uint8_t*
xpcc::Hmc6343<I2cMaster>::getMagnetometer()
{
	status &= ~NEW_MAGNETOMETER_DATA;
	return data+6;
}

template < typename I2cMaster >
uint16_t
xpcc::Hmc6343<I2cMaster>::getHeading()
{
	uint16_t* rawData = reinterpret_cast<uint16_t*>(data);
	status &= ~NEW_ATTITUDE_DATA;
	return xpcc::swap(rawData[7]);
}

template < typename I2cMaster >
int16_t
xpcc::Hmc6343<I2cMaster>::getPitch()
{
	uint16_t* rawData = reinterpret_cast<uint16_t*>(data);
	status &= ~NEW_ATTITUDE_DATA;
	return static_cast<int16_t>(xpcc::swap(rawData[8]));
}

template < typename I2cMaster >
int16_t
xpcc::Hmc6343<I2cMaster>::getRoll()
{
	uint16_t* rawData = reinterpret_cast<uint16_t*>(data);
	status &= ~NEW_ATTITUDE_DATA;
	return static_cast<int16_t>(xpcc::swap(rawData[9]));
}

template < typename I2cMaster >
int16_t
xpcc::Hmc6343<I2cMaster>::getTemperature()
{
	uint16_t* rawData = reinterpret_cast<uint16_t*>(data);
	status &= ~NEW_TEMPERATURE_DATA;
	return static_cast<int16_t>(xpcc::swap(rawData[10]));
}

template < typename I2cMaster >
uint8_t*
xpcc::Hmc6343<I2cMaster>::getData()
{
	return data;
}

// MARK: - register access

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::writeCommand(hmc6343::Command command)
{
	// busy waiting
	while(!timeout.isExpired() || adapter.getState() == xpcc::i2c::adapter::BUSY)
		;

	buffer[0] = command;
	adapter.initialize(buffer, 1, 0, 0);

	while(!I2cMaster::startSync(&adapter))
		;

	return true;
}

template < typename I2cMaster >
bool
xpcc::Hmc6343<I2cMaster>::writeRegister(hmc6343::Register reg, uint8_t value)
{
	// busy waiting
	while(!timeout.isExpired() || adapter.getState() == xpcc::i2c::adapter::BUSY)
		;

	buffer[0] = hmc6343::COMMAND_WRITE_EEPROM;
	buffer[1] = reg;
	buffer[2] = value;
	adapter.initialize(buffer, 3, 0, 0);

	while(!I2cMaster::startSync(&adapter))
		;
	// 10ms timing delay
	timeout.restart(10);

	return true;
}

template < typename I2cMaster >
uint8_t
xpcc::Hmc6343<I2cMaster>::readRegister(hmc6343::Register reg)
{
	// busy waiting
	while(!timeout.isExpired() || adapter.getState() == xpcc::i2c::adapter::BUSY)
		;

	buffer[0] = hmc6343::COMMAND_READ_EEPROM;
	buffer[1] = reg;
	adapter.initialize(buffer, 2, buffer, 1);

	while(!I2cMaster::startSync(&adapter))
		;
	// 10ms timing delay
	timeout.restart(10);

	return buffer[0];
}

// MARK: - update

template < typename I2cMaster >
void
xpcc::Hmc6343<I2cMaster>::update()
{
	if (running != NOTHING_RUNNING)
	{
		if (adapter.getState() == xpcc::i2c::adapter::NO_ERROR)
		{
			switch (running)
			{
				case READ_ACCELERATION_RUNNING:
					status |= NEW_ACCELERATION_DATA;
					break;
				case READ_MAGNETOMETER_RUNNING:
					status |= NEW_MAGNETOMETER_DATA;
					break;
				case READ_ATTITUDE_RUNNING:
					status |= NEW_ATTITUDE_DATA;
					break;
				case READ_TEMPERATURE_RUNNING:
					status |= NEW_TEMPERATURE_DATA;
					break;

				default:
					break;
			}
			running = NOTHING_RUNNING;
		}
		if (adapter.getState() == xpcc::i2c::adapter::ERROR)
		{
			running = NOTHING_RUNNING;
		}
	}
	else if (timeout.isExpired())
	{
		// starting
		if (pending & START_ACCELERATION_PENDING)
		{
			buffer[0] = hmc6343::COMMAND_POST_ACCEL_DATA;
			adapter.initialize(buffer, 1, 0, 0);

			if (I2cMaster::start(&adapter))
			{
				pending &= ~START_ACCELERATION_PENDING;
				pending |= READ_ACCELERATION_PENDING;
				timeout.restart(1);
			}
		}
		else if (pending & START_MAGNETOMETER_PENDING)
		{
			buffer[0] = hmc6343::COMMAND_POST_MAG_DATA;
			adapter.initialize(buffer, 1, 0, 0);

			if (I2cMaster::start(&adapter))
			{
				pending &= ~START_MAGNETOMETER_PENDING;
				pending |= READ_MAGNETOMETER_PENDING;
				timeout.restart(1);
			}
		}
		else if (pending & START_ATTITUDE_PENDING)
		{
			buffer[0] = hmc6343::COMMAND_POST_HEADING_DATA;
			adapter.initialize(buffer, 1, 0, 0);

			if (I2cMaster::start(&adapter))
			{
				pending &= ~START_ATTITUDE_PENDING;
				pending |= READ_ATTITUDE_PENDING;
				timeout.restart(1);
			}
		}
		else if (pending & START_TEMPERATURE_PENDING)
		{
			buffer[0] = hmc6343::COMMAND_POST_TILT_DATA;
			adapter.initialize(buffer, 1, 0, 0);

			if (I2cMaster::start(&adapter))
			{
				pending &= ~START_TEMPERATURE_PENDING;
				pending |= READ_TEMPERATURE_PENDING;
				timeout.restart(1);
			}
		}
		// reading
		else if (pending & READ_ACCELERATION_PENDING)
		{
			adapter.initialize(buffer, 0, data, 6);

			if (I2cMaster::start(&adapter))
			{
				pending &= ~READ_ACCELERATION_PENDING;
				running = READ_ACCELERATION_RUNNING;
			}
		}
		else if (pending & READ_MAGNETOMETER_PENDING)
		{
			adapter.initialize(buffer, 0, data+6, 6);

			if (I2cMaster::start(&adapter))
			{
				pending &= ~READ_MAGNETOMETER_PENDING;
				running = READ_MAGNETOMETER_RUNNING;
			}
		}
		else if (pending & READ_ATTITUDE_PENDING)
		{
			adapter.initialize(buffer, 0, data+12, 6);

			if (I2cMaster::start(&adapter))
			{
				pending &= ~READ_ATTITUDE_PENDING;
				running = READ_ATTITUDE_RUNNING;
			}
		}
		else if (pending & READ_TEMPERATURE_PENDING)
		{
			adapter.initialize(buffer, 0, data+14, 6);

			if (I2cMaster::start(&adapter))
			{
				pending &= ~READ_TEMPERATURE_PENDING;
				running = READ_TEMPERATURE_RUNNING;
			}
		}
	}
}
