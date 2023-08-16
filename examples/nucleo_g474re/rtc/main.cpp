// coding: utf-8
/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
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
#include <modm/platform.hpp>
#include <modm/processing.hpp>

using namespace modm::literals;

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

static constexpr Rtc::Prescaler prediv(127, 225);
static constexpr Rtc::DateTime datetime = {
	.year = 0,
	.month = 12,
	.date = 31,
	.weekday = 1,
	.hour = 23,
	.minute = 59,
	second: 30,
};

int
main()
{
	Board::initialize();

	modm::PeriodicTimer timer(std::chrono::milliseconds(1000));

	/// TODO: Possibly move into Board::SystemClock::enable()
	Rcc::enableLowSpeedExternalCrystal();
    Rcc::enableRealTimeClock(Rcc::RealTimeClockSource::LowSpeedExternalCrystal);

	Rtc::initialize(datetime, prediv);
	while (true)
	{
		if (timer.execute())
		{
			float ss;
			Rtc::DateTime dt;
			if (Rtc::getSubSecond(ss) && Rtc::getDateTime(dt))
			{
				MODM_LOG_INFO << "Date: " << dt.date << "-" << dt.month << "-" << dt.year << modm::endl;
				MODM_LOG_INFO << "Weekday: " << dt.weekday << modm::endl;
				MODM_LOG_INFO << "Time: " << dt.hour << ":" << dt.minute << ":" << dt.second << modm::endl;
				MODM_LOG_INFO << "Sub Second: " << ss << modm::endl;
			}
			else
			{
				MODM_LOG_INFO << "Error: Unable to get Date Time and Sub Second" << modm::endl;
			}
		}
	}

	return 0;
}