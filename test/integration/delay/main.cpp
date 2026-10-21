/*
 * Copyright (c) 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/time/cycle_counter.hpp>

#include "../integration_test.hpp"

using namespace Board;

/**
 * Benchmarks `modm::delay_ns()` and `modm::delay_us()` with the cycle counter.
 * A delay must never be more than 20% too short, and delays of 1000 cycles or more
 * must not be more than 20% too long.
 */
static bool passed{true};

modm_fastdata modm::CycleCounter counter;
#ifdef TCNT1
constexpr uint32_t SystemCoreClock = F_CPU;
// The cycle counter uses the 16-bit Timer1, so longer delays cannot be measured
constexpr uint32_t MaxCycles = 0xffff;
#else
constexpr uint32_t MaxCycles = 0xffff'ffff;
#endif

static void
run_delay_ns(uint32_t ns)
{
	{
		modm::atomic::Lock _;
		counter.start();
		modm::delay_ns(ns);
		counter.stop();
	}
	const uint32_t cycles = counter.cycles();
	const uint32_t expected = uint64_t(SystemCoreClock) * ns / 1'000'000'000ull;
	const uint32_t real = counter.nanoseconds();
	const char verdict = (cycles < expected*1.2f ? (cycles > expected*0.8f ? ' ' : '<') : '>');
	MODM_LOG_INFO.printf("%8lu | %7lu | %7lu | %8lu      %c\n", ns, expected, cycles, real, verdict) << modm::flush;
	// short delays are dominated by the call overhead of ~30 cycles and may be too long
	if (expected <= MaxCycles and (verdict == '<' or (verdict == '>' and expected >= 1000))) passed = false;
}

static void
run_delay_us(uint32_t us)
{
	{
		modm::atomic::Lock _;
		counter.start();
		modm::delay_us(us);
		counter.stop();
	}
	const uint32_t cycles = counter.cycles();
	const uint32_t expected = uint64_t(SystemCoreClock) * us / 1'000'000ull;
	const uint32_t real = counter.microseconds();
	const char verdict = (cycles < expected*1.2f ? (cycles > expected*0.8f ? ' ' : '<') : '>');
	MODM_LOG_INFO.printf("%8lu | %8lu | %8lu | %8lu      %c\n", us, expected, cycles, real, verdict) << modm::flush;
	if (expected <= MaxCycles and (verdict == '<' or (verdict == '>' and expected >= 1000))) passed = false;
}

static void
run_test_ns(bool short_test=false)
{
	MODM_LOG_INFO << "\nmodm::delay_ns for system clock = " << SystemCoreClock << modm::endl;
	MODM_LOG_INFO << "    expected       |      measured\n";
	MODM_LOG_INFO << "      ns |  cycles |  cycles |       ns\n";

	if (short_test)
	{
		run_delay_ns(       100);
		run_delay_ns(     1'000);
		run_delay_ns(    10'000);
		run_delay_ns(   100'000);
		run_delay_ns( 1'000'000);
		run_delay_ns(10'000'000);
	}
	else
	{
		for (uint32_t ii=0; ii <= 10000; ii += 10) run_delay_ns(ii);
		run_delay_ns(100'000);
		run_delay_ns(1'000'000);
		run_delay_ns(10'000'000);
	}
}

static void
run_test_us(bool short_test=false)
{
	MODM_LOG_INFO << "\nmodm::delay_us for system clock = " << SystemCoreClock << modm::endl;
	MODM_LOG_INFO << "     expected       |       measured\n";
	MODM_LOG_INFO << "      us |  cycles  |  cycles  |       us\n";

	if (short_test)
	{
		run_delay_us(      1);
		run_delay_us(      5);
		run_delay_us(     10);
		run_delay_us(    100);
		run_delay_us(  1'000);
		run_delay_us( 10'000);
		run_delay_us(100'000);
	}
	else
	{
		for (uint32_t ii=1; ii <= 1000; ii += 1) run_delay_us(ii);
		run_delay_us( 10'000);
		run_delay_us(100'000);
	}
}

#ifdef RCC
// The STM32 also measures the delays with the clock after reset
struct BootClock
{
	// STM32F0 needs /2
	static constexpr float scalar = 1.0 / 1;
	static constexpr uint32_t Frequency = Rcc::BootFrequency * scalar;
	static constexpr uint32_t Usart1 = Frequency;
	static constexpr uint32_t Usart2 = Frequency;
	static constexpr uint32_t Usart3 = Frequency;
	static constexpr uint32_t Usart4 = Frequency;
	static constexpr uint32_t Usart5 = Frequency;
	static constexpr uint32_t Usart6 = Frequency;
	static constexpr uint32_t Usart10 = Frequency;
	static constexpr uint32_t Uart4 = Frequency;
	static constexpr uint32_t Uart5 = Frequency;
	static constexpr uint32_t Uart7 = Frequency;
	static constexpr uint32_t Uart8 = Frequency;
	static constexpr uint32_t Uart9 = Frequency;
	static constexpr uint32_t Lpuart1 = Frequency;
};

// Boards that log via RTT have no stlink::Uart, which needs no clock setup.
// Qualified lookup only uses the fallback if the board does not declare these.
namespace rtt_fallback
{
struct Uart
{
	template< class... > static void connect() {}
	template< class, auto... > static void initialize() {}
};
namespace Tx { using Tx = void; }
}
namespace Board::stlink { using namespace ::rtt_fallback; }
#endif

int main()
{
#ifdef RCC
	Board::stlink::Uart::connect<Board::stlink::Tx::Tx>();
	Board::stlink::Uart::initialize<BootClock, 115200_Bd, 5_pct>();
	counter.initialize(true);

	run_test_ns();
	run_test_us();
#endif

	Board::initialize();
	counter.initialize(true);

	run_test_ns();
	run_test_us();

	return finishTest(passed);
}
