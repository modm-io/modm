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
#define MODM_VL53_TRANSPORT_HPP

#include <cstdint>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/driver/storage/i2c_eeprom.hpp>
#include <modm/processing/fiber.hpp>

namespace modm
{

class Vl53TransportBase
{
public:
	virtual ~Vl53TransportBase() = default;

	virtual uint8_t
	writeMulti(uint16_t reg, const uint8_t *values, uint32_t size) = 0;
	virtual uint8_t
	readMulti(uint16_t reg, uint8_t *values, uint32_t size) = 0;
	virtual uint8_t
	writeByte(uint16_t reg, uint8_t value) = 0;
	virtual uint8_t
	readByte(uint16_t reg, uint8_t *value) = 0;
	virtual uint8_t
	resetSensor() = 0;
};

template<typename SpiMaster, typename Cs, typename Lpn>
class Vl53SpiTransport : public modm::SpiDevice<SpiMaster>, public Vl53TransportBase
{
public:
	Vl53SpiTransport()
	{
		this->attachConfigurationHandler([] {
			SpiMaster::setDataMode(SpiMaster::DataMode::Mode3);
			SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
			SpiMaster::setDataSize(SpiMaster::DataSize::Bit8);
		});
	};

	Vl53SpiTransport(const Vl53SpiTransport &) = delete;

	Vl53SpiTransport &
	operator=(const Vl53SpiTransport &) = delete;

	uint8_t
	writeMulti(uint16_t register_address, const uint8_t *p_values, uint32_t size) override;

	uint8_t
	readMulti(uint16_t register_address, uint8_t *p_values, uint32_t size) override;

	uint8_t
	readByte(uint16_t register_address, uint8_t *p_value) override
	{
		return this->readMulti(register_address, p_value, 1);
	}

	uint8_t
	writeByte(uint16_t register_address, uint8_t value) override
	{
		return this->writeMulti(register_address, &value, 1);
	}

	uint8_t
	resetSensor() override;

private:
	uint8_t addr_buffer[2];

	static uint16_t
	SPI_WRITE_MASK(uint16_t x) noexcept
	{
		return static_cast<uint16_t>(x | 0x8000);
	}

	static uint16_t
	SPI_READ_MASK(uint16_t x) noexcept
	{
		return static_cast<uint16_t>(x & static_cast<uint16_t>(~0x8000));
	}
};

template<typename I2cMaster, typename Lpn>
class Vl53I2cTransport : public I2cEeprom<I2cMaster, 2>, public Vl53TransportBase
{
public:
	Vl53I2cTransport(const uint8_t address) : I2cEeprom<I2cMaster, 2>{address} {};

	Vl53I2cTransport(const Vl53I2cTransport &) = delete;

	Vl53I2cTransport &
	operator=(const Vl53I2cTransport &) = delete;

	uint8_t
	writeMulti(uint16_t register_address, const uint8_t *p_values, uint32_t size) override;

	uint8_t
	readMulti(uint16_t register_address, uint8_t *p_values, uint32_t size) override;

	inline uint8_t
	readByte(uint16_t register_address, uint8_t *p_value) override;

	inline uint8_t
	writeByte(uint16_t register_address, uint8_t value) override;

	uint8_t
	resetSensor() override;
};

}  // namespace modm

#include "vl53_transport_impl.hpp"

#endif
