/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/*
# nRF52840-DK UART logging

EasyDMA UART logger example.

- Uses BSP default UARTE logger on `P0.06` (TX) and `P0.08` (RX).
- Prints periodic log output.

Use this to validate UARTE wiring and host serial connection.
*/

#include <modm/board.hpp>

using LogUart = Board::uart::Uart;

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "nRF52840-DK UART logging via Board BSP" << modm::endl;

	while (true)
	{
		uint8_t data;
		if (Board::uart::Uart::read(data)) {
			Board::uart::Uart::write(data);
			MODM_LOG_INFO << "echo: 0x" << modm::hex << uint16_t(data) << modm::endl;
			Board::Leds::toggle();
		}
	}
}