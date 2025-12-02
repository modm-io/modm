/*
 * Copyright (c) 2025, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_VL53_TRANSPORT_HPP
#error "Don't include this file directly, use 'bmi088_transport.hpp' instead!"
#endif

namespace modm
{

template<typename SpiMaster, typename Cs, typename Lpn>
uint8_t
Vl53SpiTransport<SpiMaster, Cs, Lpn>::writeMulti(uint16_t RegisterAddress, const uint8_t *p_values,
												 uint32_t size)
{
	addr_buffer[0] = SPI_WRITE_MASK(RegisterAddress) >> 8;
	addr_buffer[1] = SPI_WRITE_MASK(RegisterAddress) & 0xFF;

	modm::this_fiber::poll([&] { return this->acquireMaster(); });
	Cs::reset();
	SpiMaster::transfer(addr_buffer, nullptr, sizeof(addr_buffer));
	SpiMaster::transfer(&p_values[0], nullptr, size);

	if (this->releaseMaster()) { Cs::set(); }
	return 0;
}

template<typename SpiMaster, typename Cs, typename Lpn>
uint8_t
Vl53SpiTransport<SpiMaster, Cs, Lpn>::readMulti(uint16_t RegisterAddress, uint8_t *p_values,
												uint32_t size)
{
	addr_buffer[0] = SPI_READ_MASK(RegisterAddress) >> 8;
	addr_buffer[1] = SPI_READ_MASK(RegisterAddress) & 0xFF;

	modm::this_fiber::poll([&] { return this->acquireMaster(); });
	Cs::reset();
	SpiMaster::transfer(&addr_buffer[0], nullptr, 2);
	SpiMaster::transfer(nullptr, p_values, size);
	if (this->releaseMaster()) { Cs::set(); }
	return 0;
}

template<typename SpiMaster, typename Cs, typename Lpn>
uint8_t
Vl53SpiTransport<SpiMaster, Cs, Lpn>::resetSensor()
{
	Lpn::reset();
	modm::this_fiber::sleep_for(100ms);
	Lpn::set();
	modm::this_fiber::sleep_for(100ms);
	return 0;
}

template<typename I2cMaster, typename Lpn>
uint8_t
Vl53I2cTransport<I2cMaster, Lpn>::writeMulti(uint16_t RegisterAddress, const uint8_t *p_values,
											 uint32_t size)
{
	const auto status = I2cEeprom<I2cMaster, 2>::write(RegisterAddress, p_values, size);
	return static_cast<uint8_t>(not status);
};

template<typename I2cMaster, typename Lpn>
uint8_t
Vl53I2cTransport<I2cMaster, Lpn>::readMulti(uint16_t RegisterAddress, uint8_t *p_values,
											uint32_t size)
{
	uint8_t data_write[2];
	data_write[0] = (RegisterAddress >> 8) & 0xFF;
	data_write[1] = RegisterAddress & 0xFF;

	const auto status = this->writeRead(&data_write[0], 2, p_values, size);
	return static_cast<uint8_t>(not status);
};

template<typename I2cMaster, typename Lpn>
uint8_t
Vl53I2cTransport<I2cMaster, Lpn>::readByte(uint16_t RegisterAddress, uint8_t *p_value)
{
	uint8_t data_write[2];

	data_write[0] = (RegisterAddress >> 8) & 0xFF;
	data_write[1] = RegisterAddress & 0xFF;

	const auto status = this->writeRead(&data_write[0], 2, p_value, 1);

	return static_cast<uint8_t>(not status);
}

template<typename I2cMaster, typename Lpn>
uint8_t
Vl53I2cTransport<I2cMaster, Lpn>::writeByte(uint16_t RegisterAddress, uint8_t value)
{
	const auto status = I2cEeprom<I2cMaster, 2>::write(RegisterAddress, &value, 1);
	return static_cast<uint8_t>(not status);
}

template<typename I2cMaster, typename Lpn>
uint8_t
Vl53I2cTransport<I2cMaster, Lpn>::resetSensor()
{
	Lpn::reset();
	modm::this_fiber::sleep_for(100ms);
	Lpn::set();
	modm::this_fiber::sleep_for(100ms);
	return 0;
}

}  // namespace modm