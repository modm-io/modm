/*
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

#include <concepts>
#include <algorithm>

#include <modm/math/utils/misc.hpp>

#include <modm/ui/color/concepts.hpp>
#include "concepts.hpp"

#include <modm/ui/shape/point.hpp>
#include <modm/ui/shape/line.hpp>
#include <modm/ui/shape/section.hpp>

#include "accessor_image.hpp"

namespace modm::graphic
{

template<class C, shape::Size R>
class BufferMemory;

/**
 * @brief 		Framebuffer with random access and low-lvl drawing methods. Each pixel has it's own address
 *
 * @tparam	C	color::Gray{>= 8}, color::Rgb, color::Hsv or color::RgbStacked - to be expanded in the future
 * @tparam	R	Resolution - R.x: horizontal, R.y: vertical
 *
 * @author		Thomas Sommer
 * @ingroup		modm_ui_graphic
 */
template<color::ColorPlanar C, shape::Size R>
class BufferMemory<C, R> : public BufferInterface<C>, public Canvas<C, R>
{
protected:
	BufferMemory(C* colormap) : Canvas<C, R>(colormap) {};

	template<color::ColorPlanar CO>
	constexpr BufferMemory(const BufferMemory<CO, R> &other)
	{
		std::copy(other.buffer[0], other.buffer[0] + sizeof(other.buffer) / sizeof(C), buffer[0]);
	}

	template<color::ColorPalletized CO>
	constexpr BufferMemory(const BufferMemory<CO, R> &other)
	{
		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&other));
	}

	template<color::ColorPlanar CO>
	void operator=(const BufferMemory<CO, R> &other)
	{
		std::copy(other.buffer[0], other.buffer[0] + sizeof(other.buffer) / sizeof(CO), buffer[0]);
	}

	template<color::ColorPalletized CO>
	void operator=(const BufferMemory<CO, R> &other)
	{
		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&other));
	}

protected:
	static constexpr size_t BY = R.y;
	C buffer[BY][R.x];

	C::ValueType clearValue(C color = 0) const {
		return color.getValue();
	}

	/**
	 * @brief				Write Image with foreign Color
	 *
	 * @param accessor		ImageAccessor inheriting an accessor::Flash or accessor::Ram
	 * @param placement		Placement for the image
	 */
	template<color::Color CO, template<typename> class Accessor>
	void
	writeImage(ImageAccessor<CO, Accessor> accessor);

	/**
	 * @brief 				Write Image with same C
	 *
	 * @param accessor		ImageAccessor inheriting an accessor::Flash or accessor::Ram
	 * @param placement		Placement for the image
	 */
	template<template<typename> class Accessor>
	void
	writeImage(ImageAccessor<C, Accessor> accessor);

	/**
	 * @warning 	These methods do not check sanity!
	 *				It is your responsibility to check if the shape
	 *				is within buffer boundaries.
	 */
	void drawBlind(const shape::Point& point);
	void drawBlind(const shape::HLine& hline);
	void drawBlind(const shape::VLine& vline);
	void drawBlind(const shape::Section& section);

	C getBlind(const shape::Point& point) const
	{
		return buffer[point.y][point.x];
	}

	C& operator()(const shape::Point& point)
	{
		return buffer[point.y][point.x];
	}

	template<class, shape::Size>
	friend class BufferMemory;
};
}  // namespace modm

#include "buffer_memory_impl.hpp"
#include "buffer_memory_draw_impl.hpp"

#include "buffer_memory_palletized.hpp"
// #include "buffer_memory_palletized_mono.hpp"