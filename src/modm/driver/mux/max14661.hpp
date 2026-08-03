/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/register.hpp>
#include "max14661_transport.hpp"

namespace modm
{

/// @ingroup modm_driver_max14661
struct max14661
{
	static inline uint8_t address(uint8_t bits = 0)
	{ return 0x4C | (bits & 0b11); }

	/// The 16 AB_ pins that can be routed to COMA and/or COMB.
	enum class
	Channel : uint16_t
	{
		Ch1  = Bit0,
		Ch2  = Bit1,
		Ch3  = Bit2,
		Ch4  = Bit3,
		Ch5  = Bit4,
		Ch6  = Bit5,
		Ch7  = Bit6,
		Ch8  = Bit7,
		Ch9  = Bit8,
		Ch10 = Bit9,
		Ch11 = Bit10,
		Ch12 = Bit11,
		Ch13 = Bit12,
		Ch14 = Bit13,
		Ch15 = Bit14,
		Ch16 = Bit15,
	};
	MODM_FLAGS16(Channel);
};

/**
 * MAX14661 Beyond-the-Rails 16:2 analog multiplexer.
 *
 * A serially controlled, dual-channel analog multiplexer allowing any of
 * the 16 AB pins to be connected to either common pin (COMA, COMB)
 * simultaneously in any combination. Beyond-the-Rails technology allows
 * switching ±5.5V signals from a single +1.6V to +5.5V supply.
 *
 * Channels are addressed with the `max14661::Channel` flags, which can be
 * combined with `|` to connect multiple channels to the same common pin
 * at once:
 *
 * @code
 * modm::Max14661<Max14661TransportI2c<MyI2cMaster>> mux;
 * mux.setChannelsA(max14661::Channel::Ch1 | max14661::Channel::Ch5);
 * mux.setChannelsB(max14661::Channel::Ch2);
 * @endcode
 *
 * @tparam	Transport	Either `Max14661TransportI2c` or `Max14661TransportSpi`.
 *
 * @see Max14661TransportI2c
 * @see Max14661TransportSpi
 *
 * @ingroup modm_driver_max14661
 * @author	Niklas Hauser
 */
template < class Transport >
class Max14661 : public max14661, public Transport
{
public:
	/// Constructor.
	/// @param	address	I2C slave address (7bit, unshifted), default is A1=A0=0.
	///					Ignored when used with `Max14661TransportSpi`.
	Max14661(uint8_t address = max14661::address())
	: Transport(address) {}

	/// Connect the given channels to COMA, disconnecting all others on
	/// this bank.
	bool inline
	setChannelsA(Channel_t channels)
	{ return Transport::writeChannelsA(channels.value); }

	/// Connect the given channels to COMB, disconnecting all others on
	/// this bank.
	bool inline
	setChannelsB(Channel_t channels)
	{ return Transport::writeChannelsB(channels.value); }

	/// Disconnect all channels from COMA.
	bool inline
	disableChannelsA()
	{ return setChannelsA(Channel_t()); }

	/// Disconnect all channels from COMB.
	bool inline
	disableChannelsB()
	{ return setChannelsB(Channel_t()); }
};

} // namespace modm
