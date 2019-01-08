// coding: utf-8
/*
 * Copyright (c) 2017, Raphael Lehmann
 * Copyright (c) 2017-2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AMS5915_HPP
#define MODM_AMS5915_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/math/utils/endianness.hpp>

namespace modm
{

// forward declaration for friending with ams5915::Data
template < typename I2cMaster >
class Ams5915;

/// @ingroup modm_driver_ams5915
struct ams5915
{
	struct modm_packed
	Data
	{
		template < typename I2cMaster >
		friend class Ams5915;

	public:
		uint16_t
		getPressureRaw()
		{
			// mask undefined bits
			data[0] &= 0b00111111;

			auto rData = reinterpret_cast<const uint16_t*>(data);
			const uint16_t pressureRaw{modm::fromBigEndian(*rData)};

			return pressureRaw;
		}

		/**
		 * This method returns the pressure as a normalized float from 0-1.
		 * You have to scale and offset this according to the specific sensor
		 * you have.
		 * So if you have AMS 5915-0010-D, you can measure +10mBar, you need to
		 * multiply it with 10.0f to get the pressure in mBar.
		 * If you have AMS 5915-0350-D-B, which can measure ±350mBar you first need to
		 * subtract 0.5f and then multiply it with 350.0f!
 		 */
		float
		getPressure()
		{
			const uint16_t pressureRaw{getPressureRaw()};

			// Full scale span is typically 13107, with offset 1638
			// Caution: sensors may output values slightly exceeding the expected range!
			const uint16_t offset{1638};
			const uint16_t span{13107};

			if(pressureRaw <= offset) {
				return 0.f;
			} else if (pressureRaw >= offset + span) {
				return 1.f;
			}

			return static_cast<float>(pressureRaw - offset) / span;
		}

		/**
		 * This method returns the temperature of the pressure sensor in °C.
		 */
		float
		getTemperature()
		{
			uint16_t temperatureRaw = (data[3] >> 5) | (static_cast<uint16_t>(data[2]) << 3);
			return ((temperatureRaw * 200.f) / 2048.f) - 50.f;
		}
	private:
		// 0: MSB
		// ..
		// 3: LSB
		uint8_t data[4];
	};
};

/**
 * @ingroup modm_driver_ams5915
 * @author	Raphael Lehman, Niklas Hauser
 */
template < typename I2cMaster >
class Ams5915 : public ams5915, public modm::I2cDevice<I2cMaster, 1, I2cReadTransaction>
{
public:
	/**
	 * @param	data	a ams5915::Data object
	 */
	Ams5915(Data &data, uint8_t i2cAddress = 0x28)
	:	I2cDevice<I2cMaster,1,I2cReadTransaction>(i2cAddress), data(data)
	{
		this->transaction.configureRead(data.data, 4);
	}

	/// pings the sensor
	modm::ResumableResult<bool>
	ping()
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->transaction.configurePing() and this->startTransaction());

		RF_WAIT_WHILE( this->isTransactionRunning() );

		this->transaction.configureRead(data.data, 4);

		RF_END_RETURN( this->wasTransactionSuccessful() );
	}

	/// reads the Pressure registers and buffers the results
	inline modm::ResumableResult<bool>
	readPressure()
	{
		return this->runTransaction();
	}

public:
	/// Get the data object for this sensor.
	inline Data&
	getData()
	{ return data; }

private:
	Data &data;
};

}	// namespace modm

#endif // MODM_AMS5915_HPP
