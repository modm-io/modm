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

#include "../integration_test.hpp"

using namespace modm::platform;

int main()
{
	// Test SPI send and receive in loopback mode: we must receive what we sent.
	Board::initialize();

	SpiMaster0::connect<GpioB0::Sck, GpioA9::Miso, GpioA10::Mosi>();
	SpiMaster0::initialize<Board::SystemClock, 1_MHz>();

	SpiMaster0::setLocalLoopback(true);

	const uint8_t tx[] = {0xa5, 0x21};
	uint8_t rx[2]{};

	SpiMaster0::transfer(tx, rx, 2);

	return finishTest(rx[0] == tx[0] and rx[1] == tx[1]);
}
