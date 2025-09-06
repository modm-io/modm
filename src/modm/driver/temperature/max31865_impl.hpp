// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2023, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MAX31865_HPP
#error "Don't include this file directly, use 'max31865.hpp' instead!"
#endif

namespace modm
{

template<typename SpiMaster, typename Cs, max31865::Pt pt>
Max31865<SpiMaster, Cs, pt>::Max31865(Data &data) : data(data)
{
	this->attachConfigurationHandler([]{ SpiMaster::setDataMode(SpiMaster::DataMode::Mode3); });
	Cs::setOutput(modm::Gpio::High);
}

template<typename SpiMaster, typename Cs, max31865::Pt pt>
void
Max31865<SpiMaster, Cs, pt>::initialize()
{
	config = Config();
	config.set(Config::FaultStatusClear);
	config.set(Rejection_t(Rejection::Rejection50Hz));
	writeSingleRegister(Register::WriteConfiguration, config.value);
	config = Config_t(readSingleRegister(Register::ReadConfiguration));
}

template<typename SpiMaster, typename Cs, max31865::Pt pt>
void
Max31865<SpiMaster, Cs, pt>::readout()
{
	config.set(Config::VBias);
	writeSingleRegister(Register::WriteConfiguration, config.value);
	modm::this_fiber::sleep_for(10ms);

	config.set(Config::OneShot);
	writeSingleRegister(Register::WriteConfiguration, config.value);
	modm::this_fiber::sleep_for(65ms);

	// data.data = readTwoRegisters(Register::ReadRtdMsb);
	d = readTwoRegisters(Register::ReadRtdMsb);
	data.data = d;

	config.reset(Config::VBias);
	writeSingleRegister(Register::WriteConfiguration, config.value);
}

template<typename SpiMaster, typename Cs, max31865::Pt pt>
uint8_t
Max31865<SpiMaster, Cs, pt>::readSingleRegister(Register address)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });

	Cs::reset();
	buffer[0] = uint8_t(address);
	SpiMaster::transfer(buffer.data(), nullptr, 1);
	SpiMaster::transfer(nullptr, buffer.data(), 1);

	if (this->releaseMaster()) { Cs::set(); }

	return buffer[0];
}

template<typename SpiMaster, typename Cs, max31865::Pt pt>
uint16_t
Max31865<SpiMaster, Cs, pt>::readTwoRegisters(Register address)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });

	Cs::reset();
	buffer[0] = uint8_t(address);
	SpiMaster::transfer(buffer.data(), nullptr, 1);
	SpiMaster::transfer(nullptr, buffer.data(), 2);

	if (this->releaseMaster()) { Cs::set(); }

	return static_cast<uint16_t>(buffer[0] << 8 | buffer[1]);
}

template<typename SpiMaster, typename Cs, max31865::Pt pt>
void
Max31865<SpiMaster, Cs, pt>::writeSingleRegister(Register address, uint8_t data)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });

	Cs::reset();
	buffer[0] = uint8_t(address);
	buffer[1] = data;
	SpiMaster::transfer(buffer.data(), nullptr, 2);

	if (this->releaseMaster()) { Cs::set(); }
}

}  // namespace modm
