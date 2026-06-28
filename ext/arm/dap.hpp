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

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @ingroup modm_cmsis_dap
/// @{

// Original functions by DAP.h
void DAP_Setup(void);

// DAP Vendor Command IDs 0-31
#define ID_DAP_Vendor(id) (0x80U + (id))
#ifndef ID_DAP_Invalid
#define ID_DAP_Invalid 0xFFU
#endif
uint32_t DAP_ProcessVendorCommand(const uint8_t *request, uint8_t *response);

uint32_t DAP_ExecuteCommand(const uint8_t *request, uint8_t *response);

// Original functions by DAP config
uint8_t DAP_GetVendorString(char *str);
uint8_t DAP_GetProductString(char *str);
uint8_t DAP_GetSerNumString(char *str);
uint8_t DAP_GetTargetDeviceVendorString(char *str);
uint8_t DAP_GetTargetDeviceNameString(char *str);
uint8_t DAP_GetTargetBoardVendorString(char *str);
uint8_t DAP_GetTargetBoardNameString(char *str);
uint8_t DAP_GetProductFirmwareVersionString(char *str);
uint32_t DAP_GetTimestamp(void);

// Additional functions by modm
typedef enum
{
	DAP_PIN_SET_LOW = 0,
	DAP_PIN_SET_HIGH = 1,
	DAP_PIN_READ,
	DAP_PIN_DIR_OUT,
	DAP_PIN_DIR_IN,

	DAP_PORT_SETUP_JTAG,
	DAP_PORT_SETUP_SWD,
	DAP_PORT_OFF,
} DAP_Command_t;

uint32_t DAP_PinSwclkTck(DAP_Command_t cmd);
uint32_t DAP_PinSwdioTms(DAP_Command_t cmd);
uint32_t DAP_PinTdi(DAP_Command_t cmd);
uint32_t DAP_PinTdo(DAP_Command_t cmd);
uint32_t DAP_PinnTrst(DAP_Command_t cmd);
uint32_t DAP_PinnReset(DAP_Command_t cmd);

void DAP_LedConnectedOut(DAP_Command_t cmd);
void DAP_LedRunningOut(DAP_Command_t cmd);

void DAP_Port(DAP_Command_t cmd);

uint8_t DAP_Reset(void);

/// @}

#ifdef __cplusplus
}

namespace modm::dap
{

/// @ingroup modm_cmsis_dap
/// @{

template<typename Pin>
uint32_t cmd(DAP_Command_t cmd)
{
	switch (cmd)
	{
		case DAP_PIN_SET_LOW: Pin::reset(); break;
		case DAP_PIN_SET_HIGH: Pin::set(); break;
		case DAP_PIN_READ: return Pin::read();
		case DAP_PIN_DIR_OUT: Pin::setOutput(); break;
		case DAP_PIN_DIR_IN: Pin::setInput(); break;
		default: break;
	}
	return 0u;
}

struct Pin
{
	void (*set)();
	void (*reset)();
	bool (*read)();
	void (*setOutput)();
	void (*setInput)();

	template<typename Gpio>
	static Pin from()
	{
		return Pin{
			.set = &Gpio::set,
			.reset = &Gpio::reset,
			.read = &Gpio::read,
			.setOutput = &Gpio::setOutput,
			.setInput = &Gpio::setInput};
	}

	uint32_t operator()(DAP_Command_t cmd) const
	{
		switch (cmd)
		{
			case DAP_PIN_SET_LOW: reset(); break;
			case DAP_PIN_SET_HIGH: set(); break;
			case DAP_PIN_READ: return read();
			case DAP_PIN_DIR_OUT: setOutput(); break;
			case DAP_PIN_DIR_IN: setInput(); break;
			default: break;
		}
		return 0u;
	}
};

/// @}

} // namespace modm::dap

#endif
