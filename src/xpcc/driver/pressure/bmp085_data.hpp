/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <cmath>

#include <xpcc/debug/logger/logger.hpp>

#ifndef XPCC_BMP085_DATA_HPP
#define XPCC_BMP085_DATA_HPP

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DISABLED

// Forward declaration the test class
class Bmp085Test;

namespace xpcc
{

template < typename I2cMaster >
class Bmp085;

namespace bmp085data
{

/**
 * Holds the calibration data from the sensor.
 * Values are used for calculation of calibrated
 * sensor values from raw sensor data
 */
struct xpcc_packed
Calibration
{
	int16_t  ac1;
	int16_t  ac2;
	int16_t  ac3;
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;

	int16_t  b1;
	int16_t  b2;

	int16_t  mb;
	int16_t  mc;
	int16_t  md;
};

class xpcc_packed
DataBase
{
	template < typename I2cMaster >
	friend class ::xpcc::Bmp085;

	// Grant unit test full access to private members.
	friend class ::Bmp085Test;

public:
	/// DATA Access
	inline Calibration &
	getCalibration() {
		return calibration;
	}

public:
	/// Notify data class about changed buffer of raw temperature data.
	void inline
	rawTemperatureTouched() { meta &= ~TEMPERATURE_CALCULATED; }

	/// Notify data class about changed buffer of raw pressure data.
	void inline
	rawPressureTouched() { meta &= ~PRESSURE_CALCULATED; }

protected:
	// The order of these private variable is optimized for alignment of 4
	Calibration calibration;

protected:
	/// The raw data that was read from the sensor
	/// 0 .. 1 temperature data
	/// 2 .. 4 pressure data
	uint8_t raw[5];

	// bit 7-6: The mode in which the sensor operates
	// bit 2: calibrationCalculated
	// bit 1: temperatureCalculated
	// bit 0: pressureCalculated
	uint8_t meta = 0;

	enum
	{
		// Remember if the double precision calibration coefficients are already calculated
		CALIBRATION_CALCULATED = Bit2,
		/// Remember if the raw data was already converted to calibrated temperature
		TEMPERATURE_CALCULATED = Bit1,
		/// Remember if the raw data was already converted to calibrated pressure
		PRESSURE_CALCULATED = Bit0,
	};
};

class xpcc_packed
Data : public DataBase
{
public:
	/**
	 * Get the calibrated temperature for the device in 0.1 degree Celsius
	 *
	 * If recalculation is necessary it is done on the fly.
	 * No I2C transaction.
	 */
	int16_t inline
	getTemperature();

	void inline
	getTemperature(int16_t &temp) { temp = getTemperature(); };

	void inline
	getTemperature(float &temp) { temp = float(getTemperature() / 10.0f); };

	void inline
	getTemperature(double &temp) { temp = double(getTemperature()) / double(10.0); };

	/**
	 * Get the calibrated pressure from the device in Pascal.
	 *
	 * If recalculation is necessary it is done on the fly.
	 * No I2C transaction.
	 */
	int32_t inline
	getPressure();


protected:
	/**
	 * Use the calibration data read from the sensor to
	 * calculate the calibrated temperature from the
	 * raw data.
	 * The result is stored in this struct for further
	 * access.
	 * Implemented differently for Fixed Point, Single and
	 * Double Precision by template specialsation.
	 */
	void inline
	calculateCalibratedTemperature();

	/**
	 * See calculateCalibratedTemperature()
	 */
	void inline
	calculateCalibratedPressure();


private:
	// Fixed Point
	int16_t calibratedTemperature; // in 0.1 degree Celsius
	int32_t calibratedPressure;    // in Pa

	int32_t b5; // calculated in calculateCalibratedTemperature, needed for calculateCalibratedPressure
};

class xpcc_packed
DataDouble : public DataBase
{
public:

	int16_t inline
	getTemperature();

	void inline
	getTemperature(int16_t &temp);

	void inline
	getTemperature(float &temp);

	void inline
	getTemperature(double &temp);

	int32_t inline
	getPressure();

protected:
	void inline
	calculateCalibratedTemperature();

	void inline
	calculateCalibratedPressure();

private:
	// Double Precision
	// Prepared coefficients for temperature conversion in double precision
	double c5, c6, mc, md;

	// Prepared coefficients for pressure conversion in double precision
	double x0, x1, x2, s, y00, y11, y2, p0, p1, p2;

	// Stored temperature, needed for pressure calculation
	double calibratedTemperatureDouble;

	// Stored pressure in double precision in hPa
	double calibratedPressureDouble;
};

} // bmp085data namespace

} // xpcc namespace

#include "bmp085_data_impl_fp.hpp"
#include "bmp085_data_impl_double.hpp"

#endif // XPCC_BMP085_DATA_HPP
