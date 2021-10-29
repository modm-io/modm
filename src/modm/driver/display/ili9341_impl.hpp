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

#pragma once
#include "ili9341.hpp"

#include <limits>

using namespace modm::shape;
using namespace modm::color;
using namespace modm::graphic;

#define MODM_INIT_BUFFER(...) {\
	constexpr uint8_t data[]__VA_ARGS__;\
	std::copy(std::begin(data), std::end(data), std::begin(array_u8));\
	}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::initialize()
{
	RF_BEGIN();
	RF_CALL(reset());
	RF_CALL(set(Toggle::Enable, false));

	MODM_INIT_BUFFER({0x39, 0x2c, 0x00, 0x34, 0x02})
	RF_CALL(this->writeCommand(Command::PowerCtrlA, array_u8.begin(), array_u8.begin() + 5));

 	MODM_INIT_BUFFER({0x00, 0xc1, 0x30})
	RF_CALL(this->writeCommand(Command::PowerCtrlB, array_u8.begin(), array_u8.begin() + 3));

	MODM_INIT_BUFFER({0x85, 0x10, 0x7a})
	RF_CALL(this->writeCommand(Command::TimingCtrlA, array_u8.begin(), array_u8.begin() + 3));

	MODM_INIT_BUFFER({0x00, 0x00})
	RF_CALL(this->writeCommand(Command::TimingCtrlB, array_u8.begin(), array_u8.begin() + 2));

	MODM_INIT_BUFFER({0x64, 0x03, 0x12, 0x81})
	RF_CALL(this->writeCommand(Command::PowerOnSequenceCtrl, array_u8.begin(), array_u8.begin() + 4));

	RF_CALL(this->writeCommand(Command::PowerCtrl1, uint8_t(0x1b)));
	RF_CALL(this->writeCommand(Command::PowerCtrl2, uint8_t(0x12)));

	MODM_INIT_BUFFER({0x08, 0x26})  // 0x3e, 0x28 };
	RF_CALL(this->writeCommand(Command::VComCtrl1, array_u8.begin(), array_u8.begin() + 2));

	RF_CALL(this->writeCommand(Command::VComCtrl2, uint8_t(0xb7)));
	// constexpr uint8_t pumpRatioCtrl[] { 0x20 };
	RF_CALL(this->writeCommand(Command::PixelFormatSet, uint8_t(0x55)));
	
	MODM_INIT_BUFFER({0x00, 0x1a})
	RF_CALL(this->writeCommand(Command::FrameCtrlNormalMode, array_u8.begin(), array_u8.begin() + 2));

	MODM_INIT_BUFFER({0x0a, 0xa2, 0x27, 0x00})  //{ 0x08, 0x82, 0x27, 0x00 };
	RF_CALL(this->writeCommand(Command::DisplayFunctionCtrl, array_u8.begin(), array_u8.begin() + 4));

	RF_CALL(this->writeCommand(Command::Enable3Gamma, uint8_t(0x00)));
	RF_CALL(this->writeCommand(Command::GammaSet, uint8_t(0x01)));

	// MODM_INIT_BUFFER({ 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0xe4, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00});
	MODM_INIT_BUFFER({0x0f, 0x1d, 0x1a, 0x0a, 0x0d, 0x07, 0x49, 0x66, 0x3b, 0x07, 0x11, 0x01, 0x09, 0x05, 0x04})
	RF_CALL(this->writeCommand(Command::PositiveGammaCorrection, array_u8.begin(), array_u8.begin() + 15));

	// MODM_INIT_BUFFER({ 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f});
	MODM_INIT_BUFFER({0x00, 0x18, 0x1d, 0x02, 0x0f, 0x04, 0x36, 0x13, 0x4c, 0x07, 0x13, 0x0f, 0x2e, 0x2f, 0x05})
	RF_CALL(this->writeCommand(Command::NegativeGammaCorrection, array_u8.begin(), array_u8.begin() + 15));

	RF_CALL(set(Toggle::SleepDisable, true));
	timeout.restart(120ms);
	RF_WAIT_UNTIL(timeout.isExpired());
	RF_CALL(set(Toggle::Inversion, false));
	RF_CALL(setOrientation(orientation));
	RF_CALL(set(Toggle::Enable, true));

	RF_END();
}

#undef MODM_INIT_BUFFER

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::reset(bool hardReset /* = false */)
{
	RF_BEGIN();

	if (hardReset)
	{
		Reset::set();
		timeout.restart(5ms);
		RF_WAIT_UNTIL(timeout.isExpired());
		Reset::reset();
		timeout.restart();
		RF_WAIT_UNTIL(timeout.isExpired());
		Reset::set();
		timeout.restart();
		RF_WAIT_UNTIL(timeout.isExpired());
	} else
	{
		RF_CALL(this->writeCommand(Command::SwReset));
		timeout.restart(5ms);
		RF_WAIT_UNTIL(timeout.isExpired());
	}
	RF_END();
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<uint16_t>
modm::Ili9341<Transport, Reset, BC>::getIcModel()
{
	RF_BEGIN();
	// TODO Need function testing
	RF_CALL(this->readData(ReadCommand::Id, array_u16, 2));
	RF_END_RETURN(array_u16[1]);
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::set(Toggle toggle, bool state)
{
	RF_BEGIN();
	RF_CALL(this->writeCommand(Command(uint8_t(toggle) | uint8_t(state))));
	RF_END();
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::set(ReadWrite reg, uint8_t value)
{
	RF_BEGIN();
	RF_CALL(this->writeCommand(Command(reg), value));
	RF_END();
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<uint8_t>
modm::Ili9341<Transport, Reset, BC>::get(ReadWrite reg)
{
	RF_BEGIN();
	RF_END_RETURN_CALL(this->readData(Command(uint8_t(reg) | uint8_t(Bit0))));
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::setOrientation(
	Orientation orientation)
{
	RF_BEGIN();
	this->orientation = orientation;

	using MemoryAccessCtrl = ili9341_register::MemoryAccessCtrl;
	using OrientationFlags = graphic::OrientationFlags;

	madCtrl = MemoryAccessCtrl::PIXEL_DIR;

	// FIXME adjust for flipped x-y in Buffer
	if (orientation & OrientationFlags::Portrait)
		madCtrl |= MemoryAccessCtrl::MEMORY_MODE;

	if (orientation & OrientationFlags::TopDown)
		madCtrl |= MemoryAccessCtrl::MEMORY_DIR_Y;

	if (bool(orientation & OrientationFlags::Portrait) == bool(orientation & OrientationFlags::TopDown))
		madCtrl |= MemoryAccessCtrl::MEMORY_DIR_X;

	RF_CALL(this->writeCommand(Command::MemoryAccessCtrl, madCtrl.value));

	RF_END();
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::setScrollArea(uint16_t topFixedRows, uint16_t firstRow, uint16_t bottomFixedRows)
{
	RF_BEGIN();

	array_u16[0] = topFixedRows;
	array_u16[1] = firstRow;
	array_u16[2] = bottomFixedRows;

	RF_CALL(this->writeCommand(Command::VerticalScrollDefinition, array_u16.begin(), array_u16.begin() + 3));
	RF_END();
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::scrollTo(uint16_t row)
{
	RF_BEGIN();
	RF_CALL(this->writeCommand(Command::VerticalScrollStartAddr, row));
	RF_END();
}

// ---------------------------------------------------------------------------

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::updateClipping()
{
	RF_BEGIN();

	p.array_u16[0] = this->clipping.topLeft.y;
	p.array_u16[1] = this->clipping.bottomRight.y - 1;
	RF_CALL(this->writeCommand(Command::ColumnAddressSet, p.array_u16.begin(), p.array_u16.begin() + 2));

	p.array_u16[0] = this->clipping.topLeft.x;
	p.array_u16[1] = this->clipping.bottomRight.x - 1;
	RF_CALL(this->writeCommand(Command::PageAddressSet, p.array_u16.begin(), p.array_u16.begin() + 2));

	RF_CALL(this->writeCommand(Command::MemoryWrite));
	p.pixels = uint64_t(this->clipping.getWidth()) * this->clipping.getHeight();

	RF_END();
}

template<class Transport, class Reset, size_t BC>
template<Color CO, template<typename> class Accessor>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::writeImage(ImageAccessor<CO, Accessor> accessor) {
	// Found no cast for a share the memory between all of the writeImage()-methods.
	// This waste of RAM will be history once Resumable Functions become history.
	static ImageAccessor<CO, Accessor> a;
	
	RF_BEGIN();

	a = accessor;
	
	this->clipping = this->getIntersection(a.getSection());
	RF_CALL(updateClipping());

	a.incrementRow_preparePixel();
	p.scanner = this->clipping.topLeft;

 	while (p.pixels)
	{
		// Fill array_color
		for(p.i = 0; p.i < std::min<uint32_t>(p.pixels, BC); p.i++) {
			if constexpr (ColorPalletized<CO>) {
				// Apply colormap
				p.array_color[p.i] = colormap[ a.nextPixel().getValue() ];
			} else {
				// convert color
				p.array_color[p.i] = a.nextPixel();
			}

			if (++p.scanner.y == this->clipping.bottomRight.y) {
				p.scanner.x++;
				p.scanner.y = this->clipping.topLeft.y;
				a.incrementRow_preparePixel();
			}
		}
		
		// Transfer array_color
		RF_CALL(this->writePixels(p.array_color.begin(), p.array_color.begin() + p.i));
		p.pixels -= p.i;
	}

	RF_END();
}

template<class Transport, class Reset, size_t BC>
template<template<typename> class Accessor>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::writeImage(ImageAccessor<ColorType, Accessor> accessor)
{
	// Found no cast for a share the memory between all of the writeImage()-methods.
	// This waste of RAM will be history once Resumable Functions become history.
	static ImageAccessor<ColorType, Accessor> a;

	RF_BEGIN();
	
	a = accessor;

	this->clipping = this->getIntersection(a.getSection());
	RF_CALL(updateClipping());

	// Buffer is clipped?
	// REIMPLEMENT clipped array_color detection
	if (true /* placement.y < 0 or placement.y + a.getSize().y >= this->getHeight() */)
	{
 		// Transfer array_color row by row
		p.pixels_bulk = this->clipping.getHeight();
		while(p.pixels >= p.pixels_bulk) {
			RF_CALL(this->writeData(a.getPointer() + 1, p.pixels_bulk));
			a.incrementRow();
			p.pixels -= p.pixels_bulk;
		}
	} else
	{
		// Transfer array_color in one turn
		// TODO Why +1 ???
		RF_CALL(this->writePixels(a.getPointer() + 1, a.getPointer() + p.pixels + 1));
	}
	RF_END();
}

template<class Transport, class Reset, size_t BC>
template<ColorPattern P>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::writePattern(Rectangle rectangle, P pattern) {
	RF_BEGIN();

	this->clipping = this->getIntersection(rectangle);
	RF_CALL(updateClipping());

    p.scanner = this->clipping.topLeft;

 	while (p.pixels)
	{
		// Fill array_color
		for(p.i = 0; p.i < std::min<uint32_t>(p.pixels, BC); p.i++) {
			p.array_color[p.i] = pattern(p.scanner);			
			if (++p.scanner.y == this->clipping.bottomRight.y) {
				p.scanner.x++;
				p.scanner.y = this->clipping.topLeft.y;
			}
		}
		// Transfer array_color
		RF_CALL(this->writePixels(p.array_color.begin(), p.array_color.begin() + p.i));
		p.pixels-= p.i;
	}

	RF_END();
}

// Fundamental drawing of shapes
template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::drawBlind(const Point& point)
{
	RF_BEGIN();
	// This snipers single pixels and is quite heavy because anytime
	// the clipping and thus the DataCommand-line is modulated.
	// 
	// OPTIMIZE Modulate DC with hardware: Timer connected with a DMA.
	// 1. An array contains periods for modulation of the DC-Line: [1, 2, 1, 2, 1] => 1cmd, 2data, 1cmd, 1cmd, 2data
	// 2. A DMA sequences these periods into a Timers compare-register
	// 3. The timers clock is connected to Spi-transfer-complete.
	// 4. Compare match toggles the DC-Line and triggers the DMA
	// OPTIMIZE Now it's possible to array_color remote drawing-instructions and do efficient DMA transfers
	// without CPU intervention.
	p.array_u16[0] = point.y;
	p.array_u16[1] = p.array_u16[0];
	RF_CALL(this->writeCommand(Command::ColumnAddressSet, p.array_u16, 2));
	
	p.array_u16[0] = point.x;
	p.array_u16[1] = p.array_u16[0];
	RF_CALL(this->writeCommand(Command::PageAddressSet, p.array_u16, 2));

	RF_CALL(this->writeCommand(Command::MemoryWrite));
	
	RF_END_RETURN_CALL(this->writeData(this->color.getValue()));
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::drawBlind(const HLine& hline)
{
	RF_BEGIN();
	RF_END_RETURN_CALL(drawBlind(Section(hline.start, {hline.end_x, hline.start.y + 1})));
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::drawBlind(const VLine& vline)
{
	RF_BEGIN();
	RF_END_RETURN_CALL(drawBlind(Section(vline.start, {vline.start.x + 1, vline.end_y})));
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::drawBlind(const Section& section)
{
	RF_BEGIN();
	
	this->clipping = section;
	RF_CALL(updateClipping());

	while(p.pixels) {
		// DMAs DataLenght register is limited to uint16_t, thus can be max 2^16-1
		p.pixels_bulk = std::min<uint32_t>(p.pixels, std::numeric_limits<uint16_t>::max());
		RF_CALL(this->writePixels(this->color, p.pixels_bulk));
		p.pixels -= p.pixels_bulk;
	}

	RF_END();
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::clear(const std::size_t color_idx)
{
	RF_BEGIN();

	p.temp_color = this->color;
	this->color = colormap[color_idx];

	RF_CALL(this->drawBlind(this->asSection()));
	
	this->color = p.temp_color;

	RF_END();
}

template<class Transport, class Reset, size_t BC>
modm::ResumableResult<void>
modm::Ili9341<Transport, Reset, BC>::clear(const ColorType color)
{
	RF_BEGIN();

	p.temp_color = this->color;
	this->color = color;

	RF_CALL(this->drawBlind(this->asSection()));
	
	this->color = p.temp_color;

	RF_END();
}