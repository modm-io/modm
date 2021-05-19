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
#	error	"Don't include this file directly, use 'ili9341.hpp' instead!"
#endif

namespace modm
{

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::initialize()
{
	constexpr uint8_t pwrCtrlA[] { 0x39, 0x2c, 0x00, 0x34, 0x02 };
	constexpr uint8_t pwrCtrlB[] { 0x00, 0xc1, 0x30 };
	constexpr uint8_t timCtrlA[] { 0x85, 0x10, 0x7a };
	constexpr uint8_t timCtrlB[] { 0x00, 0x00 };
	constexpr uint8_t pwrOnSeqCtrl[] { 0x64, 0x03, 0x12, 0x81 };
	constexpr uint8_t pwrCtrl1[] { 0x1b };
	constexpr uint8_t pwrCtrl2[] { 0x12 };
	constexpr uint8_t vcomCtrl1[] { 0x08, 0x26 };//0x3e, 0x28 };
	constexpr uint8_t vcomCtrl2[] { 0xb7 };
	// constexpr uint8_t pumpRatioCtrl[] { 0x20 };
	constexpr uint8_t pixelFormat[] { 0x55 };
	constexpr uint8_t frameCtrl[] { 0x00, 0x1a };
	constexpr uint8_t dispFuncCtrl[] { 0x0a, 0xa2, 0x27, 00 };//{ 0x08, 0x82, 0x27, 0x00 };
	constexpr uint8_t enable3G[] { 0x00 };
	constexpr uint8_t gammaSet[] { 0x01 };
//	constexpr uint8_t positiveGammaCorr[] { 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08,
//			0xe4, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00 };
//	constexpr uint8_t negativeGammaCorr[] { 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07,
//			0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f };
	constexpr uint8_t positiveGammaCorr[] { 0x0f, 0x1d, 0x1a, 0x0a, 0x0d, 0x07,
			0x49, 0x66, 0x3b, 0x07, 0x11, 0x01, 0x09, 0x05, 0x04 };
	constexpr uint8_t negativeGammaCorr[] { 0x00, 0x18, 0x1d, 0x02, 0x0f, 0x04,
			0x36, 0x13, 0x4c, 0x07, 0x13, 0x0f, 0x2e, 0x2f, 0x05 };

	reset();

	{
		BatchHandle h(*this);

		this->writeCommand(Command::DisplayOff);

		this->writeCommand(Command::PowerCtrlA, pwrCtrlA, sizeof(pwrCtrlA));
		this->writeCommand(Command::PowerCtrlB, pwrCtrlB, sizeof(pwrCtrlB));
		this->writeCommand(Command::TimingCtrlA, timCtrlA, sizeof(timCtrlA));
		this->writeCommand(Command::TimingCtrlB, timCtrlB, sizeof(timCtrlB));
		this->writeCommand(Command::PowerOnSequenceCtrl, pwrOnSeqCtrl, sizeof(pwrOnSeqCtrl));
		this->writeCommand(Command::PowerCtrl1, pwrCtrl1, sizeof(pwrCtrl1));
		this->writeCommand(Command::PowerCtrl2, pwrCtrl2, sizeof(pwrCtrl2));
		this->writeCommand(Command::VComCtrl1, vcomCtrl1, sizeof(vcomCtrl1));
		this->writeCommand(Command::VComCtrl2, vcomCtrl2, sizeof(vcomCtrl2));
		this->writeCommand(Command::PixelFormatSet, pixelFormat, sizeof(pixelFormat));
		this->writeCommand(Command::FrameCtrlNormalMode, frameCtrl, sizeof(frameCtrl));
		this->writeCommand(Command::DisplayFunctionCtrl, dispFuncCtrl, sizeof(dispFuncCtrl));
		this->writeCommand(Command::Enable3Gamma, enable3G, sizeof(enable3G));
		this->writeCommand(Command::GammaSet, gammaSet, sizeof(gammaSet));
		this->writeCommand(Command::PositiveGammaCorrection, positiveGammaCorr,
				sizeof(positiveGammaCorr));
		this->writeCommand(Command::NegativeGammaCorrection, negativeGammaCorr,
				sizeof(negativeGammaCorr));

		this->writeCommand(Command::LeaveSleep);
		modm::delay_ms(120);
		this->writeCommand(Command::InversionOff);
		this->writeCommand(Command::DisplayOn);

		setOrientation(orientation);
	}
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::reset(bool hardReset /* = false */)
{
	if (hardReset)
	{
		Reset::set();
		modm::delay_ms(5);
		Reset::reset();
		modm::delay_ms(5);
		Reset::set();
		modm::delay_ms(5);
	}
	else {
		BatchHandle h(*this);
		this->writeCommand(Command::SwReset);
		modm::delay_ms(5);
	}
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
uint16_t
Ili9341<Interface, Reset, Backlight, BufferSize>::getIcModel()
{
	BatchHandle h(*this);

	uint8_t buffer[4] { 0 };
	this->readData(Command::ReadId4, buffer, 4);
	return (buffer[2] << 8) | buffer[3];
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
inline void
Ili9341<Interface, Reset, Backlight, BufferSize>::setOrientation(glcd::Orientation orientation)
{
	using MemoryAccessCtrl_t = ili9341::MemoryAccessCtrl_t;
	using MemoryAccessCtrl = ili9341::MemoryAccessCtrl;
	MemoryAccessCtrl_t madCtrl { MemoryAccessCtrl::BGR };

	switch (orientation)
	{
		case glcd::Orientation::Portrait90:
			madCtrl |= MemoryAccessCtrl::MV | MemoryAccessCtrl::MX;
			break;
		case glcd::Orientation::Landscape180:
			madCtrl |= MemoryAccessCtrl::MX | MemoryAccessCtrl::MY;
			break;
		case glcd::Orientation::Portrait270:
			madCtrl |= MemoryAccessCtrl::MV | MemoryAccessCtrl::MY;
			break;
		default:
//			madCtrl |= MemoryAccessCtrl::ML;
			break;
	}

	this->orientation = orientation;

	BatchHandle h(*this);
	this->writeCommandValue8(Command::MemoryAccessCtrl, madCtrl.value);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::turnOn()
{
	BatchHandle h(*this);
	this->writeCommand(Command::DisplayOn);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::turnOff()
{
	BatchHandle h(*this);
	this->writeCommand(Command::DisplayOff);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::setIdle(bool enable)
{
	BatchHandle h(*this);
	this->writeCommand(enable ? Command::IdleModeOn : Command::IdleModeOff);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::enableSleep(bool enable)
{
	BatchHandle h(*this);
	this->writeCommand(enable ? Command::EnterSleep : Command::LeaveSleep);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::setBrightness(uint8_t level)
{
	BatchHandle h(*this);
	this->writeCommand(Command::WriteBrightness, &level, 1);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::setInvert(bool invert)
{
	BatchHandle h(*this);
	this->writeCommand(invert ? Command::InversionOn : Command::InversionOff);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::clear()
{
	auto const saveForegroundColor { foregroundColor };
	foregroundColor = backgroundColor;
	fillRectangle(glcd::Point(0, 0), Width, Height);
	foregroundColor = saveForegroundColor;
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::drawHorizontalLine(
		glcd::Point start, uint16_t length)
{
	uint16_t const pixelValue { modm::toBigEndian(foregroundColor.color) };
	auto minLength { std::min(std::size_t(length), BufferSize) };
	uint16_t *buffer16 { reinterpret_cast<uint16_t *>(buffer) };
	std::fill(buffer16, buffer16+minLength, pixelValue);

	BatchHandle h(*this);

	setClipping(start.getX(), start.getY(), length, 1);
	while (length > BufferSize)
	{
		this->writeData(buffer, BufferSize * 2);
		length -= BufferSize;
	}
	this->writeData(buffer, length * 2);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::drawVerticalLine(
		glcd::Point start, uint16_t length)
{
	uint16_t const pixelValue { modm::toBigEndian(foregroundColor.color) };
	auto minLength { std::min(std::size_t(length), BufferSize) };
	uint16_t *buffer16 { reinterpret_cast<uint16_t *>(buffer) };
	std::fill(buffer16, buffer16+minLength, pixelValue);

	BatchHandle h(*this);

	setClipping(start.getX(), start.getY(), 1, length);
	while (length > BufferSize)
	{
		this->writeData(buffer, BufferSize * 2);
		length -= BufferSize;
	}
	this->writeData(buffer, length * 2);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::fillRectangle(
		glcd::Point upperLeft, uint16_t width, uint16_t height)
{
	auto const x { upperLeft.getX() };
	auto const y { upperLeft.getY() };
	std::size_t pixelCount { std::size_t(width) * std::size_t(height) };

	uint16_t const pixelValue { modm::toBigEndian(foregroundColor.color) };
	auto minLength { std::min(std::size_t(pixelCount), BufferSize) };
	uint16_t *buffer16 { reinterpret_cast<uint16_t *>(buffer) };
	std::fill(buffer16, buffer16+minLength, pixelValue);

	BatchHandle h(*this);

	setClipping(x, y, width, height);
	while (pixelCount > BufferSize)
	{
		this->writeData(buffer, BufferSize * 2);
		pixelCount -= BufferSize;
	}
	if (pixelCount)
		this->writeData(buffer, pixelCount * 2);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::fillCircle(
		glcd::Point center, uint16_t radius)
{
	uint8_t const setColor[] { uint8_t((foregroundColor.color >> 8) & 0xff),
			uint8_t(foregroundColor.color & 0xff) };

	int16_t f = 1 - radius;
	int16_t ddF_x = 0;
	int16_t ddF_y = -2 * radius;
	uint16_t x = 0;
	uint16_t y = radius;

	BatchHandle h(*this);

	setClipping(center.getX() - radius, center.getY(), 2 * radius, 1);
	for (std::size_t i = 0; i < 2 * radius; ++i)
		this->writeData(setColor, 2);

	while(x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;

		setClipping(center.getX() - x, center.getY() - y, 2 * x, 1);
		for (std::size_t i = 0; i < 2 * x; ++i)
			this->writeData(setColor, 2);
		setClipping(center.getX() - y, center.getY() - x, 2 * y, 1);
		for (std::size_t i = 0; i < 2 * y; ++i)
			this->writeData(setColor, 2);
		setClipping(center.getX() - x, center.getY() + y, 2 * x, 1);
		for (std::size_t i = 0; i < 2 * x; ++i)
			this->writeData(setColor, 2);
		setClipping(center.getX() - y, center.getY() + x, 2 * y, 1);
		for (std::size_t i = 0; i < 2 * y; ++i)
			this->writeData(setColor, 2);
	}
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::drawImageRaw(glcd::Point upperLeft,
		uint16_t width, uint16_t height, modm::accessor::Flash<uint8_t> data)
{
	uint8_t const setColor[] { uint8_t((foregroundColor.color >> 8) & 0xff),
			uint8_t(foregroundColor.color & 0xff) };
	uint8_t const clearColor[] { uint8_t((backgroundColor.color >> 8) & 0xff),
			uint8_t(backgroundColor.color & 0xff) };

	BatchHandle h(*this);

	setClipping(upperLeft.getX(), upperLeft.getY(), width, height);

	uint8_t bit = 0x01;
	for (uint16_t r = 0; r < height; ++r)
	{
		for (uint16_t w = 0; w < width; ++w)
		{
			uint8_t byte = data[(r / 8) * width + w];
			if (byte & bit)
				this->writeData(setColor, 2);
			else
				this->writeData(clearColor, 2);
		}
		// TODO: optimize, use ROL (rotate left)
		bit <<= 1;
		if (bit == 0)
			bit = 0x01;
	}
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::drawRaw(glcd::Point upperLeft,
		uint16_t width, uint16_t height, color::Rgb565* data)
{
	BatchHandle h(*this);

	uint16_t* buffer = (uint16_t*)data;
	for(size_t i = 0; i < size_t(width*height); i++) {
		buffer[i] = modm::fromBigEndian(buffer[i]);
	}

	setClipping(upperLeft.getX(), upperLeft.getY(), width, height);
	this->writeData((uint8_t*)buffer, width * height * 2);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::setScrollArea(
		uint16_t topFixedRows, uint16_t bottomFixedRows, uint16_t firstRow)
{
	BatchHandle h(*this);

	uint8_t arg[]
	{
		uint8_t((topFixedRows >> 8) & 0xff), uint8_t(topFixedRows & 0xff),
		uint8_t((firstRow >> 8) & 0xff), uint8_t(firstRow & 0xff),
		uint8_t((bottomFixedRows >> 8) & 0xff), uint8_t(bottomFixedRows & 0xff)
	};
	this->writeCommand(Command::VerticalScrollDefinition, arg, 6);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::scrollTo(uint16_t row)
{
	BatchHandle h(*this);

	uint8_t arg[] { uint8_t((row >> 8) & 0xff), uint8_t(row & 0xff) };
	this->writeCommand(Command::VerticalScrollStartAddr, arg, 2);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::setColoredPixel(
		int16_t x, int16_t y, color::Rgb565 const &color)
{
	auto const pixelColor { color };
	uint8_t const setColor[] { uint8_t((pixelColor.color >> 8) & 0xff), uint8_t(pixelColor.color & 0xff) };

	BatchHandle h(*this);

	this->setClipping(x, y, 1, 1);
	this->writeData(setColor, 2);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::setClipping(
		uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	uint8_t buffer[4];

	buffer[0] = uint8_t((x >> 8) & 0xff);
	buffer[1] = uint8_t(x & 0xff);
	buffer[2] = uint8_t(((x + width - 1) >> 8) & 0xff);
	buffer[3] = uint8_t((x + width - 1) & 0xff);
	this->writeCommand(Command::ColumnAddressSet, buffer, 4);

	buffer[0] = uint8_t((y >> 8) & 0xff);
	buffer[1] = uint8_t(y & 0xff);
	buffer[2] = uint8_t(((y + height - 1) >> 8) & 0xff);
	buffer[3] = uint8_t((y + height - 1) & 0xff);
	this->writeCommand(Command::PageAddressSet, buffer, 4);
	this->writeCommand(Command::MemoryWrite);
}

template <class Interface, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<Interface, Reset, Backlight, BufferSize>::drawBitmap(glcd::Point upperLeft,
		uint16_t width, uint16_t height, modm::accessor::Flash<uint8_t> data)
{
	BatchHandle h(*this);

	setClipping(upperLeft.getX(), upperLeft.getY(), width, height);
	for (int i = 0; i < width * height; ++i) {
		buffer[0] = data[i*2+1];
		buffer[1] = data[i*2];
		this->writeData(buffer, 2);
	}
//	this->writeData(data.getPointer(), width * height * 2);
}

} // namespace modm
