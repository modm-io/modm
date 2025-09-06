// coding: utf-8
/*
 * Copyright (c) 2022, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ADIS16470_HPP
	#error	"Don't include this file directly, use 'adis16470.hpp' instead!"
#endif

#include <modm/math/utils.hpp>

namespace modm
{

template<class SpiMaster, class Cs>
void
Adis16470<SpiMaster, Cs>::initialize()
{
	this->attachConfigurationHandler([]
	{
		SpiMaster::setDataMode(SpiMaster::DataMode::Mode3);
		SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
	});

	Cs::setOutput(modm::Gpio::High);
	timeout.restart(tStall);
}


template<class SpiMaster, class Cs>
std::optional<uint16_t>
Adis16470<SpiMaster, Cs>::readRegister(Register reg)
{
	if (getRegisterAccess(reg) == AccessMethod::Write) {
		// Reading this register is not permitted
		return std::nullopt;
	}

	// Ensure CS was not asserted for T_stall
	timeout.wait();

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	buffer[0] = uint8_t(reg) & 0b0111'1111;
	buffer[1] = 0;
	SpiMaster::transfer(buffer.data(), nullptr, 2);

	if (this->releaseMaster()) {
		Cs::set();
	}
	modm::this_fiber::sleep_for(tStall);
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	SpiMaster::transfer(buffer.data(), &buffer[2], 2);

	if (this->releaseMaster()) {
		Cs::set();
	}
	timeout.restart(tStall);

	return (static_cast<uint16_t>(buffer[2]) << 8) | buffer[3];
}

template<class SpiMaster, class Cs>
modm::adis16470::DiagStat_t
Adis16470<SpiMaster, Cs>::readDiagStat()
{
	tmp = readRegister(Register::DIAG_STAT);
	return DiagStat_t(*tmp);
}

template<class SpiMaster, class Cs>
modm::adis16470::MscCtrl_t
Adis16470<SpiMaster, Cs>::readMscCtrl()
{
	tmp = readRegister(Register::MSC_CTRL);
	return MscCtrl_t(*tmp);
}

template<class SpiMaster, class Cs>
bool
Adis16470<SpiMaster, Cs>::writeRegister(Register reg, uint16_t value)
{
	if (getRegisterAccess(reg) == AccessMethod::Read) {
		// Writing to this register is not permitted
		return false;
	}

	// Ensure CS was not asserted for T_stall
	timeout.wait();

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	buffer[0] = (uint8_t(reg) & 0b0111'1111) | 0b1000'0000;
	buffer[1] = static_cast<uint8_t>(value);
	SpiMaster::transfer(buffer.data(), nullptr, 2);

	if (this->releaseMaster()) {
		Cs::set();
	}
	modm::this_fiber::sleep_for(tStall);
	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	buffer[0] = ((uint8_t(reg) + 1) & 0b0111'1111) | 0b1000'0000;
	buffer[1] = static_cast<uint8_t>(value >> 8);
	SpiMaster::transfer(buffer.data(), nullptr, 2);

	if (this->releaseMaster()) {
		Cs::set();
	}
	timeout.restart(tStall);

	return true;
}

template<class SpiMaster, class Cs>
void
Adis16470<SpiMaster, Cs>::writeMscCtrl(modm::adis16470::MscCtrl_t value)
{
	writeRegister(Register::MSC_CTRL, value.value);

	// Writing to MSC_CTRL take approx. 3ms
	timeout.restart(3ms);
}

template<class SpiMaster, class Cs>
void
Adis16470<SpiMaster, Cs>::writeGlobCmd(modm::adis16470::GlobCmd_t value)
{
	writeRegister(Register::GLOB_CMD, value.value);
}

template<class SpiMaster, class Cs>
template<frequency_t frequency, percent_t tolerance>
void
Adis16470<SpiMaster, Cs>::setDataOutputFrequency()
{
	// Output data rate R = 2000SPS / (DEC_RATE + 1)
	constexpr uint16_t decRate = ((2000 / frequency) - 1) & 0b111'1111'1111;
	constexpr uint16_t actualFrequency = 2000 / (decRate + 1);

	static_assert(frequency < 2000, "Maximum data output rate is 2000Hz");
	modm::PeripheralDriver::assertBaudrateInTolerance<actualFrequency, frequency, tolerance>();
	writeRegister(Register::DEC_RATE, decRate);
}

template<class SpiMaster, class Cs>
bool
Adis16470<SpiMaster, Cs>::readRegisterSequence(std::span<const Register> sequence, std::span<uint16_t> values)
{
	if(sequence.size() != values.size()) {
		// Mismatching std::span sizes
		return false;
	}

	// Ensure CS was not asserted for T_stall
	timeout.wait();

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	for (i = 0; i < sequence.size(); i++) {
		if (getRegisterAccess(sequence[i]) == AccessMethod::Write) {
			// Reading this register is not permitted
			if (this->releaseMaster()) {
				Cs::set();
			}
			timeout.restart(tStall);
			return false;
		}

		buffer[0] = uint8_t(sequence[i]) & 0b0111'1111;
		buffer[1] = 0;
		SpiMaster::transfer(buffer.data(), &buffer[2], 2);

		if (this->releaseMaster()) {
			Cs::set();
		}

		modm::this_fiber::sleep_for(tStall);

		modm::this_fiber::poll([&]{ return this->acquireMaster(); });
		Cs::reset();

		if (i != 0) {
			values[i-1] = (static_cast<uint16_t>(buffer[2]) << 8) | buffer[3];
		}
	}

	// one additionl transfer to retrieve value of last register
	buffer[0] = uint8_t(sequence[0]) & 0b0111'1111;
	buffer[1] = 0;
	SpiMaster::transfer(buffer.data(), &buffer[2], 2);
	values[values.size()-1] = (static_cast<uint16_t>(buffer[2]) << 8) | buffer[3];

	if (this->releaseMaster()) {
		Cs::set();
	}
	timeout.restart(tStall);

	return true;
}

template<class SpiMaster, class Cs>
bool
Adis16470<SpiMaster, Cs>::readRegisterBurst(std::array<uint16_t, 11>& data)
{
	buffer.fill(0);
	buffer[0] = 0x68;

	modm::this_fiber::poll([&]{ return this->acquireMaster(); });
	Cs::reset();

	SpiMaster::transfer(buffer.data(), reinterpret_cast<uint8_t*>(data.data()), 22);

	if (this->releaseMaster()) {
		Cs::set();
	}
	timeout.restart(tStall);

	// Calulate checksum
	checksum = 0;
	for (i = 0; i < 18; i++) {
		checksum += reinterpret_cast<uint8_t*>(data.data() + 1)[i];
	}

	// Fix endianness
	for (i = 1; i < 11; i++) {
		data[i] = modm::fromBigEndian(data[i]);
	}

	return checksum == data[10];
}

} // namespace modm
