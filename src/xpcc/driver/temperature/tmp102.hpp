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
#include <xpcc/architecture/peripheral/i2c_adapter.hpp>

namespace xpcc
{

namespace tmp102
{
	enum class
	Register
	{
		Temperature = 0x00,
		Configuration = 0x01,
		LowTemperature = 0x02,
		HighTemperature = 0x03
	};

	enum Temperature
	{
		TEMPERATURE_EXTENDED_MODE = 0x01
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
}

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
template < typename I2cMaster >
class Tmp102 : private xpcc::I2cWriteReadAdapter
{
public:
	/**
	 * @param	data		pointer to a 2 uint8_t buffer
	 * @param	address		Default address is 0x48 (alternatives are 0x49, 0x4A and 0x4B)
	 */
	Tmp102(uint8_t* data, uint8_t address=0x48);

	bool
	configure(uint8_t lsb=tmp102::CONFIGURATION_CONVERSION_RATE_4HZ,
			  uint8_t msb=tmp102::CONFIGURATION_CONVERTER_RESOLUTION_12BIT);

	/// starts a temperature conversion right now
	ALWAYS_INLINE void
	startConversion();

	/**
	 * read the Temperature registers and buffer the results
	 * sets isNewDataAvailable() to `true`
	 */
	ALWAYS_INLINE void
	readTemperature();

	/**
	 * `true`, when new data has been read from the sensor and is buffered,
	 * `false`, when the data has been accessed
	 */
	ALWAYS_INLINE bool
	isNewDataAvailable();

	/// @return pointer to 8bit array containing temperature as big endian int16_t
	uint8_t*
	getData();

	/// @return the temperature as a signed float in Celcius
	float
	getTemperature();

	void
	update();

private:
	enum class
	Running {
		Nothing,
		ReadTemperature,
		StartConversion
	} running;

	struct Status {
		bool startConversionPending	: 1;
		bool readTemperaturePending	: 1;
		bool newTemperatureData		: 1;
	} status;

	uint8_t config;
	uint8_t* data;
	uint8_t buffer[3];
};

} // namespace xpcc

#include "tmp102_impl.hpp"

#endif // XPCC_TMP102_HPP
