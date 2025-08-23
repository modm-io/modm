// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ADS816X_HPP
#	error "Don't include this file directly! Use 'ads816x.hpp' instead."
#endif

//#include <bit>
//#include <span>

namespace modm
{

template <typename SpiMaster, typename Cs>
void
Ads816x<SpiMaster, Cs>::initialize(Mode mode)
{
	// Unlock register access by writing magic byte
	registerAccess(Command::Write, Register::REG_ACCESS, 0b1010'1010);

	// Power up everything except the internal REFby2 buffer
	registerAccess(Command::Write, Register::PD_CNTL, 0b0001'0000);

	// Data type: Only ADC value
	registerAccess(Command::Write, Register::DATA_CNTL, uint8_t(DataFormat::OnlyResult));

	// V_ref = 4.096V (internal reference)
	registerAccess(Command::Write, Register::OFST_CAL, 0b010);

	// AIN0 ... AIN7 are separate channels
	registerAccess(Command::Write, Register::AIN_CFG, 0x00);

	// All individual channels are single-ended inputs; connect the AIN-COM pin to GND
	registerAccess(Command::Write, Register::COM_CFG, 0x00);

	// Set mode
	registerAccess(Command::Write, Register::DEVICE_CFG, uint8_t(mode));
}

template <typename SpiMaster, typename Cs>
uint16_t
Ads816x<SpiMaster, Cs>::manualModeConversion(uint8_t afterNextChannel)
{
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	buffer[0] = uint8_t(Command::Write) | (uint16_t(Register::CHANNEL_ID) >> 8);
	buffer[1] = uint16_t(Register::CHANNEL_ID);
	buffer[2] = (afterNextChannel & 0b0000'0111);

	SpiMaster::transfer(buffer, buffer2, 3);

	if (this->releaseMaster()) {
		Cs::set();
	}

	return static_cast<uint16_t>((static_cast<uint16_t>(buffer2[0]) << 8) | buffer2[1]);
}

/*
// FIXME: Does not work correctly...
template <typename SpiMaster, typename Cs>
template<std::size_t N>
void
Ads816x<SpiMaster, Cs>::autoSequenceConversion(uint8_t channelsBitmask, std::span<uint16_t, N> result)
{
	if (std::popcount(channelsBitmask) > N) {
		return;
	}

	// Write channel bitmask config (AUTO_SEQ_CH)
	registerAccess(Command::Write, Register::AUTO_SEQ_CFG1, channelsBitmask);

	modm::this_fiber::sleep_for(tConv);

	// Set SEQ_START (0b1 << 0) bit
	registerAccess(Command::SetBits, Register::SEQ_START, 0b1);

	// Read conversion results
	for (buffer2[0] = 0; buffer2[0] < std::popcount(channelsBitmask); buffer2[0]++) {
		modm::this_fiber::sleep_for(tConv);

		modm::this_fiber::poll([&]{ return this->acquireMaster(); });
		Cs::reset();

		SpiMaster::transfer(nullptr, buffer, 2);

		if (this->releaseMaster()) {
			Cs::set();
		}

		result[buffer2[0]] = buffer[0] << 8 | buffer[1];
	}
}
*/

template <typename SpiMaster, typename Cs>
uint8_t
Ads816x<SpiMaster, Cs>::registerAccess(Command command, Register reg, uint8_t value)
{
	buffer[0] = uint8_t(command) | (uint16_t(reg) >> 8);
	buffer[1] = uint16_t(reg);
	buffer[2] = value;

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	SpiMaster::transfer(buffer, nullptr, 3);

	if (command == Command::Read) {
		Cs::set();
		modm::this_fiber::sleep_for(tConv);

		// To retrieve the result of the read command we issue another 3 byte
		// transfer, nullptr instead of manual NoOperation (= 0b000...) command
		Cs::reset();
		SpiMaster::transfer(nullptr, buffer, 3);
	}

	if (this->releaseMaster()) {
		Cs::set();
	}

	return buffer[0];
}

} // namespace modm
