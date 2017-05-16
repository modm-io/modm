/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>
#include <modm/processing/processing.hpp>
#include <modm/debug/logger.hpp>

/**
 * Example of CAN Hardware on STM32 F3 Discovery Board.
 *
 * Connect PB8 / PB9 to a CAN transceiver which is connected to a CAN bus.
 *
 * Tested in hardware on 2013-12-17 by Sascha Schade.
 */

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

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

	Board::LedNorth::set();

	// Initialize Usart
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(10);

	MODM_LOG_INFO << "CAN Test Program" << modm::endl;

	MODM_LOG_INFO << "Initializing Can ..." << modm::endl;
	// Initialize Can
	GpioInputB8::connect(Can1::Rx, Gpio::InputType::PullUp);
	GpioOutputB9::connect(Can1::Tx, Gpio::OutputType::PushPull);
	Can1::initialize<Board::systemClock, Can1::Bitrate::kBps125>(9);

	MODM_LOG_INFO << "Setting up Filter for Can ..." << modm::endl;
	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	// Send a message
	MODM_LOG_INFO << "Sending message on Can ..." << modm::endl;
	modm::can::Message msg1(1, 1);
	msg1.setExtended(true);
	msg1.data[0] = 0x11;
	Can1::sendMessage(msg1);

	modm::ShortPeriodicTimer pTimer(100);

	while (1)
	{
		if (Can1::isMessageAvailable())
		{
			MODM_LOG_INFO << "Can: Message is available..." << modm::endl;
			modm::can::Message message;
			Can1::getMessage(message);
			displayMessage(message);
		}

		if (pTimer.execute()) {
			Board::LedNorth::toggle();

			static uint8_t idx = 0;
			modm::can::Message msg1(1, 1);
			msg1.setExtended(true);
			msg1.data[0] = idx;
			Can1::sendMessage(msg1);

			++idx;
		}
	}

	return 0;
}
