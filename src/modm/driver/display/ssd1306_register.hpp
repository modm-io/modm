/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

namespace modm
{

struct ssd1306_register
{
protected:
	enum Transfer : uint8_t
	{
		COMMAND_BURST = 0x00,
		DATA_BURST = 0x40,
		COMMAND = 0x80,
		DATA = 0xC0
	};

	enum FundamentalCommands : uint8_t
	{
		ContrastControl = 0x81,  // Range 1-255
		EntireDisplayResumeToRam = 0xA4,
		EntireDisplayIgnoreRam = 0xA5,
		NormalDisplay = 0xA6,
		InvertedDisplay = 0xA7,
		DisplayOff = 0xAE,
		DisplayOn = 0xAF,
	};

	enum AdressingCommands : uint8_t
	{
		MemoryMode = 0x20,  // enum MemoryMode
		// HORIZONTAL and VERTICAL addressing only
		ColumnAddress = 0x21,  // Range 0-127
		PageAddress = 0x22,    // Range 0-7
		// PAGE addressing only
		PageStartAddress = 0xB0,  // Range 0-7
		LowerColumnStartAddress = 0x00,
		HigherColumnStartAddress = 0x10,
	};

	enum HardwareConfigCommands : uint8_t
	{
		DisplayStartLine = 0x40,
		SegmentRemap0 = 0xA0,
		SegmentRemap127 = 0xA1,
		MultiplexRatio = 0xA8,  // Range 16-64
		ComOutputScanDirectionIncrement = 0xC0,
		ComOutputScanDirectionDecrement = 0xC8,
		DisplayOffset = 0xD3,  // Range 0-63
		ComPinsOrder = 0xDA,   // enum ComPinsOrder
	};

	enum ScrollingCommands : uint8_t
	{
		HorizontalScrollRight = 0x26,
		HorizontalScrollLeft = 0x27,
		VerticalAndHorizontalScrollRight = 0x29,
		VerticalAndHorizontalScrollLeft = 0x2A,
		VerticalScrollArea = 0xA3,
		DisableScroll = 0x2E,
		EnableScroll = 0x2F,
	};

	enum MemoryMode : uint8_t
	{
		HORIZONTAL = 0,
		VERTICAL = 1,
		PAGE = 2
	};

	enum TimingAndDrivingCommands : uint8_t
	{
		ChargePump = 0x8D,               // enum ChargePump
		DisplayClockDivideRatio = 0xD5,  // [7:4] Frequency [3:0] Prescaler
		PreChargePeriod = 0xD9,
		V_DeselectLevel = 0xDB,  // 0: ~0.65 x VCC, 1: 0.71 x VCC, 2: 0.77 x VCC, 3: 0.83 x VCC
		Nop = 0xE3
	};

	enum ChargePump : uint8_t
	{
		DISABLE = 0x00,
		V7_5 = 0x14,
		V8_5 = 0x94,
		V9 = 0x95,
	};
};

}  // namespace modm
