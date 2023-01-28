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
#include <modm/processing/protothread.hpp>
#include <modm/processing/timer.hpp>
#include <optional>

using MyI2cMaster	= modm::platform::I2cMaster1;
using I2cScl 		= modm::platform::GpioB8;
using I2cSda 		= modm::platform::GpioB9;


class RtcThread : public modm::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		if(PT_CALL(rtc.oscillatorRunning())) {
			MODM_LOG_ERROR << "RTC oscillator is running." << modm::endl;
		}
		else {
			MODM_LOG_ERROR << "RTC oscillator is NOT running." << modm::endl;
		}

		MODM_LOG_INFO << "Setting date/time to 01.01.2020 00:00.00h" << modm::endl;
		dateTime.days = 1;
		dateTime.months = 1;
		dateTime.years = 20;
		dateTime.hours = 0;
		dateTime.minutes = 0;
		dateTime.seconds = 0;
		while(not PT_CALL(rtc.setDateTime(dateTime))) {
			MODM_LOG_ERROR << "Unable to set date/time." << modm::endl;
			timeout.restart(500ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		timeout.restart(500ms);
		PT_WAIT_UNTIL(timeout.isExpired());

		if(PT_CALL(rtc.oscillatorRunning())) {
			MODM_LOG_ERROR << "RTC oscillator is running." << modm::endl;
		}
		else {
			MODM_LOG_ERROR << "RTC oscillator is NOT running." << modm::endl;
		}

		while (true)
		{
			dateTime2 = PT_CALL(rtc.getDateTime());
			if(dateTime2.has_value()) {
				MODM_LOG_INFO.printf("%02u.%02u.%02u ", dateTime2->days, dateTime2->months, dateTime2->years);
				MODM_LOG_INFO.printf("%02u:%02u.%02uh\n", dateTime2->hours, dateTime2->minutes, dateTime2->seconds);
			}
			else {
				MODM_LOG_ERROR << "Unable to read from RTC." << modm::endl;
			}

			timeout.restart(2500ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::Mcp7941x<MyI2cMaster> rtc{};
	modm::mcp7941x::DateTime dateTime{};
	std::optional<modm::mcp7941x::DateTime> dateTime2{};
	modm::ShortTimeout timeout;
};


using namespace Board;

int
main()
{
	Board::initialize();
	Leds::setOutput();

	MyI2cMaster::connect<I2cScl::Scl, I2cSda::Sda>();
	MyI2cMaster::initialize<SystemClock, 100_kHz>();

	MODM_LOG_INFO << "RTC MCP7941x Example on Nucleo-F429ZI" << modm::endl;

	modm::Mcp7941xEeprom<MyI2cMaster> eeprom{};
	if (auto data = RF_CALL_BLOCKING(eeprom.getUniqueId())) {
		MODM_LOG_INFO << "Unique ID (EUI-48/64): ";
		MODM_LOG_INFO << modm::hex << (*data)[0] << modm::ascii << ":";
		MODM_LOG_INFO << modm::hex << (*data)[1] << modm::ascii << ":";
		MODM_LOG_INFO << modm::hex << (*data)[2] << modm::ascii << ":";
		MODM_LOG_INFO << modm::hex << (*data)[3] << modm::ascii << ":";
		MODM_LOG_INFO << modm::hex << (*data)[4] << modm::ascii << ":";
		MODM_LOG_INFO << modm::hex << (*data)[5] << modm::ascii << ":";
		MODM_LOG_INFO << modm::hex << (*data)[6] << modm::ascii << ":";
		MODM_LOG_INFO << modm::hex << (*data)[7] << modm::ascii << modm::endl;
	}
	else {
		MODM_LOG_ERROR << "Unable to read unique ID from RTC." << modm::endl;
	}
	modm::delay(500ms);

	RtcThread rtcThread;

	while (true)
	{
		LedGreen::toggle();
		rtcThread.update();
	}

	return 0;
}
