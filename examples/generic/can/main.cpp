/*
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/board.hpp>

#ifndef MODM_BOARD_HAS_LOGGER
#define GENERIC_EXAMPLE_UART
#endif
#define GENERIC_EXAMPLE_CAN
#include "../connect.hpp"

#ifndef MODM_BOARD_HAS_LOGGER
modm::IODeviceWrapper< Generic::Uart::Instance, modm::IOBuffer::BlockIfFull > loggerDevice;
modm::log::Logger modm::log::info(loggerDevice);
#endif
using MyCan = Generic::Can::Instance;

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

static void
displayMessage(const modm::can::Message& message)
{
	static uint32_t receiveCounter = 0;
	receiveCounter++;

	MODM_LOG_INFO << "id  =" << message.getIdentifier();
	if (message.isExtended()) {
		MODM_LOG_INFO << " extended";
	}
	else {
		MODM_LOG_INFO << " standard";
	}
	if (message.isRemoteTransmitRequest()) {
		MODM_LOG_INFO << ", rtr";
	}
	MODM_LOG_INFO << modm::endl;

	MODM_LOG_INFO << "dlc =" << message.getLength() << modm::endl;
	if (!message.isRemoteTransmitRequest())
	{
		MODM_LOG_INFO << "data=";
		for (uint32_t i = 0; i < message.getLength(); ++i) {
			MODM_LOG_INFO << modm::hex << message.data[i] << modm::ascii << ' ';
		}
		MODM_LOG_INFO << modm::endl;
	}
	MODM_LOG_INFO << "# received=" << receiveCounter << modm::endl;
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::Leds::setOutput();
#ifndef MODM_BOARD_HAS_LOGGER
    Generic::Uart::connect();
#endif
	MODM_LOG_INFO << "CAN Test Program" << modm::endl;

	MODM_LOG_INFO << "Initializing Can1..." << modm::endl;
	Generic::Can::connect();

	MODM_LOG_INFO << "Setting up Filter for Can1..." << modm::endl;
	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	// Send a message
	MODM_LOG_INFO << "Sending message on Can1..." << modm::endl;
	modm::can::Message msg1(1, 1);
	msg1.setExtended(true);
	msg1.data[0] = 0x11;
	MyCan::sendMessage(msg1);

	while (true)
	{
		if (MyCan::isMessageAvailable())
		{
			MODM_LOG_INFO << "Can1: Message is available..." << modm::endl;
			modm::can::Message message;
			MyCan::getMessage(message);
			displayMessage(message);

			Board::Leds::toggle();
		}
	}

	return 0;
}
