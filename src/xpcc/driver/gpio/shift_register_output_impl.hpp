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
 *
 * $Id: shift_register_output_impl.hpp 610 2011-09-21 22:31:12Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SHIFT_REGISTER_OUTPUT_HPP
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
Spi xpcc::ShiftRegisterOutput<Spi, Store, N>::spi;

template <typename Spi, typename Store, size_t N>
Store xpcc::ShiftRegisterOutput<Spi, Store, N>::store;

template <typename Spi, typename Store, size_t N>
uint8_t xpcc::ShiftRegisterOutput<Spi, Store, N>::value[N];

// ----------------------------------------------------------------------------
template <typename Spi, typename Store, size_t N>
void
xpcc::ShiftRegisterOutput<Spi, Store, N>::initialize()
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
xpcc::ShiftRegisterOutput<Spi, Store, N>::operator [] (uint_fast8_t port) const
{
	return cache[port];
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Store, size_t N>
uint8_t&
xpcc::ShiftRegisterOutput<Spi, Store, N>::operator [] (uint_fast8_t port)
{
	return cache[port];
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Store, size_t N>
void
xpcc::ShiftRegisterOutput<Spi, Store, N>::update()
{
	for (int_fast8_t i = N-1; i >= 0; --i) {
		spi.write(cache[i]);
	}
	
	// Pulse the hc595 store clock to load the shifted bits
	store.set();
	// FIXME delay 20-100ns
	store.reset();
}
