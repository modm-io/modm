/*
 * Copyright (c) 2019, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef MODM_ILI9341_SPI_HPP
#define MODM_ILI9341_SPI_HPP

#include "ili9341.hpp"
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/platform/spi/spi_base.hpp>

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

	__attribute__((noinline)) void
	writeCommand(Command command)
	{
		Dc::reset(); // enable command
		SPI::transferBlocking(i(command));
		Dc::set(); // reset to data
	}
	__attribute__((noinline)) void
	writeCommand(Command command, uint8_t const *args, std::size_t length)
	{
		Dc::reset(); // enable command
		SPI::transferBlocking(i(command));
		Dc::set(); // reset to data
		if (length != 0)
		{
			SPI::transferBlocking(const_cast<unsigned char *>(args), nullptr, length);
		}
	}
	void
	writeData(uint8_t const *data, std::size_t length)
	{
		SPI::transferBlocking(const_cast<unsigned char *>(data), nullptr, length);
	}
	void
	writeCommandValue8(Command command, uint8_t value)
	{
		writeCommand(command, &value, 1);
	}

	void
	readData(Command command, uint8_t *buffer, std::size_t length)
	{
		using modm::platform::SpiBase;
		uint8_t b[4];

		Dc::reset(); // enable command
		// SPI::Hal::setDataSize(SpiBase::DataSize::Bit9);
		SPI::transferBlocking(i(command) << 1);
		SPI::Hal::setDataSize(SpiBase::DataSize::Bit8);
		Dc::set(); // reset to data
		SPI::transferBlocking(b /*nullptr*/, buffer, length);
	}
	uint8_t
	readData(Command command)
	{
		writeCommand(command);
		return SPI::transferBlocking(0x00);
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
template <class SPI, class Cs, class Dc, class Reset, class Backlight, std::size_t BufferSize = 320>
using Ili9341Spi = Ili9341<
	Ili9341SPIInterface<SPI, Cs, Dc>,
	Reset, Backlight, BufferSize>;

} // namespace modm

#endif // MODM_ILI9341_SPI_HPP
