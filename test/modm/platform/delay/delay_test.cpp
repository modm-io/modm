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

#include "delay_test.hpp"

#include <modm/platform.hpp>

using namespace modm::platform;

static uint32_t cyclesMs(uint32_t ms) { return uint64_t(SystemCoreClock) * ms / 1'000ul; }
static uint32_t cyclesUs(uint32_t us) { return uint64_t(SystemCoreClock) * us / 1'000'000ul; }
static uint32_t cyclesNs(uint32_t ns)
{
	const uint32_t cycles = uint64_t(SystemCoreClock) * ns / 1'000'000'000ul;
	return cycles ? cycles : 1;
}

static void
run_delay_ns(uint32_t ns, uint32_t lower=5, uint32_t upper=15)
{
	uint32_t start, stop;
	{
		modm::atomic::Lock _;
#ifdef DWT
		start = DWT->CYCCNT;
#else
		SysTick->VAL = 0;
		stop = SysTick->VAL;
#endif
		modm::delay_ns(ns);
#ifdef DWT
		stop = DWT->CYCCNT;
#else
		start = SysTick->VAL;
#endif
	}
	const uint32_t cycles = (stop - start) - 4;
	TEST_ASSERT_EQUALS_RANGE(cycles, cyclesNs(ns * lower / 10), cyclesNs(ns * upper / 10));
}

static void
run_delay_us(uint32_t us, uint32_t lower=8, uint32_t upper=15)
{
	uint32_t start, stop;
	{
		modm::atomic::Lock _;
#ifdef DWT
		start = DWT->CYCCNT;
#else
		SysTick->VAL = 0;
		stop = SysTick->VAL;
#endif
		modm::delay_us(us);
#ifdef DWT
		stop = DWT->CYCCNT;
#else
		start = SysTick->VAL;
#endif
	}
	const uint32_t cycles = (stop - start) - 4;
	TEST_ASSERT_EQUALS_RANGE(cycles, cyclesUs(us * lower / 10), cyclesUs(us * upper / 10));
}

static void
run_delay_ms(uint32_t ms, uint32_t lower=8, uint32_t upper=15)
{
	uint32_t start, stop;
	{
		modm::atomic::Lock _;
#ifdef DWT
		start = DWT->CYCCNT;
#else
		SysTick->VAL = 0;
		stop = SysTick->VAL;
#endif
		modm::delay_ms(ms);
#ifdef DWT
		stop = DWT->CYCCNT;
#else
		start = SysTick->VAL;
#endif
	}
	const uint32_t cycles = (stop - start) - 4;
	TEST_ASSERT_EQUALS_RANGE(cycles, cyclesMs(ms * lower / 10), cyclesMs(ms * upper / 10));
}

void
DelayTest::setUp()
{
#ifndef DWT
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->LOAD = (1ul << 23);
#endif
}

void
DelayTest::testDelayMs()
{
	run_delay_ms(1, 10, 20);
	run_delay_ms(5);
	run_delay_ms(10);
	run_delay_ms(50);
	run_delay_ms(100);
	// run_delay_ms(500);
	// run_delay_ms(1000);
}

void
DelayTest::testDelayUs()
{
	run_delay_us(1, 8, 20);
	run_delay_us(5, 8, 20);
	run_delay_us(10);
	run_delay_us(50);
	run_delay_us(100);
	run_delay_us(500);
	run_delay_us(1000);
}

void
DelayTest::testDelayNs()
{
	run_delay_ns(1, 10, 10000);
	run_delay_ns(5, 10, 10000);
	run_delay_ns(10, 10, 10000);
	run_delay_ns(50, 10, 1000);
	run_delay_ns(100, 10, 100);
	run_delay_ns(500);
	run_delay_ns(1000);
	run_delay_ns(10'000);
	run_delay_ns(100'000);
	run_delay_ns(1'000'000);
}


