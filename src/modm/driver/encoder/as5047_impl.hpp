// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2024, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AS5047_HPP
#error "Don't include this file directly, use 'as5047.hpp' instead!"
#endif

namespace modm
{

template<typename SpiMaster, typename Cs>
As5047<SpiMaster, Cs>::As5047(Data &data) : data(data)
{
	this->attachConfigurationHandler([]() { SpiMaster::setDataMode(SpiMaster::DataMode::Mode1); });
	Cs::setOutput(modm::Gpio::High);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
As5047<SpiMaster, Cs>::readout()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());

	Cs::reset();
	outBuffer[1] = static_cast<uint8_t>(Register::ReadAngleunc);
	outBuffer[0] = static_cast<uint8_t>(static_cast<uint16_t>(Register::ReadAngleunc) >> 8);
	RF_CALL(SpiMaster::transfer(outBuffer, inBuffer, 2));
	Cs::set();

	modm::delay_us(1);

	Cs::reset();
	outBuffer[1] = 0;
	outBuffer[0] = 0;
	RF_CALL(SpiMaster::transfer(outBuffer, inBuffer, 2));
	data.data = static_cast<uint16_t>(inBuffer[1]) | (static_cast<uint16_t>(inBuffer[0]) << 8);

	if (this->releaseMaster()) { Cs::set(); }
	RF_END();
}

}  // namespace modm