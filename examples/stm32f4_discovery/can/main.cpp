/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/board/board.hpp>

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;
modm::log::Logger modm::log::info(loggerDevice);

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

static void
displayMessage(const modm::can::Message& message)
{
	static uint32_t receiveCounter = 0;
	receiveCounter++;

	MODM_LOG_INFO<< "id  =" << message.getIdentifier();
	if (message.isExtended()) {
		MODM_LOG_INFO<< " extended";
	}
	else {
		MODM_LOG_INFO<< " standard";
	}
	if (message.isRemoteTransmitRequest()) {
		MODM_LOG_INFO<< ", rtr";
	}
	MODM_LOG_INFO<< modm::endl;

	MODM_LOG_INFO<< "dlc =" << message.getLength() << modm::endl;
	if (!message.isRemoteTransmitRequest())
	{
		MODM_LOG_INFO << "data=";
		for (uint32_t i = 0; i < message.getLength(); ++i) {
			MODM_LOG_INFO<< modm::hex << message.data[i] << modm::ascii << ' ';
		}
		MODM_LOG_INFO<< modm::endl;
	}
	MODM_LOG_INFO<< "# received=" << receiveCounter << modm::endl;
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// Initialize Usart
	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::systemClock, 115200>();

	MODM_LOG_INFO << "CAN Test Program" << modm::endl;

	MODM_LOG_INFO << "Dividing filter bank..." << modm::endl;
	CanFilter::setStartFilterBankForCan2(14);

	MODM_LOG_INFO << "Initializing Can1..." << modm::endl;
	// Initialize Can1
	Can1::connect<GpioB8::Rx, GpioB9::Tx>(Gpio::InputType::PullUp);
	Can1::initialize<Board::systemClock, Can1::Bitrate::kBps125>(9);

	MODM_LOG_INFO << "Setting up Filter for Can1..." << modm::endl;
	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	MODM_LOG_INFO << "Initializing Can2..." << modm::endl;
	// Initialize Can2
	Can2::connect<GpioB5::Rx, GpioB6::Tx>(Gpio::InputType::PullUp);
	Can2::initialize<Board::systemClock, Can2::Bitrate::kBps125>(12);

	MODM_LOG_INFO << "Setting up Filter for Can2..." << modm::endl;
	// Receive every message
	CanFilter::setFilter(14, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	// Send a message
	MODM_LOG_INFO << "Sending message on Can1..." << modm::endl;
	modm::can::Message msg1(1, 1);
	msg1.setExtended(true);
	msg1.data[0] = 0x11;
	Can1::sendMessage(msg1);

	// Send a message
	MODM_LOG_INFO << "Sending message on Can2..." << modm::endl;
	msg1.data[0] = 0x22;
	Can2::sendMessage(msg1);


	while (1)
	{
		if (Can1::isMessageAvailable())
		{
			MODM_LOG_INFO << "Can1: Message is available..." << modm::endl;
			modm::can::Message message;
			Can1::getMessage(message);
			displayMessage(message);
		}
		if (Can2::isMessageAvailable())
		{
			MODM_LOG_INFO << "Can2: Message is available..." << modm::endl;
			modm::can::Message message;
			Can2::getMessage(message);
			displayMessage(message);
		}
	}

	return 0;
}
