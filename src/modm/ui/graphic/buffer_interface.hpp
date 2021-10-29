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

#include <modm/ui/color/concepts.hpp>
#include <modm/ui/shape/point.hpp>

#include "canvas.hpp"

namespace modm::graphic
{

/**
 * @brief 		Interface to Buffer to serve graphic::ImageAccessor
 *
 * @tparam C 	ColorType. Althought it's an interface, the Variation over C is
 * 				required for optimal performance. Anyways // TODO find a way to get rid of this C
 */

// FIXME BufferMemory::UnderlyingType must pass T
template<color::Color C, typename T = uint8_t>
class BufferInterface
{
public:
	virtual shape::Size
	getSize() const = 0;

	virtual const T*
	getBuffer() const = 0;

protected:
	BufferInterface() = default;
};
}  // namespace modm