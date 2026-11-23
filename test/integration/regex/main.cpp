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

// std::regex needs std::locale, which links most of libstdc++ and newlib's
// time functions. They must link together with modm:ext:printf, and wcsftime
// must format through its swprintf.

#include <modm/board.hpp>
#include <ctime>
#include <cwchar>
#include <regex>

#include "../integration_test.hpp"

static const char *volatile number{"12345"};
static const char *volatile word{"modm"};

int
main()
{
	Board::initialize();
	const std::regex digits{"[0-9]+"};
	bool passed = std::regex_match(number, digits) and not std::regex_match(word, digits);

	std::tm time{};
	time.tm_year = 126;
	time.tm_mon = 10;
	time.tm_mday = 23;
	time.tm_hour = 18;
	time.tm_min = 5;
	time.tm_sec = 9;
	wchar_t buffer[32];
	passed &= (std::wcsftime(buffer, 32, L"%Y-%m-%d %H:%M:%S", &time) == 19);
	passed &= (std::wcscmp(buffer, L"2026-11-23 18:05:09") == 0);

	return finishTest(passed);
}
