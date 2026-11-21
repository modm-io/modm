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

// Hammers one std::atomic from the main loop and a fast timer interrupt, which
// on the Cortex-M0+ both go through modm::atomic::Lock, nested inside another
// lock that protects a plain counter. The inner locks must neither release the
// spinlock nor unmask the interrupts. The first phase only runs on core 0, so
// that a hang cannot be caused by the other core holding the spinlock. The
// second phase runs on both cores. Lost increments or an interrupt that stopped
// firing fail the test, a deadlock never finishes it.

#include <modm/board.hpp>
#include <modm/platform/core/multicore.hpp>
#include <hardware/structs/rosc.h>
#include <hardware/structs/timer.h>
#include <atomic>

#include "../integration_test.hpp"

using namespace modm::platform;

static constexpr uint32_t PhaseUs{5'000'000};
static constexpr uint32_t PeriodUs[2]{7, 11};

static std::atomic<uint32_t> counter{0};
static std::atomic<bool> core1_done{false};
static volatile uint32_t deadline{0};
static volatile uint32_t main_count[2]{};
static volatile uint32_t isr_count[2]{};
static uint32_t random[2]{};
static volatile uint32_t nested_count{0};
static volatile uint32_t nested_errors{0};

static bool
running()
{
	return int32_t(deadline - timer_hw->timerawl) > 0;
}

static void
increment()
{
	modm::atomic::Lock outer;
	const uint32_t value = nested_count;
	counter.fetch_add(1);
	{
		modm::atomic::Lock inner;
	}
	if (not __get_PRIMASK() or not (sio_hw->spinlock_st & 1))
		nested_errors = nested_errors + 1;
	nested_count = value + 1;
}

template<uint8_t Core>
static void
tick()
{
	timer_hw->intr = 1u << Core;
	increment();
	isr_count[Core] = isr_count[Core] + 1;
	// xorshift32, so that the interrupt hits every instruction of the lock
	uint32_t x = random[Core];
	x ^= x << 13; x ^= x >> 17; x ^= x << 5;
	random[Core] = x;
	if (running()) timer_hw->alarm[Core] = timer_hw->timerawl + PeriodUs[Core] + (x & 7);
}

MODM_ISR(TIMER_IRQ_0) { tick<0>(); }
MODM_ISR(TIMER_IRQ_1) { tick<1>(); }

template<uint8_t Core>
static void
hammer()
{
	const auto irq = Core ? TIMER_IRQ_1_IRQn : TIMER_IRQ_0_IRQn;
	NVIC_EnableIRQ(irq);
	timer_hw->alarm[Core] = timer_hw->timerawl + PeriodUs[Core];
	while (running())
	{
		increment();
		main_count[Core] = main_count[Core] + 1;
	}
	NVIC_DisableIRQ(irq);
}

static void
core1_main()
{
	hammer<1>();
	core1_done = true;
	while (true) __WFE();
}

int
main()
{
	Board::initialize();
	timer_hw->inte |= 0b11;
	// A different interleaving on every boot, the clocks are otherwise deterministic
	uint32_t seed{};
	for (uint8_t ii = 0; ii < 32; ii++) seed = (seed << 1) | rosc_hw->randombit;
	random[0] = seed | 1;
	random[1] = ~seed | 1;

	MODM_LOG_INFO << "core 0 only" << modm::endl << modm::flush;
	deadline = timer_hw->timerawl + PhaseUs;
	hammer<0>();

	MODM_LOG_INFO << "both cores" << modm::endl << modm::flush;
	deadline = timer_hw->timerawl + PhaseUs;
	multicore::Core1::run(core1_main);
	hammer<0>();
	while (not core1_done) ;

	const uint32_t main0 = main_count[0], main1 = main_count[1];
	const uint32_t isr0 = isr_count[0], isr1 = isr_count[1];
	const uint32_t total = counter.load(), expected = main0 + main1 + isr0 + isr1;
	const uint32_t nested = nested_count, errors = nested_errors;
	MODM_LOG_INFO << "main " << main0 << " " << main1 << " isr " << isr0 << " " << isr1
				  << " counter " << total << " nested " << nested << " errors " << errors
				  << " expected " << expected << modm::endl;

	const bool passed = total == expected and nested == expected and errors == 0 and
						isr0 > 2 * PhaseUs / (PeriodUs[0] + 7) / 4 and
						isr1 > PhaseUs / (PeriodUs[1] + 7) / 4;
	return finishTest(passed);
}
