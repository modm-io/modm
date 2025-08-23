
// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2024, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/math/geometry/angle_int.hpp>

namespace modm
{

/// @ingroup modm_driver_as5600
struct as5600
{

	// @see datasheet page 19
	enum Register : uint8_t
	{
		ZMCO = 0x00,
		ZPOS = 0x01,       // two bytes
		MPOS = 0x03,       // two bytes
		MANG = 0x05,       // two bytes
		CONF = 0x07,       // two bytes
		ANGLE_RAW = 0x0C,  // two bytes
		ANGLE = 0x0E,      // two bytes
		STATUS = 0x0B,
		AGC = 0x1A,
		MAGNITUDE = 0x1B,  // two bytes
		BURN = 0xFF,

		// Only AS5600L supports changing the I2c address
		I2C_ADDR = 0x20,
		I2C_UPDT = 0x21,
	};

	// @see datasheet page 20
	enum class Config : uint16_t
	{
		PM0 = Bit0,  // Power Mode
		PM1 = Bit1,

		HYST0 = Bit2,  // Hysteresis
		HYST1 = Bit3,

		OUTS0 = Bit4,  // Output Stage
		OUTS1 = Bit5,

		PWM0 = Bit6,  // PWM Frequency
		PWM1 = Bit7,

		SF0 = Bit8,  // Slow Filter
		SF1 = Bit9,

		FTH0 = Bit10,  // Fast Filter Threshold
		FTH1 = Bit11,
		FTH2 = Bit12,

		WD = Bit13  // Watchdog    0: Off, 1: On
	};
	MODM_FLAGS16(Config);

	enum class PowerMode : uint16_t
	{
		NOM = 0,
		LPM1 = 1,
		LPM2 = 2,
		LPM3 = 3,
	};
	using PowerMode_t = Configuration<Config_t, PowerMode, 0b11, 0>;

	enum class Hysteresis : uint16_t
	{
		OFF = 0,
		LSB = 1,
		LSB2 = 2,
		LSB3 = 3
	};
	using Hysteresis_t = Configuration<Config_t, Hysteresis, 0b11, 2>;

	enum class OutputStage : uint16_t
	{
		ANALOG = 0,
		ANALOG_REDUCED = 1,
		PWM = 2
	};
	using OutputStage_t = Configuration<Config_t, OutputStage, 0b11, 4>;

	enum class PWMFrequency : uint16_t
	{
		Hz115 = 0,
		Hz230 = 1,
		Hz460 = 2,
		Hz920 = 3
	};
	using PWMFrequency_t = Configuration<Config_t, PWMFrequency, 0b11, 6>;

	enum class SlowFilter : uint16_t
	{
		x16 = 0,
		x8 = 1,
		x4 = 2,
		x2 = 3
	};
	using SlowFilter_t = Configuration<Config_t, SlowFilter, 0b11, 8>;

	enum class FastFilterThreshold : uint16_t
	{
		LSB6 = 0,
		LSB7 = 1,
		LSB9 = 2,
		LSB18 = 3,
		LSB21 = 4,
		LSB24 = 5,
		LSB10 = 6
	};
	using FastFilterThreshold_t = Configuration<Config_t, FastFilterThreshold, 0b11, 10>;

	enum class Burn : uint8_t
	{
		ANGLE = Bit3,  // Burn Angle data
		CONFIG = Bit2  // Burn Config data
	};

	// @see datasheet page 21
	enum class Status : uint8_t
	{
		MagnetTooStrong = Bit3,
		MagnetTooWeak = Bit4,
		MagnetDetected = Bit5
	};
	MODM_FLAGS8(Status);

	using Data = modm::IntegerAngle<12>;
};

/// @ingroup modm_driver_as5600
template<class I2cMaster>
class As5600 : public as5600, public modm::I2cDevice<I2cMaster, 2>
{
public:
	/** AS5600 has hardwired address 0x36
	 * AS4500L has default address 0x40 but supports programming a different one
	 */
	As5600(Data &data, uint8_t address = 0x36) : I2cDevice<I2cMaster, 2>(address), data(data) {}

	/** Reset to Power up state.
	 * Useful for developement, not required in production.
	 */
	bool
	reset()
	{
		bool success = true;

		/// The config registers span from 0x00 to 0x08
		for (uint8_t reg = 0x00; reg < 0x08; reg += 2)
		{
			buffer[0] = reg;
			buffer[1] = 0;
			buffer[2] = 0;

			this->transaction.configureWrite(buffer, 3);

			this->runTransaction();
			success &= this->wasTransactionSuccessful();
		}

		return success;
	}

	bool
	configure(Config_t config)
	{
		return write(Register::CONF, config.value);
	}

	bool
	setI2cAddress(uint8_t address)
	{
		return write(Register::I2C_ADDR, address);
	}

  /// Wait 1ms after setting the lower limit
	bool
	setLowerLimit(Data data)
	{
		return write(Register::ZPOS, data.data);
	}

  /// Wait 1ms after setting the upper limit
	bool
	setUpperLimit(Data data)
	{
		return write(Register::MPOS, data.data);
	}

	bool
	setMaxAngle(Data data)
	{
		return write(Register::MANG, data.data);
	}

  /// Permanently burn configurations
	/// @warning As5600 can be burned only 3 times!
	bool
	burn(Burn flags)
	{
		buffer[0] = static_cast<uint8_t>(flags);
		return write(Register::BURN, buffer, 1);
	}

	Data
	getRawValue()
	{
		const uint16_t result = read<uint16_t>(Register::ANGLE_RAW);

		// Raw value requires masking
		return Data(result & Data::max);
	}

	Status
	getStatus()
	{
		return static_cast<Status>(read<uint8_t>(Register::STATUS));
	}

	uint16_t
	getMagnitude()
	{
		return read<uint16_t>(Register::MAGNITUDE);
	}

	/** Automated Gain Control
	 *
	 * The AS5600 uses Automatic Gain Control in a closed loop to compensate
	 * for variations of the magnetic field strength due to changes of temperature,
	 * airgap between IC and magnet, and magnet degradation.
	 *
	 * For the most robust performance, the gain value should be in the center of its range.
	 * The airgap of the physical system can be adjusted to achieve this value.
	 *
	 * In 5V operation, range is 0-255
	 * In 3.3V operation, range is reduced to 0-128
	 */
	uint8_t
	getAgcValue()
	{
		return read<uint8_t>(Register::AGC);
	}

	bool
	read()
	{
		data.data = read<uint16_t>(Register::ANGLE);

		return this->wasTransactionSuccessful();
	}

	inline Data &
	getData()
	{
		return data;
	}

private:
	template<std::unsigned_integral T>
	bool
	write(Register reg, T value)
	{
		buffer[0] = reg;

		if constexpr (std::is_same_v<T, uint8_t>)
		{
			buffer[1] = value;
		} else
		{
			buffer[1] = value >> 8;
			buffer[2] = value;
		}

		this->transaction.configureWrite(buffer, 1 + sizeof(T));

		return this->runTransaction();
	}

	template<std::unsigned_integral T>
	T
	read(Register reg)
	{
		buffer[0] = reg;
		this->transaction.configureWriteRead(buffer, 1, buffer + 1, sizeof(T));

		this->runTransaction();

		T result;

		if constexpr (std::is_same_v<T, uint8_t>)
		{
			result = buffer[1];
		} else
		{
			result = buffer[1] << 8 | buffer[2];
		}

		return result;
	}

	Data &data;
	uint8_t buffer[3];
};
}  // namespace modm
