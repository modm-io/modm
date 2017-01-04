/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012-2016, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SOFTWARE_SPI_MASTER_HPP
#define XPCC_SOFTWARE_SPI_MASTER_HPP

#include <stdint.h>
#include <modm/architecture/interface/spi_master.hpp>
#include <modm/architecture/driver/delay.hpp>
#include "../../gpio/generic/gpio.hpp"
#include "type_ids.hpp"

namespace xpcc
{

/**
 * Software emulation of a Simple Spi.
 *
 * @tparam	SCK			clock pin [output]
 * @tparam	MOSI		master out slave in pin [output]
 * @tparam	MISO		master in slave out pin [input]
 *
 * @ingroup	spi
 * @author	Niklas Hauser
 * @see		gpio
 */
template< typename SCK,
		  typename MOSI,
		  typename MISO = GpioUnused >
class SoftwareSpiMaster : public ::xpcc::SpiMaster
{
public:
	static const TypeId::SoftwareSpiMasterMosi Mosi;
	static const TypeId::SoftwareSpiMasterMiso Miso;
	static const TypeId::SoftwareSpiMasterSck  Sck;

public:
	// start documentation inherited
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


	static xpcc::ResumableResult<uint8_t>
	transfer(uint8_t data);

	static xpcc::ResumableResult<void>
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

} // namespace xpcc

#include "spi_master_impl.hpp"

#endif // XPCC_SOFTWARE_SPI_MASTER_HPP
