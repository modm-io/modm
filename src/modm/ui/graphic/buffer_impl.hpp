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
#include "buffer.hpp"

using namespace modm::shape;
using namespace modm::color;

namespace modm::graphic {

template<Color C, Size R>
void
Buffer<C, R>::clear(ColorType color)
{
	// OPTIMIZE support DMA
	std::fill(this->buffer[0], this->buffer[0] + sizeof(this->buffer) / sizeof(decltype(this->clearValue(color))), this->clearValue(color));
	cursor = {0, 0};
}

template<Color C, Size R>
void
Buffer<C, R>::operator<<=(const size_t shift)
{
	for (size_t yb = 0; yb < Buffer<C, R>::BY; yb++)
	{
		size_t x = 0;
		while (x < R.x - shift)
		{
			this->buffer[yb][x] = this->buffer[yb][x + shift];
			x++;
		}
		const auto clear = this->clearValue();
		while (x < R.x)
			this->buffer[yb][x++] = clear;
	}
}

template<Color C, Size R>
void
Buffer<C, R>::operator>>=(const size_t shift)
{
	for (size_t yb = 0; yb < Buffer<C, R>::BY; yb++)
	{
		size_t x = R.x;
		while (x > shift)
		{
			x--;
			this->buffer[yb][x] = this->buffer[yb][x - shift];
		}
		const auto clear = this->clearValue();
		while (x)
			this->buffer[yb][--x] = clear;
	}
}
} // namespace modm::graphic