/*
* Copyright (c) 2022, Christopher Durand
*
* This file is part of the modm project.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
// ----------------------------------------------------------------------------

#ifndef MODM_MCP990X_HPP
#define MODM_MCP990X_HPP

#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <array>

namespace modm
{

/// @ingroup modm_driver_mcp990x
struct mcp990x
{
protected:
	/// @cond
	enum class
	Register : uint8_t
	{
		IntTempHigh		= 0x00,
		Ext1TempHigh	= 0x01,
		Config			= 0x03,
		Ext1TempLow		= 0x10,
		Ext2TempHigh	= 0x23,
		Ext2TempLow		= 0x24,
		Ext1Ideality	= 0x27,
		Ext2Ideality	= 0x28,
		IntTempLow		= 0x29,
		Ext3TempHigh	= 0x2a,
		Ext3TempLow		= 0x2b,
		Ext3Ideality	= 0x31,
		ProductId		= 0xfd
	};

	enum class
	Config : uint8_t
	{
		MaskAlert = Bit7,
		Standby = Bit6,
		AlertMode = Bit5,
		DisableErrorCorrectionExternalDiode1 = Bit4,
		DisableErrorCorrectionExternalDiode2 = Bit3,
		ExtendedRange = Bit2,
		DisableDynamicAveraging = Bit1,
		DisableAntiParallelDiodeMode = Bit0
	};
	MODM_FLAGS8(Config);
	/// @endcond
public:
	enum class ExternalDiode : uint8_t
	{
		Diode1 = 0,
		Diode2 = 1,
		Diode3 = 2
	};

	struct modm_packed
	Data
	{
		static constexpr auto ExtendedOffset{64}; // °C
		uint8_t data[2];

		/// @return temperature in 0.125 °C steps
		constexpr int16_t
		getTemperatureFractional()
		{
			return ((data[1] << 3) | (data[0] >> 5)) - (ExtendedOffset * 8);
		}

		/// @return temperature integral part in °C
		constexpr int8_t
		getTemperatureInteger()
		{
			return data[1] - ExtendedOffset;
		}

		/// @return temperature as float in °C
		constexpr float
		getTemperature()
		{
			return getTemperatureFractional() / 8.f;
		}
	};
};

/**
 * Simple driver for MCP9902/3/4 I2C temperature sensor.
 *
 * @ingroup modm_driver_mcp990x
 *
 * @author Christopher Durand
 */
template <class I2cMaster>
class Mcp990x : public mcp990x, public I2cDevice<I2cMaster, 3>
{
public:
	/// \param address I2C address, MCP990xT-1: 0x4c, -2: 0x4d, -A: adjustable, see datasheet
	Mcp990x(Data& data, uint8_t address = 0x4d);

	/// Initialize sensor
	modm::ResumableResult<bool>
	initialize();

	/// Detect sensor
	modm::ResumableResult<bool>
	ping();

	/// Read internal temperature sensor
	/// \pre sensor is succesfully initialized
	modm::ResumableResult<bool>
	readInternalTemperature();

	/// Read external diode temperature sensor
	/// \pre sensor is succesfully initialized
	modm::ResumableResult<bool>
	readExternalDiodeTemperature(ExternalDiode diode);

	/// Set external diode ideality factor
	/// See datasheet for values
	modm::ResumableResult<bool>
	setExternalDiodeIdealityFactor(ExternalDiode diode, uint8_t idealitySetting);

	inline Data&
	getData()
	{ return data_; }

private:
	// MCP9902: 0x20, MCP9903: 0x21, MCP9904: 0x25
	static constexpr uint8_t DeviceIds[3]{0x20, 0x21, 0x25};

	static constexpr Register ExtHighReg[3] {
		Register::Ext1TempHigh,
		Register::Ext2TempHigh,
		Register::Ext3TempHigh
	};
	static constexpr Register ExtLowReg[3] {
		Register::Ext1TempLow,
		Register::Ext2TempLow,
		Register::Ext3TempLow
	};
	static constexpr Register ExtIdealityReg[3] {
		Register::Ext1Ideality,
		Register::Ext2Ideality,
		Register::Ext3Ideality
	};

	modm::ResumableResult<bool>
	write(Register reg, uint8_t config);

	modm::ResumableResult<bool>
	read(Register reg, uint8_t& value);

	modm::ResumableResult<bool>
	readTemperature(Register highReg, Register lowReg);

	Data& data_;
	std::array<uint8_t, 2> buffer_;
};

}	// namespace modm

#include "mcp990x_impl.hpp"

#endif // MODM_MCP990X_HPP
