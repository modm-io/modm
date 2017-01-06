/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009, 2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SHIFT_REGISTER_OUTPUT_HPP
	#error	"Don't include this file directly, use 'shift_register_output.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
/*
 * 74HC595
 * 
 * Data is moved to the output registers on a low-to-high transition of
 * RCLK (= Store). 
 */
// ----------------------------------------------------------------------------
template <typename Spi, typename Store, size_t N>
Spi modm::ShiftRegisterOutput<Spi, Store, N>::spi;

template <typename Spi, typename Store, size_t N>
Store modm::ShiftRegisterOutput<Spi, Store, N>::store;

template <typename Spi, typename Store, size_t N>
uint8_t modm::ShiftRegisterOutput<Spi, Store, N>::value[N];

// ----------------------------------------------------------------------------
template <typename Spi, typename Store, size_t N>
void
modm::ShiftRegisterOutput<Spi, Store, N>::initialize()
{
	spi.initialize();
	store.setOutput(modm::Gpio::Low);
	
	for (uint_fast8_t i = 0; i < N; ++i) {
		cache[i] = 0;
	}
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Store, size_t N>
const uint8_t&
modm::ShiftRegisterOutput<Spi, Store, N>::operator [] (uint_fast8_t port) const
{
	return cache[port];
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Store, size_t N>
uint8_t&
modm::ShiftRegisterOutput<Spi, Store, N>::operator [] (uint_fast8_t port)
{
	return cache[port];
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Store, size_t N>
void
modm::ShiftRegisterOutput<Spi, Store, N>::update()
{
	for (int_fast8_t i = N-1; i >= 0; --i) {
		spi.write(cache[i]);
	}
	
	// Pulse the hc595 store clock to load the shifted bits
	store.set();
	// FIXME delay 20-100ns
	store.reset();
}
