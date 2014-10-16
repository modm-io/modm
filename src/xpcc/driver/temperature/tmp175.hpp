// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TMP175_HPP
#define XPCC_TMP175_HPP

#include <stdint.h>
#include <xpcc/architecture/peripheral/i2c_adapter.hpp>

namespace xpcc
{

/**
 * @see		TMP175
 * @ingroup	temperature
 */
namespace tmp175
{
	enum Register
	{
		REGISTER_TEMPERATURE = 0x00,
		REGISTER_CONFIGURATION = 0x01,
		REGISTER_T_LOW = 0x02,
		REGISTER_T_HIGH = 0x03
	};

	enum Temperature
	{
		TEMPERATURE_EXTENDED_MODE = 0x01
	};

	enum Config1
	{// first byte
		CONFIGURATION_SHUTDOWN_MODE = 0x01,
		CONFIGURATION_THERMOSTAT_MODE = 0x02,
		CONFIGURATION_POLARITY = 0x04,
		CONFIGURATION_FAULT_QUEUE = (0b11 << 3),
		CONFIGURATION_FAULT_QUEUE_1 = (0b00 << 3),
		CONFIGURATION_FAULT_QUEUE_2 = (0b01 << 3),
		CONFIGURATION_FAULT_QUEUE_4 = (0b10 << 3),
		CONFIGURATION_FAULT_QUEUE_6 = (0b11 << 3),
		CONFIGURATION_CONVERTER_RESOLUTION = (0b11 << 5),
		CONFIGURATION_CONVERTER_RESOLUTION_9BIT  = (0b00 << 5),
		CONFIGURATION_CONVERTER_RESOLUTION_10BIT = (0b01 << 5),
		CONFIGURATION_CONVERTER_RESOLUTION_11BIT = (0b10 << 5),
		CONFIGURATION_CONVERTER_RESOLUTION_12BIT = (0b11 << 5),
		CONFIGURATION_ONE_SHOT = 0x80
	};
}

/**
 * @brief	TMP175 digital temperature sensor driver
 *
 * The TMP175 is a digital temperature sensor with a two-wire interface
 * and measures temperature over a range of -40 to +125 deg Celsius with a
 * resolution of 1/16 (0.0625) deg C and an accuracy of up to 1.5 deg C.
 *
 * The sensor has a default refresh rate of 4Hz but can be raised up to
 * 30Hz by repeatedly manually starting a conversion (with
 * startConversion()), which lasts 26ms.
 *
 * To convert the raw data into degrees Celsius, cast the MSB and LSB into
 * a signed 16bit integer, shift it right by 4 (or 3 in extended mode) and
 * divide by 16 (or use the getTemperature() method).
 *
 * If you are only interested in the integer value of the temperature,
 * simply only use the MSB (getData()[0]) when not in extended mode.
 *
 * @see <a href="http://www.ti.com/lit/ds/symlink/tmp175.pdf">Datasheet</a>
 *
 * @ingroup temperature
 * @author	Niklas Hauser
 *
 * @tparam I2cMaster Asynchronous Interface
 */
template < typename I2cMaster >
class Tmp175 : protected xpcc::I2cWriteReadAdapter
{
public:
	/**
	 * @param	data		pointer to a 2 uint8_t buffer
	 * @param	address		Default address is 0b0110111
	 */
	Tmp175(uint8_t* data, uint8_t address=0b0110111);

	bool
	configure(tmp175::Config1 msb=tmp175::CONFIGURATION_CONVERTER_RESOLUTION_12BIT);

	/// starts a temperature conversion right now
	ALWAYS_INLINE void
	startConversion();

	/**
	 * read the Temperature registers and buffer the results
	 * sets isNewDataAvailable() to @c true
	 */
	ALWAYS_INLINE void
	readTemperature();

	/**
	 * @c true, when new data has been read from the sensor and is buffered,
	 * @c false, when the data has been accessed
	 */
	ALWAYS_INLINE bool
	isNewDataAvailable();

	/// @return pointer to 8bit array containing temperature as big endian int16_t
	uint8_t*
	getData();

	/// @return the temperature as a signed float in Celsius
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

#include "tmp175_impl.hpp"

#endif // XPCC_TMP175_HPP
