/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/fiber.hpp>
#include <modm/architecture/interface/i2c_device.hpp>

namespace modm
{

/// @ingroup modm_driver_stmpe811
struct stmpe811
{
	enum class Register : uint8_t
	{
		ChipId = 0x00,
		SysCtrl1 = 0x03,
		SysCtrl2 = 0x04,
		IntSta = 0x0B,
		GpioAf = 0x17,
		AdcCtrl1 = 0x20,
		AdcCtrl2 = 0x21,
		TscCtrl = 0x40,
		TscCfg = 0x41,
		FifoTh = 0x4A,
		FifoSta = 0x4B,
		FifoSize = 0x4C,
		TscFractionZ = 0x56,
		TscIDrive = 0x58,
		TscDataXyz = 0xD7,
	};

	struct Point
	{
		uint16_t x; ///< raw 12-bit ADC value
		uint16_t y; ///< raw 12-bit ADC value
	};
};

/**
 * STMPE811 resistive touch screen controller.
 *
 * @ingroup modm_driver_stmpe811
 */
template< class I2cMaster >
class Stmpe811 : public stmpe811, public modm::I2cDevice< I2cMaster >
{
public:
	constexpr Stmpe811(uint8_t address = 0x41) : modm::I2cDevice< I2cMaster >(address) {}

	/// Resets the controller and enables the touch screen in X/Y mode.
	/// @return false if the chip does not answer with its ID.
	bool
	initialize()
	{
		uint8_t id[2];
		if (not read(Register::ChipId, id, 2) or id[0] != 0x08 or id[1] != 0x11) return false;
		// Soft reset
		write(Register::SysCtrl1, 0x02);
		modm::this_fiber::sleep_for(std::chrono::milliseconds(10));
		write(Register::SysCtrl1, 0x00);
		modm::this_fiber::sleep_for(std::chrono::milliseconds(2));
		return
			// Enable the ADC and touch screen clocks, the GPIO and temperature sensor stay off
			write(Register::SysCtrl2, 0x0C) and
			// The touch screen pins use their alternate function
			write(Register::GpioAf, 0x00) and
			// 80 clock sample time, 12-bit, internal reference, ADC clock 3.25 MHz
			write(Register::AdcCtrl1, 0x49) and
			write(Register::AdcCtrl2, 0x01) and
			// Average 4 samples, 500us touch detect delay, 500us settling time
			write(Register::TscCfg, 0x9A) and
			write(Register::FifoTh, 0x01) and
			write(Register::FifoSta, 0x01) and
			write(Register::FifoSta, 0x00) and
			write(Register::TscFractionZ, 0x01) and
			write(Register::TscIDrive, 0x01) and
			// Enable the touch screen controller in X/Y acquisition mode
			write(Register::TscCtrl, 0x03) and
			write(Register::IntSta, 0xFF);
	}

	/// Reads the newest raw position, if the screen is touched.
	/// @return true if the screen is touched and the point is valid.
	bool
	read(Point &point)
	{
		uint8_t status, size;
		if (not read(Register::TscCtrl, &status, 1) or not (status & 0x80)) return false;
		if (not read(Register::FifoSize, &size, 1) or size == 0) return false;
		// Drop all but the newest sample
		uint8_t data[4];
		while (size--) if (not read(Register::TscDataXyz, data, 4)) return false;
		point.x = (uint16_t(data[0]) << 4) | (data[1] >> 4);
		point.y = (uint16_t(data[1] & 0x0F) << 8) | data[2];
		return true;
	}

protected:
	bool
	write(Register reg, uint8_t value)
	{
		buffer[0] = uint8_t(reg);
		buffer[1] = value;
		return modm::I2cDevice< I2cMaster >::write(buffer, 2);
	}

	bool
	read(Register reg, uint8_t *data, uint8_t length)
	{
		buffer[0] = uint8_t(reg);
		return modm::I2cDevice< I2cMaster >::writeRead(buffer, 1, data, length);
	}

private:
	uint8_t buffer[2];
};

}	// namespace modm
