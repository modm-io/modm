/*
 * Copyright (c) 2024, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "spi_bdma_test.hpp"

#include <modm/platform.hpp>
#include <modm/board.hpp>

using namespace modm::platform;

using Spi = SpiMaster6_Dma<Bdma::Channel0, Bdma::Channel1>;
using Miso = GpioA6;
using Sck = GpioC12;

void
SpiBdmaTest::setUp()
{
	Bdma::enable();
	Spi::connect<Miso::Miso, Sck::Sck>();
	Spi::initialize<Board::SystemClock, 16_MHz, 10_pct>();
}

void
SpiBdmaTest::testReceive()
{
	constexpr std::array<uint8_t, 4> ones{0xff, 0xff, 0xff, 0xff};
	constexpr std::array<uint8_t, 4> zeros{};
	modm_section(".data_d3_sram") static std::array<uint8_t, 4> buffer{};

	Miso::configure(Miso::InputType::PullUp);
	modm::delay_ns(500);
	Spi::transferBlocking(nullptr, buffer.data(), buffer.size());
	TEST_ASSERT_TRUE(buffer == ones);

	Miso::configure(Miso::InputType::PullDown);
	modm::delay_ns(500);
	Spi::transferBlocking(nullptr, buffer.data(), buffer.size());
	TEST_ASSERT_TRUE(buffer == zeros);

	Miso::configure(Miso::InputType::PullUp);
	modm::delay_ns(500);
	Spi::transferBlocking(nullptr, buffer.data(), buffer.size());
	TEST_ASSERT_TRUE(buffer == ones);

	Miso::configure(Miso::InputType::Floating);
}
