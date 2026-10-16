// coding: utf-8
/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 * Copyright (c) 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

int
main()
{
	Board::initialize();
	Board::Leds::setOutput();
#ifdef MODM_BOARD_HAS_LOGGER
	MODM_LOG_INFO << "Initialize RTC" << modm::endl;
#endif
	const bool inited = Rtc::initialize<Board::SystemClock>();
#ifdef MODM_BOARD_HAS_LOGGER
	if (not inited) { MODM_LOG_INFO << "RTC was already initialized." << modm::endl; }
#endif

	constexpr auto cdt = modm::DateTime::fromBuildTime();
	if (Rtc::dateTime() < cdt) Rtc::setDateTime(cdt);
#ifdef MODM_BOARD_HAS_LOGGER
	MODM_LOG_INFO << "Compile DateTime: " << cdt << modm::endl;
	MODM_LOG_INFO << "YMD: " << cdt.year_month_day() << modm::endl;
	MODM_LOG_INFO << "HMS: " << cdt.hh_mm_ss() << modm::endl;
	MODM_LOG_INFO << "Weekday: " << cdt.weekday() << modm::endl;
#endif


	while (true)
	{
		const auto dt = Rtc::dateTime();
#ifdef MODM_BOARD_HAS_LOGGER
		const auto now = Rtc::now();
		MODM_LOG_INFO << dt << " (" << dt.weekday() << ") = " << now << " since 1970" << modm::endl;
		modm::delay(1.1s);
#else
		static uint8_t prev_second{};
		if (prev_second != dt.seconds().count())
		{
			prev_second = dt.seconds().count();
			Board::Leds::toggle();
		}
		modm::delay(10ms);
#endif

	}

	return 0;
}
