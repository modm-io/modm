// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SHIFT_REGISTER_INPUT_HPP
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
Spi xpcc::ShiftRegisterInput<Spi, Load, N>::spi;

template <typename Spi,	typename Load, size_t N>
Load xpcc::ShiftRegisterInput<Spi, Load, N>::load;

template <typename Spi,	typename Load, size_t N>
uint8_t xpcc::ShiftRegisterInput<Spi, Load, N>::value[N];

// ----------------------------------------------------------------------------
template <typename Spi,	typename Load, size_t N>
void
xpcc::ShiftRegisterInput<Spi, Load, N>::initialize()
{
	spi.initialize();
	store.setOutput(xpcc::gpio::LOW);
	
	for (uint_fast8_t i = 0; i < N; ++i) {
		cache[i] = 0;
	}
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Store, size_t N>
const uint8_t&
xpcc::ShiftRegisterInput<Spi, Store, N>::operator [] (uint_fast8_t port) const
{
	return cache[port];
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Store, size_t N>
uint8_t&
xpcc::ShiftRegisterInput<Spi, Store, N>::operator [] (uint_fast8_t port)
{
	return cache[port];
}

// ----------------------------------------------------------------------------
template <typename Spi,	typename Load, size_t N>
void
xpcc::ShiftRegisterInput<Spi, Load, N>::update()
{
	/* Load Parallel */
	store.set();
	// FIXME delay 20-100ns
	store.reset();
	
	for (int_fast8_t i = N-1; i >= 0; --i) {
		cache[i] = spi.write(0);
	}
}
