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

#include "fdcan_test.hpp"

#include <modm/platform.hpp>
#include <modm/board.hpp>

using namespace modm::platform;

void
FdcanTest::setUp()
{
	Fdcan1::initialize<Board::SystemClock, 500_kbps, 1_pct>(9, Fdcan1::Mode::TestInternalLoopback);

	// receive all extended messages
	Fdcan1::setExtendedFilter(0, Fdcan1::FilterConfig::Fifo1,
			modm::can::ExtendedIdentifier(0),
			modm::can::ExtendedMask(0));
}

void
FdcanTest::testSendReceive()
{
	modm::can::Message message{0x12345678, 7};
	constexpr std::string_view data = "\xDE\xAD\xBE\xEF\x12\x34\x56";
	std::copy(std::begin(data), std::begin(data) + 7, message.data);

	TEST_ASSERT_FALSE(Fdcan1::isMessageAvailable());
	TEST_ASSERT_TRUE(Fdcan1::sendMessage(message));

	modm::delay_ms(1);

	modm::can::Message receivedMessage;
	TEST_ASSERT_TRUE(Fdcan1::getMessage(receivedMessage));
	TEST_ASSERT_EQUALS(receivedMessage.getIdentifier(), 0x12345678u);
	TEST_ASSERT_EQUALS(receivedMessage.getLength(), 7);
	TEST_ASSERT_TRUE(receivedMessage.isExtended());
	TEST_ASSERT_FALSE(receivedMessage.isRemoteTransmitRequest());
	TEST_ASSERT_TRUE(std::equal(std::begin(data), std::begin(data) + 7, message.data));
}

void
FdcanTest::testFilters()
{
	Fdcan1::setStandardFilter(27, Fdcan1::FilterConfig::Fifo0,
			modm::can::StandardIdentifier(0x108),
			modm::can::StandardMask(0x1F8));

	modm::can::Message message{0x188, 0};
	message.setExtended(false);
	Fdcan1::sendMessage(message);
	modm::delay_ms(1);
	TEST_ASSERT_FALSE(Fdcan1::isMessageAvailable());

	message.setIdentifier(0xF09);
	Fdcan1::sendMessage(message);
	modm::delay_ms(1);
	TEST_ASSERT_TRUE(Fdcan1::isMessageAvailable());
	TEST_ASSERT_TRUE(Fdcan1::getMessage(message));
	TEST_ASSERT_FALSE(message.isExtended());
}

void
FdcanTest::testBuffers()
{
	modm::can::Message message{0x4711, 0};
	// send 8 messages, exceeds internal peripheral queue size
	for (uint_fast8_t i = 0; i <= 8; ++i) {
		message.setLength(i);
		for (uint_fast8_t dataIndex = 0; dataIndex < i; ++dataIndex) {
			message.data[dataIndex] = i;
		}
		Fdcan1::sendMessage(message);
	}

	modm::delay_ms(10);

	// try to receive same messages
	modm::can::Message receivedMessage;
	for (uint_fast8_t i = 0; i <= 8; ++i) {
		TEST_ASSERT_TRUE(Fdcan1::getMessage(receivedMessage));

		TEST_ASSERT_EQUALS(receivedMessage.getIdentifier(), 0x4711u);
		TEST_ASSERT_EQUALS(receivedMessage.getLength(), i);

		for (uint_fast8_t dataIndex = 0; dataIndex < i; ++dataIndex) {
			TEST_ASSERT_EQUALS(receivedMessage.data[dataIndex], i);
		}
	}
	TEST_ASSERT_FALSE(Fdcan1::isMessageAvailable());
	TEST_ASSERT_FALSE(Fdcan1::getMessage(message));
}
