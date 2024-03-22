/*
 * Copyright (c) 2024, Elias H.
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DW3110_DEFINITIONS_HPP
#define MODM_DW3110_DEFINITIONS_HPP

#include <stdint.h>

#include <modm/architecture/interface/register.hpp>
#include <modm/math/utils/bit_constants.hpp>

namespace modm
{

/// @ingroup modm_driver_dw3110
struct Dw3110Register
{
	enum class FastCommand : uint8_t
	{
		CMD_TXRXOFF = (0x0 << 1) | 1,  //< Puts the device into IDLE state and clears any events.
		CMD_TX = (0x1 << 1) | 1,       //< Immediate start of transmission
		CMD_RX = (0x2 << 1) | 1,       //< Enable RX immediately
		CMD_DTX = (0x3 << 1) | 1,      //< Delayed TX w.r.t. DX_TIME
		CMD_DRX = (0x4 << 1) | 1,      //< Delayed RX w.r.t. DX_TIME
		CMD_DTX_TS = (0x5 << 1) | 1,   //< Delayed TX w.r.t. TX timestamp + DX_TIME
		CMD_DRX_TS = (0x6 << 1) | 1,   //< Delayed RX w.r.t. TX timestamp + DX_TIME
		CMD_DTX_RS = (0x7 << 1) | 1,   //< Delayed TX w.r.t. RX timestamp + DX_TIME
		CMD_DRX_RS = (0x8 << 1) | 1,   //< Delayed RX w.r.t. RX timestamp + DX_TIME
		CMD_DTX_REF = (0x9 << 1) | 1,  //< Delayed TX w.r.t. DREF_TIME + DX_TIME
		CMD_DRX_REF = (0xA << 1) | 1,  //< Delayed RX w.r.t. DREF_TIME + DX_TIME
		CMD_CCA_TX = (0xB << 1) | 1,   //< TX if no preamble detected
		CMD_TX_W4R =
			(0xC << 1) | 1,  //< Start TX immediately, then when TX is done, enable the receiver
		CMD_DTX_W4R = (0xD << 1) | 1,  //< Delayed TX w.r.t. DX_TIME, then enable receiver
		CMD_DTX_TS_W4R =
			(0xE << 1) | 1,  //< Delayed TX w.r.t. TX timestamp + DX_TIME, then enable receiver
		CMD_DTX_RS_W4R =
			(0xF << 1) | 1,  //< Delayed TX w.r.t. RX timestamp + DX_TIME, then enable receiver
		CMD_DTX_REF_W4R =
			(0x10 << 1) | 1,  //< Delayed TX w.r.t. DREF_TIME + DX_TIME, then enable receiver
		CMD_CCA_TX_W4R =
			(0x11 << 1) | 1,             //< TX packet if no preamble detected, then enable receiver
		CMD_CLR_IRQS = (0x12 << 1) | 1,  //< Clear all interrupt events
		CMD_DB_TOGGLE =
			(0x13 << 1) | 1,  //< Toggle double buffer pointer / notify the device that the host
							  // has finished processing the received buffer/data.
	};

	enum class SpiMode : uint8_t
	{
		FULL_READ = 0b01 << 6,
		FULL_WRITE = 0b11 << 6,
		SHORT_READ = 0b00 << 6,
		SHORT_WRITE = 0b10 << 6,
	};

	enum class DwRegister : uint16_t
	{
		DEV_ID = ((0x00 & 0x1F) << 9) | ((0x00 & 0x7F) << 2),       //< Device Identifier
		RX_BUFFER_0 = ((0x12 & 0x1F) << 9) | ((0x00 & 0x7F) << 2),  //< Receive data buffer
		RX_BUFFER_1 = ((0x13 & 0x1F) << 9) | ((0x00 & 0x7F) << 2),  //< Second receive data buffer
		TX_BUFFER = ((0x14 & 0x1F) << 9) | ((0x00 & 0x7F) << 2),    //< Transmit data buffer
		SYS_STATE = ((0x0F & 0x1F) << 9) | ((0x30 & 0x7F) << 2),    //< System states
		TX_FCTRL = ((0x00 & 0x1F) << 9) | ((0x24 & 0x7F) << 2),     //< Transmit frame control
	};
};

}  // namespace modm

#endif /* MODM_DW3110_DEFINITIONS_HPP */
