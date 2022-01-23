/*
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "base.hpp"

namespace modm::platform
{

/**
 * Gpio OpenDrain template, which remaps the behavior of the Gpio pin to
 * simulate an open-drain output (with internal pullups if needed).
 *
 * @see BitBangI2cMaster
 * @ingroup	modm_platform_gpio
 */
template< class Pin >
class GpioOpenDrain : public Pin
{
	static inline Gpio::InputType inputType = Gpio::InputType::Floating;
	static_assert(Pin::direction == modm::Gpio::Direction::InOut, "Pin must inherit from modm::GpioIO");
public:
	using IO = GpioOpenDrain<typename Pin::IO>;
	using Output = IO;
	using Input = IO;
	using Type = typename Pin::Type;

	static constexpr modm::Gpio::Direction direction = modm::Gpio::Direction::Out;

	enum class
	OutputType
	{
		PushPull,
		OpenDrain,
	};

public:
	inline static void setInput() {}
	inline static void setInput(Gpio::InputType type) { inputType = type; }
	inline static void configure(Gpio::InputType type) { inputType = type; }

	inline static void setOutput() {}
	inline static void setOutput(OutputType) {}
	inline static void setOutput(bool status) { set(status); }

	/// maps to `setInput(InputType::Floating)` or `setInput(InputType::PullUp)`
	inline static void set() { Pin::setInput(inputType); }
	/// maps to `setOutput(::modm::Gpio::Low)`
	inline static void reset() { Pin::setOutput(::modm::Gpio::Low); }
	inline static void set(bool status) { status ? set() : reset(); }
	inline static bool isSet()
	{ return (Pin::getDirection() == modm::Gpio::Direction::In); }

	inline static modm::Gpio::Direction getDirection()
	{ return modm::Gpio::Direction::Out; }
};

} // namespace modm::platform
