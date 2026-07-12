/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <tusb.h>
#include <modm/board.hpp>
#include <cmsis/dap.hpp>

static uint8_t swd_port{0};
uint32_t DAP_ProcessVendorCommand(const uint8_t *request, uint8_t *response)
{
	if (request[0] == ID_DAP_Vendor(0))
	{
		DAP_PinSwclkTck(DAP_PIN_DIR_IN);
		DAP_PinSwdioTms(DAP_PIN_DIR_IN);
		swd_port = request[1];

		response[0] = ID_DAP_Vendor(0);
		response[1] = 0;
		return (2U << 16) | 2U;
	}

	// Default fallback for unknown vendor commands
	*response = ID_DAP_Invalid;
	return ((1U << 16) | 1U);
}

// Select the port
// openocd -f interface/cmsis-dap.cfg -c "transport select swd" -c init -c "cmsis-dap cmd 128 [0-3]" -c shutdown
#ifdef STATIC_PINS
using Swclk0 = Board::A0;
using Swdio0 = Board::A1;
using Swclk1 = Board::A2;
using Swdio1 = Board::A3;
using Swclk2 = Board::A4;
using Swdio2 = Board::A5;
using Swclk3 = Board::A6;
using Swdio3 = Board::A7;

uint32_t DAP_PinSwclkTck(DAP_Command_t cmd)
{
	if (swd_port == 0) return modm::dap::cmd<Swclk0>(cmd);
	if (swd_port == 1) return modm::dap::cmd<Swclk1>(cmd);
	if (swd_port == 2) return modm::dap::cmd<Swclk2>(cmd);
	return modm::dap::cmd<Swclk3>(cmd);
}
uint32_t DAP_PinSwdioTms(DAP_Command_t cmd)
{
	if (swd_port == 0)  return modm::dap::cmd<Swdio0>(cmd);
	if (swd_port == 1)  return modm::dap::cmd<Swdio1>(cmd);
	if (swd_port == 2)  return modm::dap::cmd<Swdio2>(cmd);
	return modm::dap::cmd<Swdio3>(cmd);
}
#else
const modm::dap::Pin swclk[4]{
	modm::dap::Pin::from<Board::A0>(),
	modm::dap::Pin::from<Board::A2>(),
	modm::dap::Pin::from<Board::A4>(),
	modm::dap::Pin::from<Board::A6>(),
};
const modm::dap::Pin swdio[4]{
	modm::dap::Pin::from<Board::A1>(),
	modm::dap::Pin::from<Board::A3>(),
	modm::dap::Pin::from<Board::A5>(),
	modm::dap::Pin::from<Board::A7>(),
};

uint32_t DAP_PinSwclkTck(DAP_Command_t cmd)
{
	return swclk[swd_port & 0b11](cmd);
}
uint32_t DAP_PinSwdioTms(DAP_Command_t cmd)
{
	return swdio[swd_port & 0b11](cmd);
}
#endif

void DAP_LedConnectedOut(DAP_Command_t cmd) { Board::LedRed::set(cmd); }

extern "C"
{
const char*
tud_string_desc_arr[] =
{
	NULL,			// 0: Language
	"modm",			// 1: Manufacturer
	"Multi-DAP",	// 2: Product
	NULL,			// 3: Serials, should use chip ID
	"CMSIS-DAP v2",	// 4: Interface 0 must contain "CMSIS-DAP"
};
}

static uint8_t request_buf[CFG_TUD_VENDOR_RX_BUFSIZE];
static uint8_t response_buf[CFG_TUD_VENDOR_TX_BUFSIZE];

int main()
{
	Board::initialize();
	DAP_Setup();

	Board::initializeUsb();
	tusb_init();

	while (true)
	{
		tud_task();

		if (tud_vendor_available()) {
			if (tud_vendor_read(request_buf, sizeof(request_buf)) > 0) {
				const uint32_t response_size = DAP_ExecuteCommand(request_buf, response_buf);
				tud_vendor_write(response_buf, (uint16_t)response_size);
				tud_vendor_write_flush();
			}
		}
		// modm::this_fiber::yield(); when running in a fiber!
	}

	return 0;
}

