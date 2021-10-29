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

#pragma once
#ifndef MODM_SSD1306_HPP
#define MODM_SSD1306_HPP

#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/utils.hpp>
#include <modm/processing/timer.hpp>

#include <modm/ui/graphic/display.hpp>
#include <modm/ui/graphic/buffer.hpp>
#include <modm/ui/color/gray.hpp>

#include "ssd1306_defines.hpp"

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
template<class I2cMaster, uint16_t H = 64>
class Ssd1306 : public ssd1306,
				public graphic::Display<Monochrome, shape::Size(128, H), false>,
				public I2cDevice<I2cMaster, 3, ssd1306::Ssd1306_I2cWriteTransaction>
{
	static_assert((H == 64) or (H == 32), "Display height must be either 32 or 64 pixel!");
public:
	using ColorType = Monochrome;
	using Buffer = graphic::Buffer<ColorType, Size(128, H)>;

	Ssd1306(uint8_t address = 0x3C)
		: graphic::Display<ColorType, Size(128, H), false>(true), I2cDevice<I2cMaster, 3, ssd1306::Ssd1306_I2cWriteTransaction>(address)
	{}

	/// Pings the display
	bool inline pingBlocking()
	{ return RF_CALL_BLOCKING(this->ping()); }

	/// initializes for 3V3 with charge-pump
	bool inline initializeBlocking()
	{ return RF_CALL_BLOCKING(initialize()); }


	/// Use this method to synchronize writing to the displays buffer
	/// to avoid tearing.
	/// @return	`true` if the frame buffer is not being copied to the display
	bool isWritable()
	{ return this->transaction.isWriteable(); }

	// MARK: - TASKS
	/// initializes for 3V3 with charge-pump asynchronously
	modm::ResumableResult<bool>
	initialize();

	modm::ResumableResult<bool>
	set(ssd1306_register::Toggle toggle, bool state);

	modm::ResumableResult<bool>
	setContrast(uint8_t contrast = 0xCE)
	{
		commandBuffer[0] = FundamentalCommands::ContrastControl;
		commandBuffer[1] = contrast;
		return writeCommands(2);
	}

	/**
	 * @param orientation	display::Orientation::Landscape0 or display::Orientation::Landscape180
	 */
	modm::ResumableResult<bool>
	setOrientation(graphic::Orientation orientation);

	modm::ResumableResult<bool>
	configureScroll(uint8_t placement, uint8_t size, ScrollDirection direction, ScrollStep steps);

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

    // TODO Abstract these write(...) cause is common to all I2c Displays!
	// Write BufferInterface
	// Caution: placement.y rounds to multiples of 8
	modm::ResumableResult<bool>
	write(graphic::BufferInterface<ColorType> &buffer, shape::Point placement = {0, 0}) {
		RF_BEGIN();
		RF_END_RETURN_CALL(writeImage(graphic::ImageAccessor<ColorType, modm::accessor::Ram>(&buffer, placement)));
	}

	// Write Flash Image
	modm::ResumableResult<bool>
	write(const uint8_t *addr, shape::Point placement = {0, 0}) {
		RF_BEGIN();
		RF_END_RETURN_CALL(writeImage(graphic::ImageAccessor<ColorType, modm::accessor::Flash>(addr, placement)));
	}

	// Clear whole screen with color
	modm::ResumableResult<bool>
	clear(ColorType color = 0);

protected:
	virtual modm::ResumableResult<bool>
	updateClipping();

	// Write monochrome Image
	template<template<typename> class Accessor>
	modm::ResumableResult<bool>
	writeImage(graphic::ImageAccessor<ColorType, Accessor> accessor);

	modm::ResumableResult<bool>
	writeCommands(std::size_t length);

	/**
	 * MemoryMode::HORIZONTAL and MemoryMode::VERTICAL have the best performance
	 * because the whole buffer is send in one transaction.
	 */
	virtual modm::ResumableResult<bool>
	initializeMemoryMode();

	// Static variables for resumable functions
	uint8_t commandBuffer[7];
	bool transaction_success;
};

}  // namespace modm

#include "ssd1306_impl.hpp"

#endif  // MODM_SSD1306_HPP
