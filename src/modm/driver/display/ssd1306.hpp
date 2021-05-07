/*
 * Copyright (c) 2014, 2016-2017, Sascha Schade
 * Copyright (c) 2014-2016, 2018, Niklas Hauser
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SSD1306_HPP
#define MODM_SSD1306_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/utils.hpp>
#include <modm/processing/timer.hpp>
#include <modm/ui/display/monochrome_graphic_display_vertical.hpp>

#include "ssd1306_register.hpp"

namespace modm
{

/// @ingroup modm_driver_ssd1306
struct ssd1306 : public ssd1306_register
{
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
		Right = HorizontalScrollRight,
		Left = HorizontalScrollLeft,
		// RightBottom = VerticalAndHorizontalScrollRight,
		// LeftBottom = VerticalAndHorizontalScrollLeft,
	};

	enum class
	DisplayMode : uint8_t
	{
		Normal = NormalDisplay,
		Inverted = InvertedDisplay
	};

public:
	/// @cond
	class Ssd1306_I2cWriteTransaction : public modm::I2cWriteTransaction
	{
	public:
		Ssd1306_I2cWriteTransaction(uint8_t address);

		bool
		configureDisplayWrite(const uint8_t *buffer, std::size_t size);

	protected:
		virtual Writing
		writing() override;

		virtual void
		detaching(modm::I2c::DetachCause cause) override;

		inline bool
		isWritable()
		{ return !transfer_active; }

	private:
		uint8_t transfer_type;
		bool transfer_active;
	};
	/// @endcond
};  // struct ssd1306

/**
 * Driver for SSD1306 based OLED-displays using I2C.
 * This display is only rated to be driven with 400kHz, which limits
 * the frame rate to about 40Hz.
 *
 * @author	Niklas Hauser
 * @author	Thomas Sommer
 * @ingroup	modm_driver_ssd1306
 */
template<class I2cMaster, uint8_t Height = 64>
class Ssd1306 : public ssd1306,
				public MonochromeGraphicDisplayVertical<128, Height>,
				public I2cDevice<I2cMaster, 3, ssd1306::Ssd1306_I2cWriteTransaction>
{
	static_assert((Height == 64) or (Height == 32), "Display height must be either 32 or 64 pixel!");

public:
	Ssd1306(uint8_t address = 0x3C);

	/// Pings the display
	bool inline pingBlocking()
	{ return RF_CALL_BLOCKING(this->ping()); }

	/// initializes for 3V3 with charge-pump
	bool inline initializeBlocking()
	{ return RF_CALL_BLOCKING(initialize()); }

	/// Update the display with the content of the RAM buffer.
	void
	update() override
	{ RF_CALL_BLOCKING(startWriteDisplay()); }

	/// Use this method to synchronize writing to the displays buffer
	/// to avoid tearing.
	/// @return	`true` if the frame buffer is not being copied to the display
	bool isWritable()
	{ return this->transaction.isWriteable(); }

	// MARK: - TASKS
	/// initializes for 3V3 with charge-pump asynchronously
	modm::ResumableResult<bool>
	initialize();

	// starts a frame transfer and waits for completion
	virtual modm::ResumableResult<bool>
	writeDisplay();

	modm::ResumableResult<bool>
	setDisplayMode(DisplayMode mode = DisplayMode::Normal)
	{
		commandBuffer[0] = mode;
		return writeCommands(1);
	}

	modm::ResumableResult<bool>
	setContrast(uint8_t contrast = 0xCE)
	{
		commandBuffer[0] = FundamentalCommands::ContrastControl;
		commandBuffer[1] = contrast;
		return writeCommands(2);
	}

	/**
	 * \param orientation	glcd::Orientation::Landscape0 or glcd::Orientation::Landscape180
	 */
	modm::ResumableResult<bool>
	setOrientation(glcd::Orientation orientation);

	modm::ResumableResult<bool>
	configureScroll(uint8_t origin, uint8_t size, ScrollDirection direction, ScrollStep steps);

	modm::ResumableResult<bool>
	enableScroll()
	{
		commandBuffer[0] = ScrollingCommands::EnableScroll;
		return writeCommands(1);
	}

	modm::ResumableResult<bool>
	disableScroll()
	{
		commandBuffer[0] = ScrollingCommands::DisableScroll;
		return writeCommands(1);
	}

protected:
	modm::ResumableResult<bool>
	writeCommands(std::size_t length);

	virtual modm::ResumableResult<void>
	initializeMemoryMode();

	virtual modm::ResumableResult<void>
	startWriteDisplay();

	uint8_t commandBuffer[7];
	bool transaction_success;
};

}  // namespace modm

#include "ssd1306_impl.hpp"

#endif  // MODM_SSD1306_HPP
