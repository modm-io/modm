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
	this->attachConfigurationHandler([]() { SpiMaster::setDataMode(SpiMaster::DataMode::Mode3); });
	Cs::setOutput(modm::Gpio::High);
}

template<typename SpiMaster, typename Cs, max31865::Pt pt>
modm::ResumableResult<void>
Max31865<SpiMaster, Cs, pt>::initialize()
{
	RF_BEGIN();
	config = Config();
	config.set(Config::FaultStatusClear);
	config.set(Rejection_t(Rejection::Rejection50Hz));
	RF_CALL(writeSingleRegister(Register::WriteConfiguration, config.value));
	config = Config_t(RF_CALL(readSingleRegister(Register::ReadConfiguration)));
	RF_END();
}

template<typename SpiMaster, typename Cs, max31865::Pt pt>
modm::ResumableResult<void>
Max31865<SpiMaster, Cs, pt>::readout()
{
	RF_BEGIN();
	config.set(Config::VBias);
	RF_CALL(writeSingleRegister(Register::WriteConfiguration, config.value));
	timeout.restart(10ms);
	RF_WAIT_UNTIL(timeout.isExpired());

	config.set(Config::OneShot);
	RF_CALL(writeSingleRegister(Register::WriteConfiguration, config.value));
	timeout.restart(65ms);
	RF_WAIT_UNTIL(timeout.isExpired());

	// data.data = RF_CALL(readTwoRegisters(Register::ReadRtdMsb));
	d = RF_CALL(readTwoRegisters(Register::ReadRtdMsb));
	data.data = d;

	config.reset(Config::VBias);
	RF_CALL(writeSingleRegister(Register::WriteConfiguration, config.value));

	RF_END();
}

template<typename SpiMaster, typename Cs, max31865::Pt pt>
modm::ResumableResult<uint8_t>
Max31865<SpiMaster, Cs, pt>::readSingleRegister(Register address)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	Cs::reset();
	buffer[0] = uint8_t(address);
	RF_CALL(SpiMaster::transfer(buffer.data(), nullptr, 1));
	RF_CALL(SpiMaster::transfer(nullptr, buffer.data(), 1));

	if (this->releaseMaster()) { Cs::set(); }

	RF_END_RETURN(buffer[0]);
}

template<typename SpiMaster, typename Cs, max31865::Pt pt>
modm::ResumableResult<uint16_t>
Max31865<SpiMaster, Cs, pt>::readTwoRegisters(Register address)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	Cs::reset();
	buffer[0] = uint8_t(address);
	RF_CALL(SpiMaster::transfer(buffer.data(), nullptr, 1));
	RF_CALL(SpiMaster::transfer(nullptr, buffer.data(), 2));

	if (this->releaseMaster()) { Cs::set(); }

	RF_END_RETURN(static_cast<uint16_t>(buffer[0] << 8 | buffer[1]));
}

template<typename SpiMaster, typename Cs, max31865::Pt pt>
modm::ResumableResult<void>
Max31865<SpiMaster, Cs, pt>::writeSingleRegister(Register address, uint8_t data)
{
	RF_BEGIN();
	RF_WAIT_UNTIL(this->acquireMaster());

	Cs::reset();
	buffer[0] = uint8_t(address);
	buffer[1] = data;
	RF_CALL(SpiMaster::transfer(buffer.data(), nullptr, 2));

	if (this->releaseMaster()) { Cs::set(); }

	RF_END();
}

}  // namespace modm