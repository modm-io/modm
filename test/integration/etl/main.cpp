/*
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <etl/unordered_map.h>
#include <numbers>
#include <chrono>
#include <modm/debug/logger.hpp>

using namespace std::chrono_literals;

int main()
{
	// Create an unordered_map of three strings (that map to strings)
	etl::unordered_map<uint8_t, double, 3> u = {
		{15, std::numbers::pi},
		{42, std::numbers::e},
		{87, std::numbers::sqrt2}
	};

	MODM_LOG_INFO << "Iterate and print keys and values using structured binding (since C++17):\n";
	for( const auto& [key, value] : u ) {
		MODM_LOG_INFO << "Key:[" << key << "] Value:[" << value << "]\n";
	}

	// Access non-existant element causing an assertion
	MODM_LOG_INFO << u[12];

	return 0;
}
