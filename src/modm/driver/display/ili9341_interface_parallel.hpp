/*
 * Copyright (c) 2020, Pavel Pletenev
 * Copyright (c) 2020, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once
#include "ili9341.hpp"

#include <modm/math/utils/endianness.hpp>

#include "ili9341_defines.hpp"

namespace modm
{

/// @ingroup modm_driver_ili9341
template<class Interface>
class Ili9341InterfaceParallel: public ili9341_register
{
	Interface& interface;
public:
	Ili9341InterfaceParallel(Interface& interface)
	: interface(interface) {}

	void
	writeCommand(Command command)
	{
		interface.writeIndex(uint8_t(command));
	}
	void
	writeCommand(Command command, uint8_t const *args, std::size_t length)
	{
		interface.writeIndex(uint8_t(command));
		for(std::size_t i=0; i<length; ++i)
			interface.writeData(args[i]);
	}

	void
	writeData(color::Rgb565 pixel)
	{
		interface.writeData(pixel.getValue());
	}

	void
	writeData(color::Rgb565 const *pixels, std::size_t length)
	{
		for(std::size_t i=0; i < length; ++i)
			interface.writeData(pixels[i].getValue);
	}

	void
	writeCommandValue8(Command command, uint8_t value)
	{
		writeCommand(command, &value, 1);
	}

	void
	readData(Command command, uint8_t *array_color, std::size_t length)
	{
		interface.writeIndex(uint8_t(command));
		interface.readData();
		for(std::size_t i=0; i<length; ++i)
			array_color[i] = interface.readData();
	}
	uint8_t
	readData(Command command)
	{
		writeCommand(command);
		interface.readData();
		return interface.readData();
	}
};

} // namespace modm