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

// Fuzzes modm::atomic::Queue with a timer interrupt at a random high rate:
// first the interrupt produces and the main loop consumes, then the other way
// around. Every element is several words wide, so that torn copies, lost or
// duplicated elements and reordering of the element and index accesses are
// detected. The large queue uses 16-bit indices, which are not natively
// atomic on AVR.

#include <modm/board.hpp>
#include <modm/architecture/interface/atomic_queue.hpp>
#include <atomic>

#include "../integration_test.hpp"

using namespace std::chrono_literals;

// Several words wide, so that interrupting the copy of an element is likely
struct Item
{
#ifdef MODM_CPU_AVR
	static constexpr uint8_t Words{4};
#else
	static constexpr uint8_t Words{8};
#endif
	uint32_t words[Words]{};

	constexpr Item() = default;
	constexpr explicit Item(uint32_t sequence)
	{
		for (uint8_t ii = 0; ii < Words; ii++) words[ii] = (sequence + ii) * 2654435761ul;
	}

	constexpr bool
	valid(uint32_t expected) const
	{
		for (uint8_t ii = 0; ii < Words; ii++)
			if (words[ii] != (expected + ii) * 2654435761ul) return false;
		return true;
	}
};

enum class
Phase : uint8_t
{
	Idle,
	InterruptProduces,
	InterruptConsumes,
};
static std::atomic<Phase> phase{Phase::Idle};

static modm::atomic::Queue<Item, 7> small_queue;
static modm::atomic::Queue<Item, 260> large_queue;
static std::atomic<bool> use_large{false};

// Only written by the interrupt and read by the main loop
static std::atomic<uint32_t> isr_sequence, isr_full, isr_empty, isr_errors, isr_calls;
static void
increment(std::atomic<uint32_t>& counter)
{ counter.store(counter.load(std::memory_order_relaxed) + 1, std::memory_order_relaxed); }

// Pseudo random period of the next interrupt
static uint16_t
next_period()
{
	static uint16_t state{0xACE1};
	state ^= state << 7;
	state ^= state >> 9;
	state ^= state << 8;
	return state;
}

template< class Queue >
static void
interrupt(Queue& queue)
{
	increment(isr_calls);
	switch (phase.load(std::memory_order_relaxed))
	{
		case Phase::InterruptProduces:
			if (queue.push(Item{isr_sequence})) increment(isr_sequence);
			else increment(isr_full);
			break;
		case Phase::InterruptConsumes:
			if (queue.isEmpty()) { increment(isr_empty); break; }
			if (not queue.get().valid(isr_sequence)) increment(isr_errors);
			increment(isr_sequence);
			queue.pop();
			break;
		default:
			break;
	}
}

static void
handle_interrupt()
{
	if (use_large.load(std::memory_order_relaxed)) interrupt(large_queue);
	else interrupt(small_queue);
}

#ifdef MODM_CPU_AVR
// Timer1 in CTC mode with 0.5µs ticks: interrupts every 20-148µs
static constexpr uint16_t MinTicks{40};
static constexpr uint32_t AveragePeriodUs{84};
MODM_ISR(TIMER1_COMPA)
{
	OCR1A = MinTicks + (next_period() & 0xff);
	handle_interrupt();
}
static void
start_timer()
{
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS11);
	OCR1A = MinTicks;
	TIMSK1 = (1 << OCIE1A);
}
static void
stop_timer()
{ TIMSK1 = 0; }
#else
// Timer2 with 1µs ticks: interrupts every 4-35µs
using Timer = modm::platform::Timer2;
static constexpr uint16_t MinTicks{4};
static constexpr uint32_t AveragePeriodUs{20};
MODM_ISR(TIM2)
{
	Timer::acknowledgeInterruptFlags(Timer::InterruptFlag::Update);
	Timer::setOverflow(MinTicks + (next_period() & 0x1f));
	handle_interrupt();
}
static void
start_timer()
{
	Timer::enable();
	Timer::setMode(Timer::Mode::UpCounter);
	Timer::setPrescaler(Board::SystemClock::Timer2 / 1'000'000);
	Timer::setOverflow(MinTicks);
	Timer::applyAndReset();
	Timer::enableInterrupt(Timer::Interrupt::Update);
	Timer::enableInterruptVector(true, 10);
	Timer::start();
}
static void
stop_timer()
{ Timer::disableInterrupt(Timer::Interrupt::Update); }
#endif

static bool
check(const char* name, bool passed)
{
	MODM_LOG_INFO << (passed ? "pass: " : "FAIL: ") << name << modm::endl;
	return passed;
}

// Stalls the main loop for a random time of up to twice the time the
// interrupt needs to fill or drain the queue, so that both the full and the
// empty queue are exercised.
template< class Queue >
static void
stall()
{
	static uint16_t state{0x1234};
	state ^= state << 7; state ^= state >> 9; state ^= state << 8;
	const uint32_t max_us = 2 * Queue::getMaxSize() * AveragePeriodUs;
	modm::delay_us(state % max_us);
}

static void
reset_counters()
{
	for (auto* counter : {&isr_sequence, &isr_full, &isr_empty, &isr_errors, &isr_calls})
		counter->store(0);
}

template< class Queue >
static bool
interrupt_produces(Queue& queue, const char* name)
{
	reset_counters();
	uint32_t expected{0}, errors{0}, empty{0};
	const auto start = modm::Clock::now();
	phase = Phase::InterruptProduces;
	while (modm::Clock::now() - start < 1s)
	{
		if (queue.isEmpty())
		{
			// stall sometimes when the queue is empty, so that it fills up
			if ((++empty & 0x3f) == 0) stall<Queue>();
			continue;
		}
		if (not queue.get().valid(expected)) errors++;
		expected++;
		queue.pop();
	}
	phase = Phase::Idle;
	// drain the remaining elements
	while (queue.isNotEmpty())
	{
		if (not queue.get().valid(expected)) errors++;
		expected++;
		queue.pop();
	}
	MODM_LOG_INFO << name << ": interrupt produced " << isr_sequence.load() << ", main consumed "
				  << expected << ", full " << isr_full.load() << ", errors " << errors << modm::endl;
	return check(name, errors == 0 and expected == isr_sequence and expected > 1000 and isr_full > 0);
}

template< class Queue >
static bool
interrupt_consumes(Queue& queue, const char* name)
{
	reset_counters();
	uint32_t sequence{0}, full{0};
	const auto start = modm::Clock::now();
	phase = Phase::InterruptConsumes;
	while (modm::Clock::now() - start < 1s)
	{
		if (queue.push(Item{sequence})) sequence++;
		// stall sometimes when the queue is full, so that it drains
		else if ((++full & 0x3f) == 0) stall<Queue>();
	}
	// let the interrupt drain the queue
	while (queue.isNotEmpty()) ;
	phase = Phase::Idle;
	MODM_LOG_INFO << name << ": main produced " << sequence << ", interrupt consumed "
				  << isr_sequence.load() << ", empty " << isr_empty.load() << ", errors " << isr_errors.load() << modm::endl;
	return check(name, isr_errors == 0 and isr_sequence == sequence and sequence > 1000 and full > 0 and isr_empty > 0);
}

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "atomic::Queue fuzzed by a random high-rate interrupt" << modm::endl;
	bool passed = true;

	start_timer();

	passed &= interrupt_produces(small_queue, "interrupt -> main, 8-bit index");
	passed &= interrupt_consumes(small_queue, "main -> interrupt, 8-bit index");

	use_large = true;
	passed &= interrupt_produces(large_queue, "interrupt -> main, 16-bit index");
	passed &= interrupt_consumes(large_queue, "main -> interrupt, 16-bit index");

	stop_timer();
	MODM_LOG_INFO << "interrupts: " << isr_calls.load() << " in the last phase" << modm::endl;
	return finishTest(passed);
}
