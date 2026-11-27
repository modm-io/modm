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

#include <modm/board.hpp>
#include <modm/platform/gpio/any.hpp>
#include "../integration_test.hpp"

using namespace modm::platform;
using namespace std::chrono_literals;

/**
 * Compares GpioPin and AnyGpio against the static GPIO classes on all platforms,
 * only through the GPIO API. For the register level test on STM32 see
 * `gpio_runtime`. Nothing needs to be connected to the test pin.
 */
#ifndef GPIO_RUNTIME_STRESS_SECONDS
#define GPIO_RUNTIME_STRESS_SECONDS 2
#endif

#if defined(MODM_OS_HOSTED)
#elif defined(__AVR__)
using TestPin = GpioB5;
#elif defined(NRF_P0)
using TestPin = GpioP0_26;
#elif defined(SIO_BASE)
using TestPin = Gpio15;
#elif defined(PIOA)
using TestPin = GpioA6;
#else
using TestPin = GpioA06;
#endif

static bool passed{true};
static uint32_t operations{}, mismatches{};

static void
check(bool condition, uint16_t line)
{
	if (condition) return;
	passed = false;
	if (not mismatches++) MODM_LOG_INFO << "FAILED in line " << line << modm::endl;
}
#define CHECK(expr) check((expr), __LINE__)

static uint16_t rng_state{0xACE1};
static uint16_t
rng()
{
	rng_state ^= rng_state << 7;
	rng_state ^= rng_state >> 9;
	rng_state ^= rng_state << 8;
	return rng_state;
}

static void settle() { modm::delay(5us); }

template< class Pin >
static void
apply(const Pin &pin, uint8_t operation, bool value)
{
	switch(operation)
	{
		case 0: pin.set(); break;
		case 1: pin.reset(); break;
		case 2: pin.set(value); break;
		case 3: pin.toggle(); break;
		default: pin.setOutput(value); break;
	}
}

#ifndef MODM_OS_HOSTED
template< class Static, class Runtime >
static void
compareOutput(const Runtime &runtime, uint8_t operation, bool start, bool value)
{
	Static::setOutput(start);
	apply(Static{}, operation, value);
	const bool expected = Static::isSet();
	settle();
	const bool level = Static::read();

	Static::setOutput(start);
	apply(runtime, operation, value);
	CHECK(runtime.isSet() == expected);
	CHECK(runtime.getDirection() == modm::Gpio::Direction::Out);
	settle();
	CHECK(runtime.read() == level);
	operations++;
}

template< class Static, class Runtime >
static void
compareInput(const Runtime &runtime, typename Static::InputType type)
{
	Static::setInput(type);
	settle();
	const bool level = Static::read();
	Static::setOutput(not level);

	runtime.setInput(type);
	CHECK(runtime.getDirection() == modm::Gpio::Direction::In);
	settle();
	CHECK(runtime.read() == level);
	operations++;
}

template< class Static >
static void
compare()
{
	static constexpr GpioPin pin{Static{}};
	static constexpr modm::AnyGpio any{Static{}};
	using InputType = typename Static::InputType;

	const uint16_t random = rng();
	const uint8_t operation = (random & 0xff) % 5;
	const bool start = random & 0x100, value = random & 0x200;
	compareOutput<Static>(pin, operation, start, value);
	compareOutput<Static>(any, operation, start, value);

	compareInput<Static>(pin, InputType::PullUp);
	compareInput<Static>(any, InputType::PullUp);
	if constexpr (requires { InputType::PullDown; })
	{
		compareInput<Static>(pin, InputType::PullDown);
		compareInput<Static>(any, InputType::PullDown);
	}
	Static::disconnect();
}

/// The same configuration types on every platform, also for what it does not have
template< class Static >
static void
comparePortable()
{
	static constexpr modm::AnyGpio any{Static{}};
	using InputType = modm::AnyGpio::InputType;
	using OutputType = modm::AnyGpio::OutputType;
	using OutputSpeed = modm::AnyGpio::OutputSpeed;
	using StaticInput = typename Static::InputType;

	Static::setInput(StaticInput::PullUp);
	settle();
	const bool pulled = Static::read();
	Static::setInput(StaticInput::Floating);
	CHECK(any.setInput(InputType::PullUp));
	settle();
	CHECK(any.read() == pulled);
	CHECK(any.configure(InputType::PullUp));
	CHECK(any.setInput(InputType::Floating));
	CHECK(any.getDirection() == modm::Gpio::Direction::In);

	constexpr bool hasPullDown = requires { StaticInput::PullDown; };
	CHECK(any.setInput(InputType::PullDown) == hasPullDown);
	CHECK(any.configure(InputType::PullDown) == hasPullDown);
	CHECK(any.getDirection() == modm::Gpio::Direction::In);

	CHECK(any.setOutput(OutputType::PushPull));
	CHECK(any.getDirection() == modm::Gpio::Direction::Out);
	for (const auto speed : {OutputSpeed::Low, OutputSpeed::Medium, OutputSpeed::High, OutputSpeed::VeryHigh})
	{
		CHECK(any.setOutput(OutputType::PushPull, speed));
		CHECK(any.configure(OutputType::PushPull, speed));
	}
	any.set(); CHECK(any.isSet() and Static::isSet());
	any.reset(); CHECK(not any.isSet() and not Static::isSet());

	any.setInput();
	constexpr bool hasOpenDrain = requires { Static::OutputType::OpenDrain; };
	CHECK(any.setOutput(OutputType::OpenDrain) == hasOpenDrain);
	CHECK(any.getDirection() == (hasOpenDrain ? modm::Gpio::Direction::Out : modm::Gpio::Direction::In));
	any.setOutput(OutputType::PushPull);
	Static::disconnect();
	operations++;
}
#endif

static void
checkUnused()
{
	static constexpr modm::AnyGpio table[]{GpioUnused{}, {}};
	for (const auto pin : table)
	{
		CHECK(pin.isUnused());
		for (uint8_t operation = 0; operation < 5; operation++) apply(pin, operation, true);
		pin.setOutput();
		pin.setInput();
		pin.disconnect();
		CHECK(not pin.isSet() and not pin.read());
		CHECK(pin.getDirection() == modm::Gpio::Direction::In);
		operations++;
	}
}

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "Runtime GPIO platform test" << modm::endl;

	const auto start = modm::Clock::now();
	while (modm::Clock::now() - start < std::chrono::seconds{GPIO_RUNTIME_STRESS_SECONDS})
	{
		checkUnused();
#ifndef MODM_OS_HOSTED
		compare<TestPin>();
		compare<GpioInverted<TestPin>>();
		comparePortable<TestPin>();
		comparePortable<GpioInverted<TestPin>>();
#endif
	}
	MODM_LOG_INFO << operations << " operations, " << mismatches << " mismatches" << modm::endl;
	CHECK(operations > 0);

	return finishTest(passed);
}
