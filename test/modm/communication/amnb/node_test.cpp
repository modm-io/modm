/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "node_test.hpp"
#include <modm-test/mock/shared_medium.hpp>
#include <modm-test/mock/clock.hpp>
#include <modm/communication/amnb/node.hpp>
#include <vector>

using namespace modm::amnb;
using namespace modm_test;
using namespace std::chrono_literals;
using milli_clock = modm_test::chrono::milli_clock;
using micro_clock = modm_test::chrono::micro_clock;

void
AmnbNodeTest::setUp()
{
	SharedMedium::reset();
}

// ----------------------------------------------------------------------------
void
AmnbNodeTest::testBroadcast()
{
	DeviceWrapper<SharedMedium> dev;
	Node node(dev, 0x08);
	{
		node.broadcast(0x10);
		node.broadcast(0x20, uint32_t(0x03020100));
		node.update(); node.update();
		const uint8_t raw[] = {0x7E, 0x7E, 84, 0x08, 0x10, 0, 0x7E, 0x7E, 58, 0x08, 0x20, 4, 0, 1, 2, 3};
		TEST_ASSERT_EQUALS(SharedMedium::raw_transmitted.size(), sizeof(raw));
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::transmitted, raw, sizeof(raw));
	}
	// test automatic retransmit
	SharedMedium::reset();
	SharedMedium::fail_tx_index = 2;
	{
		node.broadcast(0x70);
		for(uint32_t ii=0; ii < 20000; ii += 10) { node.update(); micro_clock::setTime(ii); }
		// first transmission attempt fails
		const uint8_t raw[] = {0x7E, 0x7E, 110, 0x7E, 0x7E, 110, 0x08, 0x70, 0};
		TEST_ASSERT_EQUALS(SharedMedium::raw_transmitted.size(), sizeof(raw));
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::raw_transmitted, raw, sizeof(raw));
	}
}

void
AmnbNodeTest::testRequest()
{
	DeviceWrapper<SharedMedium> dev;
	Node node(dev, 0x08);
	// should discard unrelated response
	SharedMedium::add_queued_rx({0x7E, 0x7E, 251, 0x08, 0x70, 0xc0}, 6);
	{
		micro_clock::setTime(0); milli_clock::setTime(0);
		modm::ResumableResult< Result<> > res{0};
		for(uint32_t ii=0; (res = node.request(0x10, 0x80)).getState() == modm::rf::Running; ii += 10)
		{ micro_clock::setTime(ii); milli_clock::setTime(ii/1000); node.update(); }

		TEST_ASSERT_EQUALS(res.getResult().error(), Error::RequestTimeout);
		TEST_ASSERT_FALSE(bool(res.getResult()));
		TEST_ASSERT_EQUALS(SharedMedium::tx_count, 6u);
		TEST_ASSERT_EQUALS(SharedMedium::rx_count, 12u);
	}
	SharedMedium::reset();
	// System error allocation failed
	SharedMedium::add_queued_rx({0x7E, 0x7E, 217, 16, 128, 129, 4}, 6);
	{
		micro_clock::setTime(0); milli_clock::setTime(0);
		modm::ResumableResult< Result<> > res{0};
		for(uint32_t ii=0; (res = node.request(0x10, 0x80)).getState() == modm::rf::Running; ii += 10)
		{ micro_clock::setTime(ii); milli_clock::setTime(ii/1000); node.update(); }

		TEST_ASSERT_EQUALS(res.getResult().error(), Error::ResponseAllocationFailed);
		TEST_ASSERT_FALSE(bool(res.getResult()));

		TEST_ASSERT_EQUALS(SharedMedium::tx_count, 6u);
		TEST_ASSERT_EQUALS(SharedMedium::rx_count, 13u);
	}
	SharedMedium::reset();
	// System error allocation failed
	SharedMedium::add_queued_rx({0x7E, 0x7E, 32, 16, 128, 161, 7}, 6);
	{
		micro_clock::setTime(0); milli_clock::setTime(0);
		modm::ResumableResult< Result<uint8_t, uint8_t> > res{0};
		for(uint32_t ii=0; (res = node.request<uint8_t, uint8_t>(0x10, 0x80, uint8_t(0x10))).getState() == modm::rf::Running; ii += 10)
		{ micro_clock::setTime(ii); milli_clock::setTime(ii/1000); node.update(); }

		TEST_ASSERT_EQUALS(res.getResult().error(), Error::UserError);
		TEST_ASSERT_FALSE(bool(res.getResult()));
		TEST_ASSERT_EQUALS(*res.getResult().userError(), 0x07u);

		TEST_ASSERT_EQUALS(SharedMedium::tx_count, 7u);
		TEST_ASSERT_EQUALS(SharedMedium::rx_count, 14u);
	}
	SharedMedium::reset();
	// should discard unrelated response
	SharedMedium::add_queued_rx({0x7E, 0x7E, 225, 0x08, 0x70, 0x60});
	// should discard invalid response
	SharedMedium::add_queued_rx({0x7E, 0x7E, 0, 0x10, 0x80, 0x60});
	// should use real valid response
	SharedMedium::add_queued_rx({0x7E, 0x7E, 243, 16, 128, 100, 4, 5, 6, 7}, 10);
	{
		micro_clock::setTime(0); milli_clock::setTime(0);
		modm::ResumableResult< Result<uint32_t> > res{0};
		for(uint32_t ii=0;
		    (res = node.request<uint32_t>(0x10, 0x80, uint32_t(0x03020100))).getState() == modm::rf::Running;
		    ii += 10)
		{ micro_clock::setTime(ii); milli_clock::setTime(ii/1000); node.update(); }

		TEST_ASSERT_EQUALS(res.getResult().error(), Error::Ok);
		TEST_ASSERT_TRUE(bool(res.getResult()));
		TEST_ASSERT_EQUALS(*res.getResult().result(), 0x07060504ul);
		TEST_ASSERT_EQUALS(*res.getResult(), 0x07060504ul);

		TEST_ASSERT_EQUALS(SharedMedium::tx_count, 10u);
		TEST_ASSERT_EQUALS(SharedMedium::rx_count, 32u);
	}
}

void
AmnbNodeTest::testAction()
{
	static uint8_t trig{0};
	static uint8_t count{0};
	Action actions[] =
	{
		{1, [private_count = uint8_t(10)]() mutable
			{
				trig |= 1;
				count += ++private_count;
			}
		},
		{2, [private_count = uint16_t(20)](const uint32_t& data) mutable
			{
				TEST_ASSERT_EQUALS(data, uint32_t(0x03020100));
				trig |= 2;
				count += ++private_count;
			}
		},
		{3, [](const uint32_t& data) -> Response
			{
				TEST_ASSERT_EQUALS(data, uint32_t(0x03020100));
				trig |= 4;
				return uint32_t(0x07060504);
			}
		},
		{4, []() -> Response
			{
				trig |= 8;
				return ErrorResponse(uint8_t(3));
			}
		},
		{5, [](const uint32_t& data) -> Response
			{
				TEST_ASSERT_EQUALS(data, uint32_t(0x03020100));
				trig |= 16;
				return ErrorResponse(uint32_t(0xbaadc0de));
			}
		},
	};
	DeviceWrapper<SharedMedium> dev;
	Node node(dev, 0x08, actions);

	// System Error: Wrong payload size!
	trig = 0;
	SharedMedium::reset();
	SharedMedium::add_rx({0x7E, 0x7E, 104, 8, 1, 68, 0, 1, 2, 3});
	{
		node.update(); node.update();
		TEST_ASSERT_EQUALS(trig, 0);
		const uint8_t raw[] = {0x7E, 0x7E, 237, 8, 1, 97, 2};
		TEST_ASSERT_EQUALS(SharedMedium::transmitted.size(), sizeof(raw));
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::transmitted, raw, sizeof(raw));
	}
	// System Error: No action!
	trig = 0;
	SharedMedium::reset();
	SharedMedium::add_rx({0x7E, 0x7E, 163, 8, 10, 64});
	{
		node.update(); node.update();
		TEST_ASSERT_EQUALS(trig, 0);
		const uint8_t raw[] = {0x7E, 0x7E, 76, 8, 10, 129, 3};
		TEST_ASSERT_EQUALS(SharedMedium::transmitted.size(), sizeof(raw));
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::transmitted, raw, sizeof(raw));
	}

	// Positive Response without argument
	trig = 0;
	SharedMedium::reset();
	SharedMedium::add_rx({0x7E, 0x7E, 227, 8, 1, 64});
	{
		node.update(); node.update();
		TEST_ASSERT_EQUALS(trig, 1);
		TEST_ASSERT_EQUALS(count, 11);
		const uint8_t raw[] = {0x7E, 0x7E, 42, 8, 1, 96};
		TEST_ASSERT_EQUALS(SharedMedium::transmitted.size(), sizeof(raw));
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::transmitted, raw, sizeof(raw));
	}
	// Positive Response with argument
	trig = 0;
	SharedMedium::reset();
	SharedMedium::add_rx({0x7E, 0x7E, 141, 8, 2, 68, 0, 1, 2, 3});
	{
		node.update(); node.update();
		TEST_ASSERT_EQUALS(trig, 2);
		TEST_ASSERT_EQUALS(count, 11+21);
		const uint8_t raw[] = {0x7E, 0x7E, 209, 8, 2, 96};
		TEST_ASSERT_EQUALS(SharedMedium::transmitted.size(), sizeof(raw));
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::transmitted, raw, sizeof(raw));
	}
	// Positive Reponse with argument and return value
	trig = 0;
	SharedMedium::reset();
	SharedMedium::add_rx({0x7E, 0x7E, 46, 8, 3, 68, 0, 1, 2, 3});
	{
		node.update(); node.update();
		TEST_ASSERT_EQUALS(trig, 4);
		const uint8_t raw[] = {0x7E, 0x7E, 84, 8, 3, 100, 4, 5, 6, 7};
		TEST_ASSERT_EQUALS(SharedMedium::transmitted.size(), sizeof(raw));
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::transmitted, raw, sizeof(raw));
	}
	// Negative Response without argument
	trig = 0;
	SharedMedium::reset();
	SharedMedium::add_rx({0x7E, 0x7E, 233, 8, 4, 64});
	{
		node.update(); node.update();
		TEST_ASSERT_EQUALS(trig, 8);
		const uint8_t raw[] = {0x7E, 0x7E, 94, 8, 4, 161, 3};
		TEST_ASSERT_EQUALS(SharedMedium::transmitted.size(), sizeof(raw));
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::transmitted, raw, sizeof(raw));
	}
	// Negative Response with argument
	trig = 0;
	SharedMedium::reset();
	SharedMedium::add_rx({0x7E, 0x7E, 227, 8, 5, 68, 0, 1, 2, 3});
	{
		node.update(); node.update();
		TEST_ASSERT_EQUALS(trig, 16);
		const uint8_t raw[] = {0x7E, 0x7E, 133, 8, 5, 164, 0xde, 0xc0, 0xad, 0xba};
		TEST_ASSERT_EQUALS(SharedMedium::transmitted.size(), sizeof(raw));
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::transmitted, raw, sizeof(raw));
	}
}

void
AmnbNodeTest::testListener()
{
	static uint8_t trig{0};
	static uint8_t count{0};
	Listener listeners[] =
	{
		{1, [private_count = uint8_t(10)](uint8_t sender) mutable
			{
				TEST_ASSERT_EQUALS(sender, 0x20);
				trig |= 1;
				count += ++private_count;
			}
		},
		{2, [private_count = uint16_t(20)](uint8_t sender, const uint32_t& data) mutable
			{
				TEST_ASSERT_EQUALS(sender, 0x10);
				TEST_ASSERT_EQUALS(data, uint32_t(0x03020100));
				trig |= 2;
				count += ++private_count;
			}
		},
		// Test multiple listerners on the same id
		{2, [](uint8_t sender, const uint32_t& data)
			{
				TEST_ASSERT_EQUALS(sender, 0x10);
				TEST_ASSERT_EQUALS(data, uint32_t(0x03020100));
				trig |= 4;
			}
		},
	};
	DeviceWrapper<SharedMedium> dev;
	Node node(dev, 8, listeners);

	trig = 0;
	SharedMedium::add_rx({0x7E, 0x7E, 4, 32, 1, 0});
	{
		node.update();
		TEST_ASSERT_EQUALS(trig, 1);
		TEST_ASSERT_EQUALS(count, 11);
		node.update();
		TEST_ASSERT_EQUALS(trig, 1);
		TEST_ASSERT_EQUALS(count, 11);
	}
	trig = 0;
	SharedMedium::add_rx({0x7E, 0x7E, 102, 16, 2, 4, 0, 1, 2, 3});
	{
		node.update();
		TEST_ASSERT_EQUALS(trig, 6);
		TEST_ASSERT_EQUALS(count, 11+21);
		node.update();
		TEST_ASSERT_EQUALS(trig, 6);
		TEST_ASSERT_EQUALS(count, 11+21);
	}
	trig = 0;
	SharedMedium::add_rx({0x7E, 0x7E, 4, 32, 1, 0});
	SharedMedium::add_rx({0x7E, 0x7E, 102, 16, 2, 4, 0, 1, 2, 3});
	{
		node.update();
		TEST_ASSERT_EQUALS(trig, 1);
		TEST_ASSERT_EQUALS(count, 11+21+12);
		node.update();
		TEST_ASSERT_EQUALS(trig, 7);
		TEST_ASSERT_EQUALS(count, 11+21+12+22);
	}
}
