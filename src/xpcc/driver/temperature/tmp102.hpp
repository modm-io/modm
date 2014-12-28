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

#include <xpcc/architecture/interface/register.hpp>
#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/processing/coroutine.hpp>
#include <xpcc/math/utils/endianness.hpp>

namespace xpcc
{

// forward declaration for friending with tmp102::Data
template < class I2cMaster >
class Tmp102;

struct tmp102
{
protected:
	/// @cond
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
		OneShot = Bit7,
		// Resolution 6:5
		// Fault Queue 4:3
		Polarity = Bit2,
		ThermostatMode = Bit1,
		ShutdownMode = Bit0,
	};
	XPCC_FLAGS8(Config1);
	typedef Value< Config1_t, 2, 5 > Resolution_t;

	enum class
	Config2 : uint8_t
	{
		Alert = Bit5,
		ExtendedMode = Bit4,
	};
	XPCC_FLAGS8(Config2);

	enum class
	ConversionRate : uint8_t
	{
		Hz0_25 = 0,
		Hz1 = Bit6,
		Hz4 = Bit7,
		Hz8 = Bit7 | Bit6,
	};
	typedef Configuration< Config2_t, ConversionRate, (Bit7 | Bit6) > ConversionRate_t;

	XPCC_FLAGS_GROUP(Register_t,
			Config1_t, Config2_t);

	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }
	/// @endcond

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
		Faults1 = 0,
		Faults2 = Bit3,
		Faults4 = Bit4,
		Faults6 = Bit4 | Bit3
	};
protected:
	/// @cond
	typedef Configuration< Config1_t, FaultQueue, (Bit4 | Bit3) > FaultQueue_t;
	/// @endcond
public:

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
			int16_t temp = xpcc::fromBigEndian(*rData);
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
 * @ingroup driver_temperature
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
	xpcc::Timeout<> timeout;
	uint16_t updateTime;

	Config1_t config_msb;
	Config2_t config_lsb;

	volatile uint8_t i2cTask;
	volatile uint8_t i2cSuccess;
	xpcc::I2cTagAdapter< xpcc::I2cWriteReadAdapter > adapter;
};

} // namespace xpcc

#include "tmp102_impl.hpp"

#endif // XPCC_TMP102_HPP
