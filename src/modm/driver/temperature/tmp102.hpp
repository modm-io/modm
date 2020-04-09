/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2011-2016, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TMP102_HPP
#define MODM_TMP102_HPP

#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/math/utils/endianness.hpp>

#include "lm75.hpp"

namespace modm
{

// forward declaration for friending with tmp102::Data
template < class I2cMaster >
class Tmp102;

/// @ingroup modm_driver_tmp102
struct tmp102 : public lm75
{
protected:
	/// @cond
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
	MODM_FLAGS8(Config1);
	typedef Value< Config1_t, 2, 5 > Resolution_t;

	enum class
	Config2 : uint8_t
	{
		// Conversion 6:7
		Alert = Bit5,
		ExtendedMode = Bit4
	};
	MODM_FLAGS8(Config2);

	enum class
	ConversionRate : uint8_t
	{
		Hz0_25 = 0,
		Hz1 = Bit6,
		Hz4 = Bit7,
		Hz8 = Bit7 | Bit6,
	};
	typedef Configuration< Config2_t, ConversionRate, (Bit7 | Bit6) > ConversionRate_t;
	/// @endcond
public:

	struct modm_packed
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
			int16_t temp = modm::fromBigEndian(*rData);
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
 * @ingroup modm_driver_tmp102
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Tmp102 :	public tmp102, public Lm75< I2cMaster >,
				protected modm::pt::Protothread
{
public:
	/// Constructor, requires a tmp102::Data object,
	/// sets address to default of 0x48 (alternatives are 0x49, 0x4A and 0x4B).
	Tmp102(Data &data, uint8_t address=0x48);

	void modm_always_inline
	update()
	{ run(); }

	// MARK: Configuration
	// @param	rate	Update rate in Hz: 0 to 33. (Use 0 to update at 0.25Hz).
	modm::ResumableResult<bool>
	setUpdateRate(uint8_t rate);

	/// Enables extended mode with 13 bit data format.
	modm::ResumableResult<bool>
	enableExtendedMode(bool enable = true);

	/// param[out]	result	contains comparator mode alert in the configured polarity
	modm::ResumableResult<bool>
	readComparatorMode(bool &result);

	/// Writes the upper limit of the alarm.
	modm::ResumableResult<bool> modm_always_inline
	setUpperLimit(float temperature)
	{ return setLimitRegister(Register::TemperatureUpperLimit, temperature); }

	/// Writes the lower limit of the alarm.
	modm::ResumableResult<bool> modm_always_inline
	setLowerLimit(float temperature)
	{ return setLimitRegister(Register::TemperatureLowerLimit, temperature); }

	/// starts a temperature conversion right now
	modm::ResumableResult<bool>
	startConversion();

	inline Data&
	getData();

private:
	bool
	run();

	modm::ResumableResult<bool>
	writeConfiguration(uint8_t length=3);

	modm::ResumableResult<bool>
	setLimitRegister(Register reg, float temperature);

	uint16_t updateTime;
	modm::ShortTimeout timeout;

	Config2_t config_lsb;
};

} // namespace modm

#include "tmp102_impl.hpp"

#endif // MODM_TMP102_HPP
