/*
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/rtc/mcp7941x.hpp>
#include <modm/processing.hpp>

using MyI2cMaster	= modm::platform::I2cMaster1;
using I2cScl 		= modm::platform::GpioB8;
using I2cSda 		= modm::platform::GpioB9;

modm::Mcp7941x<MyI2cMaster> rtc{};

modm::Fiber fiber_name([]
{
	if(rtc.oscillatorRunning()) {
		MODM_LOG_ERROR << "RTC oscillator is running." << modm::endl;
	}
	else {
		MODM_LOG_ERROR << "RTC oscillator is NOT running." << modm::endl;
	}

	MODM_LOG_INFO << "Setting date/time to 01.01.2020 00:00.00h" << modm::endl;
	modm::mcp7941x::DateTime dateTime{};
	dateTime.days = 1;
	dateTime.months = 1;
	dateTime.years = 20;
	dateTime.hours = 0;
	dateTime.minutes = 0;
	dateTime.seconds = 0;
	while(not rtc.setDateTime(dateTime)) {
		MODM_LOG_ERROR << "Unable to set date/time." << modm::endl;
		modm::this_fiber::sleep_for(500ms);
	}

	modm::this_fiber::sleep_for(500ms);

	if(rtc.oscillatorRunning()) {
		MODM_LOG_ERROR << "RTC oscillator is running." << modm::endl;
	}
	else {
		MODM_LOG_ERROR << "RTC oscillator is NOT running." << modm::endl;
	}

	while (true)
	{
		auto dateTime2 = rtc.getDateTime();
		if(dateTime2.has_value()) {
			MODM_LOG_INFO.printf("%02u.%02u.%02u ", dateTime2->days, dateTime2->months, dateTime2->years);
			MODM_LOG_INFO.printf("%02u:%02u.%02uh\n", dateTime2->hours, dateTime2->minutes, dateTime2->seconds);
		}
		else {
			MODM_LOG_ERROR << "Unable to read from RTC." << modm::endl;
		}

		modm::this_fiber::sleep_for(2.5s);
	}
});

modm::Fiber fiber_blink([]
{
	Board::LedGreen::setOutput();
	while(true)
	{
		Board::LedGreen::toggle();
		modm::this_fiber::sleep_for(0.5s);
	}
});

int
main()
{
	Board::initialize();

	MyI2cMaster::connect<I2cScl::Scl, I2cSda::Sda>();
	MyI2cMaster::initialize<Board::SystemClock, 100_kHz>();

	MODM_LOG_INFO << "RTC MCP7941x Example on Nucleo-F429ZI" << modm::endl;

	modm::Mcp7941xEeprom<MyI2cMaster> eeprom{};
	if (auto data = eeprom.getUniqueId()) {
		MODM_LOG_INFO << "Unique ID (EUI-48/64): " << modm::hex;
		MODM_LOG_INFO << (*data)[0] << ":";
		MODM_LOG_INFO << (*data)[1] << ":";
		MODM_LOG_INFO << (*data)[2] << ":";
		MODM_LOG_INFO << (*data)[3] << ":";
		MODM_LOG_INFO << (*data)[4] << ":";
		MODM_LOG_INFO << (*data)[5] << ":";
		MODM_LOG_INFO << (*data)[6] << ":";
		MODM_LOG_INFO << (*data)[7] << modm::endl;
	}
	else {
		MODM_LOG_ERROR << "Unable to read unique ID from RTC." << modm::endl;
	}
	modm::delay(500ms);
	modm::fiber::Scheduler::run();

	return 0;
}
