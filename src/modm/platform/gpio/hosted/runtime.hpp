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

#include <modm/architecture/interface/gpio.hpp>
#include <cstdint>

namespace modm::platform
{

/**
 * Hosted targets have no GPIO pins: this only gives `modm::AnyGpio` a base, so
 * that code using it can be compiled and tested on the host.
 *
 * @ingroup modm_platform_gpio
 */
class GpioPin
{
public:
	enum class InputType { Floating, PullUp, PullDown };
	enum class OutputType { PushPull, OpenDrain };

protected:
	uintptr_t addr;
	uint32_t setBits;

	constexpr GpioPin(uintptr_t addr, uint32_t setBits) : addr(addr), setBits(setBits) {}

public:
	void setOutput() const {}
	void setOutput(bool) const {}
	void setOutput(OutputType) const {}
	void configure(OutputType) const {}
	void setInput() const {}
	void setInput(InputType) const {}
	void configure(InputType) const {}
	::modm::Gpio::Direction getDirection() const { return ::modm::Gpio::Direction::In; }
	void disconnect() const {}
	void set() const {}
	void set(bool) const {}
	void reset() const {}
	void toggle() const {}
	bool isSet() const { return false; }
	bool read() const { return false; }
};

} // namespace modm::platform
