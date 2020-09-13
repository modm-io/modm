/*
 * Copyright (c) 2019, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_ILI9341_SPI_HPP
#	error	"Don't include this file directly, use 'ili9341_spi.hpp' instead!"
#endif

namespace modm
{

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::Ili9341()
:	rotation_(Rotation::Rotate0)
{
	this->attachConfigurationHandler([]() {
			SPI::setDataMode(SPI::DataMode::Mode0);
			SPI::setDataOrder(SPI::DataOrder::MsbFirst);
		});

	Cs::setOutput(modm::Gpio::High);
	Dc::setOutput();
	Reset::setOutput(modm::Gpio::High);
	Backlight::setOutput(modm::Gpio::Low);
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::initialize()
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
	constexpr uint8_t pumpRatioCtrl[] { 0x20 };
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

	this->acquireMaster();
	Cs::reset();

	writeCommand(Command::DisplayOff);

	writeCommand(Command::PowerCtrlA, pwrCtrlA, sizeof(pwrCtrlA));
	writeCommand(Command::PowerCtrlB, pwrCtrlB, sizeof(pwrCtrlB));
	writeCommand(Command::TimingCtrlA, timCtrlA, sizeof(timCtrlA));
	writeCommand(Command::TimingCtrlB, timCtrlB, sizeof(timCtrlB));
	writeCommand(Command::PowerOnSequenceCtrl, pwrOnSeqCtrl, sizeof(pwrOnSeqCtrl));
	writeCommand(Command::PowerCtrl1, pwrCtrl1, sizeof(pwrCtrl1));
	writeCommand(Command::PowerCtrl2, pwrCtrl2, sizeof(pwrCtrl2));
	writeCommand(Command::VComCtrl1, vcomCtrl1, sizeof(vcomCtrl1));
	writeCommand(Command::VComCtrl2, vcomCtrl2, sizeof(vcomCtrl2));
	writeCommand(Command::PixelFormatSet, pixelFormat, sizeof(pixelFormat));
	writeCommand(Command::FrameCtrlNormalMode, frameCtrl, sizeof(frameCtrl));
	writeCommand(Command::DisplayFunctionCtrl, dispFuncCtrl, sizeof(dispFuncCtrl));
	writeCommand(Command::Enable3Gamma, enable3G, sizeof(enable3G));
	writeCommand(Command::GammaSet, gammaSet, sizeof(gammaSet));
	writeCommand(Command::PositiveGammaCorrection, positiveGammaCorr,
			sizeof(positiveGammaCorr));
	writeCommand(Command::NegativeGammaCorrection, negativeGammaCorr,
			sizeof(negativeGammaCorr));

	writeCommand(Command::LeaveSleep);
	modm::delay_ms(120);
	writeCommand(Command::InversionOff);
	writeCommand(Command::DisplayOn);

	setRotation(rotation_);

	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::reset(bool hardReset /* = false */)
{
	if (hardReset) {
		Reset::set();
		modm::delay_ms(5);
		Reset::reset();
		modm::delay_ms(5);
		Reset::set();
		modm::delay_ms(5);
	} else {
		this->acquireMaster();
		Cs::reset();
		writeCommand(Command::SwReset);
		modm::delay_ms(5);
		if (this->releaseMaster())
			Cs::set();
	}
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
uint16_t
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::getIcModel()
{
	uint8_t buffer[4] { 0 };

	this->acquireMaster();
	Cs::reset();

	readData(Command::ReadId4, buffer, 4);

	if (this->releaseMaster())
		Cs::set();

	return (buffer[2] << 8) | buffer[3];
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::setRotation(Rotation rotation)
{
	MemoryAccessCtrl_t madCtrl { MemoryAccessCtrl::BGR };

	switch (rotation) {
	case Rotation::Rotate90:
		madCtrl |= MemoryAccessCtrl::MV | MemoryAccessCtrl::MX;
		break;
	case Rotation::Rotate180:
		madCtrl |= MemoryAccessCtrl::MX | MemoryAccessCtrl::MY;
		break;
	case Rotation::Rotate270:
		madCtrl |= MemoryAccessCtrl::MV | MemoryAccessCtrl::MY;
		break;
	default:
//		madCtrl |= MemoryAccessCtrl::ML;
		break;
	}

	rotation_ = rotation;

	this->acquireMaster();
	Cs::reset();
	writeCommandValue8(Command::MemoryAccessCtrl, madCtrl.value);
	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::turnOn()
{
	this->acquireMaster();
	Cs::reset();
	writeCommand(Command::DisplayOn);
	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::turnOff()
{
	this->acquireMaster();
	Cs::reset();
	writeCommand(Command::DisplayOff);
	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::setIdle(bool enable)
{
	this->acquireMaster();
	Cs::reset();
	writeCommand(enable ? Command::IdleModeOn : Command::IdleModeOff);
	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::enableSleep(bool enable)
{
	this->acquireMaster();
	Cs::reset();
	writeCommand(enable ? Command::EnterSleep : Command::LeaveSleep);
	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::setBrightness(uint8_t level)
{
	this->acquireMaster();
	Cs::reset();
	writeCommand(Command::WriteBrightness, &level, 1);
	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::setInvert(bool invert)
{
	this->acquireMaster();
	Cs::reset();
	writeCommand(invert ? Command::InversionOn : Command::InversionOff);
	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::clear()
{
	auto const saveForegroundColor { foregroundColor };
	foregroundColor = backgroundColor;
	fillRectangle(glcd::Point(0, 0), Height, Width);
	foregroundColor = saveForegroundColor;
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::drawHorizontalLine(glcd::Point start,
		uint16_t length)
{
	auto const fgColor { foregroundColor.getValue() };
	auto minLength { std::min(std::size_t(length), BufferSize) };
	uint16_t *buffer16 { reinterpret_cast<uint16_t *>(buffer) };
	for (std::size_t i = 0; i < minLength; ++i)
		buffer16[i] = modm::toBigEndian(fgColor);

	this->acquireMaster();
	Cs::reset();

	setClipping(start.getX(), start.getY(), length, 1);
	while (length > BufferSize) {
		writeData(buffer, BufferSize * 2);
		length -= BufferSize;
	}
	writeData(buffer, length * 2);

	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::drawVerticalLine(glcd::Point start,
		uint16_t length)
{
	auto const fgColor { foregroundColor.getValue() };
	auto minLength { std::min(std::size_t(length), BufferSize) };
	uint16_t *buffer16 { reinterpret_cast<uint16_t *>(buffer) };
	for (std::size_t i = 0; i < minLength; ++i)
		buffer16[i] = modm::toBigEndian(fgColor);

	this->acquireMaster();
	Cs::reset();

	setClipping(start.getX(), start.getY(), 1, length);
	while (length > BufferSize) {
		writeData(buffer, BufferSize * 2);
		length -= BufferSize;
	}
	writeData(buffer, length * 2);

	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::fillRectangle(glcd::Point upperLeft,
		uint16_t width, uint16_t height)
{
	auto const x { upperLeft.getX() };
	auto const y { upperLeft.getY() };
	std::size_t pixelCount { std::size_t(width) * std::size_t(height) };

	auto const fgColor { foregroundColor.getValue() };
	auto minLength { std::min(std::size_t(pixelCount), BufferSize) };
	uint16_t *buffer16 { reinterpret_cast<uint16_t *>(buffer) };
	for (std::size_t i = 0; i < minLength; ++i)
		buffer16[i] = modm::toBigEndian(fgColor);

	this->acquireMaster();
	Cs::reset();

	setClipping(x, y, width, height);
	while (pixelCount > BufferSize) {
		writeData(buffer, BufferSize * 2);
		pixelCount -= BufferSize;
	}
	if (pixelCount)
		writeData(buffer, pixelCount * 2);

	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::fillCircle(glcd::Point center,
		uint16_t radius)
{
	auto const fgColor { foregroundColor.getValue() };
	uint8_t const setColor[] { uint8_t((fgColor >> 8) & 0xff),
			uint8_t(fgColor & 0xff) };

	int16_t f = 1 - radius;
	int16_t ddF_x = 0;
	int16_t ddF_y = -2 * radius;
	uint16_t x = 0;
	uint16_t y = radius;

	this->acquireMaster();
	Cs::reset();

	setClipping(center.getX() - radius, center.getY(), 2 * radius, 1);
	for (std::size_t i = 0; i < 2 * radius; ++i)
		writeData(setColor, 2);

	while(x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;

		setClipping(center.getX() - x, center.getY() - y, 2 * x, 1);
		for (std::size_t i = 0; i < 2 * x; ++i)
			writeData(setColor, 2);
		setClipping(center.getX() - y, center.getY() - x, 2 * y, 1);
		for (std::size_t i = 0; i < 2 * y; ++i)
			writeData(setColor, 2);
		setClipping(center.getX() - x, center.getY() + y, 2 * x, 1);
		for (std::size_t i = 0; i < 2 * x; ++i)
			writeData(setColor, 2);
		setClipping(center.getX() - y, center.getY() + x, 2 * y, 1);
		for (std::size_t i = 0; i < 2 * y; ++i)
			writeData(setColor, 2);
	}

	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::drawImageRaw(glcd::Point upperLeft,
		uint16_t width, uint16_t height,
		modm::accessor::Flash<uint8_t> data)
{
	auto const fgColor { foregroundColor.getValue() };
	auto const bgColor { backgroundColor.getValue() };
	uint8_t const setColor[] { uint8_t((fgColor >> 8) & 0xff),
			uint8_t(fgColor & 0xff) };
	uint8_t const clearColor[] { uint8_t((bgColor >> 8) & 0xff),
			uint8_t(bgColor & 0xff) };

	this->acquireMaster();
	Cs::reset();

	setClipping(upperLeft.getX(), upperLeft.getY(), width, height);

	uint8_t bit = 0x01;
	for (uint16_t r = 0; r < height; ++r) {
		for (uint16_t w = 0; w < width; ++w) {
			uint8_t byte = data[(r / 8) * width + w];
			if (byte & bit)
				writeData(setColor, 2);
			else
				writeData(clearColor, 2);
		}
		// TODO: optimize, use ROL (rotate left)
		bit <<= 1;
		if (bit == 0)
			bit = 0x01;
	}

	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::setScrollArea(uint16_t topFixedRows,
		uint16_t bottomFixedRows, uint16_t firstRow)
{
	this->acquireMaster();
	Cs::reset();

	uint8_t arg[] {
			uint8_t((topFixedRows >> 8) & 0xff), uint8_t(topFixedRows & 0xff),
			uint8_t((firstRow >> 8) & 0xff), uint8_t(firstRow & 0xff),
			uint8_t((bottomFixedRows >> 8) & 0xff), uint8_t(bottomFixedRows & 0xff)
	};
	writeCommand(Command::VerticalScrollDefinition, arg, 6);

	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::scrollTo(uint16_t row)
{
	this->acquireMaster();
	Cs::reset();

	uint8_t arg[] { uint8_t((row >> 8) & 0xff), uint8_t(row & 0xff) };
	writeCommand(Command::VerticalScrollStartAddr, arg, 2);

	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::setColoredPixel(int16_t x, int16_t y,
		glcd::Color const &color)
{
	auto const pixelColor { color.getValue() };
	uint8_t const setColor[] { uint8_t((pixelColor >> 8) & 0xff),
			uint8_t(pixelColor & 0xff) };

	this->acquireMaster();
	Cs::reset();

	setClipping(x, y, 1, 1);
	writeData(setColor, 2);

	if (this->releaseMaster())
		Cs::set();
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::setClipping(uint16_t x, uint16_t y,
		uint16_t width, uint16_t height)
{
	uint8_t buffer[4];

	buffer[0] = uint8_t((x >> 8) & 0xff);
	buffer[1] = uint8_t(x & 0xff);
	buffer[2] = uint8_t(((x + width - 1) >> 8) & 0xff);
	buffer[3] = uint8_t((x + width - 1) & 0xff);
	writeCommand(Command::ColumnAddressSet, buffer, 4);

	buffer[0] = uint8_t((y >> 8) & 0xff);
	buffer[1] = uint8_t(y & 0xff);
	buffer[2] = uint8_t(((y + height - 1) >> 8) & 0xff);
	buffer[3] = uint8_t((y + height - 1) & 0xff);
	writeCommand(Command::PageAddressSet, buffer, 4);
	writeCommand(Command::MemoryWrite);
}

template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize>
void
Ili9341<SPI, Cs, Dc, Reset, Backlight, BufferSize>::drawBitmap(glcd::Point upperLeft,
		uint16_t width, uint16_t height, modm::accessor::Flash<uint8_t> data)
{
	this->acquireMaster();
	Cs::reset();

	setClipping(upperLeft.getX(), upperLeft.getY(), width, height);
	for (int i = 0; i < width * height; ++i) {
		buffer[0] = data[i*2+1];
		buffer[1] = data[i*2];
		writeData(buffer, 2);

	}
//	writeData(data.getPointer(), width * height * 2);

	if (this->releaseMaster())
		Cs::set();
}

}
