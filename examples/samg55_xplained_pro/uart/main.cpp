/*
 * Copyright (c) 2021, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/io/iostream.hpp>

using namespace modm::platform;
using namespace modm::literals;

// Create IO wrapper for the debug UART, which is connected to the built-in
// USB debugger virtual COM port
modm::IODeviceWrapper<Board::DebugUart, modm::IOBuffer::BlockIfFull> debugDevice;
modm::IOStream debugStream(debugDevice);

int
main()
{
	Board::initialize();

	uint32_t cycle = 0;
	while (true)
	{
		modm::delay(1s);
		debugStream.printf("Cycle: %lu\r\n", cycle);
	}
}