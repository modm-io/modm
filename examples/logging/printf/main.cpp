/*
 * Copyright (c) 2014, Kevin Läufer
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#if __has_include(<modm/board.hpp>)
#include <modm/board.hpp>
#else
#include <modm/platform.hpp>
#endif
#include <modm/debug/logger.hpp>
#include <cstdio>

int
main()
{
#if __has_include(<modm/board.hpp>)
	Board::initialize();
#endif

	// <option name="modm:io:with_long_long">yes</option>
	MODM_LOG_INFO << uint64_t(32) << modm::endl;

	// <option name="modm:io:with_float">yes</option>
	MODM_LOG_INFO << 32.0f << modm::endl;

	// <option name="modm:io:with_printf">yes</option>
	MODM_LOG_INFO.printf("hello %lu %03.3f\n", 32ul, 32.23451);

#ifndef MODM_CPU_AVR
	// The printf of the standard library prints to the logger too
	printf("Hello %s!\n", "String Formatting");
	printf("Float %f\n", 5.012423523124);
#endif

	// Compare the printf of modm with the one of the standard library
	const char format[] = ">>%5d<<";
	char buffer[32];
	snprintf(buffer, sizeof(buffer), format, -42);
	MODM_LOG_INFO << "libc  " << buffer << modm::endl;
	MODM_LOG_INFO << "modm  ";
	MODM_LOG_INFO.printf(format, -42);
	MODM_LOG_INFO << modm::endl;

#if __has_include(<modm/board.hpp>)
	uint8_t counter{0};
	while (true)
	{
		modm::delay(1s);
		Board::Leds::toggle();
		MODM_LOG_INFO.printf("Counter %3d\n", counter++);
	}
#endif
	return 0;
}
