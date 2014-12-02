// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIPHERAL_SPI_HPP
#define XPCC_PERIPHERAL_SPI_HPP

/**
 * @ingroup 	peripheral
 * @defgroup	spi		Serial Peripheral Interface (SPI)
 */

namespace xpcc
{

/// @ingroup spi
struct Spi
{
	/// Spi Data Mode, Mode0 is the most common mode
	enum class
	DataMode : uint8_t
	{
		Mode0 = 0b00,	///< clock normal,   sample on rising  edge
		Mode1 = 0b01,	///< clock normal,   sample on falling edge
		Mode2 = 0b10,	///< clock inverted, sample on rising  edge
		Mode3 = 0b11,	///< clock inverted, sample on falling edge
	};

	/// Spi Data Order, MsbFirst is the most common mode
	enum class
	DataOrder : uint8_t
	{
		MsbFirst = 0b0,
		LsbFirst = 0b1,
	};
};

} // namespace xpcc

#endif // XPCC_PERIPHERAL_SPI_HPP
