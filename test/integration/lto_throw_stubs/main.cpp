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

// Links libstdc++ APIs with throw paths using LTO and without exceptions. The
// real throw functions pull in iostreams and newlib's snprintf, which collides
// with modm:ext:printf. std::error_code links system_error.o, which must not
// collide with the stubs. A regex error must reach the modm assertion. strftime
// and asctime call sniprintf and siprintf, which must not link newlib's printf.

#include <modm/board.hpp>
#include <cstdio>
#include <ctime>
#include <functional>
#include <mutex>
#include <regex>
#include <string>
#include <system_error>
#include <vector>

#include "../integration_test.hpp"

static modm::Abandonment
expect_stub(const modm::AssertionInfo &info)
{
	printTestResult(std::string_view{info.name} == "c++");
	while (true) {}
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(expect_stub);

struct Lockable
{
	volatile bool locked{};
	bool try_lock() { if (locked) return false; locked = true; return true; }
	void lock() { locked = true; }
	void unlock() { locked = false; }
};

static Lockable lockable;
static volatile int zero{0};

int
main()
{
	Board::initialize();
	bool passed = true;

	if (std::unique_lock lock{lockable, std::try_to_lock}; lock.owns_lock()) passed &= lockable.locked;
	passed &= not lockable.locked;

	char buffer[16];
	snprintf(buffer, sizeof buffer, "%d", 42 + zero);
	std::string text{buffer};
	text += "0";
	passed &= (std::stoi(text) == 420);

	std::tm time{};
	time.tm_year = 126 + zero;
	time.tm_mon = 10;
	time.tm_mday = 23;
	char date[32];
	passed &= (std::strftime(date, sizeof date, "%Y-%m-%d", &time) == 10);
	passed &= (std::string_view{date} == "2026-11-23");
	passed &= std::string_view{std::asctime(&time)}.ends_with("2026\n");

	std::function<int()> function = [] { return zero + 1; };
	passed &= (function() == 1);

	const std::error_code error = std::make_error_code(std::errc::invalid_argument);
	passed &= (error.category() == std::generic_category());

	std::vector<int> vector(4);
	vector.push_back(zero + 7);
	passed &= (vector.at(4) == 7);

	if (not passed) return finishTest(false);
	// Must not return: expect_stub reports the result
	if (zero == 0) std::__throw_regex_error(std::regex_constants::error_collate);
	return finishTest(false);
}
