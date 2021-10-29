/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2011, 2013, Thorsten Lajewski
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2013, Hans Schily
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2015, Niclas Rohrer
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

#include <modm/ui/color/concepts.hpp>
#include <modm/ui/shape/point.hpp>
#include <modm/ui/font/fixed_width_5x8.hpp>

#include "buffer_interface.hpp"
#include "buffer_memory.hpp"

#include "accessor_image.hpp"

namespace modm::graphic
{
template<color::Color C, shape::Size R>
class Buffer : public BufferMemory<C, R>, public IOStream
{
public:
	using ColorType = C;

	Buffer(C* colormap) : BufferMemory<C, R>(colormap), IOStream(writer), writer(this)
	{ setFont(modm::font::FixedWidth5x8); }

	// ###################################################
	// # Same Color and Size: use std::copy or DMA
	constexpr Buffer(const Buffer<C, R> &other)
	{
		// OPTIMIZE support DMA
		std::copy(other.buffer[0], other.buffer[0] + sizeof(other.buffer) / sizeof(C), this->buffer[0]);

		this->colormap = other.colormap;
		this->font = other.font;
		this->font_metadata = other.font_metadata;
	}

	Buffer& operator=(const Buffer<C, R> &other)
	{
		// OPTIMIZE support DMA
		if(this != &other)
			std::copy(other.buffer[0], other.buffer[0] + sizeof(other.buffer) / sizeof(C), this->buffer[0]);

		// this->colormap = other.colormap;
		// this->font = other.font;
		// this->font_metadata = other.font_metadata;

		return *this;
	}

	constexpr Buffer(Buffer<C, R> &&other)
	{
		this->buffer = other.buffer;

		this->colormap = other.colormap;
		this->font = other.font;
		this->font_metadata = other.font_metadata;
	}

/* 	constexpr Buffer& operator=(Buffer<C, R> &&other) {
		this->buffer = other.buffer;

		this->colormap = other.colormap;
		this->font = other.font;
		this->font_metadata = other.font_metadata;
		return *this;
	} */

	// ###########################################
	// Different color but same size: Copy-algorithm varies for planar and palettized colortype
	// Thus the copying is deligated to BufferMemory
 	template<color::Color CO>
	constexpr Buffer(const Buffer<CO, R> &other) : BufferMemory<C, R>(other), IOStream(writer), writer(this)
	{
		this->colormap = other.colormap;
		this->font = other.font;
		this->font_metadata = other.font_metadata;
	}

	template<color::Color CO>
	Buffer& operator=(const Buffer<CO, R> &other)
	{
		BufferMemory<C, R>::operator=(other);
		// this->colormap = other.colormap;
		// this->font = other.font;
		// this->font_metadata = other.font_metadata;
		return *this;
	}

	// ###########################################
	// Different color and size: always use writeImage
	template<color::Color CO, shape::Size RO>
	constexpr Buffer(const Buffer<CO, RO> &other) : IOStream(writer), writer(this)
	{
		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&other));

		this->colormap = other.colormap;
		this->font = other.font;
		this->font_metadata = other.font_metadata;
	}

	// IMPLEMENT all operators
	// using BufferMemory<C, R>::operator=;
	// using BufferMemory<C, R>::operator+;
	// using BufferMemory<C, R>::operator-;
	// using BufferMemory<C, R>::operator+=;
	// using BufferMemory<C, R>::operator-=;

	// #############################################################
	// # General interface between Buffers

	/**
	 * @brief 			Writes another buffer to this buffer. If the other buffer is palettized,
	 * 					pixels will be mapped using the colormap.
	 *
	 * @tparam CO		Colortype of other buffer
	 *
	 * @param buffer 	Other buffer
	 * @param placement 	Target position for other buffer
	 */
	template<color::Color CO>
	void write(const BufferInterface<CO> &buffer, shape::Point placement) {
		this->writeImage(ImageAccessor<CO, modm::accessor::Ram>(&buffer, placement));
	}

	template<color::Color CO>
	void write(const BufferInterface<CO> &buffer) {
		// no placement passed? take a shortcut
		this->operator=(buffer);
	}

	/**
	 * @brief 			Writes an image from flash to this buffer. If the image is palettized,
	 * 					pixels will be mapped using the colormap.
	 *
	 * @tparam CO		Colortype of image
	 *
	 * @param buffer 	Address of the image in flash
	 * @param placement 	Target position for image
	 */
	template<color::Color CO>
	void write(const uint8_t *addr, shape::Point placement = {0, 0}) {
		this->writeImage(ImageAccessor<CO, modm::accessor::Flash>(addr, placement));
	}

	void clear(ColorType color = 0);

	// Idea for vertical and horizontal shift with <<= and >>=
	// https://stackoverflow.com/questions/486099/can-inner-classes-access-private-variables
	// class Horizontal {
	// public:
 		void operator<<=(const size_t shift);
		void operator>>=(const size_t shift);
	// private:
	// 	uint8_t& buffer;
	// }

	// class Vertical {
	// public:
	// 	void operator<<=(const size_t shift);
	// 	void operator>>=(const size_t shift);
	//	private:
	//	uint8_t& buffer;
	// }

	// friend class Horizontal;

	// void rotateVertical(const size_t difference);
	// void rotateHorizontal(const size_t difference);

	// Serve BufferInterface
	const uint8_t* getBuffer() const final
	{ return (uint8_t*)(this->buffer); }

	shape::Size getSize() const final
	{ return R; }

public:
	shape::Point cursor;
	bool auto_linebreak = true;

	void
	writeChar(char c);

	void
	setFont(const uint8_t *font);

	void
	setFont(const modm::accessor::Flash<uint8_t> *font)
	{ setFont(font->getPointer()); }

	// Move to font-object
	uint8_t
	getFontHeight() const {
		return font_metadata.height;
	}

	uint16_t
	getStringWidth(const char *s) const;

	static uint16_t
	getStringWidth(const char *s, const modm::accessor::Flash<uint8_t> *font);

private:
	modm::accessor::Flash<uint8_t> font;
	static constexpr uint8_t offsetWidthTable = 8;

	void linebreak()
	{
		cursor.x = 0;
		cursor.y += font_metadata.height + font_metadata.hspace;
	}

	// Interface for IOStream
	class Writer : public IODevice
	{
		Buffer *parent;
	public:
		Writer(Buffer *parent) : parent(parent) {}

		virtual void
		write(char c)
		{ this->parent->writeChar(c); }

		using IODevice::write;

		// unused
		virtual void
		flush() {}

		// unused
		virtual bool
		read(char&)
		{ return false; }
	};

	Writer writer;

	struct
	{
		uint8_t height;
		uint8_t hspace;
		uint8_t vspace;
		uint8_t first;
		uint8_t count;

		inline void
		update(modm::accessor::Flash<uint8_t> font) {
			height = font[3];
			hspace = font[4];
			vspace = font[5];
			first = font[6];
			count = font[7];
		}
	} font_metadata;

	template<color::Color, shape::Size>
	friend class Buffer;
};
}  // namespace modm::graphic

#include "buffer_impl.hpp"
#include "buffer_text_impl.hpp"