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

#include "interface_test.hpp"
#include <modm-test/mock/shared_medium.hpp>
#include <modm/communication/amnb/interface.hpp>
#include <vector>

using namespace modm::amnb;
using namespace modm_test;

class AmnbTestMessage : public Message
{
public:
	using Message::Message;

	using Message::isHeaderValid;
	using Message::isDataValid;
	using Message::setValid;
	using Message::allocate;
	using Message::self;
};

void
AmnbInterfaceTest::setUp()
{
	SharedMedium::reset();
}

// ----------------------------------------------------------------------------
void
AmnbInterfaceTest::testSerialize()
{
	DeviceWrapper<SharedMedium> dev;
	Interface<100> interface(dev); // max 100B heap allocation!
	{
		// Test inplace message tx
		AmnbTestMessage msg(7, 0x7D);
		msg.setValid();
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&msg)), InterfaceStatus::Ok);
		// with byte stuffing!
		const uint8_t raw[] = {0x7E, 0x7E, 108, 7, 0x7D, 0x5D, 0};
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::transmitted, raw, sizeof(raw));
	}
	SharedMedium::reset();
	{
		// Test inplace message tx
		AmnbTestMessage msg(200, 0x7E, 8, Type::Request);
		msg.get<uint32_t>()[0] = 0x03020100ul;
		msg.get<uint32_t>()[1] = 0x07067E7Dul;
		msg.setValid();
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&msg)), InterfaceStatus::Ok);
		// with byte stuffing!
		const uint8_t raw[] = {0x7E, 0x7E, 18, 200, 0x7D, 0x5E, 0x48, 0, 1, 2, 3, 0x7D, 0x5D, 0x7D, 0x5E, 6, 7};
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::transmitted, raw, sizeof(raw));
	}
	SharedMedium::reset();
	{
		// Test heap message tx
		AmnbTestMessage msg(10, 14, 32, Type::Error);
		for (size_t ii=0; ii < 32/sizeof(uint32_t); ii++)
			msg.get<uint32_t>()[ii] = 0x0302017Dul+ii;
		msg.setValid();
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&msg)), InterfaceStatus::Ok);
		// with byte stuffing!
		const uint8_t raw[] = {0x7E, 0x7E, 8, 10, 14, 0x9F, 32, 0, 205, 202,
				0x7D, 0x5D, 1, 2, 3,  0x7D, 0x5E, 1, 2, 3,  0x7F, 1, 2, 3,  0x80, 1, 2, 3,
				      0x81, 1, 2, 3,        0x82, 1, 2, 3,  0x83, 1, 2, 3,  0x84, 1, 2, 3};
		TEST_ASSERT_EQUALS_ARRAY(SharedMedium::transmitted, raw, sizeof(raw));
	}

	SharedMedium::reset();
	{
		// Test inplace message rx
		SharedMedium::add_rx({0x7E, 0x7E, 247, 0x7D, 0x5D, 100, 0});
		AmnbTestMessage msg;
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::Ok);
		TEST_ASSERT_TRUE(msg.isHeaderValid());
		TEST_ASSERT_TRUE(msg.isDataValid());

		TEST_ASSERT_EQUALS(msg.address(), 0x7D);
		TEST_ASSERT_EQUALS(msg.command(), 100);
		TEST_ASSERT_EQUALS(msg.length(), 0);
		TEST_ASSERT_EQUALS(msg.type(), Type::Broadcast);
		TEST_ASSERT_TRUE(msg.get() == nullptr);
	}
	SharedMedium::reset();
	{
		// Test inplace message rx
		SharedMedium::add_rx({0x7E, 0x7E, 56, 5, 15, 0x44, 0x7D, 0x5E, 1, 2, 3});
		AmnbTestMessage msg;
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::Ok);
		TEST_ASSERT_TRUE(msg.isHeaderValid());
		TEST_ASSERT_TRUE(msg.isDataValid());

		TEST_ASSERT_EQUALS(msg.address(), 5);
		TEST_ASSERT_EQUALS(msg.command(), 15);
		TEST_ASSERT_EQUALS(msg.length(), 4);
		TEST_ASSERT_EQUALS(msg.type(), Type::Request);
		TEST_ASSERT_EQUALS(*msg.get<uint32_t>(), 0x0302017Eul);
	}
	SharedMedium::reset();
	{
		SharedMedium::add_rx({0x7E, 0x7E, 82, 20, 54, 0x5F, 36, 0, 143, 101,
				1, 2, 3, 4,  2, 2, 3, 4,  3, 2, 3, 4,  4, 2, 3, 4,
				5, 2, 3, 4,  6, 2, 3, 4,  7, 2, 3, 4,  8, 2, 3, 4,  9, 2, 3, 4});

		AmnbTestMessage msg;
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::Ok);
		TEST_ASSERT_TRUE(msg.isHeaderValid());

		TEST_ASSERT_EQUALS(msg.address(), 20);
		TEST_ASSERT_EQUALS(msg.command(), 54);
		TEST_ASSERT_EQUALS(msg.length(), 36);
		TEST_ASSERT_EQUALS(msg.type(), Type::Request);

		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveData(&msg)), InterfaceStatus::Ok);
		TEST_ASSERT_TRUE(msg.isDataValid());

		const uint32_t raw[] = {0x04030201ul, 0x04030202ul, 0x04030203ul, 0x04030204ul,
								0x04030205ul, 0x04030206ul, 0x04030207ul, 0x04030208ul, 0x04030209ul};
		TEST_ASSERT_EQUALS_ARRAY(msg.get<uint32_t>(), raw, 9);
	}
}

void
AmnbInterfaceTest::testFailures()
{
	DeviceWrapper<SharedMedium> dev;
	Interface<35> interface(dev);
	{
		AmnbTestMessage msg;
		msg.setValid();

		{
			SharedMedium::fail_tx_index = 0;
			TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&msg)), InterfaceStatus::SyncWriteFailed);
			const uint8_t raw[] = {0x7E};
			TEST_ASSERT_EQUALS(SharedMedium::raw_transmitted.size(), sizeof(raw));
			TEST_ASSERT_EQUALS_ARRAY(SharedMedium::raw_transmitted, raw, sizeof(raw));
		}{
			SharedMedium::reset();
			SharedMedium::fail_tx_index = 1;
			TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&msg)), InterfaceStatus::SyncWriteFailed);
			const uint8_t raw[] = {0x7E, 0x7E};
			TEST_ASSERT_EQUALS(SharedMedium::raw_transmitted.size(), sizeof(raw));
			TEST_ASSERT_EQUALS_ARRAY(SharedMedium::raw_transmitted, raw, sizeof(raw));
		}{
			SharedMedium::reset();
			SharedMedium::fail_tx_index = 2;
			TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&msg)), InterfaceStatus::HeaderWriteFailed);
			const uint8_t raw[] = {0x7E, 0x7E, 110};
			TEST_ASSERT_EQUALS(SharedMedium::raw_transmitted.size(), sizeof(raw));
			TEST_ASSERT_EQUALS_ARRAY(SharedMedium::raw_transmitted, raw, sizeof(raw));
		}{
			SharedMedium::reset();
			SharedMedium::fail_tx_index = 4;
			TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&msg)), InterfaceStatus::HeaderWriteFailed);
			TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&msg)), InterfaceStatus::Ok);
			const uint8_t raw[] = {0x7E, 0x7E, 110,0,0,0x7E, 0x7E, 110,0,0,0};
			TEST_ASSERT_EQUALS(SharedMedium::raw_transmitted.size(), sizeof(raw));
			TEST_ASSERT_EQUALS_ARRAY(SharedMedium::raw_transmitted, raw, sizeof(raw));
		}
	}

	{
		AmnbTestMessage msg;
		// wrong synchronization
		SharedMedium::reset();
		SharedMedium::add_rx({0x00});
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::SyncReadFailed);
		// second sync byte wrong
		SharedMedium::add_rx({0x7E, 0x00});
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::SyncReadFailed);
		// sync byte read error
		SharedMedium::fail_rx_index = 1; // sync read error
		SharedMedium::add_rx({0x7E, 0x7E});
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::SyncReadFailed);

		SharedMedium::reset();
		SharedMedium::fail_rx_index = 2; // header read error
		SharedMedium::add_rx({0x7E, 0x7E, 0x00});
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::HeaderReadFailed);

		// CRC is wrong for no data
		SharedMedium::add_rx({0x7E, 0x7E, 0x00, 0x00, 0x00, 0x00});
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::HeaderInvalid);
		SharedMedium::reset();

		SharedMedium::fail_rx_index = 6; // short data read error
		SharedMedium::add_rx({0x7E, 0x7E, 0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04});
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::HeaderReadFailed);
		// wrong CRC for short data
		SharedMedium::add_rx({0x7E, 0x7E, 0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04});
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::HeaderInvalid);
		SharedMedium::reset();

		SharedMedium::fail_rx_index = 11; // long data with read error
		SharedMedium::add_rx({0x7E, 0x7E, 8, 10, 14, 0x9F, 32, 0, 205, 202, 1, 2});
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::Ok);
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveData(&msg)), InterfaceStatus::DataReadFailed);

		// allocation failed, received message too large
		SharedMedium::add_rx({0x7E, 0x7E, 82, 20, 54, 0x5F, 36, 0, 143, 101,
				1, 2, 3, 4,  2, 2, 3, 4,  3, 2, 3, 4,  4, 2, 3, 4,
				5, 2, 3, 4,  6, 2, 3, 4,  7, 2, 3, 4,  8, 2, 3, 4,  9, 2, 3, 4});
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::Ok);
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveData(&msg)), InterfaceStatus::AllocationFailed);

		// Data CRC is wrong
		SharedMedium::add_rx({0x7E, 0x7E, 8, 10, 14, 0x9F, 32, 0, 205, 202,
				1, 2, 3, 4,  2, 2, 3, 4,  3, 2, 3, 4,  4, 2, 3, 4,
				5, 2, 3, 4,  6, 2, 3, 4,  7, 2, 3, 4,  8, 2, 3, 4});
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&msg)), InterfaceStatus::Ok);
		TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveData(&msg)), InterfaceStatus::DataInvalid);
	}
}

void
AmnbInterfaceTest::testInterlock()
{
	DeviceWrapper<SharedMedium> dev;
	Interface interface(dev);
	AmnbTestMessage tx_msg;
	AmnbTestMessage rx_msg;

	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&rx_msg)), InterfaceStatus::MediumEmpty);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::Ok);
	SharedMedium::reset();

	SharedMedium::add_rx({0x7E});
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);
	interface.receiveHeader(&rx_msg);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);

	SharedMedium::add_rx({0x00});
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&rx_msg)), InterfaceStatus::SyncReadFailed);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::Ok);

	// same but with read error
	SharedMedium::reset();
	SharedMedium::fail_rx_index = 1;
	SharedMedium::add_rx({0x7E});
	interface.receiveHeader(&rx_msg);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);

	SharedMedium::add_rx({0x7E});
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&rx_msg)), InterfaceStatus::SyncReadFailed);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::Ok);

	// same but with read error later
	SharedMedium::add_rx({0x7E, 0x7E});
	interface.receiveHeader(&rx_msg);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);

	SharedMedium::add_rx({207, 7, 4});
	interface.receiveHeader(&rx_msg);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);

	SharedMedium::add_rx({0});
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&rx_msg)), InterfaceStatus::Ok);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::Ok);

	// same but with read error later
	SharedMedium::reset();
	SharedMedium::add_rx({0x7E, 0x7E});
	SharedMedium::fail_rx_index = 3;
	interface.receiveHeader(&rx_msg);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);

	SharedMedium::add_rx({207, 7, 4});
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&rx_msg)), InterfaceStatus::HeaderReadFailed);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::Ok);

	// header invalid
	SharedMedium::reset();
	SharedMedium::add_rx({0x7E, 0x7E});
	interface.receiveHeader(&rx_msg);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::MediumBusy);

	SharedMedium::add_rx({207, 7, 5, 0});
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.receiveHeader(&rx_msg)), InterfaceStatus::HeaderInvalid);
	TEST_ASSERT_EQUALS(RF_CALL_BLOCKING(interface.transmit(&tx_msg)), InterfaceStatus::Ok);
}
