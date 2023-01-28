/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2015, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_SPI_HPP
#define MODM_INTERFACE_SPI_HPP

#include <modm/architecture/interface/peripheral.hpp>

namespace modm
{

/// @ingroup modm_architecture_spi
struct Spi
{
	/// The signature of the configuration function.
	using ConfigurationHandler = void(*)();

	/// Spi Data Mode, Mode0 is the most common mode
	enum class
	DataMode : uint8_t
	{
		Mode0 = 0b00,	///< clock normal,   sample on rising  edge
		Mode1 = 0b01,	///< clock normal,   sample on falling edge
		Mode2 = 0b10,	///< clock inverted, sample on falling edge
		Mode3 = 0b11,	///< clock inverted, sample on rising  edge
	};

	/// Spi Data Order, MsbFirst is the most common mode
	enum class
	DataOrder : uint8_t
	{
		MsbFirst = 0b0,
		LsbFirst = 0b1,
	};
};

} // namespace modm

#endif // MODM_INTERFACE_SPI_HPP
