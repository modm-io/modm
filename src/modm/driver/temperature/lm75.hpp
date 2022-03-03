/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LM75_HPP
#define MODM_LM75_HPP

#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/i2c_device.hpp>

namespace modm
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

/// @ingroup modm_driver_lm75
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
	MODM_FLAGS8(Config1);

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
	struct modm_packed
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
			int16_t temp = modm::fromBigEndian(*rData);
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
 * @ingroup	modm_driver_lm75
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

	modm::ResumableResult<bool>
	configureAlertMode(ThermostatMode mode, AlertPolarity polarity, FaultQueue faults);

	/// Writes the upper limit of the alarm.
	modm::ResumableResult<bool>
	setUpperLimit(float temperature)
	{ return setLimitRegister(Register::TemperatureUpperLimit, temperature); }

	/// Writes the lower limit of the alarm.
	modm::ResumableResult<bool>
	setLowerLimit(float temperature)
	{ return setLimitRegister(Register::TemperatureLowerLimit, temperature); }

	/// reads the Temperature registers and buffers the results
	modm::ResumableResult<bool>
	readTemperature();

	inline Data&
	getData()
	{ return data; }

private:
	modm::ResumableResult<bool>
	setLimitRegister(Register reg, float temperature);

	Data &data;
	uint8_t buffer[3];
	Config1_t config_msb;
};

}	// namespace modm

#include "lm75_impl.hpp"

#endif // MODM_LM75_HPP
