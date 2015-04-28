// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_BMP085_HPP
#define XPCC_BMP085_HPP

#include <xpcc/processing/protothread.hpp>
#include <xpcc/processing/coroutine.hpp>
#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/processing/timer.hpp>

namespace xpcc
{

template < typename I2cMaster >
class Bmp085;

struct bmp085
{
protected:
	/// @cond
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint8_t
	{
		CHIP_ID = 0xD0,
		VERSION = 0xD1,
		CAL_AC1 = 0xAA,
		CAL_AC2 = 0xAC,
		CAL_AC3 = 0xAE,
		CAL_AC4 = 0xB0,
		CAL_AC5 = 0xB2,
		CAL_AC6 = 0xB4,
		CAL_B1 = 0xB6,
		CAL_B2 = 0xB8,
		CAL_MB = 0xBA,
		CAL_MC = 0xBC,
		CAL_MD = 0xBE,
		CONTROL = 0xF4,
		MSB = 0xF6,
		LSB = 0xF7,
		XLSB = 0xF8,
	};

	/// The options of REGISTER_CHIP_ID
	enum ChipId
	{
		CHIP_ID = 0x55
	};

	/// The options of REGISTER_CONTROL
	enum class
	Conversion : uint8_t
	{
		Temperature = 0x2E,
		Pressure = 0x34,
	};
	/// @endcond

public:
	enum class
	Mode : uint8_t
	{
		Mask = (0x03 << 6),
		UltraLowPower = (0x00 << 6),
		Standard = (0x01 << 6),
		HighResolution = (0x02 << 6),
		UltraHighResolution = (0x03 << 6),
	};

	/**
	 * Holds the calibration data from the sensor.
	 * Values are used for calculation of calibrated
	 * sensor values from raw sensor data
	 */
	struct ATTRIBUTE_PACKED
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

	struct ATTRIBUTE_PACKED
	Data
	{
		template < typename I2cMaster >
		friend class Bmp085;

		// DATA ACCESS
		inline Calibration &
		getCalibration()
		{
			return calibration;
		}

		/**
		 * Get the calibrated temperature for the device in 0.1 degree Celsius
		 *
		 * If recalculation is necessary it is done on the fly.
		 * No I2C transaction.
		 */
		int16_t inline
		getTemperature()
		{
			if (!(meta & TEMPERATURE_CALCULATED)) {
				calculateCalibratedTemperature();
			}
			return calibratedTemperature;
		}

		/**
		 * Get the calibrated pressure from the device in Pascal.
		 *
		 * If recalculation is necessary it is done on the fly.
		 * No I2C transaction.
		 */
		int32_t inline
		getPressure()
		{
			if (!(meta & PRESSURE_CALCULATED)) {
				calculateCalibratedPressure();
			}
			return calibratedPressure;
		}

	private:
		/**
		 * Use the calibration data read from the sensor to
		 * calculate the calibrated temperature from the
		 * raw data.
		 * The result is stored in this struct for further
		 * access.
		 */
		void inline
		calculateCalibratedTemperature();

		/**
		 * See calculateCalibratedTemperature()
		 */
		void inline
		calculateCalibratedPressure();

		// the order of these private variable is optimized for alignment of 4
		Calibration calibration;

		int16_t calibratedTemperature; // in 0.1 degree Celsius
		int32_t calibratedPressure;    // in Pa

		int32_t b5; // calculated in calculateCalibratedTemperature, needed for calculateCalibratedPressure

		/// The raw data that was read from the sensor
		/// 0 .. 1 temperature data
		/// 2 .. 4 pressure data
		uint8_t raw[5];

		// bit 7-6: The mode in which the sensor operates
		// bit 1: temperatureCalculated
		// bit 0: pressureCalculated
		uint8_t meta;

		enum
		{
			/// Remember if the raw data was already converted to calibrated temperature
			TEMPERATURE_CALCULATED = Bit1,
			/// Remember if the raw data was already converted to calibrated pressure
			PRESSURE_CALCULATED = Bit0,
		};
	};

protected:
	/// @cond
	static constexpr uint8_t
	i(Mode mode) { return uint8_t(mode); }
	static constexpr uint8_t
	i(Conversion conv) { return uint8_t(conv); }
	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }
	/// @endcond
};

/**
 * BMP085 digital absolute pressure sensor driver
 *
 * The BMP085 is a high precision digital pressure sensor with I2C interface.
 * Unfortunately this sensor is so sensitive, it will give you wrong results
 * when there is traffic on the I2C during either temperature or pressure
 * conversion. So during that time make sure no other sensors on the bus
 * are read out.
 *
 * For further information, consult the
 * <a href="http://www.bosch-sensortec.com/content/language1/downloads/BST-BMP085-DS000-06.pdf">
 * datasheet</a>.
 *
 * Also compatible to and tested with BMP180.
 *
 * @author	Niklas Hauser
 * @author  strongly-typed
 * @ingroup driver_pressure
 *
 * @tparam I2cMaster I2C interface
 */
template < typename I2cMaster >
class Bmp085 : public bmp085, public xpcc::I2cDevice<I2cMaster, 1>
{
public:
	/**
	 * @param	data		pointer to buffer of the internal data of type Data
	 * @param	address		address defaults to 0x77
	 */
	Bmp085(Data &data, uint8_t address=0x77);

	// MARK: - TASKS
	/// Reads out and stores the calibration bytes
	xpcc::co::Result<bool>
	configure(Mode mode = Mode::Standard);

	/// Do a readout sequence to convert and read temperature and then pressure from sensor
	xpcc::co::Result<bool>
	readout();

	/// Configures the sensor
	void inline
	setMode(Mode mode)
	{
		data.meta &= ~i(Mode::Mask);
		data.meta |= i(mode);
	}

public:
	/// Get the data object for this sensor.
	inline Data&
	getData()
	{ return data; }

private:
	Data &data;
	xpcc::ShortTimeout timeout;

	/**
	 * Maximum conversion time for pressure from datasheet for
	 * different oversampling settings, from ultra low power to
	 * ultra high resolution in milliseconds.
	 */
	static constexpr uint8_t conversionDelay[] = {5, 8, 14, 26};

	// Command buffer for writing to the device
	uint8_t buffer[2];
	uint8_t bufferedMode;
};

}	// namespace xpcc

#include "bmp085_impl.hpp"

#endif // XPCC_BMP085_HPP
