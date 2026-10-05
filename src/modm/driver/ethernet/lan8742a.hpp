/*
 * Copyright (c) 2026, Luca Feggi
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "lan87xx.hpp"

namespace modm
{

struct Lan8742a
{
	struct Traits
	{
		static constexpr uint16_t Identifier1 = 0x0007;
		static constexpr uint16_t Identifier2 = 0xc130;
		static constexpr uint16_t Identifier2Mask = 0xfff0;
	};

	template<ethernet::Clause22Mdio Mdio, uint8_t PhyAddress = 0>
	using Driver = ethernet::Lan87xxPhy<Mdio, Traits, PhyAddress>;
};

template<ethernet::Clause22Mdio Mdio, uint8_t Address = 0>
using Lan8742aPhy = Lan8742a::Driver<Mdio, Address>;

}  // namespace modm
