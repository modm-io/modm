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
#include <modm/platform/gpio/base.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/ui/display.hpp>

#include "ili9341_interface_parallel.hpp"
#include "ili9341_interface_spi.hpp"

namespace modm
{

/// @ingroup modm_driver_ili9341
template<class Interface, class Reset, size_t BufferSize = 1024>
class Ili9341 : public Interface, public ColorGraphicDisplay
{
	static_assert(BufferSize >= 128, "Minimum Buffer is required");

	using Toggle = ili9341_register::Toggle;
	using ReadWrite = ili9341_register::ReadWrite;
	using Command = ili9341_register::Command;
	using ReadCommand = ili9341_register::ReadCommand;

public:
	template<typename... Args>
	Ili9341(Args &&...args) : Interface(std::forward<Args>(args)...)
	{
		Reset::setOutput(modm::Gpio::High);
	}

	~Ili9341(){};

	modm::ResumableResult<void>
	initialize();

	modm::ResumableResult<void>
	reset(bool hardReset = false);

	modm::ResumableResult<uint16_t>
	getIcModel();

	uint16_t
	getWidth() const final
	{ return (orientation & display::OrientationFlags::Portrait) ? 240 : 320; }

	uint16_t
	getHeight() const final
	{ return (orientation & display::OrientationFlags::Portrait) ? 320 : 240; }

	/* modm::ResumableResult<uint32_t>
	getStatus(); // 5 bytes, Datasheet P92

	modm::ResumableResult<uint16_t>
	getPowerMode(); // 2 bytes, Datasheet P94

	modm::ResumableResult<uint16_t>
	getMadCtl(); // 2 bytes, Datasheet P95

	modm::ResumableResult<uint16_t>
	getPixelFormat(); // 2 bytes, Datasheet P96 */

	modm::ResumableResult<void>
	set(Toggle toggle, bool state);

	modm::ResumableResult<void>
	set(ReadWrite reg, uint8_t value);

	modm::ResumableResult<uint8_t>
	get(ReadWrite reg);

	// After change of Orientation, wait 1ms before sending new pixels
	modm::ResumableResult<void>
	setOrientation(display::Orientation orientation);

	modm::ResumableResult<void>
	setScrollArea(uint16_t topFixedRows, uint16_t bottomFixedRows, uint16_t firstRow);

	modm::ResumableResult<void>
	scrollTo(uint16_t row);

	// Some rectangular drawing functions, send directly to the display
	// Using DMA (f.e. SpiMaster_Dma) is highly recommended!
	void
	setPixel(display::Point pos);

	void
	clearPixel(display::Point pos);

	void
	setPixel(display::Point pos, bool value);

	void
	setPixel(display::Point pos, modm::color::Rgb565<true> color);

	color::Rgb565<true>
	getPixel(display::Point) const final {
		// TODO implement
		return modm::color::html::Black;
	}

	void
	drawHorizontalLine(display::Point origin, uint16_t length);

	void
	drawVerticalLine(display::Point origin, uint16_t length);

	void
	fillRectangle(display::Point origin, uint16_t width, uint16_t height);

	modm::ResumableResult<void>
	clear(color::Rgb565<true> color = color::html::Black);

	void
	update() final {};

	// TODO Reimplement
	// Send monochrome image stored in flash directly to the screen
	void
	drawImageRaw(display::Point origin, uint16_t width, uint16_t height,
		modm::accessor::Flash<uint8_t> data);

	// TODO Reimplement
	// Send color-image stored in flash directly to the screen
	void
	drawBitmap(display::Point origin, uint16_t width, uint16_t height,
		modm::accessor::Flash<uint8_t> data);

	// TODO Reimplement
	// Send a colored graphic-buffer directly to the screen
	void
	drawRaw(display::Point origin,
	uint16_t width, uint16_t height, color::Rgb565<false>* data);

protected:
	bool
	xOnScreen(int16_t x) const
	{ return x >= 0 and x < int16_t(getWidth()); }

	bool
	yOnScreen(int16_t y) const
	{ return y >= 0 and y < int16_t(getHeight()); }

	bool
	pointOnScreen(display::Point position) const
	{ return xOnScreen(position.x) and yOnScreen(position.y); }

	// setClipping for a single pixel
	modm::ResumableResult<void>
	setClipping(display::Point pos);

	// setClipping for an area
	modm::ResumableResult<void>
	setClipping(display::Point origin, display::Point bottomRight);

	// fill an area with buffer
	modm::ResumableResult<void>
	fill(display::Point origin, uint16_t width, uint16_t height);

	// "Local" variables, required for resumable functions

	union {
		// Buffers for Commands & Configuration
		uint8_t buffer8[15];
		uint16_t buffer16[3];
		ili9341_register::MemoryAccessCtrl_t madCtrl;

		// Parallel use in resumable function
		// No overlap permitted!
		struct {
			modm::color::Rgb565<true> buffer[BufferSize];
			display::Intersection intersection;
			uint16_t clipping_buffer[2];
			uint32_t write_pixels;
		} p; // p for parallel
	};
};

}  // namespace modm

#include "ili9341_impl.hpp"

#endif  //  MODM_ILI9341_HPP
