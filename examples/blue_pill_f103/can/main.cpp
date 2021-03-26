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

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;
modm::log::Logger modm::log::info(loggerDevice);

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

static void
displayMessage(const modm::can::Message& message, const uint8_t& filter_id)
{
	static uint32_t receiveCounter = 0;
	receiveCounter++;
	MODM_LOG_INFO << "filter_id = " << filter_id << ", ";
	MODM_LOG_INFO << message << modm::endl;
	MODM_LOG_INFO << "# received=" << receiveCounter << modm::endl;
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// Initialize Usart
	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::SystemClock, 115200_Bd>();

	MODM_LOG_INFO << "CAN Test Program" << modm::endl;

	MODM_LOG_INFO << "Dividing filter bank..." << modm::endl;
	CanFilter::setStartFilterBankForCan2(14);

	MODM_LOG_INFO << "Initializing Can1..." << modm::endl;

	// Initialize Can1
	if (false) {
		Can::connect<GpioInputA11::Rx, GpioOutputA12::Tx>(Gpio::InputType::PullUp);
	} else {
		Can::connect<GpioInputB8::Rx, GpioOutputB9::Tx>(Gpio::InputType::PullUp);
	}
	Can::initialize<Board::SystemClock, 125_kbps>(9);

	MODM_LOG_INFO << "Setting up Filter for Can1..." << modm::endl;
	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	CanFilter::setFilter(1, CanFilter::FIFO0,
			CanFilter::StandardIdentifier(0),
			CanFilter::StandardFilterMask(0));

	// Send a message
	MODM_LOG_INFO << "Sending message on Can1... nn" << modm::endl;
	modm::can::Message msg1(1, 1);
	msg1.setExtended(true);
	msg1.data[0] = 0x11;
	Can::sendMessage(msg1);

	while (true)
	{
		if (Can::isMessageAvailable())
		{
			MODM_LOG_INFO << "Can1: Message is available..." << modm::endl;
			modm::can::Message message;
			uint8_t filter_id;
			Can::getMessage(message, &filter_id);
			displayMessage(message, filter_id);

			Board::LedGreen::toggle();
		}
	}

	return 0;
}
