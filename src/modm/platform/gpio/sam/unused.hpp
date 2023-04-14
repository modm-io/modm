/*
 * Copyright (c) 2020, Niklas Hauser
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

#include "pin.hpp"

namespace modm::platform
{

/**
 * @author	Niklas Hauser
 * @ingroup	modm_platform_gpio
 */
class GpioUnused : public ::modm::GpioIO
{
public:
	using Output = GpioUnused;
	using Input = GpioUnused;
	using IO = GpioUnused;
	using Type = GpioUnused;
	static constexpr bool isInverted = false;
	static constexpr PortName port = PortName(-1);
	static constexpr uint8_t pin = 0;
	static constexpr uint32_t mask = 0;

	// GpioOutput
	static void setOutput() {}
	static void setOutput(bool) {}
	static void set() {}
	static void set(bool) {}
	static void reset() {}
	static void toggle() {}
	static bool isSet() { return false; }

	// GpioInput
	static void setInput() {}
	static bool read() { return false; }

	// GpioIO
	static Direction getDirection() { return Direction::Special; }
	static void lock() {}
	static void disconnect() {}
};

}  // namespace modm::platform
