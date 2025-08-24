/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012-2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SOFTWARE_BITBANG_SPI_MASTER_HPP
#define MODM_SOFTWARE_BITBANG_SPI_MASTER_HPP

#include <cstdint>
#include <modm/architecture/interface/spi_lock.hpp>
#include <modm/architecture/interface/spi_master.hpp>
#include <modm/architecture/interface/delay.hpp>
#include <modm/platform/gpio/connector.hpp>
#include <modm/processing/fiber.hpp>

namespace modm
{

namespace platform
{

/**
 * Software emulation of a Simple Spi.
 *
 * @tparam	Sck			clock pin [output]
 * @tparam	Mosi		master out slave in pin [output]
 * @tparam	Miso		master in slave out pin [input]
 *
 * @ingroup	modm_platform_spi_bitbang
 * @author	Niklas Hauser
 * @see		gpio
 */
template< typename Sck,
		  typename Mosi,
		  typename Miso = GpioUnused >
class BitBangSpiMaster : public ::modm::SpiMaster,
                         public SpiLock<BitBangSpiMaster<Sck, Mosi, Miso>>
{
public:
	template< class... Signals >
	static void
	connect();

	/// Baudrate is limited to 500kbps.
	template< class SystemClock, baudrate_t baudrate, percent_t tolerance=pct(5) >
	static void
	initialize();

	static void
	setDataMode(DataMode mode);

	static void
	setDataOrder(DataOrder order);

	[[deprecated("Use transfer() instead!")]] // DEPRECATED: 2026q3
	static uint8_t
	transferBlocking(uint8_t data);

	[[deprecated("Use transfer() instead!")]] // DEPRECATED: 2026q3
	static void
	transferBlocking(const uint8_t *tx, uint8_t *rx, std::size_t length);

	static uint8_t
	transfer(uint8_t data);

	static void
	transfer(const uint8_t *tx, uint8_t *rx, std::size_t length);

private:
	static void
	delay();

	static inline uint16_t delayTime{1};
	static inline uint8_t operationMode{0};
};

} // namespace platform

} // namespace modm

#include "bitbang_spi_master_impl.hpp"

#endif // MODM_SOFTWARE_BITBANG_SPI_MASTER_HPP
