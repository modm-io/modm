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
	modm::delay_ms(1);
	nReset::set();

	Cs::set();
}


template <typename SpiMaster, typename Cs, typename nReset>
modm::ResumableResult<uint16_t>
Ads868x<SpiMaster, Cs, nReset>::singleConversion()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	RF_CALL(SpiMaster::transfer(nullptr, buffer, 2));

	if (this->releaseMaster()) Cs::set();

	RF_END_RETURN( uint16_t((uint16_t(buffer[0]) << 8) | buffer[1]) );
}


template <typename SpiMaster, typename Cs, typename nReset>
modm::ResumableResult<void>
Ads868x<SpiMaster, Cs, nReset>::writeRegister(Register reg, uint32_t data)
{
	RF_BEGIN();

	// LSB (0-15)
	buffer[0] = 0b1101'0000;
	buffer[1] = uint8_t(reg);
	buffer[2] = data >> 8;
	buffer[3] = data;

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	RF_CALL(SpiMaster::transfer(buffer, nullptr, 4));

	Cs::set();

	// MSB (16-31)
	buffer[0] = 0b1101'0000;
	buffer[1] = uint8_t(reg) + 2;
	buffer[2] = data >> 24;
	buffer[3] = data >> 16;

	modm::delay_us(1);
	Cs::reset();

	RF_CALL(SpiMaster::transfer(buffer, nullptr, 4));

	if (this->releaseMaster()) Cs::set();

	RF_END();
}

template <typename SpiMaster, typename Cs, typename nReset>
modm::ResumableResult<uint32_t>
Ads868x<SpiMaster, Cs, nReset>::readRegister(Register reg)
{
	RF_BEGIN();

	uint32_t cmd = 0;
	uint32_t data = 0;
	uint8_t byte = 0;

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	// MSB (31-16)
	buffer[0] = 0b1100'1000;
	buffer[1] = uint8_t(reg) + 2;
	buffer[2] = 0;
	buffer[3] = 0;

	RF_CALL(SpiMaster::transfer(buffer, nullptr, 4));

	Cs::set();
	modm::delay_us(1);
	Cs::reset();

	RF_CALL(SpiMaster::transfer(nullptr, buffer, 4));
	// cache the MSB values temporarily
	buffer[4] = buffer[0];
	buffer[5] = buffer[1];

	Cs::set();
	modm::delay_us(1);
	Cs::reset();

	// LSB (0-15)
	buffer[0] = 0b1100'1000;
	buffer[1] = uint8_t(reg);
	buffer[2] = 0;
	buffer[3] = 0;

	RF_CALL(SpiMaster::transfer(buffer, nullptr, 4));

	Cs::set();
	modm::delay_us(1);
	Cs::reset();

	RF_CALL(SpiMaster::transfer(nullptr, buffer, 4));

	if (this->releaseMaster()) Cs::set();

	RF_END_RETURN((uint32_t(buffer[4]) << 24) |
	              (uint32_t(buffer[5]) << 16) |
	              (uint32_t(buffer[0]) <<  8) |
	               uint32_t(buffer[1]));
}

} // namespace modm
