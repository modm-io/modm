/*
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013, 2016, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include <modm/processing/timer.hpp>
#include <modm/debug/logger.hpp>

/**
 * Example of CAN Hardware on STM32 F0 Discovery Board.
 *
 * Connect PB8 / PB9 to a CAN transceiver which is connected to a CAN bus.
 *
 */

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart1, modm::IOBuffer::BlockIfFull > loggerDevice;

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

	Board::LedUp::set();

	// Initialize Usart
	Usart1::connect<GpioOutputA9::Tx>();
	Usart1::initialize<Board::systemClock, 115200>();

	MODM_LOG_INFO << "CAN Test Program" << modm::endl;

	MODM_LOG_INFO << "Initializing Can ..." << modm::endl;
	// Initialize Can
	Can::connect<GpioInputB8::Rx, GpioOutputB9::Tx>(Gpio::InputType::PullUp);
	Can::initialize<Board::systemClock, Can::Bitrate::kBps125>(9);

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
	Can::sendMessage(msg1);

	modm::ShortPeriodicTimer pTimer(1000);

	const auto silent    = static_cast<bool>(CAN->BTR & CAN_BTR_SILM);
	const auto loop_back = static_cast<bool>(CAN->BTR & CAN_BTR_LBKM);
	MODM_LOG_INFO << "Can silent mode: " << silent << modm::endl;
	MODM_LOG_INFO << "Can loop back mode: " << loop_back << modm::endl;

	while (1)
	{
		if (Can::isMessageAvailable())
		{
			MODM_LOG_INFO << "Can: Message is available..." << modm::endl;
			modm::can::Message message;
			Can::getMessage(message);
			displayMessage(message);
		}

		if (pTimer.execute()) {
			Board::LedUp::toggle();

			static uint8_t idx = 0;
			modm::can::Message msg1(1, 1);
			msg1.setExtended(true);
			msg1.data[0] = idx;
			Can::sendMessage(msg1);

			++idx;
		}
	}

	return 0;
}
