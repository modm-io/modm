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
#define MODM_BMP581_TRANSPORT_HPP

#include <cstdint>
#include <concepts>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/processing/fiber.hpp>

namespace modm
{

/// @ingroup modm_driver_bmp581
/// @{

/**
 * Concept for BMP581 transport layer
 *
 * A transport must provide methods for reading and writing registers.
 */
template<typename T>
concept Bmp581Transport = requires(T transport, uint8_t reg, uint8_t data,
                                    uint8_t* buffer, std::size_t length)
{
	{ transport.initialize() } -> std::same_as<bool>;
	{ transport.read(reg, buffer, length) } -> std::same_as<bool>;
	{ transport.write(reg, data) } -> std::same_as<bool>;
	{ transport.write(reg, buffer, length) } -> std::same_as<bool>;
};

/**
 * BMP581 I2C Transport Layer
 *
 * Implements register read/write operations over I2C.
 *
 * @tparam I2cMaster I2C master peripheral
 */
template<typename I2cMaster>
class Bmp581I2cTransport : public modm::I2cDevice<I2cMaster, 4>
{
public:
	/**
	 * Constructor
	 * @param address I2C address (0x46 with SDO to GND, 0x47 with SDO to VDDIO)
	 */
	Bmp581I2cTransport(uint8_t address = 0x46);

	/// Initialize the transport layer
	bool
	initialize();

	/// Read one or more registers starting at the given address
	bool
	read(uint8_t reg, uint8_t* buffer, std::size_t length);

	/// Write a single byte to a register
	bool
	write(uint8_t reg, uint8_t data);

	/// Write multiple bytes starting at the given register address
	bool
	write(uint8_t reg, const uint8_t* data, std::size_t length);

protected:
	uint8_t buffer_[8];
};

/**
 * BMP581 SPI Transport Layer
 *
 * Implements register read/write operations over SPI.
 * SPI Mode 0 (CPOL=0, CPHA=0) is used.
 * Maximum SPI clock: 10 MHz.
 *
 * @tparam SpiMaster SPI master peripheral
 * @tparam Cs Chip select GPIO pin
 */
template<typename SpiMaster, typename Cs>
class Bmp581SpiTransport : public modm::SpiDevice<SpiMaster>
{
public:
	/**
	 * Constructor
	 */
	Bmp581SpiTransport();

	/// Initialize the transport layer (configures CS pin)
	bool
	initialize();

	/// Read one or more registers starting at the given address
	bool
	read(uint8_t reg, uint8_t* buffer, std::size_t length);

	/// Write a single byte to a register
	bool
	write(uint8_t reg, uint8_t data);

	/// Write multiple bytes starting at the given register address
	bool
	write(uint8_t reg, const uint8_t* data, std::size_t length);

protected:
	/// SPI read flag (bit 7 set for read operations)
	static constexpr uint8_t ReadFlag = 0x80;

	uint8_t buffer_[8];
};

/// @}

} // namespace modm

#include "bmp581_transport_impl.hpp"

#endif // MODM_BMP581_TRANSPORT_HPP
