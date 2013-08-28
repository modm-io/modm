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
#include <xpcc/architecture/peripheral/spi.hpp>
#include <xpcc/architecture/driver/delay.hpp>
#include "type_ids.hpp"
#include "simple_spi.hpp"

namespace xpcc
{

/**
 * Software emulation of a Spi Master.
 *
 * @tparam	SCK			clock pin [output]
 * @tparam	MOSI		master out slave in pin [output]
 * @tparam	MISO		master in slave out pin [input]
 * @tparam	Frequency	requested SPI frequency in Hz (default = 2 MHz)
 *
 * @ingroup	spi
 * @author	Niklas Hauser
 * @see		gpio
 */
template< typename SCK,
		  typename MOSI,
		  typename MISO = GpioUnused,
		  uint32_t Baudrate = 2000000UL >
class SoftwareSpiMaster : public ::xpcc::SpiMaster
{
public:
	static const TypeId::SoftwareSpiMasterMosi Mosi;
	static const TypeId::SoftwareSpiMasterMiso Miso;
	static const TypeId::SoftwareSpiMasterSck  Sck;

public:
	// start documentation inherited
	template< uint32_t baudrate >
	static inline void
	initialize();

	static ALWAYS_INLINE bool
	start(SpiDelegate *delegate);

	static bool
	startBlocking(SpiDelegate *delegate);

	static inline void
	reset(DetachCause cause = DetachCause::SoftwareReset);
	// end documentation inherited

private:
	static inline void
	delay();

	static uint8_t
	writeReadBlocking(uint8_t data);

	static constexpr uint32_t delayTime = (1000000.0 / Baudrate) / 2.0;
	static uint8_t timingMode;

	static xpcc::SpiDelegate *myDelegate;
};

} // namespace xpcc

#include "spi_master_impl.hpp"

#endif // XPCC_SOFTWARE_SPI_MASTER_HPP
