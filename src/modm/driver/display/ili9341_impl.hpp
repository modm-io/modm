/*
 * Copyright (c) 2019, Mike Wolfram
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_ILI9341_HPP
#error "Don't include this file directly, use 'ili9341.hpp' instead!"
#endif

namespace modm
{

#define MODM_INIT_BUFFER(...) {\
	constexpr uint8_t data[]__VA_ARGS__;\
	std::copy(std::begin(data), std::end(data), std::begin(buffer8));\
	}

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<void>
Ili9341<Interface, Reset, BufferSize>::initialize()
{
	RF_BEGIN();
	RF_CALL(reset());
	RF_CALL(set(Toggle::Enable, false));

	MODM_INIT_BUFFER({0x39, 0x2c, 0x00, 0x34, 0x02})
	RF_CALL(this->writeCommand(Command::PowerCtrlA, buffer8, 5));

 	MODM_INIT_BUFFER({0x00, 0xc1, 0x30})
	RF_CALL(this->writeCommand(Command::PowerCtrlB, buffer8, 3));

	MODM_INIT_BUFFER({0x85, 0x10, 0x7a})
	RF_CALL(this->writeCommand(Command::TimingCtrlA, buffer8, 3));

	MODM_INIT_BUFFER({0x00, 0x00})
	RF_CALL(this->writeCommand(Command::TimingCtrlB, buffer8, 2));

	MODM_INIT_BUFFER({0x64, 0x03, 0x12, 0x81})
	RF_CALL(this->writeCommand(Command::PowerOnSequenceCtrl, buffer8, 4));

	RF_CALL(this->writeCommand(Command::PowerCtrl1, 0x1b));
	RF_CALL(this->writeCommand(Command::PowerCtrl2, 0x12));

	MODM_INIT_BUFFER({0x08, 0x26})  // 0x3e, 0x28 };
	RF_CALL(this->writeCommand(Command::VComCtrl1, buffer8, 2));

	RF_CALL(this->writeCommand(Command::VComCtrl2, 0xb7));
	// constexpr uint8_t pumpRatioCtrl[] { 0x20 };
	RF_CALL(this->writeCommand(Command::PixelFormatSet, 0x55));

	MODM_INIT_BUFFER({0x00, 0x1a})
	RF_CALL(this->writeCommand(Command::FrameCtrlNormalMode, buffer8, 2));

	MODM_INIT_BUFFER({0x0a, 0xa2, 0x27, 0x00})  //{ 0x08, 0x82, 0x27, 0x00 };
	RF_CALL(this->writeCommand(Command::DisplayFunctionCtrl, buffer8, 4));

	RF_CALL(this->writeCommand(Command::Enable3Gamma, 0x00));
	RF_CALL(this->writeCommand(Command::GammaSet, 0x01));

	// MODM_INIT_BUFFER({ 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0xe4, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00});
	MODM_INIT_BUFFER({0x0f, 0x1d, 0x1a, 0x0a, 0x0d, 0x07, 0x49, 0x66, 0x3b, 0x07, 0x11, 0x01, 0x09, 0x05, 0x04})
	RF_CALL(this->writeCommand(Command::PositiveGammaCorrection, buffer8, 15));

	// MODM_INIT_BUFFER({ 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f});
	MODM_INIT_BUFFER({0x00, 0x18, 0x1d, 0x02, 0x0f, 0x04, 0x36, 0x13, 0x4c, 0x07, 0x13, 0x0f, 0x2e, 0x2f, 0x05})
	RF_CALL(this->writeCommand(Command::NegativeGammaCorrection, buffer8, 15));

	RF_CALL(set(Toggle::SleepDisable, true));
	modm::delay(120ms);
	RF_CALL(set(Toggle::Inversion, false));
	RF_CALL(setOrientation(orientation));
	RF_CALL(set(Toggle::Enable, true));

	RF_END();
}

#undef MODM_INIT_BUFFER

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<void>
Ili9341<Interface, Reset, BufferSize>::reset(bool hardReset /* = false */)
{
	RF_BEGIN();

	// TODO use modm::ShortTimeout;
	if (hardReset)
	{
		Reset::set();
		modm::delay(5ms);
		Reset::reset();
		modm::delay(5ms);
		Reset::set();
		modm::delay(5ms);
	} else
	{
		RF_CALL(this->writeCommand(Command::SwReset));
		modm::delay(5ms);
	}
	RF_END();
}

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<uint16_t>
Ili9341<Interface, Reset, BufferSize>::getIcModel()
{
	RF_BEGIN();
	RF_CALL(this->readData(ReadCommand::Id, buffer8, 4));
	RF_END_RETURN(buffer8[2] << 8 | buffer8[3]);
}

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<void>
Ili9341<Interface, Reset, BufferSize>::set(Toggle toggle, bool state)
{
	RF_BEGIN();
	RF_CALL(this->writeCommand(Command(uint8_t(toggle) | uint8_t(state))));
	RF_END();
}

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<void>
Ili9341<Interface, Reset, BufferSize>::set(ReadWrite reg, uint8_t value)
{
	RF_BEGIN();
	RF_CALL(this->writeCommand(Command(reg), value));
	RF_END();
}

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<uint8_t>
Ili9341<Interface, Reset, BufferSize>::get(ReadWrite reg)
{
	RF_BEGIN();
	RF_END_RETURN_CALL(this->readData(Command(uint8_t(reg) | uint8_t(Bit0))));
}

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<void>
Ili9341<Interface, Reset, BufferSize>::setOrientation(
	display::Orientation orientation)
{
	RF_BEGIN();
	this->orientation = orientation;

	using MemoryAccessCtrl = ili9341_register::MemoryAccessCtrl;
	using OrientationFlags = modm::display::OrientationFlags;

	madCtrl = MemoryAccessCtrl::PIXEL_DIR;

	if (orientation & OrientationFlags::Portrait)
		madCtrl |= MemoryAccessCtrl::MEMORY_MODE;

	if (orientation & OrientationFlags::Rotate180)
		madCtrl |= MemoryAccessCtrl::MEMORY_DIR_Y;

	if (bool(orientation & OrientationFlags::Portrait) == bool(orientation & OrientationFlags::Rotate180))
		madCtrl |= MemoryAccessCtrl::MEMORY_DIR_X;

	RF_CALL(this->writeCommand(Command::MemoryAccessCtrl, madCtrl.value));

	RF_END();
}

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<void>
Ili9341<Interface, Reset, BufferSize>::setScrollArea(uint16_t topFixedRows,
																	uint16_t bottomFixedRows,
																	uint16_t firstRow)
{
	RF_BEGIN();

	buffer16[0] = modm::fromBigEndian(topFixedRows);
	buffer16[1] = modm::fromBigEndian(firstRow);
	buffer16[2] = modm::fromBigEndian(bottomFixedRows);

	RF_CALL(this->writeCommand(Command::VerticalScrollDefinition, buffer8, 6));
	RF_END();
}

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<void>
Ili9341<Interface, Reset, BufferSize>::scrollTo(uint16_t row)
{
	RF_BEGIN();

	buffer16[0] = modm::fromBigEndian(row);

	RF_CALL(this->writeCommand(Command::VerticalScrollStartAddr, buffer8, 2));
	RF_END();
}

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<void>
Ili9341<Interface, Reset, BufferSize>::setClipping(display::Point pos)
{
	RF_BEGIN();

	p.clipping_buffer[0] = modm::fromBigEndian(pos.x);
	p.clipping_buffer[1] = p.clipping_buffer[0];
	RF_CALL(this->writeCommand(Command::PageAddressSet, (uint8_t *)(p.clipping_buffer), 4));

	p.clipping_buffer[0] = modm::fromBigEndian(pos.y);
	p.clipping_buffer[1] = p.clipping_buffer[0];
	RF_CALL(this->writeCommand(Command::ColumnAddressSet, (uint8_t *)(p.clipping_buffer), 4));

	RF_CALL(this->writeCommand(Command::MemoryWrite));

	RF_END();
}

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<void>
Ili9341<Interface, Reset, BufferSize>::setClipping(display::Point origin,
																  display::Point bottomRight)
{
	RF_BEGIN();

	p.clipping_buffer[0] = modm::fromBigEndian(origin.x);
	p.clipping_buffer[1] = modm::fromBigEndian(int16_t(bottomRight.x - 1));
	RF_CALL(this->writeCommand(Command::PageAddressSet, (uint8_t *)(p.clipping_buffer), 4));

	p.clipping_buffer[0] = modm::fromBigEndian(origin.y);
	p.clipping_buffer[1] = modm::fromBigEndian(int16_t(bottomRight.y - 1));
	RF_CALL(this->writeCommand(Command::ColumnAddressSet, (uint8_t *)(p.clipping_buffer), 4));

	RF_CALL(this->writeCommand(Command::MemoryWrite));

	RF_END();
}

template<class Interface, class Reset, size_t BufferSize>
void
Ili9341<Interface, Reset, BufferSize>::setPixel(display::Point pos)
{
	if (pointOnScreen(pos))
	{
		RF_CALL_BLOCKING(setClipping(pos));
		RF_CALL_BLOCKING(this->writeData(color));
	}
}

template<class Interface, class Reset, size_t BufferSize>
void
Ili9341<Interface, Reset, BufferSize>::setPixel(display::Point pos, modm::color::Rgb565<true> color)
{
	if (pointOnScreen(pos))
	{
		RF_CALL_BLOCKING(setClipping(pos));
		RF_CALL_BLOCKING(this->writeData(color));
	}
}

template<class Interface, class Reset, size_t BufferSize>
void
Ili9341<Interface, Reset, BufferSize>::drawHorizontalLine(
	display::Point start, uint16_t length)
{
	fillRectangle(start, length, 1);
}

template<class Interface, class Reset, size_t BufferSize>
void
Ili9341<Interface, Reset, BufferSize>::drawVerticalLine(
	display::Point start, uint16_t length)
{
	if(xOnScreen(start.x))
		fillRectangle(start, 1, length);
}

template<class Interface, class Reset, size_t BufferSize>
void
Ili9341<Interface, Reset, BufferSize>::fillRectangle(
	display::Point origin, uint16_t width, uint16_t height)
{
	std::fill(&p.buffer[0], &p.buffer[BufferSize], color);
	RF_CALL_BLOCKING(fill(origin, width, height));
}

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<void>
Ili9341<Interface, Reset, BufferSize>::clear(color::Rgb565<true> color)
{
	RF_BEGIN();
	std::fill(&p.buffer[0], &p.buffer[BufferSize], color);
	RF_CALL(fill({0, 0}, this->getWidth(), this->getHeight()));
	RF_END();
}

// -------------------------------------------------------------

template<class Interface, class Reset, size_t BufferSize>
modm::ResumableResult<void>
Ili9341<Interface, Reset, BufferSize>::fill(
	display::Point origin, uint16_t width, uint16_t height)
{
	RF_BEGIN();

	p.intersection = display::Intersection(origin, width, height, this->getWidth(), this->getHeight());
	RF_CALL(setClipping(p.intersection.topLeft, p.intersection.bottomRight));

	p.write_pixels = p.intersection.getPixelCount();

	while (true)
	{
		if (p.write_pixels > BufferSize)
		{
			RF_CALL(this->writeData((uint8_t *)(p.buffer), 2 * BufferSize));
			p.write_pixels -= BufferSize;
		} else
		{
			RF_CALL(this->writeData((uint8_t *)(p.buffer), 2 * p.write_pixels));
			break;
		}
	}
	RF_END();
}

template <class Interface, class Reset, std::size_t BufferSize>
void
Ili9341<Interface, Reset, BufferSize>::drawImageRaw(display::Point origin,
		uint16_t width, uint16_t height, modm::accessor::Flash<uint8_t> data)
{
	(void)origin;
	(void)width;
	(void)height;
	(void)data;
/* 	uint8_t const setColor[] { uint8_t((color.color >> 8) & 0xff),
			uint8_t(color.color & 0xff) };
	uint8_t const clearColor[] { uint8_t((backgroundColor.color >> 8) & 0xff),
			uint8_t(backgroundColor.color & 0xff) };


	RF_CALL(setClipping(origin, origin + display::Point(width, height)));

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
	} */
}

template <class Interface, class Reset, size_t BufferSize>
void
Ili9341<Interface, Reset, BufferSize>::drawBitmap(display::Point origin,
		uint16_t width, uint16_t height, modm::accessor::Flash<uint8_t> data)
{
	RF_CALL_BLOCKING(setClipping(origin, origin + display::Point(width, height)));
	for (int i = 0; i < width * height; ++i) {
		p.buffer[0] = data[i*2+1];
		p.buffer[1] = data[i*2];
		this->writeData(p.buffer, 2);
	}
//	this->writeData(data.getPointer(), width * height * 2);
}

template<class Interface, class Reset, size_t BufferSize>
void
Ili9341<Interface, Reset, BufferSize>::drawRaw(display::Point origin,
		uint16_t width, uint16_t height, color::Rgb565<false>* data)
{
/* 	uint16_t* p.buffer = (uint16_t*)data;
	for(size_t i = 0; i < size_t(width*height); i++) {
		p.buffer[i] = modm::fromBigEndian(p.buffer[i]);
	}

	RF_CALL_BLOCKING(setClipping(origin, origin + display::Point(width, height)));
	RF_CALL_BLOCKING(this->writeData((uint8_t*)p.buffer, width * height * 2)); */
}

}  // namespace modm