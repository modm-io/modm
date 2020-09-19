/*
 * Copyright (c) 2020, Pavel Pletenev
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef MODM_ILI9341_PARALLEL_HPP
#define MODM_ILI9341_PARALLEL_HPP

#include "ili9341.hpp"

namespace modm
{

/// @ingroup modm_driver_ili9341
template<class INTERFACE>
class Ili9341ParallelInterface: public ili9341
{
	INTERFACE& interface;
public:
	Ili9341ParallelInterface(INTERFACE& interface)
	: interface(interface) {}

	__attribute__((noinline)) void
	writeCommand(Command command)
	{
		interface.writeIndex(i(command));
	}
	__attribute__((noinline)) void
	writeCommand(Command command, uint8_t const *args, std::size_t length)
	{
		interface.writeIndex(i(command));
		for(std::size_t i=0; i<length; ++i)
			interface.writeData(args[i]);
	}
	void
	writeData(uint8_t const *data, std::size_t length)
	{
		auto const data16 = reinterpret_cast<uint16_t const*>(data);
		size_t const length16 = length / 2;
		for(std::size_t i=0; i<length16; ++i)
			interface.writeData(modm::fromBigEndian(data16[i]));
	}
	void
	writeCommandValue8(Command command, uint8_t value)
	{
		writeCommand(command, &value, 1);
	}

	void
	readData(Command command, uint8_t *buffer, std::size_t length)
	{
		interface.writeIndex(i(command));
		interface.readData();
		for(std::size_t i=0; i<length; ++i)
			buffer[i] = interface.readData();
	}
	uint8_t
	readData(Command command)
	{
		writeCommand(command);
		interface.readData();
		return interface.readData();
	}

public:
	struct BatchHandle
	{
		BatchHandle(Ili9341ParallelInterface&) {}
		~BatchHandle() {}
	};
};

/// @ingroup modm_driver_ili9341
template <class Interface, class Reset, class Backlight, std::size_t BufferSize = 320>
using Ili9341Parallel = Ili9341<
	Ili9341ParallelInterface<Interface>,
	Reset, Backlight, BufferSize>;

} // namespace modm

#endif // MODM_ILI9341_PARALLEL_HPP
