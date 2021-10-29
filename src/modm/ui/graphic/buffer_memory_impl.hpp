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

template<ColorPlanar C, Size R>
template<Color CO, template<typename> class Accessor>
void
BufferMemory<C, R>::writeImage(ImageAccessor<CO, Accessor> accessor)
{
	const Section clipping = this->getIntersection(accessor.getSection());

	Point scanner = clipping.topLeft;
	while (scanner.x < clipping.bottomRight.x)
	{
		accessor.incrementRow_preparePixel();
		while (scanner.y < clipping.bottomRight.y)
		{
			if constexpr (ColorPalletized<CO>) {
				// Apply colormap
				buffer[scanner.y][scanner.x] = this->colormap[ accessor.nextPixel().getValue() ];
			} else {
				// convert color
				buffer[scanner.y][scanner.x] = accessor.nextPixel();
			}
			scanner.y++;
		}
		scanner.x++;
		scanner.y = clipping.topLeft.y;
	}
}

template<ColorPlanar C, Size R>
template<template<typename> class Accessor>
void
BufferMemory<C, R>::writeImage(ImageAccessor<C, Accessor> accessor)
{
	// IMPLEMENT. see ili9341 equivalent

	// std::copy
}

} // namespace modm::graphic