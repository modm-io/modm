// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SOFTWARE_SPI_MASTER_HPP
#define XPCC_SOFTWARE_SPI_MASTER_HPP

#include <stdint.h>
#include <xpcc/architecture/interface/spi_master.hpp>
#include <xpcc/architecture/driver/delay.hpp>
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
	template< class clockSource, uint32_t baudrate,
			uint16_t tolerance = Tolerance::FivePercent >
	static void
	initialize();


	static void
	setDataMode(DataMode mode);

	static void
	setDataOrder(DataOrder order);


	static uint8_t
	aquire(void *ctx, ConfigurationHandler handler = nullptr);

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
