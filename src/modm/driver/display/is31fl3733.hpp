/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/resumable.hpp>
#include <cstring>

namespace modm
{

/// @ingroup modm_driver_is31fl3733
struct is31fl3733
{
	static constexpr uint8_t SizeX{12};
	static constexpr uint8_t SizeY{16};

	enum class
	Addr : uint8_t
	{
		GND = 0b00,
		SCL = 0b01,
		SDA = 0b10,
		VCC = 0b11
	};

	/// Available I2C addresses
	static constexpr uint8_t
	addr(Addr addr2 = Addr::GND, Addr addr1 = Addr::GND)
	{
		return 0b1010000 | (uint8_t(addr2) << 2) | uint8_t(addr1);
	};

	enum class
	Register : uint16_t
	{
		// Page 0
		LED_ON_OFF = 0x8000,
		LED_OPEN = 0x8018,
		LED_SHORT = 0x8030,

		// Page 1
		PWM = 0x8100,

		// Page 2
		AUTO_BREATH_MODE = 0x8200,

		// Page 3
		CONFIGURATION = 0x8300,
		GLOBAL_CURRENT_CONTROL = 0x8301,
		ABM_1 = 0x8302,
		ABM_2 = 0x8306,
		ABM_3 = 0x830A,
		TIME_UPDATE = 0x830E,
		SW_PULL_UP = 0x830F,
		CS_PULL_DOWN = 0x8310,
		RESET = 0x8311,

		// Global
		COMMAND = 0xFD,
		COMMAND_WRITE_LOCK = 0xFE,
		INTERRUPT_MASK = 0xF0,
		INTERRUPT_STATUS =  0xF1
	};

	enum class
	Resistor : uint8_t
	{
		None = 0,
		kOhm_0_5 = 1,
		kOhm_1 = 2,
		kOhm_2 = 3,
		kOhm_4 = 4,
		kOhm_8 = 5,
		kOhm_16 = 6,
		kOhm_32 = 7,
	};

	using LedBinaryData = uint16_t[SizeX];

protected:
	static constexpr uint8_t LED_ON_OFF_size = 0x18;
	static constexpr uint8_t LED_OPEN_size = LED_ON_OFF_size;
	static constexpr uint8_t LED_SHORT_size = LED_ON_OFF_size;
	static constexpr uint8_t PWM_size = 0xC0;
	static constexpr uint8_t AUTO_BREATH_MODE_size = PWM_size;

	static constexpr bool
	hasPage(Register reg)
	{ return uint16_t(reg) & 0x8000; }

	static constexpr uint8_t
	getPage(Register reg)
	{ return ((uint16_t(reg) >> 8) & 0x0f); }
};

/**
 * @ingroup modm_driver_is31fl3733
 * @author  Niklas Hauser
 */
template < class I2cMaster >
class Is31fl3733 : public is31fl3733, public modm::I2cDevice<I2cMaster, 4>
{
public:
	Is31fl3733(uint8_t address=addr()):
		I2cDevice<I2cMaster,4>(address)
	{}

	bool
	enable(uint8_t x, uint8_t y)
	{
		if (x < SizeX and y < SizeY)
		{
			data.led_on_off[x] |= (1u << y);
			return true;
		}
		return false;
	}
	void enableAll()
	{ std::memset(data.led_on_off, 0xff, sizeof(data.led_on_off)); }

	bool
	disable(uint8_t x, uint8_t y)
	{
		if (x < SizeX and y < SizeY)
		{
			data.led_on_off[x] &= ~(1u << y);
			return true;
		}
		return false;
	}
	void disableAll()
	{ std::memset(data.led_on_off, 0, sizeof(data.led_on_off)); }

	bool
	setPwm(uint8_t x, uint8_t y, uint8_t pwm)
	{
		if (x < SizeX and y < SizeY)
		{
			data.led_pwm[x][y] = pwm;
			return true;
		}
		return false;
	}
	void setAllPwm(uint8_t pwm)
	{ std::memset(data.led_pwm, pwm, sizeof(data.led_pwm)); }

	LedBinaryData&
	ledsOpen()
	{ return led_open; }

	LedBinaryData&
	ledsShort()
	{ return led_short; }

public:
	modm::ResumableResult<bool>
	reset()
	{ return readRegister(Register::RESET, buffer); }

	modm::ResumableResult<bool>
	setGlobalCurrent(uint8_t current)
	{ return writeRegister(Register::GLOBAL_CURRENT_CONTROL, current); }

	modm::ResumableResult<bool>
	clearSoftwareShutdown()
	{ return writeRegister(Register::CONFIGURATION, 0x01); }

	modm::ResumableResult<bool>
	setSwPullUp(Resistor value)
	{ return writeRegister(Register::SW_PULL_UP, uint8_t(value)); }

	modm::ResumableResult<bool>
	setCsPullDown(Resistor value)
	{ return writeRegister(Register::CS_PULL_DOWN, uint8_t(value)); }

	modm::ResumableResult<bool>
	triggerOpenShortDetection()
	{ return writeRegister(Register::CONFIGURATION, 0b101); }

	modm::ResumableResult<bool>
	readOpenShort()
	{
		RF_BEGIN();
		if (not RF_CALL(setPage(Register::LED_OPEN))) RF_RETURN(false);

		buffer[0] = uint8_t(Register::LED_OPEN);
		this->transaction.configureWriteRead(buffer, 1, (uint8_t*)&led_open, LED_OPEN_size + LED_SHORT_size);
		RF_END_RETURN_CALL(this->runTransaction());
	}

	modm::ResumableResult<bool>
	writeOnOff()
	{
		RF_BEGIN();
		if (not RF_CALL(setPage(Register::LED_ON_OFF))) RF_RETURN(false);

		this->transaction.configureWrite(&data.addr_led_on_off, LED_ON_OFF_size+1);
		RF_END_RETURN_CALL(this->runTransaction());
	}

	modm::ResumableResult<bool>
	writePwm()
	{
		RF_BEGIN();
		if (not RF_CALL(setPage(Register::PWM))) RF_RETURN(false);

		this->transaction.configureWrite(&data.addr_led_pwm, PWM_size+1);
		RF_END_RETURN_CALL(this->runTransaction());
	}

public:
	modm::ResumableResult<bool>
	writeRegister(Register reg, uint8_t value, uint8_t offset=0)
	{
		RF_BEGIN();
		if (not RF_CALL(setPage(reg))) RF_RETURN(false);

		buffer[0] = uint8_t(reg) + offset;
		buffer[1] = value;

		this->transaction.configureWrite(buffer, 2);
		RF_END_RETURN_CALL(this->runTransaction());
	}

	modm::ResumableResult<bool>
	readRegister(Register reg, uint8_t *const value, uint8_t offset=0)
	{
		RF_BEGIN();
		if (not RF_CALL( setPage(reg) )) RF_RETURN(false);

		buffer[0] = uint8_t(reg) + offset;
		this->transaction.configureWriteRead(buffer, 1, value, 1);

		RF_END_RETURN_CALL(this->runTransaction());
	}

protected:
	modm::ResumableResult<bool>
	setPage(Register reg)
	{
		RF_BEGIN();

		if (hasPage(reg) and (getPage(reg) != current_page))
		{
			buffer[0] = uint8_t(Register::COMMAND_WRITE_LOCK);
			buffer[1] = 0xC5; // command write key
			this->transaction.configureWrite(buffer, 2);
			if (not RF_CALL(this->runTransaction())) RF_RETURN(false);

			buffer[0] = uint8_t(Register::COMMAND);
			buffer[1] = getPage(reg);
			if (not RF_CALL(this->runTransaction())) RF_RETURN(false);
			current_page = getPage(reg);
		}

		RF_END_RETURN(true);
	}

protected:
	struct LedData
	{
        const uint8_t addr_led_on_off{uint8_t(Register::LED_ON_OFF)};
		uint16_t led_on_off[SizeX];

        const uint8_t addr_led_pwm{uint8_t(Register::PWM)};
		uint8_t led_pwm[SizeX][SizeY];
	} modm_packed;

	uint16_t led_open[SizeX];
	uint16_t led_short[SizeX];

	LedData data;
	uint8_t current_page{0xff};
	uint8_t buffer[2];
};

} // namespace modm
