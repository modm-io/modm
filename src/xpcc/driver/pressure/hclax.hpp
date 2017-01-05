/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_HCLAX_HPP
#define MODM_HCLAX_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/math/utils/endianness.hpp>

namespace modm
{

// forward declaration for friending with hclax::Data
template < typename I2cMaster >
class HclaX;

struct hclax
{
	struct modm_packed
	Data
	{
		template < typename I2cMaster >
		friend class HclaX;

	public:
		/**
		 * This method returns the pressure as a normalized float from 0-1.
		 * You have to scale and offset this according to the specific sensor
		 * you have.
		 * So if you have HCLA12X5U, you can measure +12.5mBar, you need to
		 * multiply it with 12.5f to get the pressure in mBar.
		 * If you have HCLA12X5B, which can measure ±12.5mBar you first need to
		 * subtract 0.5f and then multiply it with 12.5f!
 		 */
		float
		getPressure()
		{
			// Full scale span is 0x6666, with offset 0x0666
			uint16_t *rData = reinterpret_cast<uint16_t*>(data);
			uint16_t pressure = modm::fromBigEndian(*rData) - 0x0666;
			return float(pressure) / 0x6666;
		}

	private:
		// 0: MSB
		// 1: LSB
		uint8_t data[2];
	};
};

/**
 * Driver for the HCLA differential pressure sensors.
 *
 * The device runs a cyclic program, which will store a corrected pressure value with
 * 12 bit resolution about every 250 μs within the output registers of the internal ASIC.
 *
 * @ingroup driver_pressure
 * @author	Niklas Hauser
 */
template < typename I2cMaster >
class HclaX : public hclax, public modm::I2cDevice<I2cMaster, 1, I2cReadTransaction>
{
public:
	/**
	 * @param	data	a hclax::Data object
	 * @bug The address of the sensor is by factory default set to 0x78.
	 *      This means you cannot use two HCLA sensors on the same bus!
	 *      You have to use a MUX or two seperate I2C busses.
	 */
	HclaX(Data &data)
	:	I2cDevice<I2cMaster,1,I2cReadTransaction>(0x78), data(data)
	{
		this->transaction.configureRead(data.data, 2);
	}

	/// pings the sensor
	modm::ResumableResult<bool>
	ping()
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->transaction.configurePing() and this->startTransaction());

		RF_WAIT_WHILE( this->isTransactionRunning() );

		this->transaction.configureRead(data.data, 2);

		RF_END_RETURN( this->wasTransactionSuccessful() );
	}

	/// reads the Pressure registers and buffers the results
	modm::ResumableResult<bool>
	readPressure()
	{
		RF_BEGIN();

		RF_END_RETURN_CALL( this->runTransaction() );
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

#endif // MODM_HCLAX_HPP
