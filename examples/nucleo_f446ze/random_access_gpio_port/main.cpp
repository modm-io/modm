/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <ranges>

using namespace Board;
using namespace std::literals;

int
main()
{
	Board::initialize();

	using LedPort = RandomAccessPort<LedRed, LedBlue, LedGreen>;
	LedPort::setOutput();

	// Enable output with index 2 => LedRed
	LedPort::set(2);
	modm::delay(1000ms);

	// Type-erased pin wrapper
	const auto port = LedPort{};
	port[1].set();

	GpioAccessor pin = port[0];
	pin.set();

	modm::delay(500ms);

	// Demonstrate iterator/range interface
	static_assert(std::ranges::random_access_range<LedPort>);

	auto reversedPins = port | std::views::reverse;
	while (true)
	{
		for(auto pin : reversedPins) {
			pin.toggle();
			modm::delay(250ms);
		}
	}

	return 0;
}
