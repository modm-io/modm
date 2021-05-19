/*
 * Copyright (c) 2019, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_ILI9341_HPP
#define MODM_ILI9341_HPP

#include <modm/architecture/interface/delay.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/math/utils/endianness.hpp>
#include <modm/platform/gpio/base.hpp>
#include <modm/ui/display/color_graphic_display.hpp>

namespace modm
{

/// @ingroup modm_driver_ili9341
struct ili9341
{
protected:
	/// @cond
	enum class
	Command : uint8_t
	{
		// common commands
		Nop = 0x00,
		SwReset = 0x01,
		ReadID = 0x04,
		ReadStatus = 0x09,
		ReadPowerMode = 0x0a,
		ReadMemoryAccessCtrl = 0x0b,
		ReadPixelFormat = 0x0c,
		ReadImageFormat = 0x0d,
		ReadSignalMode = 0x0e,
		ReadSelfDiagnostic = 0x0f,
		EnterSleep = 0x10,
		LeaveSleep = 0x11,
		PartialMode = 0x12,
		NormalMode = 0x13,
		InversionOff = 0x20,
		InversionOn = 0x21,
		GammaSet = 0x26,
		DisplayOff = 0x28,
		DisplayOn = 0x29,
		ColumnAddressSet = 0x2a,
		PageAddressSet = 0x2b,
		MemoryWrite = 0x2c,
		ColorSet = 0x2d,
		MemoryRead = 0x2e,
		PartialArea = 0x30,
		VerticalScrollDefinition = 0x33,
		TearingEffectOff = 0x34,
		TearingEffectOn = 0x35,
		MemoryAccessCtrl = 0x36,
		VerticalScrollStartAddr = 0x37,
		IdleModeOff = 0x38,
		IdleModeOn = 0x39,
		PixelFormatSet = 0x3a,
		WriteMemoryContinue = 0x3c,
		ReadMemoryContinue = 0x3e,
		SetTearScanLine = 0x44,
		GetScanLine = 0x45,
		WriteBrightness = 0x51,
		ReadBrightness = 0x52,
		WriteCtrlDisplay = 0x53,
		ReadCtrlDisplay = 0x54,
		WriteContentAdaptiveBrightnessCtrl = 0x55,
		ReadContentAdaptiveBrightnessCtrl = 0x56,
		WriteCabcMinimumBrightness = 0x5e,
		ReadCabcMinimumBrightness = 0x5f,
		ReadId1 = 0xda,
		ReadId2 = 0xdb,
		ReadId3 = 0xdc,
		// extended commands
		RgbInterfaceSignalCtrl = 0xb0,
		FrameCtrlNormalMode = 0xb1,
		FrameCtrlIdleMode = 0xb2,
		FrameCtrlPartialMode = 0xb3,
		InversionCtrl = 0xb4,
		BlankingPorchCtrl = 0xb5,
		DisplayFunctionCtrl = 0xb6,
		EntryModeSet = 0xb7,
		BacklightCtrl1 = 0xb8,
		BacklightCtrl2 = 0xb9,
		BacklightCtrl3 = 0xba,
		BacklightCtrl4 = 0xbb,
		BacklightCtrl5 = 0xbc,
		BacklightCtrl7 = 0xbe,
		BacklightCtrl8 = 0xbf,
		PowerCtrl1 = 0xc0,
		PowerCtrl2 = 0xc1,
		VComCtrl1 = 0xc5,
		VComCtrl2 = 0xc7,
		PowerCtrlA = 0xcb,
		PowerCtrlB = 0xcf,
		NvMemoryWrite = 0xd0,
		NvMemoryProtectionKey = 0xd1,
		NvMemoryStatus = 0xd2,
		ReadId4 = 0xd3,
		PositiveGammaCorrection = 0xe0,
		NegativeGammaCorrection = 0xe1,
		DigitalGammaCtrl = 0xe2,
		TimingCtrlA = 0xe8,
		TimingCtrlB = 0xea,
		PowerOnSequenceCtrl = 0xed,
		Enable3Gamma = 0xf2,
		InterfaceCtrl = 0xf6,
		PumpRatioCtrl = 0xf7,
	};
	static constexpr uint8_t i(Command command) { return uint8_t(command); }

	enum class
	MemoryAccessCtrl : uint8_t
	{
		MY = modm::Bit7,
		MX = modm::Bit6,
		MV = modm::Bit5,
		ML = modm::Bit4,
		BGR = modm::Bit3,
		MH = modm::Bit2
	};
	MODM_FLAGS8(MemoryAccessCtrl);
	/// @endcond

public:
	enum class
	DisplayMode : uint8_t
	{
		Normal = uint8_t(Command::InversionOff),
		Inverted = uint8_t(Command::InversionOn)
	};
};

/// @ingroup modm_driver_ili9341
template <class Interface, class Reset, class Backlight, std::size_t BufferSize = 320>
class Ili9341 : public Interface, public modm::ColorGraphicDisplay
{
	static_assert(BufferSize >= 16, "at least a small buffer is required");

	static constexpr uint16_t Width = 240;
	static constexpr uint16_t Height = 320;
	using BatchHandle = typename Interface::BatchHandle;
	using Command = ili9341::Command;
public:
	using Orientation = glcd::Orientation;
	using DisplayMode = ili9341::DisplayMode;

	template<typename... Args>
	Ili9341(Args&&... args): Interface(std::forward<Args>(args)...)
	{
		Reset::setOutput(modm::Gpio::High);
		Backlight::setOutput(modm::Gpio::Low);
	}

	void
	initialize();

	void
	reset(bool hardReset = false);

	uint16_t
	getIcModel();

	void
	turnOn();

	void
	turnOff();

	void
	enableBacklight(bool enable)
	{ Backlight::set(enable); }

	void
	setBrightness(uint8_t level);

	void
	setInvert(bool invert);

	void
	setIdle(bool enable);

	void
	enableSleep(bool enable);

	uint16_t
	getWidth() const final
	{
		switch (orientation)
		{
			case Orientation::Portrait90:
			case Orientation::Portrait270:
				return Height;
			default:
				return Width;
		}
	}

	uint16_t
	getHeight() const final
	{
		switch (orientation)
		{
			case Orientation::Portrait90:
			case Orientation::Portrait270:
				return Width;
			default:
				return Height;
		}
	}

	inline std::size_t
	getBufferWidth() const final
	{
		return Width;
	}

	inline std::size_t
	getBufferHeight() const final
	{
		return Height;
	}

	void
	setPixel(int16_t x, int16_t y) final
	{ setColoredPixel(x, y, foregroundColor); }

	void
	clearPixel(int16_t x, int16_t y) final
	{ setColoredPixel(x, y, backgroundColor); }

    // TODO implement getPixel for ili9341
	color::Rgb565
	getPixel(int16_t x, int16_t y) const final
	{
		(void) x;
		(void) y;
		return modm::color::html::White;
	}

	void
	clear() final;

	inline void
	update() final
	{ /* nothing to do, data is directly written to TFT RAM */ }

	inline void
	setOrientation(glcd::Orientation orientation);

	void
	fillRectangle(glcd::Point upperLeft, uint16_t width, uint16_t height);

	inline void
	fillRectangle(int16_t x, int16_t y, uint16_t width, uint16_t height)
	{ fillRectangle(glcd::Point(x, y), width, height); }

	void
	fillCircle(glcd::Point center, uint16_t radius);

	void
	drawImageRaw(glcd::Point upperLeft,
				 uint16_t width, uint16_t height,
				 modm::accessor::Flash<uint8_t> data) final;

	void
	drawRaw(glcd::Point upperLeft, uint16_t width, uint16_t height, color::Rgb565* data);

	void
	setScrollArea(uint16_t topFixedRows, uint16_t bottomFixedRows, uint16_t firstRow);

	void
	scrollTo(uint16_t row);

	void
	drawBitmap(glcd::Point upperLeft, uint16_t width, uint16_t height, modm::accessor::Flash<uint8_t> data);

protected:
	void
	drawHorizontalLine(glcd::Point start, uint16_t length) final;

	void
	drawVerticalLine(glcd::Point start, uint16_t length) final;

private:
	void
	setColoredPixel(int16_t x, int16_t y, color::Rgb565 const &color);

	void
	setClipping(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

	Orientation orientation{Orientation::Landscape0};

	uint8_t buffer[BufferSize * 2]{0};
};

} // namespace modm

#include "ili9341_impl.hpp"

#endif //  MODM_ILI9341_HPP
