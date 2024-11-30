/*
 * Copyright (c) 2022, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/driver/rtc/mcp7941x.hpp>
#include <modm/processing.hpp>
#include <optional>


// Set the log level
#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper<Uart0, modm::IOBuffer::BlockIfFull> loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);


using MyI2cMaster	= modm::platform::I2cMaster0;
using I2cScl 		= modm::platform::Gpio1;
using I2cSda 		= modm::platform::Gpio0;

modm::Mcp7941x<MyI2cMaster> rtc{};

modm::Fiber fiber_rtc([]
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

modm::Mcp7941xEeprom<MyI2cMaster> eeprom{};

int
main()
{
	Board::initialize();

	// initialize Uart0 for MODM_LOG_*
	Uart0::connect<GpioOutput16::Tx>();
	Uart0::initialize<Board::SystemClock, 115200_Bd>();

	MyI2cMaster::connect<I2cScl::Scl, I2cSda::Sda>();
	MyI2cMaster::initialize<Board::SystemClock, 100_kHz>();

	MODM_LOG_INFO << "RTC MCP7941x Example on Raspberry Pico" << modm::endl;


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
