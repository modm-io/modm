/*
 * Copyright (c) 2021, Thomas Sommer
 * Copyright (c) 2023, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SSD1306_COMMON_HPP
#define MODM_SSD1306_COMMON_HPP

#include <array>
#include <cstdint>
#include <utility>

namespace modm
{

/// @ingroup modm_driver_ssd1306
struct ssd1306
{
protected:
	enum class
	FundamentalCommands : uint8_t
	{
		ContrastControl = 0x81,  // Range 1-255
		EntireDisplayResumeToRam = 0xA4,
		EntireDisplayIgnoreRam = 0xA5,
		NormalDisplay = 0xA6,
		InvertedDisplay = 0xA7,
		DisplayOff = 0xAE,
		DisplayOn = 0xAF,
	};

	enum class
	AdressingCommands : uint8_t
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

	enum class
	HardwareConfigCommands : uint8_t
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

	enum class
	ScrollingCommands : uint8_t
	{
		HorizontalScrollRight = 0x26,
		HorizontalScrollLeft = 0x27,
		VerticalAndHorizontalScrollRight = 0x29,
		VerticalAndHorizontalScrollLeft = 0x2A,
		VerticalScrollArea = 0xA3,
		DisableScroll = 0x2E,
		EnableScroll = 0x2F,
	};

	enum class
	MemoryMode : uint8_t
	{
		HORIZONTAL = 0,
		VERTICAL = 1,
		PAGE = 2
	};

	enum class
	TimingAndDrivingCommands : uint8_t
	{
		ChargePump = 0x8D,               // enum ChargePump
		DisplayClockDivideRatio = 0xD5,  // [7:4] Frequency [3:0] Prescaler
		PreChargePeriod = 0xD9,
		V_DeselectLevel = 0xDB,  // 0: ~0.65 x VCC, 1: 0.71 x VCC, 2: 0.77 x VCC, 3: 0.83 x VCC
		Nop = 0xE3
	};

	enum class
	ChargePump : uint8_t
	{
		DISABLE = 0x10,
		V7_5 = 0x14,
		V8_5 = 0x94,
		V9 = 0x95,
	};

public:
	enum class
	ScrollStep : uint8_t
	{
		Frames2 = 0b111,
		Frames3 = 0b100,
		Frames4 = 0b101,
		Frames5 = 0b000,
		Frames25 = 0b110,
		Frames64 = 0b001,
		Frames128 = 0b010,
		Frames256 = 0b011
	};

	enum class
	ScrollDirection : uint8_t
	{
		Right = std::to_underlying(ScrollingCommands::HorizontalScrollRight),
		Left = std::to_underlying(ScrollingCommands::HorizontalScrollLeft),
		// RightBottom = ScrollingCommands::VerticalAndHorizontalScrollRight,
		// LeftBottom = ScrollingCommands::VerticalAndHorizontalScrollLeft,
	};

	enum class
	DisplayMode : uint8_t
	{
		Normal = std::to_underlying(FundamentalCommands::NormalDisplay),
		Inverted = std::to_underlying(FundamentalCommands::InvertedDisplay),
	};

};

/**
 * Driver for SSD1306 based OLED-displays using SPI 4-wire mode.
 *
 * @author	Raphael Lehmann
 * @ingroup	modm_driver_ssd1306
 */
template<uint8_t Height>
class Ssd1306Common : public ssd1306,
					  public MonochromeGraphicDisplayVertical<128, Height>
{
	static_assert((Height == 64) or (Height == 32), "Display height must be either 32 or 64 pixel!");

public:
	virtual modm::ResumableResult<bool>
	setDisplayMode(DisplayMode mode = DisplayMode::Normal)
	{
		commandBuffer[0] = std::to_underlying(mode);
		return writeCommands(1);
	}

	virtual modm::ResumableResult<bool>
	setContrast(uint8_t contrast = 0xCE)
	{
		commandBuffer[0] = std::to_underlying(FundamentalCommands::ContrastControl);
		commandBuffer[1] = contrast;
		return writeCommands(2);
	}

	/**
	 * \param orientation	glcd::Orientation::Landscape0 or glcd::Orientation::Landscape180
	 */
	virtual modm::ResumableResult<bool>
	setOrientation(glcd::Orientation orientation)
	{
		if (orientation == glcd::Orientation::Landscape0)
		{
			commandBuffer[0] = std::to_underlying(HardwareConfigCommands::SegmentRemap127);
			commandBuffer[1] = std::to_underlying(HardwareConfigCommands::ComOutputScanDirectionDecrement);
		}
		else if (orientation == glcd::Orientation::Landscape180)
		{
			commandBuffer[0] = std::to_underlying(HardwareConfigCommands::SegmentRemap0);
			commandBuffer[1] = std::to_underlying(HardwareConfigCommands::ComOutputScanDirectionIncrement);
		}

		return writeCommands(2);
	}

	virtual modm::ResumableResult<bool>
	configureScroll(uint8_t origin, uint8_t size, ScrollDirection direction, ScrollStep steps)
	{

		commandBuffer[0] = std::to_underlying(ScrollingCommands::DisableScroll); // disableScroll()

		uint8_t beginY = (origin > 7) ? 7 : origin;

		uint8_t endY = ((origin + size) > 7) ? 7 : (origin + size);
		if (endY < beginY) endY = beginY;

		commandBuffer[1] = std::to_underlying(direction);
		commandBuffer[2] = 0x00;
		commandBuffer[3] = beginY;
		commandBuffer[4] = std::to_underlying(steps);
		commandBuffer[5] = endY;
		commandBuffer[6] = 0x00;
		commandBuffer[7] = 0xFF;

		return writeCommands(8);
	}

	virtual modm::ResumableResult<bool>
	enableScroll()
	{
		commandBuffer[0] = std::to_underlying(ScrollingCommands::EnableScroll);
		return writeCommands(1);
	}

	virtual modm::ResumableResult<bool>
	disableScroll()
	{
		commandBuffer[0] = std::to_underlying(ScrollingCommands::DisableScroll);
		return writeCommands(1);
	}

protected:
	virtual modm::ResumableResult<bool>
	writeCommands(std::size_t length) = 0;

	//uint8_t commandBuffer[31];
	std::array<uint8_t, 32> commandBuffer;
};

}  // namespace modm

#endif  // MODM_SSD1306_COMMON_HPP
