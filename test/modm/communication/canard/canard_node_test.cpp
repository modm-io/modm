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

#include "canard_node_test.hpp"

#include <modm-test/mock/can_driver.hpp>
#include <modm-test/mock/clock.hpp>
#include <canard/canard.hpp>
#include <array>

using namespace modm::canard;
using test_clock = modm_test::chrono::micro_clock;

namespace
{

constexpr uint16_t subject{1234};
constexpr uint8_t tx_id{10};
constexpr uint8_t rx_id{20};

modm_test::platform::CanDriver tx_can;
modm_test::platform::CanDriver rx_can;
using TxBus = CanInstance<tx_can>;
using RxBus = CanInstance<rx_can>;

struct Received
{
	size_t count{};
	size_t size{};
	uint8_t source{};
	std::array<uint8_t, 64> data{};
} received;

void
onMessage(canard_subscription_t* subscription, canard_us_t, canard_prio_t,
		  uint_least8_t source, uint_least8_t, canard_payload_t payload)
{
	received.count++;
	received.source = source;
	received.size = std::min(payload.view.size, received.data.size());
	std::memcpy(received.data.data(), payload.view.data, received.size);
	release(subscription, payload);
}

constexpr canard_subscription_vtable_t subscription_vtable{onMessage};

template< class Node >
bool
publish(Node& node, std::span<const uint8_t> payload, uint8_t transfer_id = 0)
{
	return canard_publish_13b(&node.canard, now() + 1'000'000, CANARD_IFACE_BITMAP_ALL,
							  canard_prio_nominal, subject, transfer_id,
							  {{payload.size(), payload.data()}, nullptr}, nullptr);
}

void
forward()
{
	rx_can.receiveList.splice(rx_can.receiveList.end(), tx_can.sendList);
}

modm::can::Message
frame(uint32_t identifier, std::initializer_list<uint8_t> data, bool extended = true)
{
	modm::can::Message message(identifier, data.size());
	message.setExtended(extended);
	std::copy(data.begin(), data.end(), message.data);
	return message;
}

}	// namespace

void
CanardNodeTest::setUp()
{
	test_clock::enable();
	test_clock::increment(1000);
	tx_can = {};
	rx_can = {};
	tx_can.sendSlots = 255;
	rx_can.sendSlots = 255;
	received = {};
}

void
CanardNodeTest::tearDown()
{
	test_clock::disable();
}

void
CanardNodeTest::testSingleFrame()
{
	Node<TxBus> tx;
	Node<RxBus> rx;
	TEST_ASSERT_TRUE(tx.initialize(tx_id, 1));
	TEST_ASSERT_TRUE(rx.initialize(rx_id, 2));
	canard_subscription_t subscription;
	TEST_ASSERT_EQUALS(canard_subscribe_13b(&rx.canard, &subscription, subject, 64,
			CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_us, &subscription_vtable), &subscription);

	const uint8_t payload[]{1, 2, 3};
	TEST_ASSERT_TRUE(publish(tx, payload));
	tx.update();
	TEST_ASSERT_EQUALS(tx_can.sendList.size(), 1u);
	TEST_ASSERT_TRUE(tx_can.sendList.front().isExtended());
	TEST_ASSERT_EQUALS(tx_can.sendList.front().getIdentifier() & CANARD_NODE_ID_MAX, tx_id);

	forward();
	rx.update();
	TEST_ASSERT_EQUALS(received.count, 1u);
	TEST_ASSERT_EQUALS(received.source, tx_id);
	TEST_ASSERT_EQUALS(received.size, sizeof(payload));
	TEST_ASSERT_EQUALS_ARRAY(received.data.data(), payload, sizeof(payload));

	canard_destroy(&tx.canard);
	canard_unsubscribe(&rx.canard, &subscription);
	canard_destroy(&rx.canard);
	TEST_ASSERT_EQUALS(tx.heapDiagnostics().oom_count, 0u);
	TEST_ASSERT_EQUALS(rx.heapDiagnostics().oom_count, 0u);
}

void
CanardNodeTest::testMultiFrame()
{
	Node<TxBus> tx;
	Node<RxBus> rx;
	TEST_ASSERT_TRUE(tx.initialize(tx_id, 1));
	TEST_ASSERT_TRUE(rx.initialize(rx_id, 2));
	canard_subscription_t subscription;
	canard_subscribe_13b(&rx.canard, &subscription, subject, 64,
			CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_us, &subscription_vtable);

	std::array<uint8_t, 40> payload;
	for (size_t ii = 0; ii < payload.size(); ii++) payload[ii] = ii * 7;
	TEST_ASSERT_TRUE(publish(tx, payload));
	for (int ii = 0; ii < 16; ii++) tx.update();
	TEST_ASSERT_TRUE(tx_can.sendList.size() > 1);

	forward();
	rx.update();
	TEST_ASSERT_EQUALS(received.count, 1u);
	TEST_ASSERT_EQUALS(received.size, payload.size());
	TEST_ASSERT_EQUALS_ARRAY(received.data.data(), payload.data(), payload.size());

	canard_destroy(&tx.canard);
	canard_unsubscribe(&rx.canard, &subscription);
	canard_destroy(&rx.canard);
	// every allocation is returned, also of the reassembled payload
	TEST_ASSERT_EQUALS(tx.heapDiagnostics().allocated, 0u);
	TEST_ASSERT_EQUALS(rx.heapDiagnostics().allocated, 0u);
}

void
CanardNodeTest::testMalloc()
{
	Node<TxBus, 0> tx;
	Node<RxBus, 0> rx;
	TEST_ASSERT_TRUE(tx.initialize(tx_id, 1));
	TEST_ASSERT_TRUE(rx.initialize(rx_id, 2));
	canard_subscription_t subscription;
	canard_subscribe_13b(&rx.canard, &subscription, subject, 64,
			CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_us, &subscription_vtable);

	std::array<uint8_t, 40> payload{};
	payload[39] = 0x5a;
	TEST_ASSERT_TRUE(publish(tx, payload));
	for (int ii = 0; ii < 16; ii++) tx.update();
	forward();
	rx.update();
	TEST_ASSERT_EQUALS(received.count, 1u);
	TEST_ASSERT_EQUALS(received.data[39], 0x5a);

	canard_destroy(&tx.canard);
	canard_unsubscribe(&rx.canard, &subscription);
	canard_destroy(&rx.canard);
}

void
CanardNodeTest::testBackpressure()
{
	Node<TxBus> tx;
	TEST_ASSERT_TRUE(tx.initialize(tx_id, 1));
	tx_can.sendSlots = 0;

	const uint8_t payload[]{1};
	TEST_ASSERT_TRUE(publish(tx, payload));
	tx.update();
	TEST_ASSERT_EQUALS(tx_can.sendList.size(), 0u);

	tx_can.sendSlots = 1;
	tx.update();
	TEST_ASSERT_EQUALS(tx_can.sendList.size(), 1u);
	canard_destroy(&tx.canard);
}

void
CanardNodeTest::testIgnoredFrames()
{
	Node<RxBus> rx;
	TEST_ASSERT_TRUE(rx.initialize(rx_id, 2));
	canard_subscription_t subscription;
	canard_subscribe_13b(&rx.canard, &subscription, subject, 64,
			CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_us, &subscription_vtable);

	// the valid identifier of a message frame on the subject
	const uint32_t identifier = (uint32_t(canard_prio_nominal) << 26) | (uint32_t(3) << 21) |
								(uint32_t(subject) << 8) | tx_id;
	rx_can.receiveList.push_back(frame(identifier & 0x7ff, {0xe0}, false));
	auto remote = frame(identifier, {0xe0});
	remote.setRemoteTransmitRequest(true);
	rx_can.receiveList.push_back(remote);
	rx.update();
	TEST_ASSERT_EQUALS(received.count, 0u);
	canard_unsubscribe(&rx.canard, &subscription);
	canard_destroy(&rx.canard);
}

void
CanardNodeTest::testKeepNodeId()
{
	Node<RxBus> rx;
	TEST_ASSERT_TRUE(rx.initialize(rx_id, 2, {.keep_node_id = true}));
	rx_can.receiveList.push_back(frame(0x1000'0000u | rx_id, {0xe0}));
	rx.update();
	TEST_ASSERT_EQUALS(rx.collisions(), 1u);
	TEST_ASSERT_EQUALS(rx.canard.node_id, rx_id);
	canard_destroy(&rx.canard);
}

void
CanardNodeTest::testLoopback()
{
	Node<TxBus> tx;
	TEST_ASSERT_TRUE(tx.initialize(tx_id, 1, {.loopback = true}));
	const uint8_t payload[]{1, 2, 3};
	TEST_ASSERT_TRUE(publish(tx, payload));
	tx.update();
	TEST_ASSERT_EQUALS(tx_can.sendList.size(), 1u);

	// the echo is no collision, so that the node keeps its node-ID
	tx.ingest(tx_can.sendList.front());
	TEST_ASSERT_EQUALS(tx.canard.node_id, tx_id);
	canard_destroy(&tx.canard);
}

void
CanardNodeTest::testAnonymous()
{
	Node<TxBus> tx;
	TEST_ASSERT_TRUE(tx.initialize(CANARD_NODE_ID_ANONYMOUS, 1, {.keep_node_id = true}));
	const uint8_t payload[]{1, 2, 3};
	TEST_ASSERT_TRUE(tx.publishAnonymous(1, 5, payload));
	TEST_ASSERT_EQUALS(tx_can.sendList.size(), 1u);
	const auto& message = tx_can.sendList.front();
	TEST_ASSERT_TRUE(message.isExtended());
	TEST_ASSERT_EQUALS(message.getLength(), 4u);
	TEST_ASSERT_EQUALS(message.getIdentifier() & 0xff, 0u);
	TEST_ASSERT_EQUALS((message.getIdentifier() >> 8) & 0b11, 1u);
	TEST_ASSERT_EQUALS(message.getIdentifier() >> 26, uint32_t(canard_prio_nominal));
	TEST_ASSERT_EQUALS(message.data[3], 0xC5);

	const uint8_t too_long[8]{};
	TEST_ASSERT_FALSE(tx.publishAnonymous(1, 5, too_long));
	canard_destroy(&tx.canard);
}
