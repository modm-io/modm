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

#include "st7565_test.hpp"

#include <modm/driver/display/st7565.hpp>
#include <modm/platform.hpp>
#include <modm-test/mock/spi_master.hpp>

using Spi = modm_test::platform::SpiMaster;
using Pin = modm::platform::GpioUnused;

static constexpr std::size_t Width = 128;
static constexpr std::size_t Pages = 64 / 8;
// Every page starts with three command bytes: page address and column address
static constexpr std::size_t PageSize = 3 + Width;

void
St7565Test::testUpdateOrder()
{
	modm::St7565<Spi, Pin, Pin, Pin, Width, Pages * 8, false> display;
	display.clear();
	display.setPixel(1, 9);		// column 1 of page 1, bit 1
	display.setPixel(127, 63);	// last column of the last page, bit 7

	Spi::clearBuffers();
	display.update();
	TEST_ASSERT_EQUALS(Spi::getTxBufferLength(), Pages * PageSize);

	uint8_t tx[Pages * PageSize];
	Spi::popTxBuffer(tx);
	for (std::size_t page = 0; page < Pages; ++page)
	{
		for (std::size_t column = 0; column < Width; ++column)
		{
			uint8_t expected = 0;
			if (page == 1 and column == 1) expected = 1 << 1;
			if (page == 7 and column == 127) expected = 1 << 7;
			TEST_ASSERT_EQUALS(tx[page * PageSize + 3 + column], expected);
		}
	}
}
