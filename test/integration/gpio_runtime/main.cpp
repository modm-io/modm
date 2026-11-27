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
#include <algorithm>
#include <array>
#include <type_traits>
#include "../integration_test.hpp"

using namespace modm::platform;
using namespace std::chrono_literals;

/**
 * Tests modm::platform::GpioPin and modm::AnyGpio:
 *
 * - against the pin levels,
 * - in constexpr tables, with inverted and unused pins,
 * - as template argument of BitBangSpiMaster through modm::AnyGpioAdapter,
 * - with a randomized comparison against the static GPIO classes: from a random
 *   pin configuration, every operation must leave all registers of the port as
 *   the static class does.
 *
 * Prints the cost of the pin kinds next to the static GPIO classes.
 *
 * Uses PA0, PA4, PA6, PA8 and PB15, and PA5 for the clock of the bit-bang SPI.
 * Nothing needs to be connected.
 */
#ifndef GPIO_RUNTIME_STRESS_SECONDS
#define GPIO_RUNTIME_STRESS_SECONDS 5
#endif

#ifdef GPIO_CRL_MODE0
static constexpr bool HasCr{true};
#else
static constexpr bool HasCr{false};
#endif

// Tables in flash, with the pin kinds that need no object at runtime
static constexpr modm::AnyGpio table[]{GpioA0{}, GpioInverted<GpioA4>{}, GpioUnused{}, {}};
static_assert(not table[0].isUnused() and not table[1].isUnused());
static_assert(table[2].isUnused() and table[3].isUnused());
static_assert(std::is_trivially_copyable_v<modm::AnyGpio>);
// GpioPin has no way to represent it, AnyGpio does
static_assert(not std::constructible_from<GpioPin, GpioUnused>);

// A runtime pin as template argument of a class with static pins. MOSI and
// MISO are the same pin, so that a transfer reads back what it writes.
struct SckSource { static inline modm::AnyGpio pin{}; static modm::AnyGpio get() { return pin; } };
struct DataSource { static inline modm::AnyGpio pin{}; static const modm::AnyGpio& get() { return pin; } };
using Sck = modm::AnyGpioAdapter<SckSource>;
using Data = modm::AnyGpioAdapter<DataSource>;
using SpiLoopback = BitBangSpiMaster<Sck, Data, Data>;

// Global, non-const pins, so that the compiler cannot fold them into constants
GpioPin pin0{GpioA0{}};
modm::AnyGpio any0{GpioA0{}};
std::array<GpioPin, 3> pins{GpioA0{}, GpioA4{}, GpioA6{}};
std::array<modm::AnyGpio, 3> anys{GpioA0{}, GpioA4{}, GpioPin{Gpio::Port::A, 6}};

bool passed{true};
#define CHECK(expr) do { if (not (expr)) { passed = false; \
	MODM_LOG_ERROR << "FAILED: " #expr " in line " << __LINE__ << modm::endl; } } while(0)

/// IDR follows a write only after the input synchronizer
static void settle() { modm::delay(2us); }

/// Checks a pin kind against the pin level in GPIOA.
template< class Pin >
static void
checkPin(Pin pin, uint32_t mask, bool inverted = false)
{
	const auto high = [&] { return bool(GPIOA->ODR & mask) != inverted; };
	pin.setOutput(modm::Gpio::High);
	CHECK(pin.isSet()); CHECK(high());
	pin.reset();
	CHECK(not pin.isSet()); CHECK(not high());
	settle(); CHECK(not pin.read());
	pin.toggle();
	CHECK(pin.isSet()); CHECK(high());
	settle(); CHECK(pin.read());
	pin.set(false);
	CHECK(not pin.isSet()); CHECK(not high());
	settle(); CHECK(not pin.read());
	pin.set(true);
	CHECK(pin.isSet()); CHECK(high());
	settle(); CHECK(pin.read());
	pin.set();
	CHECK(pin.isSet());
	pin.reset();
}

// ----------------------------------------------------------------------------
// Comparison against the static GPIO classes
struct Regs
{
#ifdef GPIO_CRL_MODE0
	uint32_t crl, crh, odr;
#else
	uint32_t moder, otyper, ospeedr, pupdr, odr, afrl, afrh;
#ifdef GPIO_ASCR_ASC0
	uint32_t ascr;
#endif
#endif
	bool operator==(const Regs&) const = default;

	static Regs
	load(const GPIO_TypeDef *p)
	{
#ifdef GPIO_CRL_MODE0
		return {p->CRL, p->CRH, p->ODR};
#elif defined(GPIO_ASCR_ASC0)
		return {p->MODER, p->OTYPER, p->OSPEEDR, p->PUPDR, p->ODR, p->AFR[0], p->AFR[1], p->ASCR};
#else
		return {p->MODER, p->OTYPER, p->OSPEEDR, p->PUPDR, p->ODR, p->AFR[0], p->AFR[1]};
#endif
	}

	void
	store(GPIO_TypeDef *p) const
	{
		p->ODR = odr;
#ifdef GPIO_CRL_MODE0
		p->CRL = crl; p->CRH = crh;
#else
		p->OTYPER = otyper; p->OSPEEDR = ospeedr; p->PUPDR = pupdr;
		p->AFR[0] = afrl; p->AFR[1] = afrh;
#ifdef GPIO_ASCR_ASC0
		p->ASCR = ascr;
#endif
		p->MODER = moder;
#endif
	}

	/// Random, valid configuration of this one pin
	void
	randomize(uint8_t pin, uint32_t rnd)
	{
		const uint32_t bit = 1ul << pin;
		odr = (odr & ~bit) | ((rnd & 1) ? bit : 0);
#ifdef GPIO_CRL_MODE0
		uint32_t field = (rnd >> 1) & 0xf;
		if (field == 0b1100) field = 0b0100; // reserved
		auto &cr = (pin < 8) ? crl : crh;
		const uint8_t shift = (pin & 7) * 4;
		cr = (cr & ~(0xful << shift)) | (field << shift);
#else
		const uint8_t shift = pin * 2;
		const auto field = [&](uint32_t &reg, uint32_t value)
		{ reg = (reg & ~(3ul << shift)) | (value << shift); };
		field(moder, (rnd >> 1) & 3);
		field(ospeedr, (rnd >> 3) & 3);
		field(pupdr, ((rnd >> 5) & 3) % 3);
		otyper = (otyper & ~bit) | ((rnd & (1ul << 7)) ? bit : 0);
		auto &afr = (pin < 8) ? afrl : afrh;
		afr = (afr & ~(0xful << ((pin & 7) * 4))) | (((rnd >> 8) & 0xf) << ((pin & 7) * 4));
#ifdef GPIO_ASCR_ASC0
		ascr = (ascr & ~bit) | ((rnd & (1ul << 12)) ? bit : 0);
#endif
#endif
	}
};

static uint32_t rng_state{0x2F6E2B1};
static uint32_t
rng()
{
	rng_state ^= rng_state << 13;
	rng_state ^= rng_state >> 17;
	rng_state ^= rng_state << 5;
	return rng_state;
}

static constexpr uint8_t OperationCount{12};
struct Operation
{
	uint8_t kind;
	bool value;
	Gpio::OutputType output;
	Gpio::OutputSpeed speed;
	Gpio::InputType input;

	static Operation
	random()
	{
		static constexpr Gpio::OutputType outputs[]{Gpio::OutputType::PushPull, Gpio::OutputType::OpenDrain};
		static constexpr Gpio::OutputSpeed speeds[]{Gpio::OutputSpeed::Low, Gpio::OutputSpeed::Medium, Gpio::OutputSpeed::High};
		static constexpr Gpio::InputType inputs[]{Gpio::InputType::Floating, Gpio::InputType::PullUp, Gpio::InputType::PullDown};
		const uint32_t rnd = rng();
		return {uint8_t((rnd & 0xff) % OperationCount), bool(rnd & (1ul << 8)), outputs[(rnd >> 9) & 1],
				speeds[((rnd >> 10) & 3) % 3], inputs[((rnd >> 12) & 3) % 3]};
	}

	template< class Pin >
	void
	apply(const Pin &pin) const
	{
		switch(kind)
		{
			case 0: pin.setOutput(); break;
			case 1: pin.setOutput(value); break;
			case 2: pin.setOutput(output, speed); break;
			case 3: pin.configure(output, speed); break;
			case 4: pin.setInput(); break;
			case 5: pin.setInput(input); break;
			case 6: pin.configure(input); break;
			case 7: pin.set(); break;
			case 8: pin.reset(); break;
			case 9: pin.set(value); break;
			case 10: pin.toggle(); break;
			default: pin.disconnect(); break;
		}
	}
};

static uint32_t stress_operations{}, stress_mismatches{}, stress_read_checks{};
static std::array<uint32_t, OperationCount> stress_per_kind{};
static std::array<uint32_t, 4> stress_causes{};

template< class Static >
static void
stressPin()
{
	static constexpr GpioPin runtime{Static{}};
	static constexpr modm::AnyGpio any{Static{}};
	GPIO_TypeDef *const port = (Static::port == Gpio::Port::A) ? GPIOA : GPIOB;

	Regs before = Regs::load(port);
	before.randomize(Static::pin, rng());
	const Operation operation = Operation::random();

	before.store(port);
	operation.apply(Static{});
	const Regs expected = Regs::load(port);
	const bool expected_set = Static::isSet();
	const auto expected_direction = Static::getDirection();

	before.store(port);
	operation.apply(runtime);
	const Regs actual_pin = Regs::load(port);
	uint8_t causes{};
	if (actual_pin != expected) causes |= 0b0001;
	if (runtime.isSet() != expected_set) causes |= 0b0010;
	// The static getDirection() of the F1 never returns In
	if (not HasCr and runtime.getDirection() != expected_direction) causes |= 0b0100;

	before.store(port);
	operation.apply(any);
	const Regs actual_any = Regs::load(port);
	if (actual_any != expected) causes |= 0b0001;
	if (any.isSet() != expected_set) causes |= 0b0010;
	if (not HasCr and any.getDirection() != expected_direction) causes |= 0b0100;

	// A peripheral can drive the pin in alternate function mode, like the MCO
	// clock on PA8, and a floating input can change at any time: only compare
	// a stable level of a pin that is an input or output.
	settle();
	const bool level = Static::read();
	const bool read_pin = runtime.read(), read_any = any.read();
	if (runtime.getDirection() != modm::Gpio::Direction::Special and level == Static::read())
	{
		if (read_pin != level or read_any != level) causes |= 0b1000;
		stress_read_checks++;
	}

	stress_operations++;
	stress_per_kind[operation.kind]++;
	if (causes)
	{
		stress_causes[0] += bool(causes & 0b0001);
		stress_causes[1] += bool(causes & 0b0010);
		stress_causes[2] += bool(causes & 0b0100);
		stress_causes[3] += bool(causes & 0b1000);
		if (not stress_mismatches++)
			MODM_LOG_ERROR << "FAILED: operation " << operation.kind << " on P" << char('A' + uint8_t(Static::port))
						   << Static::pin << " inverted=" << Static::isInverted << " causes=" << causes
						   << " direction=" << uint8_t(expected_direction) << modm::endl;
		passed = false;
	}
}

static void
stress()
{
	const Regs port_a = Regs::load(GPIOA), port_b = Regs::load(GPIOB);
	const modm::AnyGpio unused{};

	uint32_t unused_checks{};
	const auto start = modm::Clock::now();
	auto last = start;
	while (modm::Clock::now() - start < std::chrono::seconds{GPIO_RUNTIME_STRESS_SECONDS})
	{
		for (uint8_t ii = 0; ii < 50; ii++)
		{
			stressPin<GpioA0>();
			stressPin<GpioA4>();
			stressPin<GpioA6>();
			stressPin<GpioA8>();
			stressPin<GpioB15>();
			stressPin<GpioInverted<GpioA6>>();
			stressPin<GpioInverted<GpioB15>>();

			const Regs a = Regs::load(GPIOA), b = Regs::load(GPIOB);
			Operation::random().apply(unused);
			CHECK(a == Regs::load(GPIOA) and b == Regs::load(GPIOB));
			CHECK(not unused.read() and not unused.isSet());
			CHECK(unused.getDirection() == modm::Gpio::Direction::In);
			unused_checks++;
		}
		if (modm::Clock::now() - last >= 60s)
		{
			last = modm::Clock::now();
			MODM_LOG_INFO << stress_operations << " operations, " << stress_mismatches << " mismatches" << modm::endl;
		}
	}
	port_a.store(GPIOA);
	port_b.store(GPIOB);

	MODM_LOG_INFO << "Stress: " << stress_operations << " operations against the static classes, "
				  << stress_read_checks << " reads, " << stress_mismatches << " mismatches (registers "
				  << stress_causes[0] << ", isSet " << stress_causes[1] << ", direction " << stress_causes[2]
				  << ", read " << stress_causes[3] << ")" << modm::endl;
	MODM_LOG_INFO << "Per operation:";
	for (const auto count : stress_per_kind) MODM_LOG_INFO << " " << count;
	MODM_LOG_INFO << modm::endl;
	MODM_LOG_INFO << "Unused: " << unused_checks << " operations" << modm::endl;
	CHECK(stress_operations > 0);
	for (const auto count : stress_per_kind) CHECK(count > 0);
}

// ----------------------------------------------------------------------------
constexpr uint32_t Loops = 2000;

template< class Pin >
modm_noinline void
setReset(Pin &pin)
{
	for (uint32_t ii = 0; ii < Loops; ii++) {
		pin.set(); pin.reset(); pin.set(); pin.reset();
		pin.set(); pin.reset(); pin.set(); pin.reset();
	}
}

template< class Pin >
modm_noinline void
setResetLocal(const Pin pin)
{
	for (uint32_t ii = 0; ii < Loops; ii++) {
		pin.set(); pin.reset(); pin.set(); pin.reset();
		pin.set(); pin.reset(); pin.set(); pin.reset();
	}
}

template< class Function >
static void
benchmark(const char *name, Function &&function)
{
	uint32_t best{-1u};
	for (int ii = 0; ii < 5; ii++)
	{
		const auto start = modm::PreciseClock::now();
		function();
		best = std::min<uint32_t>(best, (modm::PreciseClock::now() - start).count());
	}
	// tenths of a cycle per operation
	const uint32_t cycles10 = uint64_t(best) * (SystemCoreClock / 100'000) / (Loops * 8);
	MODM_LOG_INFO.printf("%-24s %5lu.%lu cycles\n", name, cycles10 / 10, cycles10 % 10);
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "Runtime GPIO test" << modm::endl;

	// The pin kinds must give the same levels
	checkPin(GpioA0{}, 1u << 0);
	checkPin(GpioPin{GpioA4{}}, 1u << 4);
	checkPin(modm::AnyGpio{GpioA6{}}, 1u << 6);
	// Inverted pins, also from a table in flash
	checkPin(GpioPin{GpioInverted<GpioA6>{}}, 1u << 6, true);
	checkPin(table[1], 1u << 4, true);
	for (auto pin : pins) pin.reset();
	pins[1].set(); CHECK(GPIOA->ODR & (1u << 4)); pins[1].reset();
	for (auto pin : anys) pin.setOutput(modm::Gpio::Low);
	anys[2].toggle(); CHECK(GPIOA->ODR & (1u << 6));
	anys[1].toggle(); CHECK(GPIOA->ODR & (1u << 4));
	for (auto pin : anys) pin.reset();
	CHECK(not (GPIOA->ODR & ((1u << 0) | (1u << 4) | (1u << 6))));

	// Directions and pulls of a pin that nothing else drives
	const modm::AnyGpio pull{GpioA6{}};
	pull.setOutput();
	CHECK(pull.getDirection() == modm::Gpio::Direction::Out);
	pull.setInput(Gpio::InputType::PullUp);
	CHECK(pull.getDirection() == modm::Gpio::Direction::In);
	modm::delay(100us); CHECK(pull.read());
	pull.setInput(Gpio::InputType::PullDown);
	modm::delay(100us); CHECK(not pull.read());
	// Open-drain: released to the pull-up when set, driven when reset
	pull.configure(Gpio::InputType::PullUp);
	pull.set();
	pull.setOutput(Gpio::OutputType::OpenDrain);
	if constexpr (not HasCr) { modm::delay(100us); CHECK(pull.read()); }
	pull.reset(); modm::delay(100us); CHECK(not pull.read());
	pull.disconnect();
	CHECK(pull.getDirection() == modm::Gpio::Direction::In);

	// A real driver with static pins on top of runtime pins
	SckSource::pin = GpioA5{};
	DataSource::pin = GpioInverted<GpioInverted<GpioA6>>{};
	Sck::setOutput(Sck::OutputType::PushPull);
	Data::setOutput(Data::OutputType::PushPull);
	SpiLoopback::initialize<Board::SystemClock, 500_kHz, modm::pct(100)>();
	for (const uint8_t byte : {0x00, 0xA5, 0x5A, 0xFF, 0x81})
		CHECK(SpiLoopback::transfer(byte) == byte);
	Sck::disconnect();
	Data::disconnect();

	stress();

	MODM_LOG_INFO << "Cycles per set or reset:" << modm::endl;
	GpioA0 static0;
	GpioA0::setOutput();
	benchmark("static", [&]{ setReset(static0); });
	benchmark("GpioPin", [&]{ setReset(pin0); });
	benchmark("AnyGpio", [&]{ setReset(any0); });
	benchmark("AnyGpio local copy", [&]{ setResetLocal(any0); });

	return finishTest(passed);
}
