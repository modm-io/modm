// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2020, Vivien Henry
 * Copyright (c) 2023, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ADS868x_HPP
#	error "Don't include this file directly! Use 'ads868x.hpp' instead."
#endif

// ----------------------------------------------------------------------------
namespace modm
{


template <typename SpiMaster, typename Cs, typename nReset>
void
Ads868x<SpiMaster, Cs, nReset>::initialize()
{
	Cs::setOutput(modm::Gpio::Low);
	nReset::setOutput(modm::Gpio::Low);

	modm::this_fiber::sleep_for(1ms);

	nReset::set();
	Cs::set();
}


template <typename SpiMaster, typename Cs, typename nReset>
uint16_t
Ads868x<SpiMaster, Cs, nReset>::singleConversion()
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	SpiMaster::transfer(nullptr, buffer, 2);

	if (this->releaseMaster()) Cs::set();

	return uint16_t((buffer[0] << 8) | buffer[1]);
}


template <typename SpiMaster, typename Cs, typename nReset>
void
Ads868x<SpiMaster, Cs, nReset>::writeRegister(Register reg, uint32_t data)
{
	// LSB (0-15)
	buffer[0] = 0b1101'0000;
	buffer[1] = uint8_t(reg);
	buffer[2] = data >> 8;
	buffer[3] = data;

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	SpiMaster::transfer(buffer, nullptr, 4);

	Cs::set();

	// MSB (16-31)
	buffer[0] = 0b1101'0000;
	buffer[1] = uint8_t(reg) + 2;
	buffer[2] = data >> 24;
	buffer[3] = data >> 16;

	modm::this_fiber::sleep_for(1us);
	Cs::reset();

	SpiMaster::transfer(buffer, nullptr, 4);

	if (this->releaseMaster()) Cs::set();
}

template <typename SpiMaster, typename Cs, typename nReset>
uint32_t
Ads868x<SpiMaster, Cs, nReset>::readRegister(Register reg)
{
	uint32_t result{};

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	// MSB (31-16)
	buffer[0] = 0b1100'1000;
	buffer[1] = uint8_t(reg) + 2;
	buffer[2] = 0;
	buffer[3] = 0;

	SpiMaster::transfer(buffer, nullptr, 4);

	Cs::set();
	modm::this_fiber::sleep_for(1us);
	Cs::reset();

	SpiMaster::transfer(nullptr, buffer, 4);
	result = (uint32_t(buffer[0]) << 24) | (uint32_t(buffer[1]) << 16);

	Cs::set();
	modm::this_fiber::sleep_for(1us);
	Cs::reset();

	// LSB (0-15)
	buffer[0] = 0b1100'1000;
	buffer[1] = uint8_t(reg);
	buffer[2] = 0;
	buffer[3] = 0;

	SpiMaster::transfer(buffer, nullptr, 4);

	Cs::set();
	modm::this_fiber::sleep_for(1us);
	Cs::reset();

	SpiMaster::transfer(nullptr, buffer, 4);
	result |= (uint32_t(buffer[0]) << 8) | uint32_t(buffer[1]);

	if (this->releaseMaster()) Cs::set();

	return result;
}

} // namespace modm
