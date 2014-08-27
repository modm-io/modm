// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TMP102_HPP
#define XPCC_TMP102_HPP

#include <stdint.h>
#include <xpcc/architecture/peripheral/i2c_device.hpp>
#include <xpcc/architecture/peripheral/i2c_transaction.hpp>

namespace xpcc
{

struct tmp102
{
	enum class
	Register : uint8_t
	{
		Temperature = 0x00,
		Configuration = 0x01,
		LowTemperature = 0x02,
		HighTemperature = 0x03
	};

	enum Configuration
	{// first byte
		CONFIGURATION_SHUTDOWN_MODE = 0x01,
		CONFIGURATION_THERMOSTAT_MODE = 0x02,
		CONFIGURATION_POLARITY = 0x04,
		CONFIGURATION_FAULT_QUEUE = 0x18,
		CONFIGURATION_FAULT_QUEUE_1 = 0x00,
		CONFIGURATION_FAULT_QUEUE_2 = 0x08,
		CONFIGURATION_FAULT_QUEUE_4 = 0x10,
		CONFIGURATION_FAULT_QUEUE_6 = 0x18,
		CONFIGURATION_CONVERTER_RESOLUTION = 0x60,
		CONFIGURATION_CONVERTER_RESOLUTION_12BIT = 0x60,
		CONFIGURATION_ONE_SHOT = 0x80
	};

	enum ConfigurationExtended
	{// second byte
		CONFIGURATION_EXTENDED_MODE = 0x10,
		CONFIGURATION_ALERT = 0x20,
		CONFIGURATION_CONVERSION_RATE = 0xc0,
		CONFIGURATION_CONVERSION_RATE_0_25HZ = 0x00,
		CONFIGURATION_CONVERSION_RATE_1HZ = 0x40,
		CONFIGURATION_CONVERSION_RATE_4HZ = 0x80,
		CONFIGURATION_CONVERSION_RATE_8HZ = 0xc0
	};
};

/**
 * @brief	TMP102 digital temperature sensor driver
 *
 * The TMP102 is a digital temperature sensor with a two-wire interface
 * and measures temperature over a range of -40 to +125 deg Celsius with a
 * resolution of 1/16 (0.0625) deg C and an accuracy of up to 0.5 deg C.
 *
 * The sensor has a default refresh rate of 4Hz but can be raised up to
 * 30Hz by repeatedly manually starting a conversion (with
 * startConversion()), which lasts 26ms.
 *
 * To convert the raw data into degrees Celsius, cast the MSB and LSB into
 * a signed 16bit integer, shift it right by 4 (or 3 in extended mode) and
 * devide by 16 (or use the getTemperature() method).
 *
 * If you are only interested in the integer value of the temperature,
 * simply only use the MSB (getData()[0]) when not in extended mode.
 *
 * @see <a href="http://www.ti.com/lit/ds/symlink/tmp102.pdf">Datasheet</a>
 *
 * @ingroup temperature
 * @author	Niklas Hauser
 *
 * @tparam I2cMaster Asynchronous Interface
 */
template < class I2cMaster >
class Tmp102 : public xpcc::I2cDevice< I2cMaster >, public tmp102
{
public:
	/**
	 * @param	data		pointer to a 2 uint8_t buffer
	 * @param	address		Default address is 0x48 (alternatives are 0x49, 0x4A and 0x4B)
	 */
	Tmp102(uint8_t* data, uint8_t address=0x48);

	/// @return pointer to 8bit array containing temperature as big endian int16_t
	ALWAYS_INLINE uint8_t*
	getData();

	// MARK: - Tasks
	/// pings the sensor
	bool
	startPing();

	bool ALWAYS_INLINE
	runPing();

	bool ALWAYS_INLINE
	isPingSuccessful();

	/// sets the LSB and MSB of the sensor
	bool
	startConfiguration(uint8_t lsb=CONFIGURATION_CONVERSION_RATE_4HZ,
					   uint8_t msb=CONFIGURATION_CONVERTER_RESOLUTION_12BIT);

	bool ALWAYS_INLINE
	runConfiguration();

	bool ALWAYS_INLINE
	isConfigurationSuccessful();

	/// starts a temperature conversion right now
	bool
	startConversion();

	/// runs the temperature conversion
	bool ALWAYS_INLINE
	runConversion();

	bool ALWAYS_INLINE
	isConversionSuccessful();

	/// read the Temperature registers and buffer the results
	bool
	startReadTemperature();

	bool ALWAYS_INLINE
	runReadTemperature();

	bool ALWAYS_INLINE
	isReadTemperatureSuccessful();

	// MARK: - utility
	/// @return the temperature as a signed float in Celcius
	float
	getTemperature();

private:
	struct Task
	{
		enum
		{
			Idle = 0,
			ReadTemperature,
			StartConversion,
			Configuration,
			Ping
		};
	};

	volatile uint8_t task;
	volatile uint8_t success;
	uint8_t config;
	uint8_t* data;
	uint8_t buffer[3];

	xpcc::I2cTagAdapter< xpcc::I2cWriteReadAdapter > adapter;
};

} // namespace xpcc

#include "tmp102_impl.hpp"

#endif // XPCC_TMP102_HPP
