/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SHIFT_REGISTER_INPUT_HPP
	#error	"Don't include this file directly, use 'shift_register_input.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
/*
 * 74HC165
 * 
 * Low level at SH/!LD (= Load) loads input into shift registers. Low-to-high
 * transition shifts values into the next stage.
 * 
 * Load must be low for at least 20n at 4,5V and 100ns at 2V
 * 
 * ClkInh (= CS) must be held low for clk to be active.
 * 
 * 74HC125 can be used to disable the Dout Output (Tri-State).
 */
// ----------------------------------------------------------------------------
template <typename Spi,	typename Load, size_t N>
Spi modm::ShiftRegisterInput<Spi, Load, N>::spi;

template <typename Spi,	typename Load, size_t N>
Load modm::ShiftRegisterInput<Spi, Load, N>::load;

template <typename Spi,	typename Load, size_t N>
uint8_t modm::ShiftRegisterInput<Spi, Load, N>::value[N];

// ----------------------------------------------------------------------------
template <typename Spi,	typename Load, size_t N>
void
modm::ShiftRegisterInput<Spi, Load, N>::initialize()
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
modm::ShiftRegisterInput<Spi, Store, N>::operator [] (uint_fast8_t port) const
{
	return cache[port];
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Store, size_t N>
uint8_t&
modm::ShiftRegisterInput<Spi, Store, N>::operator [] (uint_fast8_t port)
{
	return cache[port];
}

// ----------------------------------------------------------------------------
template <typename Spi,	typename Load, size_t N>
void
modm::ShiftRegisterInput<Spi, Load, N>::update()
{
	/* Load Parallel */
	store.set();
	// FIXME delay 20-100ns
	store.reset();
	
	for (int_fast8_t i = N-1; i >= 0; --i) {
		cache[i] = spi.write(0);
	}
}
