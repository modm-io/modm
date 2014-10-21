// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__BMP085_HPP
#define XPCC__BMP085_HPP

#include <xpcc/processing/protothread.hpp>
#include <xpcc/processing/coroutine.hpp>
#include <xpcc/architecture/peripheral/i2c_device.hpp>
#include <xpcc/processing/timeout.hpp>

namespace xpcc
{

struct bmp085
{
	/// The addresses of the Configuration and Data Registers
	enum Register
	{
		REGISTER_CHIP_ID         = 0xD0,
		REGISTER_VERSION         = 0xD1,
		REGISTER_CAL_AC1         = 0xAA,
		REGISTER_CAL_AC2         = 0xAC,
		REGISTER_CAL_AC3         = 0xAE,
		REGISTER_CAL_AC4         = 0xB0,
		REGISTER_CAL_AC5         = 0xB2,
		REGISTER_CAL_AC6         = 0xB4,
		REGISTER_CAL_B1          = 0xB6,
		REGISTER_CAL_B2          = 0xB8,
		REGISTER_CAL_MB          = 0xBA,
		REGISTER_CAL_MC          = 0xBC,
		REGISTER_CAL_MD          = 0xBE,
		REGISTER_CONTROL         = 0xF4,
		REGISTER_CONVERSION_MSB  = 0xF6,
		REGISTER_CONVERSION_LSB  = 0xF7,
		REGISTER_CONVERSION_XLSB = 0xF8,
	};

	/// The options of REGISTER_CHIP_ID
	enum ChipId {
		CHIP_ID = 0x55
	};

	/// The options of REGISTER_CONTROL
	enum Control {
		CONVERSION             = 0x3F,
		CONVERSION_TEMPERATURE = 0x2E,
		CONVERSION_PRESSURE    = 0x34
	};

	enum Mode {
		MODE                       = (0x03 << 6),
		MODE_ULTRA_LOW_POWER       = (0x00 << 6),
		MODE_STANDARD              = (0x01 << 6),
		MODE_HIGH_RESOLUTION       = (0x02 << 6),
		MODE_ULTRA_HIGH_RESOLUTION = (0x03 << 6),
	};
};

/**
 * \brief BMP085 digital absolute pressure sensor driver
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
 * \author	Niklas Hauser
 * \author  strongly-typed
 * \ingroup pressure
 *
 * \tparam I2cMaster I2C interface
 */
template < typename I2cMaster >
class Bmp085 : public bmp085, public xpcc::I2cDevice<I2cMaster>,
			   public xpcc::co::NestedCoroutine<1>
{
public:
	/**
	 * \param	data		pointer to buffer of 5 bytes
	 * \param	address		address defaults to 0x77
	 */
	Bmp085(uint8_t* data, uint8_t address=0x77);


	// Mark: - Set flags to start certain operations
	/// starts a temperature conversion which lasts 4.5ms
	ALWAYS_INLINE void
	startTemperatureMeasurement();

	/// read the result registers and buffers the results
	ALWAYS_INLINE void
	readTemperature();

	/// starts a pressure conversion which lasts from 4.5ms to 25.5ms
	ALWAYS_INLINE void
	startPressureMeasurement();

	/// read the result registers and buffers the results
	ALWAYS_INLINE void
	readPressure();

	/// starts all conversions and readouts in the right sequence using the right timing
	ALWAYS_INLINE void
	startReadoutSequence();


	// Mark: - Accessors to internally buffered data. No I2C transactions involved
	/// \return pointer to 8bit array containing tp temperature and pressure
	inline uint8_t*
	getData();

	/// \return pointer to 8bit array containing calibration data
	ALWAYS_INLINE uint16_t*
	getCalibrationData();
	
	/**
	 * \c true, when new pressure data has been from the sensor and buffered,
	 * \c false, when the data has been accessed once, or data is being
	 * copied into the buffer.
	 */
	ALWAYS_INLINE bool
	isNewDataAvailable();

	/**
	 * Get the calibrated temperature for the device in [ToDo unit]
	 *
	 * If recalculation is necessary it is done on the fly.
	 * No I2C transaction.
	 */
	int16_t*
	getCalibratedTemperature();

	/**
	 * Get the calibrated pressure from the device in [ToDo unit].
	 *
	 * If recalculation is necessary it is done on the fly.
	 * No I2C transaction.
	 */
	int32_t*
	getCalibratedPressure();

	/// Must be called periodically
	void
	update();

	// MARK: - TASKS
	/// pings the sensor
	xpcc::co::Result<bool>
	ping(void *ctx);

	/// Configures the sensor and reads out and stores the calibration bytes
	xpcc::co::Result<bool>
	configure(void *ctx, bmp085::Mode mode = bmp085::MODE_STANDARD);


private:
	xpcc::Timeout<> timeout;

	enum Running {
		NOTHING_RUNNING,
		START_TEMPERATURE_RUNNING,
		READ_TEMPERATURE_RUNNING,
		START_PRESSURE_RUNNING,
		READ_PRESSURE_RUNNING,
	};

	enum Status {
		START_TEMPERATURE_PENDING = 0x01,
		READ_TEMPERATURE_PENDING  = 0x02,
		START_PRESSURE_PENDING    = 0x04,
		READ_PRESSURE_PENDING     = 0x08,

		NEW_TEMPERATURE_DATA      = 0x20,
		NEW_PRESSURE_DATA         = 0x40,
		READOUT_SEQUENCE          = 0x80,
	};

	enum Calculation {
		TEMPERATURE_NEEDS_UPDATE = 0x01,
		PRESSURE_NEEDS_UPDATE    = 0x02,
	};

	bmp085::Mode config;
	Running running;
	uint8_t status;
	uint8_t calculation;
	uint8_t* data;
	uint8_t buffer[3];

	struct __attribute__ ((packed))
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
	} calibration ;

	int16_t calibratedTemperature;
	int32_t calibratedPressure;
	// calculated in getTemperature, needed for getPressure
	int32_t b5;

private:

	struct I2cTask
	{
		enum
		{
			Idle = 0,
			Configure,
			Ping,
		};
	};

	volatile uint8_t i2cTask;
	volatile uint8_t i2cSuccess;

	xpcc::I2cTagAdapter< xpcc::I2cWriteReadAdapter > adapter;
};
}

#include "bmp085_impl.hpp"

#endif // XPCC__BMP085_HPP
