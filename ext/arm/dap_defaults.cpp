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

#include <cstring>
#include <cmsis/dap.hpp>
#include <modm/architecture/interface/clock.hpp>

modm_weak uint32_t DAP_GetTimestamp(void)
{
	return modm::PreciseClock::now().time_since_epoch().count();
}

modm_weak uint8_t DAP_GetVendorString (char *) { return 0u; }
modm_weak uint8_t DAP_GetProductString (char *) { return 0u; }
modm_weak uint8_t DAP_GetSerNumString (char *) { return 0u; }
modm_weak uint8_t DAP_GetTargetDeviceVendorString (char *) { return 0u; }
modm_weak uint8_t DAP_GetTargetDeviceNameString (char *) { return 0u; }
modm_weak uint8_t DAP_GetTargetBoardVendorString (char *) { return 0u; }
modm_weak uint8_t DAP_GetTargetBoardNameString (char *) { return 0u; }
modm_weak uint8_t DAP_GetProductFirmwareVersionString (char *) { return 0u; }
//**************************************************************************************************

modm_weak uint32_t DAP_PinTdi(DAP_Command_t) { return 0u; }
modm_weak uint32_t DAP_PinTdo(DAP_Command_t) { return 0u; }
modm_weak uint32_t DAP_PinnTrst(DAP_Command_t) { return 1u; }
modm_weak uint32_t DAP_PinnReset(DAP_Command_t) { return 1u; }

modm_weak void DAP_LedConnectedOut(DAP_Command_t) {}
modm_weak void DAP_LedRunningOut(DAP_Command_t) {}

modm_weak uint8_t DAP_Reset(void) { return 0u; }

modm_weak void DAP_Port(DAP_Command_t cmd)
{
	if (cmd == DAP_PORT_SETUP_SWD)
	{
		DAP_PinTdi(DAP_PIN_DIR_IN);
		DAP_PinTdo(DAP_PIN_DIR_IN);
		DAP_PinnTrst(DAP_PIN_DIR_IN);

		DAP_PinSwclkTck(DAP_PIN_SET_LOW);
		DAP_PinSwdioTms(DAP_PIN_SET_HIGH);
		DAP_PinnReset(DAP_PIN_SET_HIGH);

		DAP_PinSwclkTck(DAP_PIN_DIR_OUT);
		DAP_PinSwdioTms(DAP_PIN_DIR_OUT);
		DAP_PinnReset(DAP_PIN_DIR_OUT);
	}
	else if (cmd == DAP_PORT_SETUP_JTAG)
	{
		DAP_PinSwclkTck(DAP_PIN_SET_LOW);
		DAP_PinSwdioTms(DAP_PIN_SET_HIGH);
		DAP_PinTdi(DAP_PIN_SET_HIGH);
		DAP_PinnTrst(DAP_PIN_SET_HIGH);
		DAP_PinnReset(DAP_PIN_SET_HIGH);

		DAP_PinSwclkTck(DAP_PIN_DIR_OUT);
		DAP_PinSwdioTms(DAP_PIN_DIR_OUT);
		DAP_PinTdi(DAP_PIN_DIR_OUT);
		DAP_PinTdo(DAP_PIN_DIR_IN);
		DAP_PinnTrst(DAP_PIN_DIR_OUT);
		DAP_PinnReset(DAP_PIN_DIR_OUT);
	}
	else if (cmd == DAP_PORT_OFF)
	{
		DAP_PinSwclkTck(DAP_PIN_DIR_IN);
		DAP_PinSwdioTms(DAP_PIN_DIR_IN);
		DAP_PinTdi(DAP_PIN_DIR_IN);
		DAP_PinTdo(DAP_PIN_DIR_IN);
		DAP_PinnTrst(DAP_PIN_DIR_IN);
		DAP_PinnReset(DAP_PIN_DIR_IN);
	}
}

#if __has_include(<tusb.h>)
#include <tusb.h>

// MS OS 2.0 descriptor set header for TinyUSB. Will be discarded if TinyUSB not used.
#define MS_OS_20_DESC_LEN 0xA2
extern "C" {
const uint8_t desc_ms_os_20[] =
{
	// Set header: length, type, windows version, total length
	U16_TO_U8S_LE(0x000A), U16_TO_U8S_LE(MS_OS_20_SET_HEADER_DESCRIPTOR), U32_TO_U8S_LE(0x06030000), U16_TO_U8S_LE(MS_OS_20_DESC_LEN),

	// MS OS 2.0 Compatible ID descriptor: length, type, compatible ID, sub compatible ID
	U16_TO_U8S_LE(0x0014), U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID), 'W', 'I', 'N', 'U', 'S', 'B', 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,// sub-compatible

	// MS OS 2.0 Registry property descriptor: length, type
	U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A - 0x14), U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY),
	U16_TO_U8S_LE(0x0007), U16_TO_U8S_LE(0x002A),// wPropertyDataType, wPropertyNameLength and PropertyName "DeviceInterfaceGUIDs\0" in UTF-16
	'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0,
	'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0,
	'G', 0, 'U', 0, 'I', 0, 'D', 0, 's', 0, 0, 0,
	U16_TO_U8S_LE(0x0050),// wPropertyDataLength
	//bPropertyData: {CDB3B5AD-293B-4663-AA36-1AAE46463776}.
	'{', 0, 'C', 0, 'D', 0, 'B', 0, '3', 0, 'B', 0, '5', 0, 'A', 0, 'D', 0, '-', 0,
	'2', 0, '9', 0, '3', 0, 'B', 0, '-', 0, '4', 0, '6', 0, '6', 0, '3', 0, '-', 0,
	'A', 0, 'A', 0, '3', 0, '6', 0, '-', 0, '1', 0, 'A', 0, 'A', 0, 'E', 0, '4', 0,
	'6', 0, '4', 0, '6', 0, '3', 0, '7', 0, '7', 0, '6', 0, '}', 0, 0, 0, 0, 0
};
}

#endif