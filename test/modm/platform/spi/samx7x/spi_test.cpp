/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "spi_test.hpp"

#include <modm/platform.hpp>
#include <modm/board.hpp>

using namespace modm::platform;

using Spck = GpioD22;
using Mosi = GpioD21;
using Miso = GpioD20;

void
SpiTest::setUp()
{
	SpiMaster0::connect<Spck::Spck, Mosi::Mosi, Miso::Miso>();
	SpiMaster0::initialize<Board::SystemClock, 1_Mbps, 1_pct>();
}

void
SpiTest::testTransferLoopback()
{
	SpiHal0::setLoopbackMode(SpiHal0::LocalLoopback::Enabled);

	uint8_t value = SpiMaster0::transferBlocking(0xAB);
	TEST_ASSERT_EQUALS(value, 0xAB);

	value = RF_CALL_BLOCKING(SpiMaster0::transfer(0x12));
	TEST_ASSERT_EQUALS(value, 0x12);

	constexpr std::array<uint8_t, 7> tx{0xDE, 0xAD, 0xBE, 0xEF, 0x12, 0x34, 0x56};
	std::array<uint8_t, 7> rx{};
	SpiMaster0::transferBlocking(tx.data(), rx.data(), 7);
	TEST_ASSERT_TRUE(rx == tx);

	rx.fill(0);
	RF_CALL_BLOCKING(SpiMaster0::transfer(tx.data(), rx.data(), 7));
	TEST_ASSERT_TRUE(rx == tx);

	SpiHal0::setLoopbackMode(SpiHal0::LocalLoopback::Disabled);
}

void
SpiTest::testReceivePin()
{
	Miso::configure(Miso::InputType::PullUp);
	modm::delay_us(1);
	uint8_t value = SpiMaster0::transferBlocking(0xAB);
	TEST_ASSERT_EQUALS(value, 0xFF);

	Miso::configure(Miso::InputType::PullDown);
	modm::delay_us(1);
	value = SpiMaster0::transferBlocking(0xAB);
	TEST_ASSERT_EQUALS(value, 0x00);

	Miso::configure(Miso::InputType::Floating);
}
