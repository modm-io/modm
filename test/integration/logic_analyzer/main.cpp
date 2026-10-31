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

#include <array>
#include <cstddef>
#include <cstdint>
#include <modm/board.hpp>
#include <modm/driver/logic_analyzer.hpp>

#include "../integration_test.hpp"

/**
 * Bit-bangs two 4 bit counters on two different GPIO ports and samples both
 * ports with the DMA logic analyzer. The captured samples are verified against
 * the known counter sequence:
 *
 * - a port must count up by exactly one between consecutive samples,
 * - the two ports must be sampled at the same time, so port B is always
 *   port A plus an offset,
 * - the capture must contain the full requested buffer.
 *
 * Three captures are checked: two ports one-shot, two ports in circular mode
 * and one port one-shot.
 *
 * The counter is driven from the main loop by polling SysTick, so that the
 * analyzer's timer and the counter clock are independent. The pins are
 * outputs; reading GPIOx_IDR of an output pin returns the driven level, which
 * makes the test independent of any external wiring.
 *
 * Pins normally used by the debugger, the ST-Link UART or the Ethernet PHY are
 * skipped: PA1/PA7 (RMII on Nucleo-144 boards), PA2/PA3 (USART2 on many
 * Nucleos) and PB3/PB4 (SWO/JTAG). PB2 is not bonded out on the Nucleo-L432KC
 * and PB7 is the blue user LED on Nucleo-144 boards, so both are skipped too.
 */

using namespace modm::platform;

namespace
{

// The two ports are written by software; a longer sample period keeps the
// write skew small on slow cores. The 4-port Gray-code test runs at 500 kHz.
constexpr uint32_t SampleRate = 100'000;
constexpr uint32_t CounterRate = 50'000;
constexpr uint8_t CounterBits = 4;
constexpr uint8_t CounterMask = (1u << CounterBits) - 1;
constexpr uint8_t CounterOffset = 0x8;

// 4 bit counter on pins that are available on all supported boards and do not
// collide with the debugger, the ST-Link UART or the Ethernet PHY:
constexpr uint8_t PinsA[CounterBits] = {0, 4, 5, 6};
constexpr uint8_t PinsB[CounterBits] = {0, 1, 5, 6};

constexpr uint32_t
bsrr(uint8_t value, const uint8_t (&pins)[CounterBits])
{
	uint32_t result = 0;
	for (uint8_t ii = 0; ii < CounterBits; ii++)
	{
		const uint8_t pin = pins[ii];
		if (value & (1u << ii))
			result |= 1ul << pin;
		else
			result |= 1ul << (pin + 16);
	}
	return result;
}

constexpr std::array<uint32_t, 1u << CounterBits>
makeTable(const uint8_t (&pins)[CounterBits], uint8_t offset)
{
	std::array<uint32_t, 1u << CounterBits> table{};
	for (uint8_t ii = 0; ii < table.size(); ii++)
		table[ii] = bsrr((ii + offset) & CounterMask, pins);
	return table;
}

constexpr auto TableA = makeTable(PinsA, 0);
constexpr auto TableB = makeTable(PinsB, CounterOffset);

uint8_t
pack(uint16_t idr, const uint8_t (&pins)[CounterBits])
{
	uint8_t value = 0;
	for (uint8_t ii = 0; ii < CounterBits; ii++) value |= ((idr >> pins[ii]) & 1) << ii;
	return value;
}

struct Verification
{
	uint32_t errors = 0;      ///< skipped counter values
	uint32_t mismatches = 0;  ///< ports not sampled at the same time
	uint32_t transitions = 0;
	size_t firstError = size_t(-1);
	size_t firstMismatch = size_t(-1);
	uint8_t errorPort = 0;
	uint8_t errorPrevious = 0;
	uint8_t errorCurrent = 0;
	uint8_t errorA = 0;
	uint8_t errorB = 0;
};

uint8_t counter = 0;

/// Restarts the SysTick phase, so that the first counter update happens a full
/// update period after the capture starts. Without this, a pending COUNTFLAG
/// and a reload shortly after can advance the counter twice between two
/// samples.
void
alignCounter()
{
	SysTick->VAL = 0;
	(void)SysTick->CTRL;
}

void
advanceCounter()
{
	counter = (counter + 1) & CounterMask;
	// Load both values first, then write the two ports back-to-back.
	const uint32_t valueA = TableA[counter];
	const uint32_t valueB = TableB[counter];
	GPIOA->BSRR = valueA;
	GPIOB->BSRR = valueB;
}

/// Runs the counter for the given number of updates.
void
runCounter(uint32_t updates)
{
	uint32_t guard = 0;
	while (updates and ++guard < 10'000'000ul)
	{
		if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
		{
			advanceCounter();
			updates--;
		}
	}
}

/// Runs the counter until a one-shot capture is full.
bool
runUntilCaptureComplete()
{
	uint32_t guard = 0;
	while (modm::LogicAnalyzer::isRunning())
	{
		if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) advanceCounter();
		// The capture takes a few milliseconds; this only failsafe loops.
		if (++guard > 10'000'000ul) return false;
	}
	return true;
}

Verification
verify(size_t count, bool twoPorts)
{
	Verification result;
	uint8_t previousA = 0;
	uint8_t previousB = 0;
	for (size_t ii = 0; ii < count; ii++)
	{
		const uint8_t a = pack(modm::LogicAnalyzer::sample(0, ii), PinsA);
		const uint8_t b = twoPorts ? pack(modm::LogicAnalyzer::sample(1, ii), PinsB) : 0;
		if (ii > 0)
		{
			const uint8_t deltaA = (a - previousA) & CounterMask;
			if (deltaA > 1)
			{
				result.errors++;
				if (result.firstError == size_t(-1))
				{
					result.firstError = ii;
					result.errorPort = 0;
					result.errorPrevious = previousA;
					result.errorCurrent = a;
				}
			}
			if (deltaA == 1) result.transitions++;
			if (twoPorts and ((b - previousB) & CounterMask) > 1)
			{
				result.errors++;
				if (result.firstError == size_t(-1))
				{
					result.firstError = ii;
					result.errorPort = 1;
					result.errorPrevious = previousB;
					result.errorCurrent = b;
				}
			}
		}
		if (twoPorts and b != ((a + CounterOffset) & CounterMask))
		{
			result.mismatches++;
			if (result.firstMismatch == size_t(-1)) result.firstMismatch = ii;
		}
		previousA = a;
		previousB = b;
	}
	return result;
}

bool
verifyTwice(Verification result, size_t count)
{
	// The software writes the two ports back-to-back, but a sample taken
	// between the two writes still sees the old value on one port. A few such
	// samples are tolerated. A half-period skew of the analyzer would mismatch
	// a quarter of all samples and fail.
	return (result.errors <= count / 16) and (result.transitions > count / 8) and
		   (result.mismatches <= count / 8);
}

}  // anonymous namespace

int
main()
{
	Board::initialize();

	GpioA0::setOutput();
	GpioA4::setOutput();
	GpioA5::setOutput();
	GpioA6::setOutput();
	GpioB0::setOutput();
	GpioB1::setOutput();
	GpioB5::setOutput();
	GpioB6::setOutput();

	GPIOA->BSRR = TableA[0];
	GPIOB->BSRR = TableB[0];

	// Use SysTick as an independent clock for the counter. The interrupt is not
	// needed, the counter is advanced by polling COUNTFLAG in the main loop.
	SysTickTimer::disable();
	constexpr uint32_t reload = Board::SystemClock::Frequency / CounterRate - 1;
	static_assert(reload < (1ul << 24), "SysTick reload value is too large!");
	SysTick->LOAD = reload;
	SysTick->VAL = 0;
	(void)SysTick->CTRL;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

	bool passed = true;

	// 1. Two ports, one-shot.
	const uint32_t rate =
		modm::LogicAnalyzer::start<Board::SystemClock, SampleRate, GpioA0, GpioA4, GpioA5, GpioA6,
								   GpioB0, GpioB1, GpioB5, GpioB6>();
	alignCounter();
	const bool complete = runUntilCaptureComplete();
	modm::LogicAnalyzer::stop();
	const size_t count = modm::LogicAnalyzer::count(0);
	const Verification once = verify(count, true);

	passed &= complete;
	passed &= (rate != 0);
	passed &= (modm::LogicAnalyzer::ports() == 2);
	passed &= (count == modm::LogicAnalyzer::Samples);
	passed &= (modm::LogicAnalyzer::count(1) == count);
	const bool oncePassed = verifyTwice(once, count);
	passed &= oncePassed;

	MODM_LOG_INFO << "one-shot: " << rate << " Hz, " << count << " samples, " << once.transitions
				  << " transitions, " << once.mismatches << " port mismatches, " << once.errors
				  << " sequence errors" << modm::endl;
	if (not oncePassed and once.errors) {
		MODM_LOG_INFO << "  first error at " << once.firstError << " port " << int(once.errorPort)
					  << ": " << int(once.errorPrevious) << " -> " << int(once.errorCurrent)
					  << " (A " << int(once.errorA) << ", B " << int(once.errorB) << ", expected B "
					  << int((once.errorA + CounterOffset) & CounterMask) << ")" << modm::endl;
	}
	if (not oncePassed and once.mismatches) {
		MODM_LOG_INFO << "  first mismatch at " << once.firstMismatch << modm::endl;
	}

	// 2. Two ports, circular mode: Fill the buffer several times and stop it.
	modm::LogicAnalyzer::start<Board::SystemClock, SampleRate, GpioA0, GpioA4, GpioA5, GpioA6,
							   GpioB0, GpioB1, GpioB5, GpioB6>(true);
	alignCounter();
	// A few extra updates move the buffer head away from zero, so that the
	// rotation of the circular buffer is verified as well.
	runCounter(2 * modm::LogicAnalyzer::Samples * CounterRate / SampleRate + 7);
	modm::LogicAnalyzer::stop();
	const size_t circularCount = modm::LogicAnalyzer::count(0);
	const Verification circular = verify(circularCount, true);

	passed &= (modm::LogicAnalyzer::ports() == 2);
	passed &= (circularCount == modm::LogicAnalyzer::Samples);
	passed &= (modm::LogicAnalyzer::count(1) == circularCount);
	passed &= verifyTwice(circular, circularCount);

	MODM_LOG_INFO << "circular: " << circularCount << " samples, head "
				  << modm::LogicAnalyzer::head(0) << ", " << circular.transitions
				  << " transitions, " << circular.mismatches << " port mismatches, "
				  << circular.errors << " sequence errors" << modm::endl;

	// 3. One port, one-shot.
	modm::LogicAnalyzer::start<Board::SystemClock, SampleRate, GpioA0, GpioA4, GpioA5, GpioA6>();
	alignCounter();
	const bool completeSingle = runUntilCaptureComplete();
	modm::LogicAnalyzer::stop();
	const size_t singleCount = modm::LogicAnalyzer::count(0);
	const Verification single = verify(singleCount, false);

	passed &= completeSingle;
	passed &= (modm::LogicAnalyzer::ports() == 1);
	passed &= (singleCount == modm::LogicAnalyzer::Samples);
	passed &= (single.errors <= singleCount / 16) and (single.transitions > singleCount / 8);

	MODM_LOG_INFO << "one port: " << singleCount << " samples, " << single.transitions
				  << " transitions, " << single.errors << " sequence errors" << modm::endl;

	return finishTest(passed);
}
