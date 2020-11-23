/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2020, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TEST_MOCK_CAN_DRIVER_HPP
#define MODM_TEST_MOCK_CAN_DRIVER_HPP

#include <modm/architecture/interface/can.hpp>
#include <modm/container/linked_list.hpp>

namespace modm_test
{

namespace platform
{

/**
 * Mock CAN peripheral interface for unittests.
 *
 * @ingroup modm_test_mock_can
 */
class CanDriver : public modm::Can
{
public:
	CanDriver();

	bool
	isMessageAvailable();

	bool
	getMessage(modm::can::Message& message);

	bool
	isReadyToSend();

	bool
	sendMessage(const modm::can::Message& message);

	static uint8_t
	getReceiveErrorCounter();

	static uint8_t
	getTransmitErrorCounter();

	static BusState
	getBusState();

public:
	/// Messages which should be received
	modm::LinkedList<modm::can::Message> receiveList;

	/// List of all messages send
	modm::LinkedList<modm::can::Message> sendList;

	/// number of messages which could be send
	uint8_t sendSlots;
};

} // namespace platform

} // namespace modm_test

#endif	// MODM_TEST_MOCK_CAN_DRIVER_HPP
