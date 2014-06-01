// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SOFTWARE_SIMPLE_SPI_HPP
#define XPCC_SOFTWARE_SIMPLE_SPI_HPP

#include <stdint.h>
#include <xpcc/architecture/peripheral/spi.hpp>
#include <xpcc/architecture/driver/delay.hpp>
#include "../../gpio/software/gpio.hpp"
#include "type_ids.hpp"
#include "../../gpio/software/gpio.hpp"

namespace xpcc
{

/**
 * Software emulation of a Simple Spi.
 *
 * @tparam	SCK			clock pin [output]
 * @tparam	MOSI		master out slave in pin [output]
 * @tparam	MISO		master in slave out pin [input]
 * @tparam	Baudrate	requested SPI baudrate in Hz (default = 2 MHz)
 *
 * @ingroup	spi
 * @author	Niklas Hauser
 * @see		gpio
 */
template< typename SCK,
		  typename MOSI,
		  typename MISO = GpioUnused,
		  uint32_t Baudrate = 2000000UL >
class SoftwareSpiSimpleMaster : public ::xpcc::SpiSimpleMaster
{
public:
	static const TypeId::SoftwareSpiMasterMosi Mosi;
	static const TypeId::SoftwareSpiMasterMiso Miso;
	static const TypeId::SoftwareSpiMasterSck  Sck;

public:
	// start documentation inherited
	template< class clockSource, uint32_t baudrate,
			uint8_t tolerance = xpcc::Tolerance::FivePercent >
	static void
	initialize();

	static void
	setDataMode(DataMode mode);

	static void
	setDataOrder(DataOrder order);

	static uint8_t
	writeReadBlocking(uint8_t data);

	static void
	transferBlocking(uint8_t *tx, uint8_t *rx, std::size_t length);
	// end documentation inherited

private:
	static void
	delay();

	static constexpr uint32_t delayTime = (1000000.0 / Baudrate) / 2.0;

	static uint8_t operationMode;
};

} // namespace xpcc

#include "simple_spi_impl.hpp"

#endif // XPCC_SOFTWARE_SIMPLE_SPI_HPP
