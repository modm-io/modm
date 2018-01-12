// coding: utf-8
/* Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2017, Raphael Lehmann
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AMSYS5915_HPP
#define MODM_AMSYS5915_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/math/utils/endianness.hpp>

namespace modm
{

// forward declaration for friending with amsys5915::Data
template < typename I2cMaster >
class Amsys5915;

struct amsys5915
{
	struct modm_packed
	Data
	{
		template < typename I2cMaster >
		friend class Amsys5915;

	public:
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
			// mask undefined bits
			data[0] &= 0b00111111;
			// Full scale span is 13107, with offset 1638
			uint16_t *rData = reinterpret_cast<uint16_t*>(data);
			uint16_t pressure = modm::fromBigEndian(*rData) - 1638;
			return static_cast<float>(pressure) / 13107.f;
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
 * Driver for the AMSYS 5915 differential and absolute pressure sensors.
 *
 * The device runs a cyclic program, which will store a corrected pressure value with
 * 12 bit resolution about every 500 μs within the output registers of the internal ASIC.
 * 
 * Datasheet: http://www.amsys.de/sheets/amsys.de.ams5915.pdf
 *
 * @ingroup driver_pressure
 * @author	Raphael Lehman, Niklas Hauser
 */
template < typename I2cMaster >
class Amsys5915 : public amsys5915, public modm::I2cDevice<I2cMaster, 1, I2cReadTransaction>
{
public:
	/**
	 * @param	data	a amsys5915::Data object
	 * @bug The address of the sensor is by factory default set to 0x28.
	 *      This means you cannot use two AMSYS 5915 sensors on the same bus!
	 *      You have to use a MUX or two seperate I2C busses.
	 */
	Amsys5915(Data &data)
	:	I2cDevice<I2cMaster,1,I2cReadTransaction>(0x28), data(data)
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

#endif // MODM_AMSYS5915_HPP
