/*
 * Copyright (c) 2021, Tomasz Wasilczyk
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/math/utils/endianness.hpp>

namespace modm::detail::st7586s
{

enum class Command : uint8_t {
	Nop = 0x00,
	Reset = 0x01,
	SleepOn = 0x10,
	SleepOff = 0x11,
	PartialOn = 0x12,
	PartialOff = 0x13,
	InverseOff = 0x20,
	InverseOn = 0x21,
	AllPixelsOn = 0x22,
	AllPixelsOff = 0x23,
	DisplayOff = 0x28,
	DisplayOn = 0x29,
	SetColumn = 0x2A,
	SetRow = 0x2B,
	WriteDisplayData = 0x2C,
	ReadDisplayData = 0x2E,
	PartialDisplayArea = 0x30,
	ScrollArea = 0x33,
	DisplayControl = 0x36,
	ScanDirection = 0x36,
	StartLine = 0x37,
	DisplayModeGray = 0x38,
	DisplayModeMono = 0x39,
	EnableDdram = 0x3A,
	DisplayDuty = 0xB0,
	FirstOutputCom = 0xB1,
	FoscDivider = 0xB3,
	PartialDisplay = 0xB4,
	NLineInversion = 0xB5,
	ReadModifyWriteOn = 0xB8,
	ReadModifyWriteOff = 0xB9,
	SetVop = 0xC0,
	VopIncrease = 0xC1,
	VopDecrease = 0xC2,
	SetBias = 0xC3,
	SetBooster = 0xC4,
	SetVopOffset = 0xC7,
	AnalogControl = 0xD0,
	AutoReadControl = 0xD7,
	OtpRdWrControl = 0xE0,
	OtpControlOut = 0xE1,
	OtpWrite = 0xE2,
	OtpRead = 0xE3,
	OtpSelectionControl = 0xE4,
	OtpProgramming = 0xE5,
	FrameRateGray = 0xF0,
	FrameRateMono = 0xF1,
	TempRange = 0xF2,
	TempGradient = 0xF4,
};

namespace payload {

struct SetColumnRow {
	uint8_t startHi = 0;
	uint8_t start;
	uint8_t endHi = 0;
	uint8_t end;

	constexpr SetColumnRow(uint8_t start, uint8_t end) : start(start), end(end) {}
};

enum class DisplayControl : uint8_t {
	ComInc = 0,
	ComDec = Bit7,
	SegInc = 0,
	SegDec = Bit6,
};

using DisplayControlFlags = modm::Flags8<DisplayControl>;
MODM_TYPE_FLAGS(DisplayControlFlags);

enum class EnableDdram : uint8_t {
	Enable = 0x02,
};

enum class NLineInversion : uint8_t {
	/**
	 * When this bit is set, inversion is idependent from frame.
	 * Otherwise, inversion occurs in every frame.
	 */
	FrameIndependent = Bit7,
};
typedef modm::Flags8<NLineInversion> NLineInversion_t;
MODM_TYPE_FLAGS(NLineInversion_t);

enum class LineInversionType : uint8_t {
	FrameInversion = 0,
	Invert_3_line = 2,
	Invert_4_line = 3,
	Invert_5_line = 4,
	Invert_6_line = 5,
	Invert_7_line = 6,
	Invert_8_line = 7,
	Invert_9_line = 8,
	Invert_10_line = 9,
	Invert_11_line = 10,
	Invert_12_line = 11,
	Invert_13_line = 12,
	Invert_14_line = 13,
	Invert_15_line = 14,
	Invert_16_line = 15,
	Invert_17_line = 16,
	Invert_18_line = 17,
	Invert_19_line = 18,
	Invert_20_line = 19,
	Invert_21_line = 20,
	Invert_22_line = 21,
	Invert_23_line = 22,
	Invert_24_line = 23,
	Invert_25_line = 24,
	Invert_26_line = 25,
	Invert_27_line = 26,
	Invert_28_line = 27,
	Invert_29_line = 28,
	Invert_30_line = 29,
	Invert_31_line = 30,
	Invert_32_line = 31,
};
typedef Configuration<NLineInversion_t, LineInversionType, 0b11111> LineInversionType_t;

struct SetVop {
	LittleEndian<uint16_t> vop;

	constexpr SetVop(const float voltage) {
		/* Formula to calculate Vop from datasheet:
		 * V0 = 3.6 + (Vop + Vof + VopInc + VopDec) * 0.04 V
		 */
		auto rawVoltage = std::round((voltage - 3.6f) / 0.04f);

		if (rawVoltage < 0.0f) rawVoltage = 0.0f;
		constexpr uint16_t maxVoltage = 0b111111111;
		if (rawVoltage > float(maxVoltage)) rawVoltage = float(maxVoltage);

		vop = rawVoltage;
	}
};

enum class SetBias : uint8_t {
	Ratio_1_14 = 0,
	Ratio_1_13 = 1,
	Ratio_1_12 = 2,
	Ratio_1_11 = 3,
	Ratio_1_10 = 4,
	Ratio_1_9 = 5,
};

enum class SetBooster : uint8_t {
	x1 = 0,
	x2 = 1,
	x3 = 2,
	x4 = 3,
	x5 = 4,
	x6 = 5,
	x7 = 6,
	x8 = 7,
};

enum class AnalogControl : uint8_t {
	Enable = 0x1D,
};

}  // namespace payload

}  // namespace modm::detail::st7586s
