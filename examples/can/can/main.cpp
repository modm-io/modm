/*
 * Copyright (c) 2013, 2016, Kevin Läufer
 * Copyright (c) 2013-2014, 2017, Sascha Schade
 * Copyright (c) 2013-2018, Niklas Hauser
 * Copyright (c) 2018, Sebastian Birke
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing/timer.hpp>

/**
 * Example of the CAN peripherals of STM32 devices with a bxCAN.
 *
 * Connect PB8 (Rx) / PB9 (Tx) to a CAN transceiver which is connected to a CAN bus.
 * Devices with two CAN peripherals also use the second one on PB5 (Rx) and
 * PB6 (Tx) or PB13 (Tx) on the STM32F469 Discovery.
 */

#ifdef CAN2
// This device has two CAN peripherals, which share their filters.
#define HAS_SECOND_CAN
using FirstCan = Can1;
using SecondCan = Can2;
#ifdef STM32F469xx
using SecondCanTx = GpioB13;
#else
using SecondCanTx = GpioB6;
#endif
#else
using FirstCan = Can;
#endif

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

	MODM_LOG_INFO<< "dlc =" << message.getDataLengthCode() << modm::endl;
	MODM_LOG_INFO<< "length =" << message.getLength() << modm::endl;
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
	Board::Leds::setOutput();

	MODM_LOG_INFO << "CAN Test Program" << modm::endl;

#ifdef HAS_SECOND_CAN
	MODM_LOG_INFO << "Dividing filter bank..." << modm::endl;
	// Split filter bank, otherwise the second CAN does not receive anything
	CanFilter::setStartFilterBankForCan2(14);
#endif

	MODM_LOG_INFO << "Initializing first CAN..." << modm::endl;
	FirstCan::connect<GpioB8::Rx, GpioB9::Tx>(Gpio::InputType::PullUp);
	FirstCan::initialize<Board::SystemClock, 125_kbps>(9);

	// Receive every message
	CanFilter::setFilter(0, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));

#ifdef HAS_SECOND_CAN
	MODM_LOG_INFO << "Initializing second CAN..." << modm::endl;
	SecondCan::connect<GpioB5::Rx, SecondCanTx::Tx>(Gpio::InputType::PullUp);
	SecondCan::initialize<Board::SystemClock, 125_kbps>(12);

	// Receive every message
	CanFilter::setFilter(14, CanFilter::FIFO0,
			CanFilter::ExtendedIdentifier(0),
			CanFilter::ExtendedFilterMask(0));
#endif

	modm::can::Message message(1, 1);
	message.setExtended(true);
	modm::ShortPeriodicTimer timer(1s);

	while (true)
	{
		if (FirstCan::isMessageAvailable())
		{
			MODM_LOG_INFO << "First CAN: Message is available..." << modm::endl;
			modm::can::Message received;
			FirstCan::getMessage(received);
			displayMessage(received);
		}
#ifdef HAS_SECOND_CAN
		if (SecondCan::isMessageAvailable())
		{
			MODM_LOG_INFO << "Second CAN: Message is available..." << modm::endl;
			modm::can::Message received;
			SecondCan::getMessage(received);
			displayMessage(received);
		}
#endif

		if (timer.execute())
		{
			Board::Leds::toggle();

			message.data[0] = 0x11;
			FirstCan::sendMessage(message);
#ifdef HAS_SECOND_CAN
			message.data[0] = 0x22;
			SecondCan::sendMessage(message);
#endif
		}
	}

	return 0;
}
