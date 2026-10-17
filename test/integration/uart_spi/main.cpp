/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2018, Niklas Hauser
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

#include "../integration_test.hpp"

/**
 * Tests a UART in SPI mode in a loopback: connect Tx to Rx!
 */

// The UART and its pins differ between the boards
#if defined STM32F4
// connect PA2 to PA3
using UartSpi = UartSpiMaster2;
using Ck = GpioA4;
using Tx = GpioA2;
using Rx = GpioA3;
constexpr auto Baudrate = 5.25_MHz;
#else
// connect PA9 to PA10
using UartSpi = UartSpiMaster1;
using Ck = GpioA8;
using Tx = GpioA9;
using Rx = GpioA10;
constexpr auto Baudrate = 1_MHz;
#endif

int
main()
{
	Board::initialize();

	UartSpi::connect<Ck::Ck, Tx::Tx, Rx::Rx>();
	UartSpi::initialize<Board::SystemClock, Baudrate, 0_pct>();

	bool passed = true;
	for (const uint8_t data : {0xF0, 0x0F, 0x55, 0xAA, 0x00, 0xFF})
	{
		passed &= (UartSpi::transfer(data) == data);
	}

	return finishTest(passed);
}
