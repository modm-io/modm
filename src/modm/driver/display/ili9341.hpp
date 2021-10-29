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
#define MODM_ILI9341_HPP

#include <concepts>

#include <modm/architecture/interface/delay.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/processing/resumable.hpp>

#include <modm/ui/graphic/display.hpp>
#include <modm/ui/graphic/buffer.hpp>
#include <modm/ui/color/rgb_stacked.hpp>
#include <modm/ui/shape/point.hpp>
#include <modm/ui/shape/line.hpp>
#include <modm/ui/shape/section.hpp>
#include <modm/ui/graphic/style.hpp>

#include "ili9341_interface_parallel.hpp"
#include "ili9341_interface_spi.hpp"

namespace modm
{

/// @ingroup modm_driver_ili9341

template<class Transport, class Reset, size_t BC = 512>
// requires std::derived_from<Painter, RemotePainter<R>>
class Ili9341 : public Transport, public graphic::Display<color::Rgb565, shape::Size(320, 240), true>
{
	// OPTIMIZE determine good contraints
	static_assert(BC >= 32, "Conversion Buffer < 64 pixels produces too much overhead.");
	static_assert(BC <= 2048, "Conversion Buffer > 2048 pixels doesn't make it any better.");

	using Toggle = ili9341_register::Toggle;
	using ReadWrite = ili9341_register::ReadWrite;
	using Command = ili9341_register::Command;
	using ReadCommand = ili9341_register::ReadCommand;

public:
	using ColorType = color::Rgb565;
	using BufferLandscape = graphic::Buffer<ColorType, ResolutionVector>;
	using BufferPortrait = graphic::Buffer<ColorType, ResolutionVector.swapped()>;

	template<typename... Args>
	Ili9341(Args &&...args)
		: Transport(std::forward<Args>(args)...)
	{ Reset::setOutput(modm::Gpio::High); }

	~Ili9341(){};

	ResumableResult<void>
	initialize();

	ResumableResult<void>
	reset(bool hardReset = false);

	ResumableResult<uint16_t>
	getIcModel();

	/* ResumableResult<uint32_t>
	getStatus(); // 5 bytes, Datasheet P92

	ResumableResult<uint16_t>
	getPowerMode(); // 2 bytes, Datasheet P94

	ResumableResult<uint16_t>
	getMadCtl(); // 2 bytes, Datasheet P95

	ResumableResult<uint16_t>
	getPixelFormat(); // 2 bytes, Datasheet P96 */

	ResumableResult<void>
	set(Toggle toggle, bool state);

	ResumableResult<void>
	set(ReadWrite reg, uint8_t value);

	ResumableResult<uint8_t>
	get(ReadWrite reg);

	ResumableResult<void>
	setOrientation(graphic::Orientation orientation);

	ResumableResult<void>
	setScrollArea(uint16_t topFixedRows, uint16_t firstRow, uint16_t bottomFixedRows);

	ResumableResult<void>
	scrollTo(uint16_t row);

	template<graphic::ColorPattern P>
	ResumableResult<void>
	writePattern(shape::Rectangle rectangle, P pattern);

	// Clear whole display with colormap index
	ResumableResult<void>
	clear(const std::size_t color_idx);

	// Clear whole display with color
	ResumableResult<void>
	clear(const ColorType color = 0);
private:
	// Static variables for resumable functions
	shape::Section section;
	shape::Point cursor;
	// ##################################################################

protected:
	ResumableResult<void>
	updateClipping();

	ResumableResult<void>
	setClipping(shape::Point point);

	/**
	 * Write Image with foreign Color
	 *
	 * @param accessor	ImageAccessor with underlying Flash or Ram Accessor
	 * @param placement	Placement for the image
	 */
 	template<Color CO, template<typename> class Accessor>
	ResumableResult<void>
	writeImage(graphic::ImageAccessor<CO, Accessor> accessor);

	/**
	 * Write Image with same Color
	 *
	 * @param accessor	ImageAccessor with underlying Flash or Ram Accessor
	 * @param placement	Placement for the image
	 */
	template<template<typename> class Accessor>
	ResumableResult<void>
	writeImage(graphic::ImageAccessor<ColorType, Accessor> accessor);

	ResumableResult<void> drawBlind(const shape::Point& point);
	ResumableResult<void> drawBlind(const shape::Section& section);
	ResumableResult<void> drawBlind(const shape::HLine& hline);
	ResumableResult<void> drawBlind(const shape::VLine& vline);

	ResumableResult<ColorType>
	getBlind(const shape::Point& point) const;

private:
	// Static variables for resumable functions
	// OPTIMIZE Closing check if the union is well formed
	union {
		modm::ShortTimeout timeout;

		std::array<uint8_t, 15> array_u8;
		std::array<uint16_t, 3> array_u16;
		
		ili9341_register::MemoryAccessCtrl_t madCtrl;

		// Static variables for resumable functions with parallel application
		struct
		{
			std::array<ColorType, BC> array_color;
			std::array<uint16_t, 2> array_u16;

			size_t i; // index in conversion array_color
			Point scanner; // index on display

			uint64_t pixels; // Must fit Total number of pixels: R.x * R.y = 76800
			size_t pixels_bulk; // Number of pixels of current bulk

			ColorType temp_color; // Temporary storage for a color
		} p;  // [p]arallel
	};
};

}  // namespace modm

#include "ili9341_impl.hpp"

#endif  //  MODM_ILI9341_HPP
