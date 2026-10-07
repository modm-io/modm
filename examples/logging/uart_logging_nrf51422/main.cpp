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
# nRF51422-DK UART logging

UART logger and echo example using board defaults.

- Uses UART0 logger pins from the BSP (`P0.09` TX, `P0.11` RX).
- Prints startup log message and echoes received bytes.
- Toggles LEDs on incoming data.

Useful for validating UART routing and logger setup on nRF51.
*/

#include <modm/board.hpp>

using LogUart = Board::uart::Uart;

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "nRF51422-DK UART logging via Board BSP" << modm::endl;

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
