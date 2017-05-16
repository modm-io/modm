/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011-2013, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AD840x_HPP
#	error "Don't include this file directly, use 'ad840x.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename Spi, typename Cs, typename Rs, typename Shdn>
void
modm::AD840x<Spi, Cs, Rs, Shdn>::initialize()
{
//	Spi::initialize();
	Cs::setOutput(modm::Gpio::High);
	Rs::setOutput(modm::Gpio::High);
	Shdn::setOutput(modm::Gpio::High);
}

// ----------------------------------------------------------------------------
template<typename Spi, typename Cs, typename Rs, typename Shdn>
inline void
modm::AD840x<Spi, Cs, Rs, Shdn>::reset()
{
	Rs::reset();
	modm::delayMicroseconds(1);		// wait at least 50ns
	Rs::set();
}

// ----------------------------------------------------------------------------
template<typename Spi, typename Cs, typename Rs, typename Shdn>
inline void
modm::AD840x<Spi, Cs, Rs, Shdn>::shutdown()
{
	Shdn::reset();
}

template<typename Spi, typename Cs, typename Rs, typename Shdn>
inline void
modm::AD840x<Spi, Cs, Rs, Shdn>::resume()
{
	Shdn::set();
}

// ----------------------------------------------------------------------------
template<typename Spi, typename Cs, typename Rs, typename Shdn>
void
modm::AD840x<Spi, Cs, Rs, Shdn>::setValue(ad840x::Channel channel, uint8_t data)
{
	Cs::reset();
	
	// only the last 10 bits shifted in are used
	Spi::write(channel & 0x03);
	Spi::write(data);
	
	Cs::set();
}
