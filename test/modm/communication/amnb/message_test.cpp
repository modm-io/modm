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

#include <modm/communication/amnb/message.hpp>
#include "message_test.hpp"
#include <algorithm>

namespace modm
{

template <typename T, typename = void>
struct has_to_chars : std::false_type {};
template <typename T>
struct has_to_chars<T, std::void_t<decltype(to_chars(std::declval<const T&>()))>> : std::true_type {};

template<typename T, typename std::enable_if<has_to_chars<T>::value, int>::type = 0>
IOStream&
operator << (IOStream& stream, const T& v)
{ stream << to_chars(v); return stream; }

}

using namespace modm::amnb;

class AmnbTestMessage : public Message
{
public:
	using Message::Message;

	using Message::isHeaderValid;
	using Message::isDataValid;
	using Message::setValid;
	using Message::allocate;
	using Message::self;

	Header& getHeader() { return header; }
	Storage& getStorage() { return storage; }
};

// ----------------------------------------------------------------------------
void
AmnbMessageTest::testConstructor()
{
	TEST_ASSERT_EQUALS(sizeof(Message), 32u);

	{
		AmnbTestMessage msg;
		TEST_ASSERT_EQUALS(msg.address(), 0u);
		TEST_ASSERT_EQUALS(msg.command(), 0u);
		TEST_ASSERT_EQUALS(msg.length(), 0u);
		TEST_ASSERT_EQUALS(msg.type(), Type::Broadcast);
		TEST_ASSERT_TRUE(msg.get() == nullptr);
		TEST_ASSERT_FALSE(msg.isHeaderValid());
		TEST_ASSERT_FALSE(msg.isDataValid());
		msg.setValid();
		TEST_ASSERT_TRUE(msg.isHeaderValid());
		TEST_ASSERT_TRUE(msg.isDataValid());
	}
	{
		AmnbTestMessage msg(4, 16);
		TEST_ASSERT_EQUALS(msg.address(), 4u);
		TEST_ASSERT_EQUALS(msg.command(), 16u);
		TEST_ASSERT_EQUALS(msg.length(), 0u);
		TEST_ASSERT_EQUALS(msg.type(), Type::Broadcast);
		TEST_ASSERT_TRUE(msg.get() == nullptr);
		TEST_ASSERT_FALSE(msg.isHeaderValid());
		TEST_ASSERT_FALSE(msg.isDataValid());
		msg.setValid();
		TEST_ASSERT_TRUE(msg.isHeaderValid());
		TEST_ASSERT_TRUE(msg.isDataValid());
	}
	{
		struct Small
		{
			uint32_t val1{1};
			uint8_t val2{2};
			uint16_t val3{3};
		};
		AmnbTestMessage msg(6, 12, sizeof(Small));
		TEST_ASSERT_EQUALS(msg.address(), 6);
		TEST_ASSERT_EQUALS(msg.command(), 12);
		TEST_ASSERT_EQUALS(msg.length(), sizeof(Small));
		TEST_ASSERT_EQUALS(msg.type(), Type::Broadcast);
		TEST_ASSERT_FALSE(msg.isHeaderValid());
		TEST_ASSERT_FALSE(msg.isDataValid());
		msg.setValid();
		TEST_ASSERT_TRUE(msg.isHeaderValid());
		TEST_ASSERT_TRUE(msg.isDataValid());

		const auto data = msg.get<Small>();
		TEST_ASSERT_TRUE(data != nullptr);
		new (data) Small;
		TEST_ASSERT_EQUALS(data->val1, 1u);
		TEST_ASSERT_EQUALS(data->val2, 2u);
		TEST_ASSERT_EQUALS(data->val3, 3u);

		// type too large for payload
		TEST_ASSERT_TRUE(msg.get<uint32_t[3]>() == nullptr);

		TEST_ASSERT_FALSE(msg.isHeaderValid());
		TEST_ASSERT_FALSE(msg.isDataValid());
		msg.setValid();
		TEST_ASSERT_TRUE(msg.isHeaderValid());
		TEST_ASSERT_TRUE(msg.isDataValid());
	}
}

void
AmnbMessageTest::testAllocator()
{
	{
		AmnbTestMessage msg(200);
		TEST_ASSERT_TRUE(msg.allocate());
	}
	{
		struct Large
		{
			uint8_t buffer[42];
		};
		AmnbTestMessage msg(sizeof(Large));
		TEST_ASSERT_EQUALS(msg.address(), 0u);
		TEST_ASSERT_EQUALS(msg.command(), 0u);
		TEST_ASSERT_EQUALS(msg.length(), sizeof(Large));
		TEST_ASSERT_EQUALS(msg.type(), Type::Broadcast);
		TEST_ASSERT_FALSE(msg.isHeaderValid());
		TEST_ASSERT_FALSE(msg.isDataValid());
		msg.setValid();
		TEST_ASSERT_TRUE(msg.isHeaderValid());
		TEST_ASSERT_TRUE(msg.isDataValid());

		const auto data = msg.get<Large>();
		TEST_ASSERT_TRUE(data != nullptr);
		TEST_ASSERT_TRUE(data->buffer != nullptr);

		// type too large
		TEST_ASSERT_TRUE(msg.get<uint32_t[11]>() == nullptr);
	}
}

void
AmnbMessageTest::testLifetime()
{
	{
		AmnbTestMessage msg(10);
		*msg.get() = 0x10;
		TEST_ASSERT_EQUALS(*msg.get(), 0x10);
		{
			AmnbTestMessage short_msg(20);
			*short_msg.get() = 0x20;
			TEST_ASSERT_EQUALS(*short_msg.get(), 0x20);
			msg = short_msg;
		}
		TEST_ASSERT_EQUALS(msg.length(), 20);
		TEST_ASSERT_EQUALS(*msg.get(), 0x20);
	}
	{
		AmnbTestMessage msg(40);
		*msg.get() = 0x40;
		TEST_ASSERT_EQUALS(*msg.get(), 0x40);
		TEST_ASSERT_EQUALS(*(msg.getStorage().large.data), 1);
		{
			AmnbTestMessage short_msg(50);
			*short_msg.get() = 0x50;
			TEST_ASSERT_EQUALS(*short_msg.get(), 0x50);
			TEST_ASSERT_EQUALS(*(short_msg.getStorage().large.data), 1);

			short_msg = msg;
			TEST_ASSERT_EQUALS(*short_msg.get(), 0x40);
			TEST_ASSERT_EQUALS(msg.length(), 40);
			TEST_ASSERT_EQUALS(msg.getStorage().large.data, short_msg.getStorage().large.data);
			TEST_ASSERT_EQUALS(*(msg.getStorage().large.data), 2);
		}
		TEST_ASSERT_TRUE(msg.get());
		TEST_ASSERT_EQUALS(*(msg.getStorage().large.data), 1);
	}
	{
		AmnbTestMessage msg(40);
		*msg.get() = 0x40;
		TEST_ASSERT_EQUALS(*msg.get(), 0x40);
		TEST_ASSERT_EQUALS(*(msg.getStorage().large.data), 1);
		{
			AmnbTestMessage short_msg(50);
			*short_msg.get() = 0x50;
			TEST_ASSERT_EQUALS(*short_msg.get(), 0x50);

			msg = short_msg;
			TEST_ASSERT_EQUALS(*msg.get(), 0x50);
			TEST_ASSERT_EQUALS(msg.length(), 50);
			TEST_ASSERT_EQUALS(msg.getStorage().large.data, short_msg.getStorage().large.data);
			TEST_ASSERT_EQUALS(*(msg.getStorage().large.data), 2);
		}
		TEST_ASSERT_EQUALS(msg.length(), 50);
		TEST_ASSERT_TRUE(msg.get());
		TEST_ASSERT_EQUALS(*msg.get(), 0x50);
		TEST_ASSERT_EQUALS(*(msg.getStorage().large.data), 1);
	}
	{
		AmnbTestMessage msg(40);
		*msg.get() = 0x40;
		TEST_ASSERT_EQUALS(*msg.get(), 0x40);
		TEST_ASSERT_EQUALS(*(msg.getStorage().large.data), 1);
		{
			AmnbTestMessage short_msg(50);
			*short_msg.get() = 0x50;
			TEST_ASSERT_EQUALS(*short_msg.get(), 0x50);

			msg = std::move(short_msg);
			TEST_ASSERT_TRUE(short_msg.get() == nullptr);
			TEST_ASSERT_EQUALS(short_msg.length(), 0);
			TEST_ASSERT_EQUALS(*msg.get(), 0x50);
			TEST_ASSERT_EQUALS(msg.length(), 50);
			TEST_ASSERT_EQUALS(*(msg.getStorage().large.data), 1);
		}
		TEST_ASSERT_EQUALS(msg.length(), 50);
		TEST_ASSERT_TRUE(msg.get());
		TEST_ASSERT_EQUALS(*msg.get(), 0x50);
		TEST_ASSERT_EQUALS(*(msg.getStorage().large.data), 1);
	}
}


#include <modm/communication/amnb/handler.hpp>

void
AmnbMessageTest::testSerialize()
{
	{
		AmnbTestMessage msg(6, 12, 4, Type::Request);
		*msg.get<uint32_t>() = 0x08070605ul;
		msg.setValid();

		const uint8_t raw[] = {197, 6, 12, 0x44, 5, 6, 7, 8};
		TEST_ASSERT_EQUALS_ARRAY(msg.self(), raw, sizeof(raw));
	}
	{
		AmnbTestMessage msg;
		const uint8_t raw[] = {197, 4, 15, 0x64, 0, 1, 2, 3};
		std::memcpy(msg.self(), raw, sizeof(raw));

		TEST_ASSERT_EQUALS(msg.address(), 4);
		TEST_ASSERT_EQUALS(msg.command(), 15);
		TEST_ASSERT_EQUALS(msg.length(), 4);
		TEST_ASSERT_EQUALS(msg.type(), Type::Response);
		TEST_ASSERT_TRUE(msg.isHeaderValid());
		TEST_ASSERT_TRUE(msg.isDataValid());
		const auto data = msg.get<uint32_t>();
		TEST_ASSERT_TRUE(data != nullptr);
		TEST_ASSERT_EQUALS(*data, 0x03020100ul);
	}
	// failures
	{
		AmnbTestMessage msg;
		// wrong length, wrong crc
		const uint8_t raw[] = {18, 4, 15, 0xde};
		std::memcpy(msg.self(), raw, sizeof(raw));
		TEST_ASSERT_EQUALS(msg.length(), 0u); // assumes large frame
		TEST_ASSERT_FALSE(msg.isHeaderValid());
		TEST_ASSERT_FALSE(msg.isDataValid());
	}

	/*
	{
		AmnbTestMessage msg(0x10, 0x80, 1, Type::UserError);
		// *msg.get<uint32_t>() = 0x07060504ul;
		// *msg.get<Error>() = Error::AllocationFailed;
		*msg.get<uint8_t>() = 0x07;
		msg.setValid();

		const uint8_t raw[32] = {0};
		TEST_ASSERT_EQUALS_ARRAY(msg.self(), raw, sizeof(raw));
	}
	//*/
}
