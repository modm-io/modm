/*
 * Copyright (c) 2026, Joel Schulz-Andres
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BMP581_TRANSPORT_HPP
#error "Don't include this file directly, use 'bmp581_transport.hpp' instead!"
#endif

namespace modm
{

// ----------------------------------------------------------------------------
// I2C Transport Implementation
// ----------------------------------------------------------------------------

template<typename I2cMaster>
Bmp581I2cTransport<I2cMaster>::Bmp581I2cTransport(uint8_t address)
	: I2cDevice<I2cMaster>(address)
{}

template<typename I2cMaster>
bool
Bmp581I2cTransport<I2cMaster>::initialize()
{
	return true;
}

template<typename I2cMaster>
bool
Bmp581I2cTransport<I2cMaster>::read(uint8_t reg, uint8_t* data, std::size_t length)
{
	uint8_t reg_addr = reg;
	return I2cDevice<I2cMaster>::writeRead(&reg_addr, 1, data, length);
}

template<typename I2cMaster>
bool
Bmp581I2cTransport<I2cMaster>::write(uint8_t reg, uint8_t data)
{
	uint8_t buffer[]{reg, data};
	return I2cDevice<I2cMaster>::write(buffer, sizeof(buffer));
}

template<typename I2cMaster>
bool
Bmp581I2cTransport<I2cMaster>::write(uint8_t reg, const uint8_t* data, std::size_t length)
{
	if (length > sizeof(buffer_) - 1) { return false; }
	buffer_[0] = reg;
	for (std::size_t i = 0; i < length; ++i) { buffer_[i + 1] = data[i]; }
	return I2cDevice<I2cMaster>::write(&buffer_[0], length + 1);
}

// ----------------------------------------------------------------------------
// SPI Transport Implementation
// ----------------------------------------------------------------------------

template<typename SpiMaster, typename Cs>
Bmp581SpiTransport<SpiMaster, Cs>::Bmp581SpiTransport()
{}

template<typename SpiMaster, typename Cs>
bool
Bmp581SpiTransport<SpiMaster, Cs>::initialize()
{
	Cs::setOutput(true);
	return true;
}

template<typename SpiMaster, typename Cs>
bool
Bmp581SpiTransport<SpiMaster, Cs>::read(uint8_t reg, uint8_t* data, std::size_t length)
{
	modm::this_fiber::poll([this] { return this->acquireMaster(); });
	Cs::reset();

	uint8_t cmd = reg | ReadFlag;
	SpiMaster::transfer(&cmd, nullptr, 1);
	SpiMaster::transfer(nullptr, data, length);

	if (this->releaseMaster()) { Cs::set(); }

	return true;
}

template<typename SpiMaster, typename Cs>
bool
Bmp581SpiTransport<SpiMaster, Cs>::write(uint8_t reg, uint8_t data)
{
	modm::this_fiber::poll([this] { return this->acquireMaster(); });
	Cs::reset();

	uint8_t buffer[]{uint8_t(reg & ~ReadFlag), data};
	SpiMaster::transfer(buffer, nullptr, sizeof(buffer));

	if (this->releaseMaster()) { Cs::set(); }

	return true;
}

template<typename SpiMaster, typename Cs>
bool
Bmp581SpiTransport<SpiMaster, Cs>::write(uint8_t reg, const uint8_t* data, std::size_t length)
{
	modm::this_fiber::poll([this] { return this->acquireMaster(); });
	Cs::reset();

	uint8_t cmd = reg & ~ReadFlag;
	SpiMaster::transfer(&cmd, nullptr, 1);
	SpiMaster::transfer(data, nullptr, length);

	if (this->releaseMaster()) { Cs::set(); }

	return true;
}

}  // namespace modm
