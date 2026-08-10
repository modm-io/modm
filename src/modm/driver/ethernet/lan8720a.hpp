/*
 * Copyright (c) 2020, Mike Wolfram
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_LAN8720A_HPP
#define MODM_LAN8720A_HPP

#include "lan87xx.hpp"

namespace modm
{

template <uint8_t PhyAddress = 0>
struct Lan8720a : Lan87xx
{
	static_assert(PhyAddress < 32, "Clause 22 PHY addresses are five bits wide");
	static constexpr uint8_t Address = PhyAddress;
	static constexpr uint16_t PhyIdentifier1 = 0x0007;
	static constexpr uint16_t PhyIdentifier2 = 0xc0f0;
	static constexpr uint16_t PhyIdentifier2Mask = 0xfff0;

	template <ethernet::Clause22Mdio Mdio>
	static InitializationResult initialize()
	{
		return Lan87xx::initialize<Mdio, Address>(
				PhyIdentifier1, PhyIdentifier2, PhyIdentifier2Mask);
	}

	template <ethernet::Clause22Mdio Mdio>
	static LinkStatusResult readLinkStatus()
	{
		return Lan87xx::readLinkStatus<Mdio, Address>();
	}
};

} // namespace modm

#endif // MODM_LAN8720A_HPP
