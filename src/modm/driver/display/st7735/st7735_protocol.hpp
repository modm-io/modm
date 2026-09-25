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

#include <array>
#include <cstdint>
#include <modm/architecture/utils.hpp>
#include <modm/ui/display/orientation.hpp>

namespace modm::st7735
{

/// ST7735 orientation, addressing and initialization settings for one panel.
struct PanelProfile
{
	struct OrientationConfiguration
	{
		uint8_t madctl;
		uint16_t offsetX;
		uint16_t offsetY;
	};

	uint16_t landscapeWidth;
	uint16_t landscapeHeight;
	std::array<OrientationConfiguration, 4> orientations;
	bool bgr;
	bool inverted;
	std::array<uint8_t, 3> frameRateNormal;
	std::array<uint8_t, 3> frameRateIdle;
	std::array<uint8_t, 6> frameRatePartial;
	uint8_t frameInversion;
	std::array<uint8_t, 3> powerControl1;
	uint8_t powerControl2;
	std::array<uint8_t, 2> powerControl3;
	std::array<uint8_t, 2> powerControl4;
	std::array<uint8_t, 2> powerControl5;
	uint8_t vcomControl;
	std::array<uint8_t, 16> positiveGamma;
	std::array<uint8_t, 16> negativeGamma;
};

/// 80x160 HannStar panel used by the WeAct Studio MiniSTM32H723 LCD board.
///
/// Initialization values and offsets are from WeAct's LCD test source:
/// https://github.com/WeActStudio/WeActStudio.MiniSTM32H723
inline constexpr PanelProfile HannStar80x160{
	.landscapeWidth = 160,
	.landscapeHeight = 80,
	.orientations = {{
		{0xa0, 1, 26},  // Landscape0: WeAct landscape rotated 180 degrees
		{0x00, 26, 1},  // Portrait90: WeAct portrait rotated 180 degrees
		{0x60, 1, 26},  // Landscape180: WeAct landscape
		{0xc0, 26, 1},  // Portrait270: WeAct portrait
	}},
	.bgr = true,
	.inverted = true,
	.frameRateNormal = {0x01, 0x2c, 0x2d},
	.frameRateIdle = {0x01, 0x2c, 0x2d},
	.frameRatePartial = {0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d},
	.frameInversion = 0x07,
	.powerControl1 = {0xa2, 0x02, 0x84},
	.powerControl2 = 0xc5,
	.powerControl3 = {0x0a, 0x00},
	.powerControl4 = {0x8a, 0x2a},
	.powerControl5 = {0x8a, 0xee},
	.vcomControl = 0x0e,
	.positiveGamma = {0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2b, 0x39, 0x00,
					  0x01, 0x03, 0x10},
	.negativeGamma = {0x03, 0x1d, 0x07, 0x06, 0x2e, 0x2c, 0x29, 0x2d, 0x2e, 0x2e, 0x37, 0x3f, 0x00,
					  0x00, 0x02, 0x10},
};

}  // namespace modm::st7735

/// @cond
namespace modm::detail::st7735
{

enum class Command : uint8_t
{
	Nop = 0x00,
	Reset = 0x01,
	SleepOn = 0x10,
	SleepOff = 0x11,
	NormalDisplayModeOn = 0x13,
	InverseOff = 0x20,
	InverseOn = 0x21,
	DisplayOff = 0x28,
	DisplayOn = 0x29,
	SetColumn = 0x2a,
	SetRow = 0x2b,
	WriteDisplayData = 0x2c,
	MemoryDataAccessControl = 0x36,
	InterfacePixelFormat = 0x3a,
	FrameRateControlNormal = 0xb1,
	FrameRateControlIdle = 0xb2,
	FrameRateControlPartial = 0xb3,
	FrameInversionControl = 0xb4,
	PowerControl1 = 0xc0,
	PowerControl2 = 0xc1,
	PowerControl3 = 0xc2,
	PowerControl4 = 0xc3,
	PowerControl5 = 0xc4,
	VcomControl1 = 0xc5,
	PositiveGammaControl = 0xe0,
	NegativeGammaControl = 0xe1,
};

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

constexpr std::size_t
orientationIndex(glcd::Orientation orientation)
{ return static_cast<std::size_t>(orientation); }

}  // namespace modm::detail::st7735
/// @endcond
