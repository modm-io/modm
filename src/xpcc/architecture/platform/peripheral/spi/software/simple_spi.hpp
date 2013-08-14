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
#include "type_ids.hpp"

namespace xpcc
{

/**
 * Software emulation of a Simple Spi.
 *
 * @tparam	Clk			clock pin [output]
 * @tparam	Mosi		master out slave in pin [output]
 * @tparam	Miso		master in slave out pin [input]
 * @tparam	Frequency	requested SPI frequency in Hz (default = 2 MHz)
 *
 * @ingroup	spi
 * @author	Niklas Hauser
 * @see		gpio
 */
template< typename Clk,
		  typename Mosi,
		  typename Miso = GpioUnused,
		  uint32_t Frequency = 2000000UL >
class SoftwareSimpleSpi : public ::xpcc::SimpleSpi
{
public:
	static const TypeId::SoftwareSpiMaster Id;

	static void
	initialize();

	static uint8_t
	writeReadBlocking(uint8_t data);

	static ALWAYS_INLINE void
	writeBlocking(uint8_t data);

	static ALWAYS_INLINE bool
	write(uint8_t data);

	static ALWAYS_INLINE uint8_t
	getResult();

	static inline bool
	transfer(uint8_t *tx, uint8_t *rx, std::size_t length, BufferOptions options=BufferOptions::TxRxIncrement);

	static ALWAYS_INLINE bool
	isFinished();

protected:
	static ALWAYS_INLINE void
	delay();

	static constexpr uint32_t delayTime = (1000000.0 / Frequency) / 2.0;

	static bool finished;
	static uint8_t result;
};

} // namespace xpcc

#include "simple_spi_impl.hpp"

#endif // XPCC_SOFTWARE_SIMPLE_SPI_HPP
