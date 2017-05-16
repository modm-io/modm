/*
 * Copyright (c) 2014-2016, Niklas Hauser
 * Copyright (c) 2014, 2016, Sascha Schade
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

#include <modm/ui/display/buffered_graphic_display.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/timer.hpp>

namespace modm
{

struct ssd1306
{
protected:
	/// @cond
	enum Command : uint8_t
	{
		// fundamental commands
		SetContrastControl = 0x81,
		SetChargePump = 0x8D,
		SetEntireDisplayResumeToRam = 0xA4,
		SetEntireDisplayIgnoreRam = 0xA5,
		SetNormalDisplay = 0xA6,
		SetInvertedDisplay = 0xA7,
		SetDisplayOff = 0xAE,
		SetDisplayOn = 0xAF,

		// scrolling commands
		SetHorizontalScrollRight = 0x26,
		SetHorizontalScrollLeft = 0x27,
		SetVerticalAndHorizontalScrollRight = 0x29,
		SetVerticalAndHorizontalScrollLeft = 0x2A,
		SetDisableScroll = 0x2E,
		SetEnableScroll = 0x2F,
		SetVerticalScrollArea = 0xA3,

		// addressing commands
		SetLowerColumnStartAddress = 0x00,
		SetHigherColumnStartAddress = 0x10,
		SetMemoryMode = 0x20,
		SetColumnAddress = 0x21,
		SetPageAddress = 0x22,
		SetPageStartAddress = 0xB0,

		// Hardware configuration
		SetDisplayStartLine = 0x40,
		SetSegmentRemap0 = 0xA0,
		SetSegmentRemap127 = 0xA1,
		SetMultiplexRatio = 0xA8,
		SetComOutputScanDirectionIncrement = 0xC0,
		SetComOutputScanDirectionDecrement = 0xC8,
		SetDisplayOffset = 0xD3,
		SetComPins = 0xDA,

		// timing configuration
		SetDisplayClockDivideRatio = 0xD5,
		SetPreChargePeriod = 0xD9,
		SetV_DeselectLevel = 0xDB,
		Nop = 0xE3,
	};
	/// @endcond

public:
	enum class
	Rotation : bool
	{
		Normal,
		UpsideDown
	};

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
		Right = SetHorizontalScrollRight,
		Left = SetHorizontalScrollLeft
	};

	enum class
	DisplayMode : uint8_t
	{
		Normal = SetNormalDisplay,
		Inverted = SetInvertedDisplay
	};

protected:
	/// @cond
	static constexpr uint8_t
	i(ScrollDirection direction) { return uint8_t(direction); }
	static constexpr uint8_t
	i(ScrollStep step) { return uint8_t(step); }

public:
	template < uint8_t Height >
	class DataTransmissionAdapter : public modm::I2cWriteTransaction
	{
	public:
		DataTransmissionAdapter(uint8_t address);

		void modm_always_inline
		setCommandBuffer(uint8_t *buffer)
		{ commands = buffer; }

		bool
		configureDisplayWrite(uint8_t (*buffer)[Height / 8], std::size_t size);

	protected:
		virtual Writing
		writing() override;

		virtual void
		detaching(modm::I2c::DetachCause cause) override;

	private:
		uint8_t *commands;
	public:
		bool writeable;
	};
	/// @endcond
}; // struct ssd1306

/**
 * Driver for SSD1306 based OLED-displays using I2C.
 * This display is only rated to be driven with 400kHz, which limits
 * the frame rate to about 40Hz.
 *
 * @author	Niklas Hauser
 * @ingroup	driver_display
 */
template < class I2cMaster, uint8_t Height = 64 >
class Ssd1306 : public ssd1306, public BufferedGraphicDisplay<128, Height>,
				public I2cDevice<I2cMaster, 2, ssd1306::DataTransmissionAdapter<Height>>
{
	static_assert((Height == 64) or (Height == 32), "Display height must be either 32 or 64 pixel!");

public:
	Ssd1306(uint8_t address = 0x3C);

	/// Pings the display
	bool inline
	pingBlocking()
	{
		return RF_CALL_BLOCKING(this->ping());
	}

	/// initializes for 3V3 with charge-pump
	bool inline
	initializeBlocking()
	{
		return RF_CALL_BLOCKING(initialize());
	}

	/// Update the display with the content of the RAM buffer.
	void
	update() override
	{
		RF_CALL_BLOCKING(startWriteDisplay());
	}

	/// Use this method to synchronize writing to the displays buffer
	/// to avoid tearing.
	/// @return	`true` if the frame buffer is not being copied to the display
	bool modm_always_inline
	isWritable()
	{
		return this->transaction.writeable;
	}

	// MARK: - TASKS
	/// initializes for 3V3 with charge-pump asynchronously
	modm::ResumableResult<bool>
	initialize();

	// starts a frame transfer and waits for completion
	modm::ResumableResult<bool>
	writeDisplay();


	modm::ResumableResult<bool> modm_always_inline
	setDisplayMode(DisplayMode mode = DisplayMode::Normal)
	{ return writeCommand(static_cast<Command>(mode)); }

	modm::ResumableResult<bool> modm_always_inline
	setContrast(uint8_t contrast = 0xCE)
	{ return writeCommand(Command::SetContrastControl, contrast); }

	modm::ResumableResult<bool>
	setRotation(Rotation rotation=Rotation::Normal);


	modm::ResumableResult<bool>
	configureScroll(uint8_t origin, uint8_t size,
			ScrollDirection direction, ScrollStep steps);

	modm::ResumableResult<bool> modm_always_inline
	enableScroll()
	{ return writeCommand(Command::SetEnableScroll); }

	modm::ResumableResult<bool> modm_always_inline
	disableScroll()
	{ return writeCommand(Command::SetDisableScroll); }

protected:
	/// Write a command without data
	modm::ResumableResult<bool>
	writeCommand(uint8_t command);

	/// Write a command with one byte data
	modm::ResumableResult<bool>
	writeCommand(uint8_t command, uint8_t data);

	/// Write a command with two bytes data
	modm::ResumableResult<bool>
	writeCommand(uint8_t command, uint8_t data1, uint8_t data2);

private:
	modm::ResumableResult<void>
	startWriteDisplay();

	bool
	startTransactionWithLength(uint8_t length);

private:
	uint8_t commandBuffer[14];
};

} // namespace modm

#include "ssd1306_impl.hpp"

#include "ssd1306_transmission_impl.hpp"

#endif // MODM_SSD1306_HPP
