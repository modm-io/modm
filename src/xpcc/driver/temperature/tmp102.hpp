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

#include <xpcc/architecture/peripheral/register.hpp>
#include <xpcc/architecture/peripheral/i2c_device.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/processing/coroutine.hpp>
#include <xpcc/math/utils/endianess.hpp>

namespace xpcc
{

// forward declaration for friending with tmp102::Data
template < class I2cMaster >
class Tmp102;

struct tmp102
{
protected:
	enum class
	Register : uint8_t
	{
		Configuration = 0x01,

		Temperature = 0x00,
		TemperatureLsb = 0x02,
		TemperatureMsb = 0x03
	};

	enum class
	Config1 : uint8_t
	{
		OneShot = 0x80,
		ResolutionMask = 0x60,
		FaultQueue1 = 0x00,
		FaultQueue2 = 0x08,
		FaultQueue4 = 0x10,
		FaultQueue6 = 0x18,
		FaultQueueMask = 0x18,
		Polarity = 0x04,
		ThermostatMode = 0x02,
		ShutdownMode = 0x02,
	};
	REGISTER8(Config1);

	enum class
	Config2 : uint8_t
	{
		ExtendedMode = 0x10,
		Alert = 0x20,
		ConversionRate0_25Hz = 0x00,
		ConversionRate1Hz = 0x40,
		ConversionRate4Hz = 0x80,
		ConversionRate8Hz = 0xc0,
		ConversionRateMask = 0xc0,
	};
	REGISTER8(Config2);

	REGISTER8_GROUP(Register,
			Config1, Config2);

	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }

public:
	enum class
	ThermostatMode : uint8_t
	{
		Comparator = 0,
		Interrupt = Config1::ThermostatMode
	};

	enum class
	AlertPolarity : uint8_t
	{
		ActiveLow = 0,
		ActiveHigh = Config1::Polarity
	};

	enum class
	FaultQueue : uint8_t
	{
		Faults1 = Config1::FaultQueue1,
		Faults2 = Config1::FaultQueue2,
		Faults4 = Config1::FaultQueue4,
		Faults6 = Config1::FaultQueue6
	};

	struct ATTRIBUTE_PACKED
	Data
	{
		template < class I2cMaster >
		friend class Tmp102;

	public:
		/// @return the temperature as a signed float in Celsius
		float
		getTemperature()
		{
			int16_t *rData = reinterpret_cast<int16_t*>(data);
			int16_t temp = xpcc::math::fromBigEndian(*rData);
			if (data[1] & 0x01)
			{
				// temperature extended mode
				return temp / 128.f;
			}
			return temp / 256.f;
		}

		/// @return only the signed integer part of the temperature in Celsius
		int16_t
		getTemperatureInteger()
		{
			if (data[1] & 0x01)
			{
				// temperature extended mode
				return int16_t((uint16_t(data[0]) << 1) | (data[1] >> 7));
			}
			return int16_t(data[0]);
		}

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
public:
	/// Constructor, requires a tmp102::Data object,
	/// sets address to default of 0x48 (alternatives are 0x49, 0x4A and 0x4B)
	/**
	 * @param	data		the associated Data object
	 * @param	address		Default address is 0x48 (alternatives are 0x49, 0x4A and 0x4B)
	 */
	Tmp102(Data &data, uint8_t address=0x48);

	void ALWAYS_INLINE
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
	{ return writeLimitRegister(ctx, Register::TemperatureMsb, temperature); }

	xpcc::co::Result<bool> ALWAYS_INLINE
	writeLowerLimit(void *ctx, float temperature)
	{ return writeLimitRegister(ctx, Register::TemperatureLsb, temperature); }

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

	enum
	I2cTask : uint8_t
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
	Config1_t config_msb;
	Config2_t config_lsb;
	xpcc::Timeout<> timeout;
	uint16_t updateTime;

	volatile uint8_t i2cTask;
	volatile uint8_t i2cSuccess;
	xpcc::I2cTagAdapter< xpcc::I2cWriteReadAdapter > adapter;
};

} // namespace xpcc

#include "tmp102_impl.hpp"

#endif // XPCC_TMP102_HPP
