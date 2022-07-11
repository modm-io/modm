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
#pragma once
#include "ili9341.hpp"

#include <modm/architecture/interface/spi_device.hpp>

namespace modm
{

/// @ingroup modm_driver_ili9341
template<class SPI, class Cs, class Dc>
class Ili9341SPIInterface: public ili9341, public modm::SpiDevice<SPI>
{
public:
	Ili9341SPIInterface()
	{
		this->attachConfigurationHandler([]() {
			SPI::setDataMode(SPI::DataMode::Mode0);
			SPI::setDataOrder(SPI::DataOrder::MsbFirst);
		});

		Cs::setOutput(modm::Gpio::High);
		Dc::setOutput();
	}

	modm_noinline void
	writeCommand(Command command)
	{
		if constexpr ( spi::Support_DataSize_Bit16<SPI> )
			SPI::setDataSize(SPI::DataSize::Bit8);
		Dc::reset(); // enable command
		SPI::transferBlocking(i(command));
		Dc::set(); // reset to data
	}
	modm_noinline void
	writeCommand(Command command, uint8_t const *args, std::size_t length)
	{
		if constexpr ( spi::Support_DataSize_Bit16<SPI> )
			SPI::setDataSize(SPI::DataSize::Bit8);
		Dc::reset(); // enable command
		SPI::transferBlocking(i(command));
		Dc::set(); // reset to data
		if (length != 0)
		{
			SPI::transferBlocking(args, nullptr, length);
		}
	}
	void
	writeData(uint8_t const *data, std::size_t length)
	{
		if constexpr ( spi::Support_DataSize_Bit16<SPI> )
			SPI::setDataSize(SPI::DataSize::Bit8);
		SPI::transferBlocking(data, nullptr, length);
	}

	void
	writeData(color::Rgb565 rgb565)
	{
		if constexpr ( spi::Support_DataSize_Bit16<SPI> )
			SPI::setDataSize(SPI::DataSize::Bit16);
		SPI::transferBlocking(rgb565.color);
	}

	void
	writeDataRepeat(color::Rgb565 rgb565, std::size_t repeat)
	{
		if constexpr ( spi::Support_DataSize_Bit16<SPI> )
			SPI::setDataSize(SPI::DataSize::Bit16);
		SPI::transferBlocking16(rgb565.color, repeat);
	}

	void
	writeData(color::Rgb565 const *data, std::size_t length)
	{
		if constexpr ( spi::Support_DataSize_Bit16<SPI> )
			SPI::setDataSize(SPI::DataSize::Bit16);
		SPI::transferBlocking16(reinterpret_cast<const uint16_t*>(data), nullptr, length);
	}

	void
	writeCommandValue8(Command command, uint8_t value)
	{
		if constexpr ( spi::Support_DataSize_Bit16<SPI> )
			SPI::setDataSize(SPI::DataSize::Bit8);
		writeCommand(command, &value, 1);
	}

	void
	readData(Command command, uint8_t *buffer, std::size_t length)
	{
		uint8_t b[4];

		if constexpr ( spi::Support_DataSize_Bit16<SPI> )
			SPI::setDataSize(SPI::DataSize::Bit8);
		Dc::reset();
		SPI::transferBlocking(i(command) << 1);
		Dc::set();
		SPI::transferBlocking(b /*nullptr*/, buffer, length);
	}
	uint8_t
	readData(Command command)
	{
		if constexpr ( spi::Support_DataSize_Bit16<SPI> )
			SPI::setDataSize(SPI::DataSize::Bit8);
		writeCommand(command);
		return SPI::transferBlocking(0);
	}

public:
	struct BatchHandle
	{
		Ili9341SPIInterface& i;
		BatchHandle(Ili9341SPIInterface& iface)
		: i(iface)
		{
			i.acquireMaster();
			Cs::reset();
		}
		~BatchHandle()
		{
			if (i.releaseMaster())
				Cs::set();
		}
	};
};

/// @ingroup modm_driver_ili9341
template <class SPI, class Cs, class Dc, class Reset, class Backlight>
using Ili9341Spi = Ili9341<
	Ili9341SPIInterface<SPI, Cs, Dc>,
	Reset, Backlight>;

} // namespace modm