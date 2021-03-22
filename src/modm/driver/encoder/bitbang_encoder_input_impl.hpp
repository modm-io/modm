/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BITBANG_ENCODER_INPUT_HPP
#error "Don't include this file directly, use 'bitbang_encoder_input.hpp' instead!"
#endif

#include <cmath>

template<typename SignalA, typename SignalB, uint8_t POSTSCALER,
		 std::signed_integral DeltaType>
inline uint8_t
modm::BitBangEncoderInput<SignalA, SignalB, POSTSCALER, DeltaType>::getRaw()
{
	const uint8_t read = Signals::read();
	// convert graycode to binary
	uint8_t raw = 0;
	if (read & 0b10) raw = 3;
	if (read & 0b01) raw ^= 1;
	return raw;
}

template<typename SignalA, typename SignalB, uint8_t POSTSCALER,
		 std::signed_integral DeltaType>
inline void
modm::BitBangEncoderInput<SignalA, SignalB, POSTSCALER, DeltaType>::connect()
{
	Signals::setInput(::Gpio::InputType::PullUp);

	// Tare power-on state
	modm::delay(10us);
	raw_last = getRaw();
}

template<typename SignalA, typename SignalB, uint8_t POSTSCALER,
		 std::signed_integral DeltaType>
inline void
modm::BitBangEncoderInput<SignalA, SignalB, POSTSCALER, DeltaType>::update()
{
	uint8_t raw = getRaw();
	const uint8_t diff = raw_last - raw;
	if (diff & 0b01)
	{
		raw_last = raw;
		delta += (diff & 0b10) - 1;  // bit 1 = direction (+/-)
	}
}

template<typename SignalA, typename SignalB, uint8_t POSTSCALER,
		 std::signed_integral DeltaType>
DeltaType
modm::BitBangEncoderInput<SignalA, SignalB, POSTSCALER, DeltaType>::getIncrement()
{
	::modm::atomic::Lock _;
	DeltaType val = delta;

	delta &= (POSTSCALER - 1);  // mask out higher bits

	constexpr uint8_t shift = std::log2(POSTSCALER);  // Number of fraction bits
	return val >> shift;                              // return whats left without fractions
}
