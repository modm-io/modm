/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "pin.hpp"

namespace modm
{

namespace platform
{

// For backwards compability.
using Peripheral = PeripheralPin;

// GpioConnector only exists for backwards compability with bitbang API.
template<Peripheral peripheral, template<Peripheral _> class... Signals>
struct GpioConnector
{
	template<class GpioQuery>
	static constexpr bool Contains = true;

	inline static void connect() {}
	inline static void disconnect() {}
};

}  // namespace platform

}  // namespace modm