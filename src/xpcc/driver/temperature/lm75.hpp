// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LM75_HPP
#define XPCC_LM75_HPP

#include <xpcc/architecture/interface/register.hpp>
#include <xpcc/architecture/interface/i2c_device.hpp>

namespace xpcc
{

// forward declaration for friending with lm75::Data
template < class I2cMaster >
class Lm75;

// forward declaration for friending with Lm75
template < class I2cMaster >
class Tmp102;

// forward declaration for friending with Lm75
template < class I2cMaster >
class Tmp175;

struct lm75
{
protected:
	/// @cond
	enum class
	Register : uint8_t
	{
		Temperature = 0x00,
		Configuration = 0x01,

		TemperatureLowerLimit = 0x02,
		TemperatureUpperLimit = 0x03
	};

	enum class
	Config1 : uint8_t
	{
		// Fault Queue 4:3
		Polarity = Bit2,
		ThermostatMode = Bit1,
		ShutdownMode = Bit0,
	};
	XPCC_FLAGS8(Config1);

public:
	enum class
	ThermostatMode : uint8_t
	{
		Comparator = 0,
		Interrupt = int(Config1::ThermostatMode)
	};

	enum class
	AlertPolarity : uint8_t
	{
		ActiveLow = 0,
		ActiveHigh = int(Config1::Polarity)
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
	struct xpcc_packed
	Data
	{
		template < class I2cMaster >
		friend class Lm75;

	public:
		/// @return the temperature as a signed float in Celsius
		float
		getTemperature()
		{
			int16_t *rData = reinterpret_cast<int16_t*>(data);
			int16_t temp = xpcc::fromBigEndian(*rData);
			return temp / 256.f;
		}

		/// @return only the signed integer part of the temperature in Celsius
		int8_t
		getTemperatureInteger()
		{
			return int8_t(data[0]);
		}

	private:
		uint8_t data[2];
	};
};

/**
 * LM75 digital temperature sensor driver.
 *
 * The LM75 is a digital temperature sensor with an I2C interface
 * and measures temperature over a range of -55 to +125 deg Celsius with
 * a resolution of 0.5 deg C and an accuracy of up to 2 deg C.
 *
 * The sensor has a default refresh rate between 3Hz and 10Hz which is
 * hardcoded into the sensor and cannot be changed.
 *
 * @ingroup	driver_temperature
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Lm75 : public lm75, public I2cDevice< I2cMaster, 2 >
{
	template < class OtherI2cMaster >
	friend class Tmp102;

	template < class OtherI2cMaster >
	friend class Tmp175;

public:
	/// Constructor, requires a lm75::Data object,
	/// sets address to default of 0x48 (7 alternative addresses up to 0x4F possible).
	Lm75(Data &data, uint8_t address=0x48);

	xpcc::ResumableResult<bool>
	configureAlertMode(ThermostatMode mode, AlertPolarity polarity, FaultQueue faults);

	/// Writes the upper limit of the alarm.
	xpcc::ResumableResult<bool> xpcc_always_inline
	setUpperLimit(float temperature)
	{ return setLimitRegister(Register::TemperatureUpperLimit, temperature); }

	/// Writes the lower limit of the alarm.
	xpcc::ResumableResult<bool> xpcc_always_inline
	setLowerLimit(float temperature)
	{ return setLimitRegister(Register::TemperatureLowerLimit, temperature); }

	/// reads the Temperature registers and buffers the results
	xpcc::ResumableResult<bool>
	readTemperature();

	inline Data&
	getData()
	{ return data; }

private:
	xpcc::ResumableResult<bool>
	setLimitRegister(Register reg, float temperature);

	Data &data;
	uint8_t buffer[3];
	Config1_t config_msb;
};

}	// namespace xpcc

#include "lm75_impl.hpp"

#endif // XPCC_LM75_HPP
