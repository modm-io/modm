/*
 * Copyright (c) 2021, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/platform.hpp>

using namespace modm::platform;

int main()
{
	Board::initialize();

	// Pull DP low briefly on reset to make sure USB host disconnects/reconnects
	MATRIX->CCFG_SYSIO |= (CCFG_SYSIO_SYSIO11 | CCFG_SYSIO_SYSIO10);
	GpioA22::setOutput(false);
	modm::delay_ms(5);
	GpioA22::setInput();

	Board::initializeUsb();

	tusb_init();

	while (true)
	{
		// Read any received data and echo it back
		uint8_t buf[64];
		uint32_t read_count;
		tud_task();
		read_count = UsbUart0::read(buf, sizeof(buf));
		UsbUart0::write(buf, read_count);
		Board::Led::toggle();
	}
}
