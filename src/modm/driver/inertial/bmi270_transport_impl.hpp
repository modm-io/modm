/*
 * Copyright (c) 2026, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <algorithm>
#ifndef MODM_BMI270_TRANSPORT_HPP
#error "Don't include this file directly, use 'bmi270_transport.hpp' instead!"
#endif

namespace modm
{

// SPI transport -------------------------------------------------------------------------------

template<typename SpiMaster, typename Cs>
void
Bmi270SpiTransport<SpiMaster, Cs>::initialize()
{
	Cs::setOutput(modm::Gpio::High);
}

template<typename SpiMaster, typename Cs>
std::span<uint8_t>
Bmi270SpiTransport<SpiMaster, Cs>::readRegisters(Register startReg, uint8_t count)
{
	if (count > MaxRegisterSequence) { return {}; }

	modm::this_fiber::poll([&] { return this->acquireMaster(); });
	Cs::reset();

	txBuffer_[0] = static_cast<uint8_t>(startReg) | ReadFlag;
	txBuffer_[1] = 0;
	std::fill_n(txBuffer_.begin() + 2, count, 0);
	SpiMaster::transfer(txBuffer_.data(), rxBuffer_.data(), count + 2);

	if (this->releaseMaster()) { Cs::set(); }

	return std::span{&rxBuffer_[2], count};
}

template<typename SpiMaster, typename Cs>
bool
Bmi270SpiTransport<SpiMaster, Cs>::writeRegister(Register reg, uint8_t data)
{
	const std::array<uint8_t, 1> value{data};
	return writeRegisters(reg, std::span{value});
}

template<typename SpiMaster, typename Cs>
bool
Bmi270SpiTransport<SpiMaster, Cs>::writeRegisters(Register startReg, std::span<const uint8_t> data)
{
	if (data.size() > MaxRegisterSequence) { return false; }

	modm::this_fiber::poll([&] { return this->acquireMaster(); });
	Cs::reset();

	txBuffer_[0] = static_cast<uint8_t>(startReg);
	std::copy(data.begin(), data.end(), txBuffer_.begin() + 1);
	SpiMaster::transfer(txBuffer_.data(), nullptr, data.size() + 1);

	if (this->releaseMaster()) { Cs::set(); }

	return true;
}

template<typename SpiMaster, typename Cs>
bool
Bmi270SpiTransport<SpiMaster, Cs>::readFifoData(std::span<uint8_t> data)
{
	if (data.empty()) { return true; }

	modm::this_fiber::poll([&] { return this->acquireMaster(); });
	Cs::reset();

	const std::array<uint8_t, 2> header{
		uint8_t(static_cast<uint8_t>(Register::FifoData) | ReadFlag), 0u};
	std::array<uint8_t, 2> discard{};
	SpiMaster::transfer(header.data(), discard.data(), header.size());

	std::size_t offset = 0;
	while (offset < data.size())
	{
		const std::size_t chunk = std::min<std::size_t>(data.size() - offset, MaxRegisterSequence);
		std::fill_n(txBuffer_.begin(), chunk, 0);
		SpiMaster::transfer(txBuffer_.data(), data.data() + offset, chunk);
		offset += chunk;
	}

	if (this->releaseMaster()) { Cs::set(); }

	return true;
}

// I2C transport -------------------------------------------------------------------------------

template<typename I2cMaster>
Bmi270I2cTransport<I2cMaster>::Bmi270I2cTransport(uint8_t address) : I2cDevice<I2cMaster>(address)
{}

template<typename I2cMaster>
void
Bmi270I2cTransport<I2cMaster>::initialize()
{}

template<typename I2cMaster>
std::span<uint8_t>
Bmi270I2cTransport<I2cMaster>::readRegisters(Register startReg, uint8_t count)
{
	if (count > MaxRegisterSequence) { return {}; }

	uint8_t reg = static_cast<uint8_t>(startReg);
	if (I2cDevice<I2cMaster>::writeRead(&reg, 1, &buffer_[0], count))
	{
		return std::span{&buffer_[0], count};
	}

	return {};
}

template<typename I2cMaster>
bool
Bmi270I2cTransport<I2cMaster>::writeRegister(Register reg, uint8_t data)
{
	const std::array<uint8_t, 1> value{data};
	return writeRegisters(reg, std::span{value});
}

template<typename I2cMaster>
bool
Bmi270I2cTransport<I2cMaster>::writeRegisters(Register startReg, std::span<const uint8_t> data)
{
	if (data.size() > MaxRegisterSequence) { return false; }

	buffer_[0] = static_cast<uint8_t>(startReg);
	std::copy(data.begin(), data.end(), buffer_.begin() + 1);
	return I2cDevice<I2cMaster>::write(buffer_.data(), data.size() + 1);
}

template<typename I2cMaster>
bool
Bmi270I2cTransport<I2cMaster>::readFifoData(std::span<uint8_t> data)
{
	if (data.empty()) { return true; }

	uint8_t reg = static_cast<uint8_t>(Register::FifoData);
	return I2cDevice<I2cMaster>::writeRead(&reg, 1, data.data(), data.size());
}

}  // namespace modm
