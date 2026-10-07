/*
 * Copyright (c) 2023, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <cstring>

#include "../integration_test.hpp"
#include <modm/debug/logger.hpp>

using namespace modm::literals;
using namespace modm::platform;
using namespace Board;


int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "CAN Test Program" << modm::endl;

	MODM_LOG_INFO << "Mcan1: Initializing with 125kbps / 1Mbps (FDCAN) for boards CAN transceiver (PC12/PC14)." << modm::endl;
	// Mcan1 is connted in Board::initialize(); CAN transceiver on the dev board
	Mcan1::initialize<Board::SystemClock, 125_kbps, 1_pct, 1_Mbps>(12);

	MODM_LOG_INFO << "Mcan1: Setting up Filter to receive every message." << modm::endl;
	Mcan1::setExtendedFilter(0, Mcan1::FilterConfig::Fifo0,
			modm::can::ExtendedIdentifier(0),
			modm::can::ExtendedMask(0));
	Mcan1::setStandardFilter(0, Mcan1::FilterConfig::Fifo0,
			modm::can::StandardIdentifier(0),
			modm::can::StandardMask(0));

	Mcan1::setMode(Mcan1::Mode::TestExternalLoopback);

	static volatile bool error{false};
	Mcan1::setErrorInterruptCallback([]{
		error = true;
	});

	// In loopback mode we must receive every message that we send
	bool passed = true;
	for (uint32_t counter = 1; counter <= 10; counter++)
	{
		modm::can::Message txMsg{counter, 64};
		txMsg.setExtended(true);
		for (size_t i = 0; i < txMsg.capacity; ++i) {
			txMsg.data[i] = counter;
		}
		MODM_LOG_INFO << "Mcan1: Sending message... " << txMsg << modm::endl;
		passed &= Mcan1::sendMessage(txMsg);

		modm::delay(50ms);

		modm::can::Message rxMsg;
		if (Mcan1::isMessageAvailable() and Mcan1::getMessage(rxMsg))
		{
			MODM_LOG_INFO << "Mcan1: Received message... " << rxMsg << modm::endl;
			passed &= (rxMsg.getIdentifier() == txMsg.getIdentifier()) and
					  (rxMsg.getLength() == txMsg.getLength()) and
					  (memcmp(rxMsg.data, txMsg.data, txMsg.getLength()) == 0);
		}
		else
		{
			MODM_LOG_ERROR << "Mcan1: No message received!" << modm::endl;
			passed = false;
		}
	}

	return finishTest(passed and not error);
}
