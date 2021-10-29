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

#include <bit>

#include <modm/architecture/interface/accessor.hpp>
#include <modm/math/utils/bit_constants.hpp>

#include <modm/ui/color/concepts.hpp>
#include <modm/ui/shape/point.hpp>

namespace modm::graphic
{

/**
 * @brief 			Sequential Accessor for images in RAM, Flash or SD-Card (later not yet implemented)
 * 					Some image compressions or storages are very inefficient for random access.
 * 					Also to fulfill the purpose of sequential reading an image, random access is not required.
 * 					By consequently restricting the ImageAccessor to sequential access, intercompatibility
 * 					is guaranteed.
 */
template<class C, template<typename> class>
class ImageAccessor;

/**
 * @brief 				Sequential accessor for planar stored image with clipping support
 *
 * @tparam C 			Planar Colortype of image to access
 * @tparam Accessor 	Any of modm::accessor::*
 *
 * @author				Thomas Sommer
 * @ingroup				modm_ui_graphic
 */
template<color::ColorPlanar C, template<typename> class Accessor>
class ImageAccessor<C, Accessor> : public Accessor<C>
{
public:
	ImageAccessor() = default;

	ImageAccessor(const BufferInterface<C>* buffer, shape::Point placement = {0, 0})
		: Accessor<typename C::ValueType>(buffer->getBuffer()), section(placement, placement + buffer->getSize())
	{
		col_size = buffer->getSize().x;
		initialize(placement);
	}

	ImageAccessor(const C* addr, shape::Point placement = {0, 0})
		: Accessor<C>(addr + 2)
	{
		auto flash = modm::accessor::asFlash(addr);
		section = {placement, placement + shape::Point(flash[0], flash[1])};
		col_size = flash[0];
		initialize(placement);
	}

	ImageAccessor(const C* addr, shape::Size size, shape::Point placement)
		: Accessor<C>(addr), section(placement, placement + size), col_size(size.x)
	{
		initialize(placement);
	}

	shape::Size getSection() const
	{ return section; }

	void incrementRow()
	{ this->address = addr_top++; }

	void incrementCol()
	{ this->address += col_size; }

	C
	nextPixel()
	{
		incrementCol();
		return Accessor<C>::operator*();
	}

private:
	void
	initialize(const shape::Point placement)
	{
		const shape::Point topLeft = {placement.x < 0 ? -placement.x : 0, placement.y < 0 ? -placement.y : 0};

		// TODO Test functionality
		addr_top = this->address + topLeft.y * col_size + topLeft.x;
		this->address = addr_top;
	}

	shape::Size section;
	size_t col_size;
	const C* addr_top;
};

}  // namespace modm::graphic

#include "accessor_image_palletized.hpp"