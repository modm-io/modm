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

#include <cstddef>
#include <cstdint>
#include <modm/board.hpp>
#include <modm/driver/logic_analyzer.hpp>

#include "../integration_test.hpp"

/**
 * Samples one to four GPIO ports with one pin each and verifies a Gray-coded
 * counter across the ports.
 *
 * Each port carries one bit of the counter, so a sample of all ports forms one
 * Gray code word. Only one bit changes between updates, therefore a sample
 * taken while the four ports are written cannot observe a mixture of old and
 * new values. Decoding the Gray code to binary must produce a counter that
 * increments by zero or one between samples. This verifies that all configured
 * ports are sampled on the same timer event, without relying on a particular
 * bit pattern per port.
 *
 * The per-port transition counts additionally verify that the ports are
 * mapped in the right order: bit 0 toggles most often, bit 3 least often.
 *
 * Pins: PA0, PB0, PC6 and PD2. They are available on all test boards and avoid
 * the debugger, ST-Link UART, Ethernet PHY and external memory buses. The pins
 * are outputs, so no external wiring is needed.
 */

using namespace modm::platform;

namespace
{

constexpr uint32_t SampleRate = 500'000;
constexpr uint32_t CounterRate = 250'000;
constexpr uint8_t Channels = modm::LogicAnalyzer::MaxPorts;
constexpr uint8_t Mask = (1u << Channels) - 1;
constexpr uint8_t Pins[4] = {0, 0, 6, 2};
volatile uint32_t *const Bsrr[4] = {&GPIOA->BSRR, &GPIOB->BSRR, &GPIOC->BSRR, &GPIOD->BSRR};

template<uint8_t N>
struct Starter
{
	static uint32_t
	start(bool circular)
	{
		if constexpr (N == 1)
			return modm::LogicAnalyzer::start<Board::SystemClock, SampleRate, GpioA0>(circular);
		else if constexpr (N == 2)
			return modm::LogicAnalyzer::start<Board::SystemClock, SampleRate, GpioA0, GpioB0>(
				circular);
		else if constexpr (N == 3)
			return modm::LogicAnalyzer::start<Board::SystemClock, SampleRate, GpioA0, GpioB0,
											  GpioC6>(circular);
		else
			return modm::LogicAnalyzer::start<Board::SystemClock, SampleRate, GpioA0, GpioB0,
											  GpioC6, GpioD2>(circular);
	}
};

uint8_t counter = 0;

void
writeGray(uint8_t gray)
{
	for (uint8_t ii = 0; ii < Channels; ii++)
	{
		const uint8_t pin = Pins[ii];
		*Bsrr[ii] = (gray & (1u << ii)) ? (1ul << pin) : (1ul << (pin + 16));
	}
}

void
advanceCounter()
{
	counter = (counter + 1) & Mask;
	writeGray(counter ^ (counter >> 1));
}

void
alignCounter()
{
	SysTick->VAL = 0;
	(void)SysTick->CTRL;
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
		if (++guard > 10'000'000ul) return false;
	}
	return true;
}

struct Verification
{
	uint32_t errors = 0;
	uint32_t transitions = 0;
	uint32_t portTransitions[4] = {0, 0, 0, 0};
	size_t firstError = size_t(-1);
	uint8_t previous = 0;
	uint8_t current = 0;
};

uint8_t
grayToBinary(uint8_t gray)
{
	gray ^= gray >> 1;
	gray ^= gray >> 2;
	return gray;
}

Verification
verify(size_t count)
{
	Verification result;
	uint8_t previousBinary = 0;
	uint8_t lastBits[4] = {0, 0, 0, 0};
	for (size_t ii = 0; ii < count; ii++)
	{
		uint8_t gray = 0;
		for (uint8_t channel = 0; channel < Channels; channel++)
		{
			const uint8_t bit = (modm::LogicAnalyzer::sample(channel, ii) >> Pins[channel]) & 1;
			gray |= bit << channel;
			if (ii > 0 and bit != lastBits[channel]) { result.portTransitions[channel]++; }
			lastBits[channel] = bit;
		}
		const uint8_t binary = grayToBinary(gray);
		if (ii > 0)
		{
			const uint8_t delta = (binary - previousBinary) & Mask;
			if (delta > 1)
			{
				result.errors++;
				if (result.firstError == size_t(-1))
				{
					result.firstError = ii;
					result.previous = previousBinary;
					result.current = binary;
				}
			}
			if (delta == 1) result.transitions++;
		}
		previousBinary = binary;
	}
	return result;
}

bool
verifyResult(const Verification &result, size_t count)
{
	bool passed = (result.errors <= count / 16) and (result.transitions > count / 8);
	for (uint8_t channel = 0; channel < Channels; channel++)
		passed &= (result.portTransitions[channel] > 0);
	// The Gray code bit 0 must toggle at least as often as bit 1, and so on.
	// Two-bit Gray codes toggle both bits equally often, and a 256 sample window
	// can give adjacent bits a slightly different count, so allow a tolerance.
	// A swapped or missing port on three or four channels still produces a
	// clearly larger increase.
	const uint32_t tolerance = count / 32;
	for (uint8_t channel = 1; channel < Channels; channel++)
		passed &=
			(result.portTransitions[channel] <= result.portTransitions[channel - 1] + tolerance);
	return passed;
}

}  // anonymous namespace

int
main()
{
	Board::initialize();

	GpioA0::setOutput();
	GpioB0::setOutput();
	GpioC6::setOutput();
	GpioD2::setOutput();
	writeGray(0);

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

	// 1. One-shot capture of all configured ports.
	const uint32_t rate = Starter<Channels>::start(false);
	alignCounter();
	const bool complete = runUntilCaptureComplete();
	modm::LogicAnalyzer::stop();
	const size_t count = modm::LogicAnalyzer::count(0);
	const Verification once = verify(count);

	passed &= complete;
	passed &= (rate != 0);
	passed &= (modm::LogicAnalyzer::ports() == Channels);
	passed &= (count == modm::LogicAnalyzer::Samples);
	for (uint8_t channel = 1; channel < Channels; channel++)
		passed &= (modm::LogicAnalyzer::count(channel) == count);
	const bool oncePassed = verifyResult(once, count);
	passed &= oncePassed;

	MODM_LOG_INFO << "one-shot: " << rate << " Hz, " << count << " samples, " << once.transitions
				  << " transitions, " << once.errors << " sequence errors (ports";
	for (uint8_t channel = 0; channel < Channels; channel++)
		MODM_LOG_INFO << " " << once.portTransitions[channel];
	MODM_LOG_INFO << ")" << modm::endl;
	if (not oncePassed and once.errors)
	{
		MODM_LOG_INFO << "  first error at " << once.firstError << ": " << int(once.previous)
					  << " -> " << int(once.current) << modm::endl;
	}

	// 2. Circular capture: fill the buffer several times and move its head.
	Starter<Channels>::start(true);
	alignCounter();
	runCounter(2 * modm::LogicAnalyzer::Samples * CounterRate / SampleRate + 7);
	modm::LogicAnalyzer::stop();
	const size_t circularCount = modm::LogicAnalyzer::count(0);
	const Verification circular = verify(circularCount);

	passed &= (modm::LogicAnalyzer::ports() == Channels);
	passed &= (circularCount == modm::LogicAnalyzer::Samples);
	for (uint8_t channel = 1; channel < Channels; channel++)
		passed &= (modm::LogicAnalyzer::count(channel) == circularCount);
	const bool circularPassed = verifyResult(circular, circularCount);
	passed &= circularPassed;

	MODM_LOG_INFO << "circular: " << circularCount << " samples, head "
				  << modm::LogicAnalyzer::head(0) << ", " << circular.transitions
				  << " transitions, " << circular.errors << " sequence errors (ports";
	for (uint8_t channel = 0; channel < Channels; channel++)
		MODM_LOG_INFO << " " << circular.portTransitions[channel];
	MODM_LOG_INFO << ")" << modm::endl;

	return finishTest(passed);
}
