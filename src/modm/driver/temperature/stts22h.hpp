/*
* Copyright (c) 2021, Christopher Durand
*
* This file is part of the modm project.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
// ----------------------------------------------------------------------------

#ifndef MODM_STTS22H_HPP
#define MODM_STTS22H_HPP

#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <array>

namespace modm
{

/// @ingroup modm_driver_stts22h
struct stts22h
{
protected:
	/// @cond
	enum class
	Register : uint8_t
	{
		WhoAmI = 0x01,
		TempHLimit = 0x02,
		TempLLimit = 0x03,
		Ctrl = 0x04,
		Status = 0x05,
		TempLOut = 0x06,
		TempHOut = 0x07,
		SoftwareReset = 0x0C
	};

	enum class
	Ctrl : uint8_t
	{
		LowOdrStart = Bit7,
		Bdu = Bit6,
		Avg1 = Bit5,
		Avg0 = Bit4,
		IfAddInc = Bit3,
		FreeRun = Bit2,
		TimeOutDis = Bit1,
		OneShot = Bit0
	};
	MODM_FLAGS8(Ctrl);

	enum class
	SoftwareReset : uint8_t
	{
		LowOdrEnable = Bit6,
		SwReset = Bit1
	};
	MODM_FLAGS8(SoftwareReset);

	using RegisterValue = modm::FlagsGroup<Ctrl_t, SoftwareReset_t>;

	enum class
	Status : uint8_t
	{
		UnderThl = Bit2,
		OverThh = Bit1,
		Busy = Bit0
	};
	MODM_FLAGS8(Status);
	/// @endcond
public:
	struct modm_packed
	Data
	{
		uint8_t data[2];

		/// @return temperature in 1/100th °C
		constexpr int16_t
		getTemperatureFractional()
		{
			return (int16_t(data[1]) << 8) | data[0];
		}

		/// @return temperature as float in °C
		constexpr float
		getTemperature()
		{
			return getTemperatureFractional() / 100.0f;
		}
	};
};

/**
 * Simple driver for STTS22H I2C temperature sensor.
 *
 * Only 'free-run' mode is supported.
 *
 * @ingroup modm_driver_stts22h
 *
 * @author Christopher Durand
 */
template <class I2cMaster>
class Stts22h : public stts22h, public I2cDevice<I2cMaster, 3>
{
public:
	/// \param address I2C address, selectable on device between 0x38 and 0x3f
	Stts22h(Data& data, uint8_t address = 0x3f);

	/// Initialize sensor
	modm::ResumableResult<bool>
	initialize();

	/// Detect sensor
	modm::ResumableResult<bool>
	ping();

	/// Read temperature from device
	/// \pre sensor is succesfully initialized
	modm::ResumableResult<bool>
	readTemperature();

	inline Data&
	getData()
	{ return data_; }

private:
	static constexpr inline uint8_t DeviceId{0xA0};

	modm::ResumableResult<bool>
	write(Register reg, RegisterValue value);

	modm::ResumableResult<bool>
	read(Register reg, uint8_t& value);

	modm::ResumableResult<bool>
	read(Register reg, uint8_t* buffer, uint8_t length);

	Data& data_;
	std::array<uint8_t, 2> buffer_;
};

}	// namespace modm

#include "stts22h_impl.hpp"

#endif // MODM_STTS22H_HPP
