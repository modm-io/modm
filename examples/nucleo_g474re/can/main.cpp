/*
 * Copyright (c) 2020, Raphael Lehmann
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

using namespace modm::literals;

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO


modm::IOStream&
modm::operator<<(modm::IOStream& out, const modm::can::Message& msg)
{
	out << "can::Message<id=" << msg.getIdentifier();
	out << msg.isExtended() ? ",extended" : ",standard";
	if (msg.isRemoteTransmitRequest()) out << ",rtr";
	out << ",dlc=" << msg.getDLC();
	out  << ",data=" << modm::hex;
	for (uint32_t i = 0; i < msg.getLength(); ++i) {
		out << msg.data[i];
	}
	out << modm::ascii << ">";
	return out;
}

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "CAN Test Program" << modm::endl;

	MODM_LOG_INFO << "Initializing Fdcan1..." << modm::endl;
	// Initialize Fdcan1
	Fdcan1::connect<GpioB8::Rx, GpioB9::Tx>(Gpio::InputType::PullUp);
	Fdcan1::initialize<Board::SystemClock, 125_kbps>(9);

	MODM_LOG_INFO << "Setting up Filter for Fdcan1..." << modm::endl;
	// Receive every message
	Fdcan1::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

	MODM_LOG_INFO << "Initializing Fdcan2..." << modm::endl;
	// Initialize Fdcan2
	Fdcan2::connect<GpioB5::Rx, GpioB6::Tx>(Gpio::InputType::PullUp);
	Fdcan2::initialize<Board::SystemClock, 125_kbps>(12);

	MODM_LOG_INFO << "Setting up Filter for Fdcan2..." << modm::endl;
	// Receive every message
	//CanFilter::setFilter(14, CanFilter::FIFO0,
	//		CanFilter::ExtendedIdentifier(0),
	//		CanFilter::ExtendedFilterMask(0));

	// Send a message
	MODM_LOG_INFO << "Sending message on Fdcan1..." << modm::endl;
	modm::can::Message msg1(1, 1);
	msg1.setExtended(true);
	msg1.data[0] = 0x11;
	Fdcan1::sendMessage(msg1);

	// Send a message
	MODM_LOG_INFO << "Sending message on Fdcan2..." << modm::endl;
	msg1.data[0] = 0x22;
	Fdcan2::sendMessage(msg1);


	while (true)
	{
		if (Fdcan1::isMessageAvailable())
		{
			MODM_LOG_INFO << "Fdcan1: Message is available..." << modm::endl << ;
			modm::can::Message message;
			Fdcan1::getMessage(message);
			MODM_LOG_INFO << message << modm::endl;
		}
		if (Fdcan2::isMessageAvailable())
		{
			MODM_LOG_INFO << "Fdcan2: Message is available..." << modm::endl;
			modm::can::Message message;
			Fdcan2::getMessage(message);
			MODM_LOG_INFO << message << modm::endl;
		}
	}

	return 0;
}
