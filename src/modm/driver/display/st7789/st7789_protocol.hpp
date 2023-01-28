/*
 * Copyright (c) 2022, Nikolay Semenov
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/utils.hpp>

/// @cond
namespace modm::detail::st7789
{

enum class Command : uint8_t
{
	Nop = 0x00,
	Reset = 0x01,
	SleepOn = 0x10,
	SleepOff = 0x11,
	PartialDisplayModeOn = 0x12,
	NormalDisplayModeOn = 0x13,
	InverseOff = 0x20,
	InverseOn = 0x21,
	DisplayOff = 0x28,
	DisplayOn = 0x29,
	SetColumn = 0x2A,
	SetRow = 0x2B,
	WriteDisplayData = 0x2C,
	ReadDisplayData = 0x2E,
	PartialDisplayArea = 0x30,
	VerticalScrollArea = 0x33,
	TearingEffectLineOff = 0x34,
	TearingEffectLineOn = 0x35,
	MemoryDataAccessControl = 0x36,
	StartLine = 0x37,
	IdleModeOff = 0x38,
	IdleModeOn = 0x39,
	InterfacePixelFormat = 0x3A,
	WriteMemoryContinue = 0x3C,
	ReadMemoryContinue = 0x3E,
	SetTearScanline = 0x44,
	GetTearScanline = 0x45,
	WriteDisplayBrightness = 0x51,
	ReadDisplayBrightness = 0x52,
	WriteCtrlDisplay = 0x53,
	ReadCtrlDisplay = 0x54,
	WriteContentAdaptiveBrightnessControl = 0x55,
	ReadContentAdaptiveBrightnessControl = 0x56,
	Write_CABC_MinimumBrightness = 0x5E,
	Read_CABC_MinimumBrightness = 0x5F,
	ReadAutomaticBrightnessControlSelfDiagnosticResult = 0x68,
	ReadID1 = 0xDA,
	ReadID2 = 0xDB,
	ReadID3 = 0xDC,
	RAM_Control = 0xB0,
	RAM_InterfaceControl = 0xB1,
	PorchSetting = 0xB2,
	FrameRateControlInPartialIdleMode = 0xB3,
	GateControl = 0xB7,
	DigitalGammaEnable = 0xBA,
	VCOM_Setting = 0xBB,
	LCM_Control = 0xC0,
	IDCodeSetting = 0xC1,
	VDV_VRH_CommandEnable = 0xC2,
	VRH_Set = 0xC3,
	VDV_Set = 0xC4,
	VCOM_Offset_Set = 0xC5,
	FrameRateControlInNormalMode = 0xC6,
	ContentAdaptiveBrightnessControl = 0xC7,
	RegisterValueSelection1 = 0xC8,
	RegisterValueSelection2 = 0xCA,
	PWM_FrequencySelection = 0xCC,
	PowerControl1 = 0xD0,
	Enable_VAP_VAN_SignalOutput = 0xD2,
	Command2Enable = 0xDF,
	PositiveVoltageGammaControl = 0xE0,
	NegativeVoltageGammaControl = 0xE1,
	DigitalGammaLookupTableForRed = 0xE2,
	DigitalGammaLookupTableForBlue = 0xE3,
	// GateControl = 0xE4,
	SPI2_Enable = 0xE7,
	PowerControl2 = 0xE8,
	EqualizeTimeControl = 0xE9,
	ProgramModeControl = 0xEC,
	ProgramModeEnable = 0xFA,
	NVM_Setting = 0xFC,
	ProgramAction = 0xFE,
};

struct InterfacePixelFormat
{
	uint8_t data;

	enum class RGBColorFormat : uint8_t
	{
		RGB_65k = 0b101,
		RGB_262k = 0b110,
	};

	enum class DataColorFormat : uint8_t
	{
		RGB_444 = 0b011,
		RGB_565 = 0b101,
		RGB_666 = 0b110,
	};

	InterfacePixelFormat&
	set(DataColorFormat value)
	{
		data = (data & 0b1111'1000) | static_cast<uint8_t>(value);
		return *this;
	}

	InterfacePixelFormat&
	set(RGBColorFormat value)
	{
		data = (data & 0b1000'1111) | (static_cast<uint8_t>(value) << 4);
		return *this;
	}
} modm_packed;

static_assert(sizeof(InterfacePixelFormat) == 1);

struct MemoryDataAccessControl
{
	uint8_t data;

	enum class DisplayDataLatchOrder : uint8_t
	{                     // MH
		LeftToRight = 0,  // LCD Refresh Left to Right
		RightToLeft = 1,  // LCD Refresh Right to Left
	};

	enum class RgbBgrOrder : uint8_t
	{  // RGB
		RGB = 0,
		BGR = 1,
	};

	enum class LineAddressOrder : uint8_t
	{                     // ML
		TopToBottom = 0,  // LCD Refresh Top to Bottom
		BottomToTop = 1,  // LCD Refresh Bottom to Top
	};

	enum class PageColumnOrder : uint8_t
	{  // MV
		NormalMode = 0,
		ReverseMode = 1,
	};

	enum class ColumnAddressOrder : uint8_t
	{  // MX
		LeftToRight = 0,
		RightToLeft = 1,
	};

	enum class PageAddressOrder : uint8_t
	{  // MY
		TopToBottom = 0,
		BottomToTop = 1,
	};

	MemoryDataAccessControl&
	set(DisplayDataLatchOrder value)
	{
		return set<2>(value);
	}

	MemoryDataAccessControl&
	set(RgbBgrOrder value)
	{
		return set<3>(value);
	}

	MemoryDataAccessControl&
	set(LineAddressOrder value)
	{
		return set<4>(value);
	}

	MemoryDataAccessControl&
	set(PageColumnOrder value)
	{
		return set<5>(value);
	}

	MemoryDataAccessControl&
	set(ColumnAddressOrder value)
	{
		return set<6>(value);
	}

	MemoryDataAccessControl&
	set(PageAddressOrder value)
	{
		return set<7>(value);
	}

private:
	template<uint8_t Position, typename Bit>
	MemoryDataAccessControl&
	set(Bit bit)
		requires(Position < 8)
	{
		data = (data & ~(1 << Position)) | (static_cast<uint8_t>(bit) << Position);
		return *this;
	}
} modm_packed;

static_assert(sizeof(MemoryDataAccessControl) == 1);

struct SetColumnRow
{
	uint8_t startHi;
	uint8_t startLo;
	uint8_t endHi;
	uint8_t endLo;

	constexpr SetColumnRow(uint16_t start, uint16_t end)
		: startHi((start >> 8) & 0xff),
		  startLo(start & 0xff),
		  endHi((end >> 8) & 0xff),
		  endLo(end & 0xff)
	{}
} modm_packed;

static_assert(sizeof(SetColumnRow) == 4);

}  // namespace modm::detail::st7789
/// @endcond
