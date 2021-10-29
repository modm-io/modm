/*
 * Copyright (c) 2019, Mike Wolfram
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once
#ifndef MODM_ILI9341_HPP
#error "Don't include this file directly, use 'ili9341.hpp' instead!"
#endif

#include <modm/math/utils/bit_constants.hpp>

namespace modm {

/// @ingroup modm_driver_ili9341
struct ili9341_register
{
public:
	// Bit0 => false: Off, true: On
	enum Toggle : uint8_t {
		SleepDisable = 0x10,
		Inversion = 0x20,
		Enable = 0x28,
		TearingEffect = 0x34,
		Idle = 0x38,
	};

	// Bit0 => false: Write, true: Read
	enum class ReadWrite : uint8_t {
		Brightness = 0x51,
		CtrlDisplay = 0x53,
		ContentAdaptiveBrightnessCtrl = 0x55,
		CabcMinimumBrightness = 0x5E,
	};

// protected:
	enum class Command : uint8_t
	{
		Nop = 0x00,
		SwReset = 0x01,

		GammaSet = 0x26,
		ColumnAddressSet = 0x2A,
		PageAddressSet = 0x2B,
		MemoryWrite = 0x2C,
		ColorSet = 0x2D,
		MemoryRead = 0x2E,

		PartialMode = 0x12,
		NormalMode = 0x13,

		PartialArea = 0x30,
		VerticalScrollDefinition = 0x33,
		MemoryAccessCtrl = 0x36,
		VerticalScrollStartAddr = 0x37,

		PixelFormatSet = 0x3A,
		WriteMemoryContinue = 0x3C,
		ReadMemoryContinue = 0x3E,
		SetTearScanLine = 0x44,
		GetScanLine = 0x45,

		// Extended
		RgbInterfaceSignalCtrl = 0xB0,
		FrameCtrlNormalMode = 0xB1,
		FrameCtrlIdleMode = 0xB2,
		FrameCtrlPartialMode = 0xB3,
		InversionCtrl = 0xB4,
		BlankingPorchCtrl = 0xB5,
		DisplayFunctionCtrl = 0xB6,
		EntryModeSet = 0xB7,
		BacklightCtrl1 = 0xB8,
		BacklightCtrl2 = 0xB9,
		BacklightCtrl3 = 0xBA,
		BacklightCtrl4 = 0xBB,
		BacklightCtrl5 = 0xBC,
		BacklightCtrl7 = 0xBE,
		BacklightCtrl8 = 0xBF,
		PowerCtrl1 = 0xC0,
		PowerCtrl2 = 0xC1,
		VComCtrl1 = 0xC5,
		VComCtrl2 = 0xC7,
		PowerCtrlA = 0xCB,
		PowerCtrlB = 0xCF,
		NvMemoryWrite = 0xD0,
		NvMemoryProtectionKey = 0xD1,
		NvMemoryStatus = 0xD2,
		PositiveGammaCorrection = 0xE0,
		NegativeGammaCorrection = 0xE1,
		DigitalGammaCtrl = 0xE2,
		TimingCtrlA = 0xE8,
		TimingCtrlB = 0xEA,
		PowerOnSequenceCtrl = 0xED,
		Enable3Gamma = 0xF2,
		InterfaceCtrl = 0xF6,
		PumpRatioCtrl = 0xF7,
	};

	enum class ReadCommand : uint8_t {
		// TODO got 0s only for each of ReadId*
		Id = 0x04,				// returns 4 bytes
		Id1 = 0xDA,
		Id2 = 0xDB,
		Id3 = 0xDC,
		Id4 = 0xD3,

		Status = 0x09,			// returns 5 bytes
		PowerMode = 0x0A,		// returns 2 bytes
		MemoryAccessCtrl = 0x0B,// returns 2 bytes
		PixelFormat = 0x0C,		// returns 2 bytes
		ImageFormat = 0x0D,		// returns 2 bytes
		SignalMode = 0x0E,		// returns 2 bytes
		SelfDiagnostic = 0x0F,	// returns 2 bytes
	};

	enum class MemoryAccessCtrl : uint8_t
	{
		// Rgb565 bits-order
		PIXEL_DIR = Bit3,		// 0: RGB, 1: BGR
		// LCD Refresh
		REFRESH_DIR_X = Bit2,	// 0: Left->Right, 1: Right->Left
		REFRESH_DIR_Y = Bit4,	// 0: Top->Bottom, 1: Bottom->Top
		// Memory Access
		MEMORY_MODE = Bit5,		// 0: Normal, 1: Reverse
		MEMORY_DIR_X = Bit6,	// 0: Left->Right, 1: Right->Left
		MEMORY_DIR_Y = Bit7		// 0: Top->Bottom, 1: Bottom->Top
	};
	MODM_FLAGS8(MemoryAccessCtrl);
};
}