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

#include "runtime.hpp"
#include <modm/architecture/interface/gpio.hpp>
#include <concepts>

namespace modm
{

/**
 * Runtime GPIO pin of this device, or unused.
 *
 * A value type of 8 bytes that can be copied and stored in arrays and constexpr
 * tables. Pins of this device take the inlined fast path of
 * `modm::platform::GpioPin` plus one test and branch. A default constructed pin
 * is unused like `GpioUnused`: all functions do nothing and `read()` returns
 * false.
 *
 * @code
 * std::array<modm::AnyGpio, 3> leds{GpioA5{}, GpioUnused{}, GpioPin{Gpio::Port::B, 3}};
 * for (auto led : leds) led.toggle();
 * @endcode
 *
 * Copy the pin into a local variable in hot loops: the compiler can then hoist
 * the test out of the loop and the pin is as fast as `platform::GpioPin`.
 *
 * @ingroup modm_platform_gpio
 */
class AnyGpio : protected platform::GpioPin
{
	// Bit 0 of the address marks a pin without a GPIO port: the ports are at
	// least 1 KiB aligned.
	static constexpr uintptr_t Unused{1};

	constexpr bool internal() const { return not (addr & 1u); }

public:
	using InputType = platform::Gpio::InputType;
	using OutputType = platform::Gpio::OutputType;
	using OutputSpeed = platform::Gpio::OutputSpeed;
	using Port = platform::Gpio::Port;

	using platform::GpioPin::GpioPin;

	constexpr AnyGpio() : platform::GpioPin(Unused, 0) {}

	// A template, so that classes derived from AnyGpio are not ambiguous
	template< std::same_as<platform::GpioPin> Pin >
	constexpr AnyGpio(Pin pin) : platform::GpioPin(pin) {}

	template< class Pin > requires std::same_as<typename Pin::Data, platform::detail::DataUnused>
	constexpr AnyGpio(Pin) : AnyGpio() {}

	/// @return true if the pin does nothing, like `GpioUnused`.
	constexpr bool isUnused() const { return addr == Unused; }

	void
	setOutput() const { if (internal()) [[likely]] GpioPin::setOutput(); }

	void
	setOutput(bool value) const { set(value); setOutput(); }

	void
	setOutput(OutputType type, OutputSpeed speed = OutputSpeed::MHz50) const
	{ if (internal()) [[likely]] GpioPin::setOutput(type, speed); }

	void
	configure(OutputType type, OutputSpeed speed = OutputSpeed::MHz50) const
	{ if (internal()) [[likely]] GpioPin::configure(type, speed); }

	void
	setInput() const { if (internal()) [[likely]] GpioPin::setInput(); }

	void
	setInput(InputType type) const { if (internal()) [[likely]] GpioPin::setInput(type); }

	void
	configure(InputType type) const { if (internal()) [[likely]] GpioPin::configure(type); }

	::modm::Gpio::Direction
	getDirection() const
	{
		if (internal()) [[likely]] return GpioPin::getDirection();
		return ::modm::Gpio::Direction::In;
	}

	/// Back to a floating input, detached from any peripheral signal
	void
	disconnect() const { if (internal()) [[likely]] GpioPin::disconnect(); }

	modm_always_inline void
	set() const { if (internal()) [[likely]] GpioPin::set(); }

	modm_always_inline void
	reset() const { if (internal()) [[likely]] GpioPin::reset(); }

	modm_always_inline void
	set(bool value) const { if (internal()) [[likely]] GpioPin::set(value); }

	modm_always_inline void
	toggle() const { if (internal()) [[likely]] GpioPin::toggle(); }

	modm_always_inline bool
	isSet() const { if (internal()) [[likely]] return GpioPin::isSet(); return false; }

	modm_always_inline bool
	read() const { if (internal()) [[likely]] return GpioPin::read(); return false; }
};

/**
 * Static GPIO class that forwards to the `modm::AnyGpio` of `Source::get()`.
 *
 * A bridge for classes that take their pins as template arguments, like
 * `BitBangI2cMaster`, when the pin is only known at runtime. `Source::get()`
 * can return a pin attached at runtime or an entry of a lookup table.
 *
 * @code
 * struct SclSource
 * {
 *     static inline modm::AnyGpio pin{};
 *     static modm::AnyGpio get() { return pin; }
 * };
 * using Scl = modm::AnyGpioAdapter<SclSource>;
 * @endcode
 *
 * There is no compile-time pin behind it, so it has no peripheral signals and
 * cannot be used with `connect()` or `SoftwareGpioPort`.
 *
 * @ingroup modm_platform_gpio
 */
template< class Source >
struct AnyGpioAdapter : public GpioIO
{
	using InputType = AnyGpio::InputType;
	using OutputType = AnyGpio::OutputType;
	using OutputSpeed = AnyGpio::OutputSpeed;

	/// The runtime pin behind this class
	static AnyGpio get() { return Source::get(); }

	static void setOutput() { Source::get().setOutput(); }
	static void setOutput(bool value) { Source::get().setOutput(value); }
	static void setOutput(OutputType type, OutputSpeed speed = OutputSpeed::MHz50)
	{ Source::get().setOutput(type, speed); }
	static void configure(OutputType type, OutputSpeed speed = OutputSpeed::MHz50)
	{ Source::get().configure(type, speed); }
	static void set() { Source::get().set(); }
	static void set(bool value) { Source::get().set(value); }
	static void reset() { Source::get().reset(); }
	static void toggle() { Source::get().toggle(); }
	static bool isSet() { return Source::get().isSet(); }
	static void setInput() { Source::get().setInput(); }
	static void setInput(InputType type) { Source::get().setInput(type); }
	static void configure(InputType type) { Source::get().configure(type); }
	static bool read() { return Source::get().read(); }
	static Direction getDirection() { return Source::get().getDirection(); }
	static void disconnect() { Source::get().disconnect(); }
};

static_assert(sizeof(platform::GpioPin) == 2 * sizeof(uint32_t));
static_assert(sizeof(AnyGpio) == 2 * sizeof(uint32_t));

} // namespace modm
