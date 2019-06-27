/*
 * Copyright (c) 2019, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/*
 * Minimal CAN example for LPC11C24 on LPCXpresso board.
 *
 * It sends an extended CAN message with increasing data every 5 seconds.
 * It receives any extended CAN message (not to normal messages yet (!)).
 * Use
 *   send 00000001 1 12
 * in CAN2USB to send a message with extended identifier.
 *
 * VCC of CAN (pin 20) MUST be supplied with 5.0V (4.5 ... 5.5V).
 * 3.3 V definitely does not work.
 * For the LPCXpresso board this means connecting 5.0V to lowest left connector
 * of the programming interface (PSU_VBUSX) is necessary.
 *
 * The LPC11C24 itself must be supplied with 3.3V. It dies with 5.0V.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing.hpp>
using namespace modm::literals;

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

// Create an IODeviceWrapper around the Uart Peripheral we want to use
using Uart = Uart1;
modm::IODeviceWrapper< Uart, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Uart::connect<GpioOutput1_7::Tx>();
	Uart::initialize<Board::SystemClock, 9600_Bd>();

	MODM_LOG_DEBUG << "Hello to C_CAN demo." << modm::endl;

	modm::platform::Can::initialize<Board::SystemClock, 125_kHz>();
	// Receive every extended message
	CanFilter::setFilter(
		/* id        */ CanFilter::ExtendedIdentifier(0),
		/* mask      */ CanFilter::ExtendedFilterMask(0),
		/* first mob */ 0,
		/* size      */ 16
	);

	if (modm::platform::Can::isReadyToSend()){
		MODM_LOG_DEBUG << "Ready to send." <<  modm::endl;
	} else {
		MODM_LOG_DEBUG << "NOT ready to send." << modm::endl;
	}

	MODM_LOG_DEBUG << "Bus state is " << modm::platform::Can::getBusState() << "." << modm::endl;

	modm::can::Message msg(1, 1);
	msg.setExtended(true);
	msg.data[0] = 0x11;

	modm::ShortPeriodicTimer timer(5000);

	while (true)
	{
		if (timer.execute()) {
			// Success only acknowledge the successful transmission to the CAN controller.
			// It does not guarantee that the messages was placed on the bus.
			MODM_LOG_DEBUG << (Can::sendMessage(msg) ? "success" : "fail") << modm::endl;

			Board::LedRed::toggle();
			++msg.data[0];
		}

		modm::can::Message rxMsg;
		if (Can::getMessage(rxMsg)) {
			MODM_LOG_DEBUG << rxMsg << modm::endl;
			MODM_LOG_DEBUG << (Can::sendMessage(rxMsg) ? "reply success" : "reply fail") << modm::endl;
		}
	}
	return 0;
}
