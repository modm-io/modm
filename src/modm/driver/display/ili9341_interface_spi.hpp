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

#ifndef MODM_ILI9341_HPP
#error "Don't include this file directly, use 'ili9341.hpp' instead!"
#endif

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/platform/spi/spi_base.hpp>
#include <modm/processing/resumable.hpp>

#include "ili9341_register.hpp"

namespace modm
{

/// @ingroup modm_driver_ili9341
template<class SpiMaster, class Cs, class Dc>
class Ili9341InterfaceSpi : public ili9341_register, public modm::SpiDevice< SpiMaster >, protected modm::NestedResumable<4>
{
	uint8_t read;
public:
	Ili9341InterfaceSpi()
	{
		this->attachConfigurationHandler([]() {
			SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
			SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
		});
		Cs::setOutput(modm::Gpio::High);
		Dc::setOutput();
	}

	modm::ResumableResult<void>
	writeCommand(Command command)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		Dc::reset();
		RF_CALL(SpiMaster::transfer(uint8_t(command)));
		Dc::set();

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<void>
	writeCommand(Command command, const uint8_t data)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		Dc::reset();
		RF_CALL(SpiMaster::transfer(uint8_t(command)));
		Dc::set();

		RF_CALL(SpiMaster::transfer(data));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<void>
	writeCommand(Command command, const uint8_t *data, std::size_t length)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		Dc::reset();
		RF_CALL(SpiMaster::transfer(uint8_t(command)));
		Dc::set();

		RF_CALL(SpiMaster::transfer(data, nullptr, length));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<void>
	writeData(const uint8_t *data, size_t length)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		RF_CALL(SpiMaster::transfer(data, nullptr, length));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	// OPTIMIZE determine if implementation is optimal
	modm::ResumableResult<void>
	writeData(color::Rgb565<true> rgb565)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		RF_CALL(SpiMaster::transfer((uint8_t*)(&rgb565.color), nullptr, 2));

		// Alternative
		// RF_CALL(SpiMaster::transfer(rgb565.color >> 8));
		// RF_CALL(SpiMaster::transfer(rgb565.color & 0xFF));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}

	modm::ResumableResult<uint8_t>
	readData(Command command)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		Dc::reset();
		RF_CALL(SpiMaster::transfer(uint8_t(command)));
		Dc::set();

		read = RF_CALL(SpiMaster::transfer(0x00)).getResult();

		if (this->releaseMaster())
			Cs::set();

		RF_END_RETURN(read);
	}

	modm::ResumableResult<void>
	readData(Command command, uint8_t *buffer, size_t length)
	{
		RF_BEGIN();

		RF_WAIT_UNTIL(this->acquireMaster());
		Cs::reset();

		Dc::reset();
		RF_CALL(SpiMaster::transfer(uint8_t(command)));
		Dc::set();

		RF_CALL(SpiMaster::transfer(nullptr, buffer, length));

		if (this->releaseMaster())
			Cs::set();

		RF_END();
	}
};

}  // namespace modm