// coding: utf-8
/* Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2017, Raphael Lehmann
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_AMSYS5915_HPP
#define XPCC_AMSYS5915_HPP

#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/math/utils/endianness.hpp>

namespace xpcc
{

// forward declaration for friending with amsys5915::Data
template < typename I2cMaster >
class Amsys5915;

struct amsys5915
{
	struct xpcc_packed
	Data
	{
		template < typename I2cMaster >
		friend class Amsys5915;

	public:
		uint16_t
		getPressureRaw()
		{
			// mask undefined bits
			data[0] &= 0b00111111;

			auto rData = reinterpret_cast<const uint16_t*>(data);
			const uint16_t pressureRaw{xpcc::fromBigEndian(*rData)};

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
class Amsys5915 : public amsys5915, public xpcc::I2cDevice<I2cMaster, 1, I2cReadTransaction>
{
public:
	/**
	 * @param	data	a amsys5915::Data object
	 */
	Amsys5915(Data &data, uint8_t i2cAddress = 0x28)
	:	I2cDevice<I2cMaster,1,I2cReadTransaction>(i2cAddress), data(data)
	{
		this->transaction.configureRead(data.data, 4);
	}

	/// pings the sensor
	xpcc::ResumableResult<bool>
	ping()
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->transaction.configurePing() and this->startTransaction());

		RF_WAIT_WHILE( this->isTransactionRunning() );

		this->transaction.configureRead(data.data, 4);

		RF_END_RETURN( this->wasTransactionSuccessful() );
	}

	/// reads the Pressure registers and buffers the results
	inline xpcc::ResumableResult<bool>
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

}	// namespace xpcc

#endif // XPCC_AMSYS5915_HPP
