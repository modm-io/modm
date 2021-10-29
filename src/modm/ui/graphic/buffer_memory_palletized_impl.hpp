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
#include "buffer_memory_palletized.hpp"

using namespace modm::shape;
using namespace modm::color;

namespace modm::graphic {

template<ColorPalletized C, Size R>
template<Color CO, template<typename> class Accessor>
void
BufferMemory<C, R>::writeImage(ImageAccessor<CO, Accessor> accessor)
{
	// FIXME buggy for placement.y < 0
	const Section clipping = this->getIntersection(accessor.getSection());
	const Looper looper(clipping);

	for (int_fast16_t x = clipping.topLeft.x; x < clipping.bottomRight.x; x++)
	{
		size_t yb = looper.yb_top;
		int lshift = looper.lshift_top;
		accessor.incrementRow_preparePixel();

		// Top end
		if(yb < looper.yb_bot) {
			this->buffer[yb][x] = (this->buffer[yb][x] & looper.keepmask_top) | palletizeByte(accessor, lshift, digits);
			lshift = 0;
			yb++;
		}

		// Middle part
		while(yb < looper.yb_bot)
			this->buffer[yb++][x] = palletizeByte(accessor, 0, digits);

		// Bottom end
		this->buffer[yb][x] = (this->buffer[yb][x] & looper.keepmask_bot) | palletizeByte(accessor, lshift, looper.lshift_bot);
	}
}

template<ColorPalletized C, Size R>
template<template<typename> class Accessor>
void
BufferMemory<C, R>::writeImage(ImageAccessor<C, Accessor> accessor)
{
	const Section clipping = this->getIntersection(accessor.getSection());
	const Looper looper(clipping);

	const int rshift_bot = digits - looper.lshift_top;

	// FIXME looper.lshift_top always 0 for placement.y < 0
	// Thus writeImage top offscreen is buggy
	// MODM_LOG_VAR(looper.lshift_top);

	if (looper.lshift_top) {
		// Split bytes, shift, reassemble
		for (int_fast16_t x = clipping.topLeft.x; x < clipping.bottomRight.x; x++)
		{
			accessor.incrementRow();
			size_t yb = looper.yb_top;

			// Top end
			if (clipping.topLeft.y > 0)
			{
				this->buffer[yb][x] = (this->buffer[yb][x] & looper.keepmask_top) | *accessor << looper.lshift_top;
				yb++;
			}

			// Middle part
			while (yb < looper.yb_bot)
			{
				this->buffer[yb][x] = *accessor >> rshift_bot;
				accessor.incrementCol(); // Byte
				this->buffer[yb][x] |= *accessor << looper.lshift_top;
				yb++;
			}

			// Bottom end
			if (clipping.bottomRight.y < int16_t(R.y))
				this->buffer[yb][x] = (this->buffer[yb][x] & looper.keepmask_bot) | *accessor >> rshift_bot;
		}
	} else if (clipping.topLeft.y < clipping.bottomRight.y) {
		// Copy bytes
		for (int_fast16_t x = clipping.topLeft.x; x < clipping.bottomRight.x; x++)
		{
			accessor.incrementRow();
			size_t yb = looper.yb_top;

			while (yb < looper.yb_bot)
			{
				// OPTIMIZE Concider std::copy for Accessor == modm::accessor::Ram
				this->buffer[yb++][x] = *accessor;
				accessor.incrementCol();
			}

			this->buffer[yb][x] = (this->buffer[yb][x] & looper.keepmask_bot) | *accessor;
		}
	}
}

template<ColorPalletized C, Size R>
void
BufferMemory<C, R>::invert()
{
	std::for_each(buffer[0], buffer[0] + R.x * BY, [](PalletType &value){
		value ^= allBits;
	} );
}

} // namespace modm::graphic