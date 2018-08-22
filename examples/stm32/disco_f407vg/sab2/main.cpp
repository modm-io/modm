/*
 * Copyright (c) 2018, Carl Treudler
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/communication/sab.hpp>
#include <modm/communication/sab2.hpp>

// ----------------------------------------------------------------------------
//
// NOTE: This example requires a connection between PB6 and PC11.
//

modm::sab2::Interface<Usart1, 20> sabA;
modm::sab2::Interface<Uart4, 20> sabB;

int
main()
{
	Board::initialize();

	Usart1::connect<GpioB6::Tx, GpioB7::Rx>();
	Usart1::initialize<Board::systemClock, 115200>();

	Uart4::connect<GpioC10::Tx, GpioC11::Rx>();
	Uart4::initialize<Board::systemClock, 115200>();

	while (1)
	{
		// sender
		// payload depends on Button state
		uint8_t payload;
		payload = Board::Button::read()?(0x55):(0x00);
		sabA.sendMessage(0x01, modm::sab::REQUEST, 0x02, &payload, sizeof(uint8_t));
		sabA.update();

		// receiver
		// sets blue LED depending on payload, address and command type are also checked
		// no ACKs are send
		sabB.update();
		while (sabB.isMessageAvailable()) {
			uint8_t *data;
			data = (uint8_t *) sabB.getPayload();
			if ((sabB.getAddress() == 0x01) & (sabB.getCommand() == 0x02)) {
				if  (*data == 0x55)
					{ Board::LedBlue::set(); }
				else
					{ Board::LedBlue::reset(); }
			}
			sabB.dropMessage();
		}

		Board::LedGreen::toggle();
		modm::delayMilliseconds(100);
	}

	return 0;
}
