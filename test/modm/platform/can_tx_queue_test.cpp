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

#include "can_tx_queue_test.hpp"
#include <modm/platform/can/can_tx_queue.hpp>

using modm::can::Message;
using Queue = modm::platform::CanTxQueue<8>;

static Message
frame(uint32_t id, bool extended, uint8_t tag = 0, bool rtr = false)
{
	Message msg(id, 1);
	msg.setExtended(extended);
	msg.setRemoteTransmitRequest(rtr);
	msg.data[0] = tag;
	return msg;
}

void
CanTxQueueTest::testPriority()
{
	// lower base ID wins, standard wins over extended with the same base ID,
	// remote frames have the same priority as data frames
	TEST_ASSERT_TRUE(Queue::priority(frame(0x100, false)) < Queue::priority(frame(0x101, false)));
	TEST_ASSERT_TRUE(Queue::priority(frame(0x100, false)) < Queue::priority(frame(0x100 << 18, true)));
	TEST_ASSERT_TRUE(Queue::priority(frame(0x100, false, 0, true)) < Queue::priority(frame(0x100 << 18, true)));
	TEST_ASSERT_EQUALS(Queue::priority(frame(0x100, false)), Queue::priority(frame(0x100, false, 0, true)));
	TEST_ASSERT_TRUE(Queue::priority(frame(0x0ff << 18 | 0x3ffff, true)) < Queue::priority(frame(0x100, false)));
	TEST_ASSERT_TRUE(Queue::priority(frame(0x1234, true)) < Queue::priority(frame(0x1235, true)));
	TEST_ASSERT_EQUALS(Queue::priority(frame(0x1234, true)), Queue::priority(frame(0x1234, true, 0, true)));
}

void
CanTxQueueTest::testOrder()
{
	Queue queue;
	TEST_ASSERT_TRUE(queue.isEmpty());
	TEST_ASSERT_TRUE(queue.push(frame(0x300, false, 1)));
	TEST_ASSERT_TRUE(queue.push(frame(0x100, false, 2)));
	TEST_ASSERT_TRUE(queue.push(frame(0x300, false, 3)));
	TEST_ASSERT_TRUE(queue.push(frame(0x100, false, 4)));
	TEST_ASSERT_TRUE(queue.push(frame(0x200, false, 5)));
	TEST_ASSERT_EQUALS(queue.getSize(), 5u);

	for (uint8_t tag : {2, 4, 5, 1, 3}) {
		TEST_ASSERT_TRUE(queue.isNotEmpty());
		TEST_ASSERT_EQUALS(queue.get().data[0], tag);
		queue.pop();
	}
	TEST_ASSERT_TRUE(queue.isEmpty());
	queue.pop();
	TEST_ASSERT_TRUE(queue.isEmpty());

	for (uint8_t ii = 0; ii < 8; ++ii)
		TEST_ASSERT_TRUE(queue.push(frame(0x10, true, ii)));
	TEST_ASSERT_TRUE(queue.isFull());
	TEST_ASSERT_FALSE(queue.push(frame(0x1, false)));
	for (uint8_t ii = 0; ii < 8; ++ii) {
		TEST_ASSERT_EQUALS(queue.get().data[0], ii);
		queue.pop();
	}
}

void
CanTxQueueTest::testRandom()
{
	// interleaved push and pop must give the same result as a stable sort
	Queue queue;
	uint32_t rng{1};
	uint8_t next[4]{};		// next tag per ID
	uint8_t expected[4]{};	// next tag expected per ID
	for (int ii = 0; ii < 2000; ++ii)
	{
		rng = rng * 1103515245 + 12345;
		if ((rng >> 16) & 1 and queue.isNotFull()) {
			const uint8_t id = (rng >> 20) % 4;
			TEST_ASSERT_TRUE(queue.push(frame(id, false, next[id]++)));
		}
		else if (queue.isNotEmpty()) {
			const Message& msg = queue.get();
			const uint8_t id = msg.getIdentifier();
			// no queued frame may have a lower ID than the one returned
			for (uint8_t lower = 0; lower < id; ++lower)
				TEST_ASSERT_EQUALS(next[lower], expected[lower]);
			TEST_ASSERT_EQUALS(msg.data[0], expected[id]++);
			queue.pop();
		}
	}
}

void
CanTxQueueTest::testLarge()
{
#ifndef MODM_CPU_AVR
	// more than 255 frames need 16-bit indices, which do not fit into AVR RAM
	static modm::platform::CanTxQueue<300> queue;
	for (uint16_t ii = 0; ii < 300; ++ii) {
		Message msg(3 - ii % 3, 2);
		msg.data[0] = ii;
		msg.data[1] = ii >> 8;
		TEST_ASSERT_TRUE(queue.push(msg));
	}
	TEST_ASSERT_TRUE(queue.isFull());
	// sorted by ID, and in push order for the same ID
	for (uint16_t ii = 0; ii < 300; ++ii) {
		const uint16_t id = ii / 100 + 1;
		const uint16_t tag = (3 - id) + (ii % 100) * 3;
		TEST_ASSERT_EQUALS(queue.get().getIdentifier(), id);
		TEST_ASSERT_EQUALS(uint16_t(queue.get().data[0] | (queue.get().data[1] << 8)), tag);
		queue.pop();
	}
	TEST_ASSERT_TRUE(queue.isEmpty());
#endif
}
