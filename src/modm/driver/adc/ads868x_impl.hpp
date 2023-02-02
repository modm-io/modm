// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2020, Vivien Henry
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
void Ads868x<SpiMaster, Cs, nReset>::initialize(){
	Cs::setOutput(modm::Gpio::Low);

	nReset::setOutput(modm::Gpio::Low);
	modm::delay_ms(1);
	nReset::set();

	Cs::set();
	Cs::reset();
	Cs::set();
}


template <typename SpiMaster, typename Cs, typename nReset>
uint16_t Ads868x<SpiMaster, Cs, nReset>::singleConversion(){
	uint16_t data = 0;

	Cs::reset();
	data |= SpiMaster::transferBlocking(0) << 8;
	data |= SpiMaster::transferBlocking(0);

	Cs::set();

	return data;
}


template <typename SpiMaster, typename Cs, typename nReset>
void Ads868x<SpiMaster, Cs, nReset>::writeRegister(Register reg, uint32_t data){
	uint32_t cmd;

	// LSB (0-15)
	cmd = (0b11010'00 << 25) | (static_cast<uint8_t>(reg) << 16) | (data & 0xFFFF);

	Cs::reset();
	SpiMaster::transferBlocking((uint8_t)(cmd >> 24));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 16));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 8));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 0));

	Cs::set();

	// MSB (16-31)
	cmd = (0b11010'00 << 25) | ((static_cast<uint8_t>(reg) + 2) << 16) | (data >> 16);
	Cs::reset();

	SpiMaster::transferBlocking((uint8_t)(cmd >> 24));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 16));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 8));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 0));

	Cs::set();
}

template <typename SpiMaster, typename Cs, typename nReset>
uint32_t Ads868x<SpiMaster, Cs, nReset>::readRegister(Register reg)
{
	uint32_t cmd = 0;
	uint32_t data = 0;
	uint8_t byte = 0;

	Cs::reset();

	// MSB (31-16)
	cmd = (0b11001'00 << 25) | ((static_cast<uint8_t>(reg) + 2) << 16);
	SpiMaster::transferBlocking((uint8_t)(cmd >> 24));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 16));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 8));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 0));

	Cs::set();
	Cs::reset();

	byte = SpiMaster::transferBlocking(0);
	data |= (uint32_t)byte << 24;

	byte = SpiMaster::transferBlocking(0);
	data |= (uint32_t)byte << 16;
	SpiMaster::transferBlocking(0);
	SpiMaster::transferBlocking(0);

	Cs::set();
	Cs::reset();

	// LSB (0-15)
	cmd = (0b11001'00 << 25) | (static_cast<uint8_t>(reg) << 16);
	SpiMaster::transferBlocking((uint8_t)(cmd >> 24));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 16));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 8));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 0));

	Cs::set();
	Cs::reset();

	byte = SpiMaster::transferBlocking(0);
	data |= (uint32_t)byte << 8;

	byte = SpiMaster::transferBlocking(0);
	data |= (uint32_t)byte;
	SpiMaster::transferBlocking(0);
	SpiMaster::transferBlocking(0);

	Cs::set();

	return data;
}

} // namespace modm
