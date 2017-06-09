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

#include <stdint.h>
#include <modm/architecture/interface/spi_master.hpp>
#include <modm/architecture/driver/delay.hpp>
#include <modm/platform/gpio/connector.hpp>

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
 * @ingroup	spi
 * @author	Niklas Hauser
 * @see		gpio
 */
template< typename Sck,
		  typename Mosi,
		  typename Miso = GpioUnused >
class BitBangSpiMaster : public ::modm::SpiMaster
{
public:
	// start documentation inherited
	template< template<Peripheral _> class... Signals >
	static void
	connect();

	/// Baudrate is limited to 500kbps.
	template< class SystemClock, uint32_t baudrate,
			uint16_t tolerance = Tolerance::FivePercent >
	static void
	initialize();

	static void
	setDataMode(DataMode mode);

	static void
	setDataOrder(DataOrder order);


	static uint8_t
	acquire(void *ctx, ConfigurationHandler handler = nullptr);

	static uint8_t
	release(void *ctx);


	static uint8_t
	transferBlocking(uint8_t data);

	static void
	transferBlocking(uint8_t *tx, uint8_t *rx, std::size_t length);


	static modm::ResumableResult<uint8_t>
	transfer(uint8_t data);

	static modm::ResumableResult<void>
	transfer(uint8_t *tx, uint8_t *rx, std::size_t length);
	// end documentation inherited

private:
	static void
	delay();

	static uint16_t delayTime;

	static uint8_t operationMode;
	static uint8_t count;
	static void *context;
	static ConfigurationHandler configuration;
};

} // namespace platform

} // namespace modm

#include "bitbang_spi_master_impl.hpp"

#endif // MODM_SOFTWARE_BITBANG_SPI_MASTER_HPP
