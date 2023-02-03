/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AT24MAC402_HPP
#	error	"Don't include this file directly, use 'at24mac402_impl.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

template<typename I2cMaster>
modm::At24Mac402<I2cMaster>::At24Mac402(uint8_t address)
{
	setAddress(address);
}

template<typename I2cMaster>
void
modm::At24Mac402<I2cMaster>::setAddress(uint8_t address)
{
	address_ = (address & 0b111) | 0b1010'000;
	Base::setAddress(address_);
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::At24Mac402<I2cMaster>::readMac(std::span<uint8_t, 6> data)
{
	Base::setAddress(address_ | 0b1'000);
	return Base::read(MacAddress, &data[0], 6);
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::At24Mac402<I2cMaster>::readUniqueId(std::span<uint8_t, 16> data)
{
	Base::setAddress(address_ | 0b1'000);
	return Base::read(UuidAddress, &data[0], 16);
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::At24Mac402<I2cMaster>::ping()
{
	Base::setAddress(address_);
	return Base::ping();
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::At24Mac402<I2cMaster>::writeByte(uint32_t address, uint8_t data)
{
	Base::setAddress(address_);
	return Base::writeByte(address, data);
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::At24Mac402<I2cMaster>::write(uint32_t address, const uint8_t* data, std::size_t length)
{
	Base::setAddress(address_);
	return Base::write(address, data, length);
}

template<typename I2cMaster>
template<typename T>
modm::ResumableResult<bool>
modm::At24Mac402<I2cMaster>::write(uint32_t address, const T& data)
{
	Base::setAddress(address_);
	return Base::write(address, data);
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::At24Mac402<I2cMaster>::readByte(uint32_t address, uint8_t& data)
{
	Base::setAddress(address_);
	return Base::readByte(address, data);
}

template<typename I2cMaster>
modm::ResumableResult<bool>
modm::At24Mac402<I2cMaster>::read(uint32_t address, uint8_t* data, std::size_t length)
{
	Base::setAddress(address_);
	return Base::read(address, data, length);
}

template<typename I2cMaster>
template<typename T>
modm::ResumableResult<bool>
modm::At24Mac402<I2cMaster>::read(uint32_t address, T& data)
{
	Base::setAddress(address_);
	return read(address, reinterpret_cast<uint8_t*>(&data), sizeof(T));
}
