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

#include <modm/architecture/interface/gpio.hpp>

namespace modm
{

namespace platform
{

/// @ingroup modm_platform_gpio
enum class
Peripheral
{
	BitBang,
	// ...
};

/// @ingroup	modm_platform_gpio
struct Gpio
{
	/// Each Input Pin can be configured in one of these states.
	enum class
	InputType : uint8_t
	{
		Floating,	///< The input pin is left floating
	};

	enum class
	OutputType : uint8_t
	{
		PushPull	///< push-pull on output
	};

	/// Available ports on this device.
	enum class
	Port
	{
        // ...
	};

	enum class
	Signal
	{
		BitBang,
        // ...
	};

	/// @endcond
};

/// @}

}	// namespace platform

}	// namespace modm
