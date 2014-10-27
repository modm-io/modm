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
#include <xpcc/processing/protothread.hpp>
#include <xpcc/processing/coroutine.hpp>

namespace xpcc
{

struct tmp102
{
	enum class
	ThermostatMode : uint8_t
	{
		Comparator = 0,
		Interrupt = 0x02
	};

	enum class
	AlertPolarity : uint8_t
	{
		ActiveLow = 0,
		ActiveHigh = 0x04
	};

	enum class
	FaultQueue : uint8_t
	{
		Faults1 = 0,
		Faults2 = 0x08,
		Faults4 = 0x10,
		Faults6 = 0x18,
	};

	struct __attribute__ ((packed))
	Data
	{
	public:
		/// @return the temperature as a signed float in Celsius
		float
		getTemperature()
		{
			uint16_t *rData = reinterpret_cast<uint16_t*>(data);
			int16_t temp = static_cast<int16_t>(xpcc::math::bigEndianToHost(*rData));
			if (data[1] & 0x01)
			{
				// temperature extended mode
				return (temp >> 3) / 16.f;
			}
			return (temp >> 4) / 16.f;
		}

		ALWAYS_INLINE uint8_t
		operator [](size_t index)
		{ return (index < 2) ? data[index] : 0; }

		ALWAYS_INLINE uint8_t*
		getPointer()
		{ return data; }

	private:
		uint8_t data[2];
	};
};

/**
 * TMP102 digital temperature sensor driver
 *
 * The TMP102 is a digital temperature sensor with a I2C interface
 * and measures temperature over a range of -40 to +125 deg Celsius with a
 * resolution of 1/16 (0.0625) deg C and an accuracy of up to 0.5 deg C.
 *
 * The sensor has a default refresh rate of 4Hz but can be set from
 * 0.25Hz up to 33Hz using `setUpdateRate(rate)`.
 * You can manually start a conversion with `startConversion()`, wait for
 * 30ms and then `readTemperature()` to achieve other (less frequent)
 * update rates.
 *
 * @see <a href="http://www.ti.com/lit/ds/symlink/tmp102.pdf">Datasheet</a>
 *
 * @ingroup temperature
 * @author	Niklas Hauser
 *
 * @tparam I2cMaster Asynchronous Interface
 */
template < class I2cMaster >
class Tmp102 :	public tmp102, public xpcc::I2cDevice< I2cMaster >,
				protected xpcc::pt::Protothread, protected xpcc::co::NestedCoroutine<1>
{
private:
	enum Register
	{
		REGISTER_TEMPERATURE = 0x00,
		REGISTER_CONFIGURATION = 0x01,
		REGISTER_LOW_TEMPERATURE = 0x02,
		REGISTER_HIGH_TEMPERATURE = 0x03
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

public:
	/**
	 * @param	data		pointer to a 2 uint8_t buffer
	 * @param	address		Default address is 0x48 (alternatives are 0x49, 0x4A and 0x4B)
	 */
	Tmp102(Data &data, uint8_t address=0x48);

	void
	update()
	{ run(); }

	// MARK: - Tasks
	/// pings the sensor
	xpcc::co::Result<bool>
	ping(void *ctx);

	// MARK: Configuration
	// @param	rate	Update rate in Hz: 0 to 33. (Use 0 to update at 0.25Hz).
	xpcc::co::Result<bool>
	setUpdateRate(void *ctx, uint8_t rate);

	/// Enables extended mode with 13 bit data format.
	xpcc::co::Result<bool>
	enableExtendedMode(void *ctx, bool enable = true);

	xpcc::co::Result<bool>
	configureAlertMode(void *ctx, ThermostatMode mode, AlertPolarity polarity, FaultQueue faults);

	xpcc::co::Result<bool> ALWAYS_INLINE
	writeUpperLimit(void *ctx, float temperature)
	{ return writeLimitRegister(ctx, REGISTER_HIGH_TEMPERATURE, temperature); }

	xpcc::co::Result<bool> ALWAYS_INLINE
	writeLowerLimit(void *ctx, float temperature)
	{ return writeLimitRegister(ctx, REGISTER_LOW_TEMPERATURE, temperature); }

	/// param[in]	result	contains comparator mode alert in the configured polarity
	xpcc::co::Result<bool>
	readComparatorMode(void *ctx, bool &result);

	/// starts a temperature conversion right now
	xpcc::co::Result<bool>
	startConversion(void *ctx);

	/// reads the Temperature registers and buffers the results
	xpcc::co::Result<bool>
	readTemperature(void *ctx);

public:
	Data &data;

private:
	bool
	run();

	xpcc::co::Result<bool>
	writeConfiguration(void *ctx, uint8_t length=3);

	xpcc::co::Result<bool>
	writeLimitRegister(void *ctx, Register reg, float temperature);

	enum I2cTask : uint8_t
	{
		Idle = 0,
		ReadTemperature,
		StartConversion,
		Configuration,
		LimitRegister,
		ReadAlert,
		Ping
	};

	uint8_t buffer[3];
	uint8_t config_msb;
	uint8_t config_lsb;
	xpcc::Timeout<> timeout;
	uint16_t updateTime;

	volatile uint8_t i2cTask;
	volatile uint8_t i2cSuccess;
	xpcc::I2cTagAdapter< xpcc::I2cWriteReadAdapter > adapter;
};

} // namespace xpcc

#include "tmp102_impl.hpp"

#endif // XPCC_TMP102_HPP
